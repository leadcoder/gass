/****************************************************************************
* This file is part of GASS.                                                *
* See http://code.google.com/p/gass/                                 *
*                                                                           *
* Copyright (c) 2008-2009 GASS team. See Contributors.txt for details.      *
*                                                                           *
* GASS is free software: you can redistribute it and/or modify              *
* it under the terms of the GNU Lesser General Public License as published  *
* by the Free Software Foundation, either version 3 of the License, or      *
* (at your option) any later version.                                       *
*                                                                           *
* GASS is distributed in the hope that it will be useful,                   *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU Lesser General Public License for more details.                       *
*                                                                           *
* You should have received a copy of the GNU Lesser General Public License  *
* along with GASS. If not, see <http://www.gnu.org/licenses/>.              *
*****************************************************************************/
#include "Core/Common.h"
#include "Plugins/Ogre/OgreGraphicsSystem.h"
#include "Plugins/Ogre/OgreRenderWindow.h"
#include "Plugins/Ogre/OgreViewport.h"
#include "Plugins/Ogre/OgreDebugTextOutput.h"
#include "Plugins/Ogre/OgrePostProcess.h"
#include "Plugins/Ogre/Helpers/DebugDrawer.h"
#include "Plugins/Ogre/OgreConvert.h"
#include "Plugins/Ogre/Helpers/OgreText.h"
#include "Plugins/Ogre/OgreResourceManager.h"

#include "Core/Utils/GASSException.h"
#include "Core/System/GASSSystemFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"

#include "Sim/Interface/GASSIInputSystem.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSResourceGroup.h"


#include <OgreRoot.h>
#include <OgreRenderWindow.h>
#include <OgreRenderSystem.h>
#include <OgreWindowEventUtilities.h>
#include <OgreStringConverter.h>
#include <OgreLogManager.h>
#ifndef OGRE_18
#include <Overlay/OgreOverlaySystem.h>
#endif

namespace GASS
{
	OgreGraphicsSystem::OgreGraphicsSystem(void): m_CreateMainWindowOnInit(true), 
		m_SceneMgr(NULL),
		m_UpdateMessagePump(true),
		m_DebugTextBox (new OgreDebugTextOutput()),
		m_ResourceManager(OgreResourceManagerPtr(new OgreResourceManager)),
		m_ShowStats(true),
		m_UseShaderCache(false)
	{
	}

	OgreGraphicsSystem::~OgreGraphicsSystem(void)
	{
		delete m_DebugTextBox;
	}

	void OgreGraphicsSystem::RegisterReflection()
	{
		SystemFactory::GetPtr()->Register("OgreGraphicsSystem",new GASS::Creator<OgreGraphicsSystem, ISystem>);
		RegisterProperty<std::string>( "Plugin", NULL, &GASS::OgreGraphicsSystem::AddPlugin);
		RegisterVectorProperty<std::string>("PostFilters", &GASS::OgreGraphicsSystem::GetPostFilters, &GASS::OgreGraphicsSystem::SetPostFilters);
		RegisterProperty<bool>("CreateMainWindowOnInit", &GASS::OgreGraphicsSystem::GetCreateMainWindowOnInit, &GASS::OgreGraphicsSystem::SetCreateMainWindowOnInit);
		RegisterProperty<bool>("UpdateMessagePump", &GASS::OgreGraphicsSystem::GetUpdateMessagePump, &GASS::OgreGraphicsSystem::SetUpdateMessagePump);
		RegisterProperty<bool>("ShowStats", &GASS::OgreGraphicsSystem::GetShowStats, &GASS::OgreGraphicsSystem::SetShowStats);
		RegisterProperty<bool>("UseShaderCache", &GASS::OgreGraphicsSystem::GetUseShaderCache, &GASS::OgreGraphicsSystem::SetUseShaderCache);

		//we need to register resource types here, if we wait to ::Init() ResourceManager is already initialized
		ResourceManagerPtr rm = SimEngine::Get().GetResourceManager();
		ResourceType mesh_type;
		mesh_type.Name = "MESH";
		mesh_type.Extensions.push_back("mesh");
		rm->RegisterResourceType(mesh_type);

		ResourceType texture_type;
		texture_type.Name = "TEXTURE";
		texture_type.Extensions.push_back("dds");
		texture_type.Extensions.push_back("png");
		texture_type.Extensions.push_back("bmp");
		texture_type.Extensions.push_back("tga");
		texture_type.Extensions.push_back("gif");
		texture_type.Extensions.push_back("jpg");
		rm->RegisterResourceType(texture_type);
	}

	void OgreGraphicsSystem::Init()
	{
		SimEngine::Get().GetRuntimeController()->Register(shared_from_this(),m_TaskNodeName);
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(OgreGraphicsSystem::OnViewportMovedOrResized,ViewportMovedOrResizedEvent,0));
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(OgreGraphicsSystem::OnDebugPrint,DebugPrintRequest,0));
		
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(OgreGraphicsSystem::OnInitializeTextBox,CreateTextBoxRequest ,0));

		
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(OgreGraphicsSystem::OnResourceGroupCreated,ResourceGroupCreatedEvent ,0));
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(OgreGraphicsSystem::OnResourceGroupRemoved,ResourceGroupRemovedEvent ,0));

		GetSimSystemManager()->RegisterForMessage(REG_TMESS(OgreGraphicsSystem::OnResourceLocationAdded,ResourceLocationAddedEvent,0));
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(OgreGraphicsSystem::OnResourceLocationRemoved,ResourceLocationRemovedEvent,0));

		
		
		const std::string log_folder = SimEngine::Get().GetLogFolder().GetFullPath();
		const std::string ogre_log = log_folder + "ogre.log";
		const std::string ogre_cfg = log_folder + "ogre.cfg";
		//Load plugins
		m_Root = new Ogre::Root("",ogre_cfg,ogre_log);

		//Ogre::LogManager::getSingleton().setLogDetail(Ogre::LL_LOW);

		#ifndef OGRE_18
			m_OverlaySystem = new Ogre::OverlaySystem();
		#endif



		for(int i = 0; i < m_Plugins.size(); i++)
		{
			std::string plugin_name = m_Plugins[i];
#ifdef _DEBUG
			plugin_name += "_d";
#endif
			m_Root->loadPlugin(plugin_name);
		}

		if(!m_Root->restoreConfig())
		{
			if (!m_Root->showConfigDialog())
				return;
			else
				m_Root->saveConfig();
		}

		/*if(m_CreateMainWindowOnInit)
		{
			std::string name = "MainWindow";
			Ogre::RenderWindow *window = m_Root->initialise(true,name);
			window->setDeactivateOnFocusChange(false);
			void* window_hnd = 0;
			window->getCustomAttribute("WINDOW", &window_hnd);
			void* handle = static_cast<void*>(window_hnd);
			void* main_handle = static_cast<void*>(window_hnd);
			m_Windows[name] = window;
			//We send a message when this window is cretated, usefull for other plugins to get hold of windows handle
			SystemMessagePtr window_msg(new MainWindowCreatedEvent(handle,main_handle));
			GetSimSystemManager()->SendImmediate(window_msg);
		}
		else
		{*/
			
		//}

		m_Root->initialise(false);

		m_SceneMgr = m_Root->createSceneManager(Ogre::ST_GENERIC);
		//wait that first render window is created before send message that graphic system is initialized

		if(m_UseShaderCache)
			Ogre::GpuProgramManager::getSingleton().setSaveMicrocodesToCache(true);

		// load
		
	
		

		m_ResourceManager->Init(); 

		
		
		Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_ANISOTROPIC);
		Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(7);

		

	

 
      
	}

	void OgreGraphicsSystem::OnDebugPrint(DebugPrintRequestPtr message)
	{
		std::string debug_text = message->GetText();
		m_DebugTextBox->Print(debug_text.c_str());
		m_DebugTextBox->SetActive(true);
	}

	void OgreGraphicsSystem::OnViewportMovedOrResized(ViewportMovedOrResizedEventPtr message)
	{
		std::map<std::string, OgreRenderWindowPtr>::iterator iter = m_Windows.begin();
		//resize all 
		while(iter != m_Windows.end())
		{
			iter->second->GetOgreWindow()->windowMovedOrResized();
			++iter;
		}
	}

	RenderWindowPtr OgreGraphicsSystem::GetMainRenderWindow() const
	{
		RenderWindowPtr main_win;
		if(m_Windows.size() > 0)
			main_win =  m_Windows.begin()->second;
		return main_win;
	}

	void OgreGraphicsSystem::Update(double delta_time)
	{
		if(m_UpdateMessagePump) //take care of window events?
			Ogre::WindowEventUtilities::messagePump();

		//if(m_DebugDrawer)
		//	m_DebugDrawer->build();

	

		m_Root->renderOneFrame();

		//if(m_DebugDrawer)
		//	m_DebugDrawer->clear();

		m_DebugTextBox->SetActive(true);
		m_DebugTextBox->UpdateTextBox();

		if(m_Windows.size() > 0 && m_ShowStats)
		{
			float a_fps = m_Windows.begin()->second->GetOgreWindow()->getAverageFPS(); 
			size_t tri_count = m_Windows.begin()->second->GetOgreWindow()->getTriangleCount(); 
			size_t batch_count = m_Windows.begin()->second->GetOgreWindow()->getBatchCount();

			std::stringstream sstream;
			sstream << "AVERAGE FPS:" << a_fps << "\n" <<
				"TRIANGLE COUNT: " << tri_count << "\n"
				"BATCH COUNT: " << batch_count << "\n";
			std::string stats_text = sstream.str();
			GetSimSystemManager()->SendImmediate(SystemMessagePtr( new DebugPrintRequest(stats_text)));
		}
		//update listeners
		SimSystem::Update(delta_time);
	}

	std::vector<std::string> OgreGraphicsSystem::GetPostFilters() const
	{
		return m_PostFilters;
	}

	void OgreGraphicsSystem::SetPostFilters(const std::vector<std::string> &filters)
	{
		m_PostFilters = filters;
	}

	RenderWindowPtr OgreGraphicsSystem::CreateRenderWindow(const std::string &name, int width, int height, void* external_window_handle)
	{
		Ogre::RenderWindow *window = NULL;
		if(m_Windows.find(name) != m_Windows.end())
			GASS_EXCEPT(Exception::ERR_DUPLICATE_ITEM,"Render window already exist:" + name, "OgreGraphicsSystem::CreateRenderWindow");

		if(external_window_handle)
		{
			Ogre::NameValuePairList miscParams;
			miscParams["externalWindowHandle"] = Ogre::StringConverter::toString((size_t)external_window_handle);
			window = Ogre::Root::getSingleton().createRenderWindow(name,width, height, false, &miscParams);
		}
		else
		{
			window = Ogre::Root::getSingleton().createRenderWindow(name,width, height, false);
			window->setDeactivateOnFocusChange(false);
		}
		
		OgreRenderWindowPtr win(new OgreRenderWindow(this,window));
		m_Windows[name] = win;
		void* window_hnd = 0;
		window->getCustomAttribute("WINDOW", &window_hnd);
		//We send a event when a render window is cretated, usefull for other plugins to get hold of window handles
		GetSimSystemManager()->SendImmediate(SystemMessagePtr(new RenderWindowCreatedEvent(window_hnd)));

		if(m_Windows.size() == 1) // this is our first window, send messages that graphic system is initlized
		{
			LogManager::getSingleton().stream() << "Initialise Ogre resource groups started";


			
			const std::string file = "shader_cache.bin";
			
			if(m_UseShaderCache)
			{
				std::ifstream test(file.c_str());
				if(!test.fail())
				{
					std::ifstream inp;
					inp.open(file.c_str(), std::ios::in | std::ios::binary);
					Ogre::DataStreamPtr shaderCache(OGRE_NEW Ogre::FileStreamDataStream(file, &inp, false));
					Ogre::GpuProgramManager::getSingleton().loadMicrocodeCache(shaderCache);
				}
			}

			Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

			if(m_UseShaderCache)
			{
				std::fstream output;
				output.open(file.c_str(), std::ios::out | std::ios::binary);
				Ogre::DataStreamPtr shaderCache (OGRE_NEW Ogre::FileStreamDataStream(file, &output, false));
				Ogre::GpuProgramManager::getSingleton().saveMicrocodeCache(shaderCache);
			}

			GetSimSystemManager()->SendImmediate(SystemMessagePtr(new GraphicsSystemLoadedEvent()));
		}
		return win;
	}


	std::vector<RenderWindowPtr> OgreGraphicsSystem::GetRenderWindows() const
	{
		std::vector<RenderWindowPtr> windows;
		std::map<std::string, OgreRenderWindowPtr>::const_iterator iter = m_Windows.begin();
		while(iter != m_Windows.end())
		{
			windows.push_back(iter->second);
			++iter;
		}
		return windows;
	}

	void OgreGraphicsSystem::OnInitializeTextBox(CreateTextBoxRequestPtr message)
	{
		/*Vec4 color = message->m_Color;
		Ogre::ColourValue ogre_color(color.x,color.y,color.z,color.w);

		if(!TextRenderer::getSingleton().hasTextBox(message->m_BoxID))
		TextRenderer::getSingleton().addTextBox(message->m_BoxID, message->m_Text, message->m_PosX, message->m_PosY, message->m_Width, message->m_Width, ogre_color);
		else
		TextRenderer::getSingleton().setText(message->m_BoxID,message->m_Text);*/
	}

	
	void OgreGraphicsSystem::OnResourceGroupCreated(ResourceGroupCreatedEventPtr message)
	{
		m_ResourceManager->AddResourceGroup(message->GetGroup(),true);
	}

	void OgreGraphicsSystem::OnResourceGroupRemoved(ResourceGroupRemovedEventPtr message)
	{
		m_ResourceManager->RemoveResourceGroup(message->GetGroup()->GetName());
	}


	void OgreGraphicsSystem::OnResourceLocationAdded(ResourceLocationAddedEventPtr message)
	{
		m_ResourceManager->AddResourceLocation(message->GetLocation());
	}

	void OgreGraphicsSystem::OnResourceLocationRemoved(ResourceLocationRemovedEventPtr message)
	{
		m_ResourceManager->RemoveResourceLocation(message->GetLocation());
	}


	std::vector<std::string> OgreGraphicsSystem::GetMaterialNames(std::string resource_group) const
	{
		std::vector<std::string> content;
		Ogre::MaterialManager::ResourceMapIterator iter = Ogre::MaterialManager::getSingleton().getResourceIterator();
		while(iter.hasMoreElements())
		{
			Ogre::MaterialPtr ptr = iter.getNext();
			if(resource_group == "" ||  ptr->getGroup() == resource_group)
			{
				content.push_back(ptr->getName());
			}
		}
		return content;
	}

	void OgreGraphicsSystem::ReloadMaterials()
	{
		// This is optional if you don't need textures to update
		Ogre::TextureManager::getSingletonPtr()->reloadAll();

		Ogre::GpuProgramManager* gpm = Ogre::GpuProgramManager::getSingletonPtr();
		Ogre::HighLevelGpuProgramManager* hlgpm = Ogre::HighLevelGpuProgramManager::getSingletonPtr();
		Ogre::MaterialManager* mm = Ogre::MaterialManager::getSingletonPtr();
		Ogre::ResourceGroupManager* rgm = Ogre::ResourceGroupManager::getSingletonPtr();

		// This is optional if you don't want to update low level GPU programs
		Ogre::ResourceManager::ResourceMapIterator i = gpm->getResourceIterator();
		while (i.hasMoreElements())
		{
			Ogre::ResourcePtr r = i.getNext();
			Ogre::String name = r->getName();
			Ogre::String origin = r->getOrigin();
			Ogre::String group = r->getGroup();
			Ogre::GpuProgramType type = ((Ogre::GpuProgramPtr)r)->getType();
			Ogre::String syntax = ((Ogre::GpuProgramPtr)r)->getSyntaxCode();

			if (!origin.empty())
			{
				gpm->remove(r);
				gpm->load(name, group, origin, type, syntax);
			}
		}

		// This is optional if you don't want to update high level GPU programs (most shaders)
		i = hlgpm->getResourceIterator();
		while (i.hasMoreElements())
		{
			Ogre::ResourcePtr r = i.getNext();
			Ogre::String origin = r->getOrigin();
			Ogre::String group = r->getGroup();

			if (!origin.empty())
			{
				hlgpm->remove(r);
				hlgpm->parseScript(rgm->openResource(origin, group), group);
				hlgpm->load(r->getName(), group);
			}
		}

		// This is where we reload the actual material scripts
		i = mm->getResourceIterator();
		while (i.hasMoreElements())
		{
			Ogre::ResourcePtr r = i.getNext();
			Ogre::String origin = r->getOrigin();
			Ogre::String group = r->getGroup();

			if (!origin.empty())
			{
				mm->remove(r);
				mm->parseScript(rgm->openResource(origin, group), group);
				mm->load(r->getName(), group);
			}
		}

		// Here, you reset the materials on all entities in the scene
		Ogre::SceneManager* sm = Ogre::Root::getSingleton().getSceneManagerIterator().getNext();
		Ogre::SceneManager::MovableObjectIterator j = sm->getMovableObjectIterator("Entity");
		while (j.hasMoreElements())
		{
			Ogre::Entity* e = (Ogre::Entity*)j.getNext();
			for (int k = 0; k < e->getNumSubEntities(); k++)
			{
				Ogre::SubEntity* se = e->getSubEntity(k);
				se->setMaterial(se->getMaterial());
			}
		}

		// Here, you reset the materials on all particle systems in the scene
		j = sm->getMovableObjectIterator("ParticleSystem");
		while (j.hasMoreElements())
		{
			Ogre::ParticleSystem* ps = (Ogre::ParticleSystem*)j.getNext();
			ps->setMaterialName(ps->getMaterialName());
		}
		// Do this for the rest of the objects in your scene you want to update materials for
	}

}





