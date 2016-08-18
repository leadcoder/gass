/****************************************************************************
* This file is part of GASS.                                                *
* See https://github.com/leadcoder/gass                                     *
*                                                                           *
* Copyright (c) 2008-2016 GASS team. See Contributors.txt for details.      *
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
#include "Plugins/Ogre/GASSOgreRenderWindow.h"
#include "Plugins/Ogre/GASSOgreViewport.h"
#include "Plugins/Ogre/OgreDebugTextOutput.h"
#include "Plugins/Ogre/OgrePostProcess.h"
#include "Plugins/Ogre/OgreResourceManager.h"
#include "Plugins/Ogre/Helpers/DebugDrawer.h"
#include "Plugins/Ogre/OgreConvert.h"
#include "Plugins/Ogre/Helpers/OgreText.h"
#include "Plugins/Ogre/Helpers/ResourceGroupHelper.h"

#include "Core/PluginSystem/GASSDynamicModule.h"
#include "Core/Utils/GASSException.h"
#include "Sim/GASSSystemFactory.h"
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
#include <Overlay/OgreOverlaySystem.h>

ResourceGroupHelper resourceGrouphelper;

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
		m_UpdateGroup = UGID_POST_SIM;
		m_Plugins.push_back("RenderSystem_Direct3D9");
		m_Plugins.push_back("Plugin_OctreeSceneManager");
		m_Plugins.push_back("Plugin_ParticleFX");
	}

	OgreGraphicsSystem::~OgreGraphicsSystem(void)
	{
		delete m_DebugTextBox;
	}

	void OgreGraphicsSystem::RegisterReflection()
	{
		SystemFactory::GetPtr()->Register("OgreGraphicsSystem",new GASS::Creator<OgreGraphicsSystem, SimSystem>);
		RegisterProperty<PluginVector>("Plugins", &GASS::OgreGraphicsSystem::GetPlugins, &GASS::OgreGraphicsSystem::SetPlugins);
		RegisterProperty<PostFilterVector>("PostFilters", &GASS::OgreGraphicsSystem::GetPostFilters, &GASS::OgreGraphicsSystem::SetPostFilters);
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
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(OgreGraphicsSystem::OnViewportMovedOrResized,ViewportMovedOrResizedEvent,0));
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(OgreGraphicsSystem::OnDebugPrint,DebugPrintRequest,0));
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(OgreGraphicsSystem::OnInitializeTextBox,CreateTextBoxRequest ,0));
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(OgreGraphicsSystem::OnResourceGroupCreated,ResourceGroupCreatedEvent ,0));
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(OgreGraphicsSystem::OnResourceGroupRemoved,ResourceGroupRemovedEvent ,0));
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(OgreGraphicsSystem::OnResourceLocationAdded,ResourceLocationAddedEvent,0));
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(OgreGraphicsSystem::OnResourceLocationRemoved,ResourceLocationRemovedEvent,0));
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(OgreGraphicsSystem::OnReloadMaterial,ReloadMaterial,0));

		const std::string log_folder = SimEngine::Get().GetLogFolder().GetFullPath();
		const std::string ogre_log = log_folder + "ogre.log";
		const std::string ogre_cfg = log_folder + "ogre.cfg";
		//Load plugins
		m_Root = new Ogre::Root("",ogre_cfg,ogre_log);

		//Ogre::LogManager::getSingleton().setLogDetail(Ogre::LL_LOW);

		m_OverlaySystem = new Ogre::OverlaySystem();

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

		m_Root->initialise(false);

		m_SceneMgr = m_Root->createSceneManager(Ogre::ST_GENERIC);

		m_ResourceManager->Init();

		//TODO: Add attributes for this settings
		Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_ANISOTROPIC);
		Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(7);
		LogManager::getSingleton().stream() << "Ogre initialized with:" << Ogre::Root::getSingleton().getRenderSystem()->getName();
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

	void OgreGraphicsSystem::Update(double delta_time, TaskNode* caller)
	{
		GetSimSystemManager()->SendImmediate(PreGraphicsSystemUpdateEventPtr(new PreGraphicsSystemUpdateEvent(delta_time)));

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
		SimSystem::Update(delta_time,caller);

		GetSimSystemManager()->SendImmediate(PostGraphicsSystemUpdateEventPtr(new PostGraphicsSystemUpdateEvent(delta_time)));
	}

	PostFilterVector OgreGraphicsSystem::GetPostFilters() const
	{
		return m_PostFilters;
	}

	void OgreGraphicsSystem::SetPostFilters(const PostFilterVector &filters)
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
		//We send a event when a render window is created, useful for other plugins to get hold of window handles
		GetSimSystemManager()->SendImmediate(SystemMessagePtr(new RenderWindowCreatedEvent(window_hnd)));

		if(m_Windows.size() == 1) // this is our first window, send messages that graphic system is initialized
		{
			LogManager::getSingleton().stream() << "Initialise Ogre resource groups started";

			const std::string file = "gass_shader_cache.bin";
			Ogre::GpuProgramManager::getSingleton().setSaveMicrocodesToCache(m_UseShaderCache);

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
			Ogre::MaterialPtr ptr = iter.getNext().staticCast<Ogre::Material>();
			if(resource_group == "" ||  ptr->getGroup() == resource_group)
			{
				content.push_back(ptr->getName());
			}
		}
		return content;
	}

	void OgreGraphicsSystem::OnReloadMaterial(ReloadMaterialPtr message)
	{
		ReloadResources();
	}

	void OgreGraphicsSystem::ReloadResources()
	{
		std::string errorMessages;
		Ogre::ResourceGroupManager& rgm = Ogre::ResourceGroupManager::getSingleton();
		Ogre::StringVector all_resource_groups = rgm.getResourceGroups();
		Ogre::StringVector::iterator iter = all_resource_groups.begin();
		Ogre::StringVector::iterator iter_end = all_resource_groups.end();
		for(;iter!=iter_end;iter++)
		{
			resourceGrouphelper.checkTimeAndReloadIfNeeded((*iter),errorMessages,false);
		}
		//resourceGrouphelper.checkTimeAndReloadIfNeeded("GASS",errorMessages,false);
		//ReloadMaterials();
	}

	bool OgreGraphicsSystem::HasMaterial(const std::string &mat_name) const
	{
		return Ogre::MaterialManager::getSingleton().resourceExists(mat_name);
	}

	void OgreGraphicsSystem::AddMaterial(const GraphicsMaterial &material,const std::string &base_mat_name)
	{
		Ogre::MaterialPtr ogre_mat;
		const std::string mat_name = material.Name;
		if(base_mat_name != "")
		{
			Ogre::MaterialPtr base_mat = Ogre::MaterialManager::getSingleton().getByName( base_mat_name);
			if(base_mat.isNull())
			{
				GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Base material not found:" + base_mat_name, "OgreGraphicsSystem::AddMaterial");
			}
			ogre_mat = base_mat->clone(mat_name);
		}
		else
		{
			Ogre::MaterialPtr base_mat = Ogre::MaterialManager::getSingleton().getByName( "GASS_ColorOnly");
			ogre_mat = base_mat->clone(mat_name);
		}
		SetOgreMaterial(material, ogre_mat);
	}


	void OgreGraphicsSystem::RemoveMaterial(const std::string &mat_name)
	{
		if(mat_name != "")
		{
			Ogre::MaterialManager::getSingleton().remove(mat_name);
		}
	}

	GraphicsMaterial OgreGraphicsSystem::GetMaterial(const std::string &mat_name)
	{
		GraphicsMaterial ret;
		Ogre::MaterialPtr ogre_mat = Ogre::MaterialManager::getSingleton().getByName( mat_name);
		SetGASSMaterial(ogre_mat, ret);
		return ret;
	}

	void OgreGraphicsSystem::SetOgreMaterial(const GraphicsMaterial &material, Ogre::MaterialPtr mat)
	{
		ColorRGBA diffuse = material.Diffuse;
		ColorRGB ambient = material.Ambient;
		ColorRGB specular = material.Specular;
		ColorRGB si = material.SelfIllumination;
		mat->setDiffuse(OgreConvert::ToOgre(diffuse));
		mat->setAmbient(OgreConvert::ToOgre(ambient));
		mat->setSpecular(OgreConvert::ToOgre(specular));
		mat->setSelfIllumination(OgreConvert::ToOgre(si));
		mat->setShininess(material.Shininess);
		mat->setDepthCheckEnabled(material.DepthTest);
		mat->setDepthWriteEnabled(material.DepthWrite);
	}

	void OgreGraphicsSystem::SetGASSMaterial(Ogre::MaterialPtr mat , GraphicsMaterial &material)
	{
		//pick first tech and last pass
		if (!mat.isNull())
		{
			material.Name = mat->getName();
			Ogre::Technique* tech = mat->getBestTechnique();
			//Get last pass and save materials
			if(tech && tech->getNumPasses() > 0)
			{
				Ogre::Pass*  pass = tech->getPass(tech->getNumPasses()-1);

				Ogre::ColourValue ambient =  pass->getAmbient();
				const Ogre::ColourValue diffuse = pass->getDiffuse();
				const Ogre::ColourValue specular = pass->getSpecular();
				const Ogre::ColourValue selfIllumination = pass->getSelfIllumination();
				const Ogre::ColourValue emissive = pass->getEmissive();

				material.Ambient = ColorRGB(ambient.r,ambient.g,ambient.b);
				material.Diffuse = ColorRGBA(diffuse.r,diffuse.g,diffuse.b,diffuse.a);
				material.Specular = ColorRGB(specular.r,specular.g,specular.b);

				for(unsigned int j = 0 ; j < pass->getNumTextureUnitStates(); j++)
				{
					Ogre::TextureUnitState * textureUnit = pass->getTextureUnitState(static_cast<unsigned short>(j));
					std::string texture_name = textureUnit->getTextureName();
					if(texture_name != "")
						material.Textures.push_back(texture_name);
				}
			}
		}
	}
}
