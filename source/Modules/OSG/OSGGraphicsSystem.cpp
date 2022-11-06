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



#include <memory>

#include "Core/Common.h"
#include "Sim/GASS.h"
#include "Modules/OSG/OSGImGuiHandler.h"
#include "Modules/OSG/OSGGraphicsSystem.h"
#include "Modules/OSG/OSGRenderWindow.h"
#include "Modules/OSG/Utils/TextBox.h"
#include "Modules/OSG/Components/OSGCameraComponent.h"
#include "Modules/OSG/OSGConvert.h"
#include "Modules/OSG/OSGMaterial.h"
#include "Modules/OSG/OSGSimpleGraphicsSceneManager.h"

#include "Core/Utils/GASSFileUtils.h"
#include "Core/Serialize/tinyxml2.h"
#include <osgDB/WriteFile>

namespace GASS
{
	OSGGraphicsSystem::OSGGraphicsSystem(SimSystemManagerWeakPtr manager) : Reflection(manager),
		m_DebugTextBox(new TextBox()),
		m_Viewer(nullptr),
		m_FlipDDS(false)
	{
		m_UpdateGroup = UGID_POST_SIM;
	}

	OSGGraphicsSystem::~OSGGraphicsSystem(void)
	{
		if (m_Viewer)
		{
			delete m_DebugTextBox;
			//osgDB::Registry::instance()->closeAllLibraries();
			m_Viewer->setDone(true);
			//Sleep(1000);
			//m_Viewer->stopThreading();
			//Sleep(1000);
		}
		delete m_Viewer;
	}

	void OSGGraphicsSystem::RegisterReflection()
	{
		SystemFactory::GetPtr()->Register<OSGGraphicsSystem>("OSGGraphicsSystem");
		RegisterMember("FlipDDS", &GASS::OSGGraphicsSystem::m_FlipDDS);
		RegisterMember("UseLogHandler", &GASS::OSGGraphicsSystem::m_UseLogHandler);
		ResourceManagerPtr rm = SimEngine::Get().GetResourceManager();
		ResourceType mesh_type;
		mesh_type.Name = "MESH";
		mesh_type.Extensions.emplace_back("3ds");
		mesh_type.Extensions.emplace_back("obj");
		mesh_type.Extensions.emplace_back("flt");
		mesh_type.Extensions.emplace_back("ive");
		mesh_type.Extensions.emplace_back("osg");
		mesh_type.Extensions.emplace_back("osgt");
		rm->RegisterResourceType(mesh_type);

		ResourceType texture_type;
		texture_type.Name = "TEXTURE";
		texture_type.Extensions.emplace_back("dds");
		texture_type.Extensions.emplace_back("png");
		texture_type.Extensions.emplace_back("bmp");
		texture_type.Extensions.emplace_back("tga");
		texture_type.Extensions.emplace_back("gif");
		texture_type.Extensions.emplace_back("jpg");
		rm->RegisterResourceType(texture_type);

		//add osgEarth files!
		ResourceType map_type;
		map_type.Name = "MAP";
		map_type.Extensions.emplace_back("earth");
		rm->RegisterResourceType(map_type);
	}

	struct MultiRealizeOperation : public osg::Operation
	{
		void operator()(osg::Object* obj) override
		{
			for (auto& op : _ops)
				op->operator()(obj);
		}
		std::vector<osg::ref_ptr<osg::Operation>> _ops;
	};

	struct GL3RealizeOperation : public osg::Operation
	{
		void operator()(osg::Object* object) override
		{
			osg::GraphicsContext* gc = dynamic_cast<osg::GraphicsContext*>(object);
			if (gc)
			{
				osg::State* state = gc->getState();

				// force NVIDIA-style vertex attribute aliasing, since osgEarth
				// makes use of some specific attribute registers. Later we can
				// perhaps create a reservation system for this.
				state->resetVertexAttributeAlias(false);

				// We always want to use osg modelview and projection uniforms and vertex attribute aliasing.    
				// Since we use modern opengl throughout even if OSG isn't explicitly built with GL3.
				state->setUseModelViewAndProjectionUniforms(true);
				state->setUseVertexAttributeAliasing(true);

#ifndef OSG_GL_FIXED_FUNCTION_AVAILABLE
				state->setModeValidity(GL_LIGHTING, false);
				state->setModeValidity(GL_NORMALIZE, false);
				state->setModeValidity(GL_RESCALE_NORMAL, false);
				state->setModeValidity(GL_LINE_STIPPLE, false);
				state->setModeValidity(GL_LINE_SMOOTH, false);
#endif
			}
		}
	};

	class OSGNotifyHandler : public osg::NotifyHandler
	{
	public:
		void notify(osg::NotifySeverity severity, const char* message) override
		{
			if (message == nullptr)
				return;
			if (*message == '\0')
				return;
			switch (severity)
			{
				case osg::NotifySeverity::ALWAYS:
					GASS_LOG(LINFO) << " OSG-ALWAYS:" << message;
				break;
				case osg::NotifySeverity::FATAL:
					GASS_LOG(LERROR) << " OSG-FATAL:" << message;
					break;
				case osg::NotifySeverity::WARN:
					GASS_LOG(LWARNING) << " OSG-WARN:" << message;
					break;
				case osg::NotifySeverity::NOTICE:
					GASS_LOG(LINFO) << " OSG-NOTICE:" << message;
					break;
				case osg::NotifySeverity::INFO:
					GASS_LOG(LINFO) << " OSG-INFO:" << message;
					break;
				case osg::NotifySeverity::DEBUG_INFO:
					GASS_LOG(LINFO) << " OSG-DEBUG_INFO:" << message;
					break;
				case osg::NotifySeverity::DEBUG_FP:
					GASS_LOG(LINFO) << " OSG-DEBUG_FP:" << message;
					break;
			}
		}
	};

	void OSGGraphicsSystem::RegisterSceneManager()
	{
		SceneManagerFactory::GetPtr()->Register<OSGSimpleGraphicsSceneManager>("OSGSimpleGraphicsSceneManager");
	}

	void OSGGraphicsSystem::OnSystemInit()
	{
		RegisterSceneManager();
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(OSGGraphicsSystem::OnViewportMovedOrResized, ViewportMovedOrResizedEvent, 0));
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(OSGGraphicsSystem::OnInitializeTextBox, CreateTextBoxRequest, 0));

		if(m_UseLogHandler)
			osg::setNotifyHandler(new OSGNotifyHandler());

#ifdef WIN32
		osg::DisplaySettings::instance()->setNumMultiSamples(4);
#endif
		osg::DisplaySettings::instance()->setVertexBufferHint(osg::DisplaySettings::VERTEX_BUFFER_OBJECT);

		m_Viewer = new osgViewer::CompositeViewer();
		m_Viewer->setThreadingModel(osgViewer::Viewer::SingleThreaded);
		m_Viewer->setKeyEventSetsDone(0);
		//m_Viewer->setReleaseContextAtEndOfFrameHint(false);
		MultiRealizeOperation* op = new MultiRealizeOperation();

		if (m_Viewer->getRealizeOperation())
			op->_ops.push_back(m_Viewer->getRealizeOperation());
		GL3RealizeOperation* rop = new GL3RealizeOperation();

		op->_ops.push_back(rop);
		op->_ops.push_back(new OSGImGuiHandler::GlewInitOperation);
		m_Viewer->setRealizeOperation(op);

		std::string full_path;

		ResourceManagerPtr rm = SimEngine::Get().GetResourceManager();
		FileResourcePtr font_res = rm->GetFirstResourceByName("arial.ttf");
		m_DebugTextBox->setPosition(osg::Vec3d(0, 100, 0));
		m_DebugTextBox->setFont(font_res->Path().GetFullPath());
		m_DebugTextBox->setTextSize(10);

		osgDB::ReaderWriter::Options* opt = osgDB::Registry::instance()->getOptions();
		if (opt == nullptr)
		{
			opt = new osgDB::ReaderWriter::Options();
		}

		if (m_FlipDDS)
		{
			const std::string options = opt->getOptionString();
			opt->setOptionString("dds_flip");
			osgDB::Registry::instance()->setOptions(opt);
		}
		osgDB::Registry::instance()->setBuildKdTreesHint(osgDB::Options::BUILD_KDTREES);

		UnlitMaterialConfig mat_trans({1,1,1,1});
		mat_trans.AlphaBlend = true;
		AddMaterial(&mat_trans, "WhiteTransparentNoLighting");
		
		UnlitMaterialConfig mat_opaque({ 1,1,1,1 });
		AddMaterial(&mat_opaque, "WhiteNoLighting");
		
		if (SimEngine::Get().GetResourceManager()->HasResourceGroup("MATERIALS"))
		{
			auto mrg = SimEngine::Get().GetResourceManager()->GetFirstResourceGroupByName("MATERIALS");
			auto locations = mrg->GetResourceLocations();
			for (auto loc : locations)
			{
				auto res_vec = loc->GetResources();
				for (auto res : res_vec)
				{
					auto name = (res.second)->Name();
					if (name.find(".osg") != std::string::npos)
					{
						auto path = (res.second)->Path().GetFullPath();
						auto node = osgDB::readNodeFile(path);
						auto state_set = node->getStateSet();
						if (state_set)
							m_Materials[name] = state_set;
					}
				}
			}
		}
		GetSimSystemManager()->SendImmediate(SystemMessagePtr(new GraphicsSystemLoadedEvent()));
	}


	RenderWindowVector OSGGraphicsSystem::GetRenderWindows() const
	{
		RenderWindowVector ret;
		for (size_t i = 0; i < m_Windows.size(); i++)
		{
			ret.push_back(m_Windows[i]);
		}
		return ret;
	}

	void OSGGraphicsSystem::PrintDebugText(const std::string& message)
	{
		m_DebugVec.push_back(message);
	}

	RenderWindowPtr OSGGraphicsSystem::GetMainRenderWindow() const
	{
		RenderWindowPtr main_win;
		if (m_Windows.size() > 0)
			main_win = m_Windows[0];
		return main_win;
	}


	RenderWindowPtr OSGGraphicsSystem::CreateRenderWindow(const std::string& name, int width, int height, void* external_handle)
	{

		osg::GraphicsContext::WindowingSystemInterface* wsi = osg::GraphicsContext::getWindowingSystemInterface();
		if (!wsi)
		{

			GASS_EXCEPT(Exception::ERR_INTERNAL_ERROR, "Failed to get WindowingSystemInterface", "OSGGraphicsSystem::CreateRenderWindow");
		}

		osg::DisplaySettings* ds = osg::DisplaySettings::instance().get();

		osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits(ds);

		traits->readDISPLAY();
		traits->screenNum = 0;
		traits->x = 50;
		traits->y = 50;
		traits->width = width;
		traits->height = height;
		traits->doubleBuffer = true;
		traits->sharedContext = nullptr;

		if (m_Windows.size() > 0)
		{
			traits->sharedContext = m_Windows[0]->GetOSGWindow();
		}

		if (external_handle) //external window
		{
			osg::ref_ptr<osg::Referenced> windata = new WindowData((WindowHandle)external_handle);
			traits->windowDecoration = false;
			traits->setInheritedWindowPixelFormat = true;
			traits->inheritedWindowData = windata;
			//traits->useCursor = false;
		}
		else
		{
			traits->windowDecoration = true;
			traits->windowName = name;
		}

		osg::ref_ptr<osg::GraphicsContext> graphics_context = osg::GraphicsContext::createGraphicsContext(traits.get());
		if (graphics_context.valid())
		{
			//osg::notify(osg::INFO)<<"  GraphicsWindow has been created successfully."<<std::endl;
			//need to ensure that the window is cleared make sure that the complete window is set the correct colour
			//rather than just the parts of the window that are under the camera's viewports
			graphics_context->setClearColor(osg::Vec4f(0.0f, 0.0f, 1.0f, 1.0f));
			graphics_context->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			auto* gw = dynamic_cast<osgViewer::GraphicsWindow*>(graphics_context.get());
			gw->getEventQueue()->getCurrentEventState()->setWindowRectangle(0, 0, width, height);
		}
		else
		{
			GASS_EXCEPT(Exception::ERR_INTERNAL_ERROR, "Failed to create createGraphicsContext for:" + name, "OSGGraphicsSystem::CreateRenderWindow");
		}


		OSGRenderWindowPtr win(new  OSGRenderWindow(this, graphics_context));
		m_Windows.push_back(win);
		return win;
	}

	void OSGGraphicsSystem::OnViewportMovedOrResized(ViewportMovedOrResizedEventPtr message)
	{

		osgViewer::ViewerBase::Views views;
		m_Viewer->getViews(views);
		//set same size in all viewports for the moment
		for (size_t i = 0; i < views.size(); i++)
		{
			if (views[i]->getName() == message->GetViewportName())
			{
				views[i]->getCamera()->setViewport(message->GetPositionX(), message->GetPositionY(), message->GetWidth(), message->GetHeight());
			}
		}
	}

	void OSGGraphicsSystem::OnSystemUpdate(double delta_time)
	{
		//static int tick = 0;
		GetSimSystemManager()->SendImmediate(std::make_shared<PreGraphicsSystemUpdateEvent>(delta_time));
		if (m_Viewer->done())
		{
			return;
		}
		//m_Viewer->setRunMaxFrameRate(100);
		std::string text;
		for (size_t i = 0; i < m_DebugVec.size(); i++)
		{
			text = text + m_DebugVec[i] + "\n";
		}
		m_DebugTextBox->setText(text);
		m_Viewer->frame(SimEngine::Get().GetSimulationTime());
		m_DebugVec.clear();
		//m_DebugTextBox->setT	ext("");
		//update listeners
		//SimSystem::_UpdateListeners(delta_time);
		GetSimSystemManager()->SendImmediate(std::make_shared<PostGraphicsSystemUpdateEvent>(delta_time));
	}

	void OSGGraphicsSystem::OnInitializeTextBox(CreateTextBoxRequestPtr message)
	{
		if (m_TextBoxes.end() == m_TextBoxes.find(message->m_BoxID))
		{
			auto* text_box = new TextBox();

			std::string full_path;
			ResourceManagerPtr rm = SimEngine::Get().GetResourceManager();

			//make this optional?
			FileResourcePtr font_res = rm->GetFirstResourceByName("arial.ttf");

			text_box->setPosition(osg::Vec3d(0, 0, 0));
			text_box->setFont(font_res->Path().GetFullPath());
			text_box->setTextSize(10);

			m_Viewer->getView(0)->getSceneData()->asGroup()->addChild(text_box->getGroup());

			m_TextBoxes[message->m_BoxID] = text_box;
		}

		TextBox* text_box = m_TextBoxes[message->m_BoxID];

		text_box->setText(message->m_Text);
		text_box->setPosition(osg::Vec3d(message->m_PosX, message->m_PosY, 0));
		Vec4 color = message->m_Color;
		text_box->setColor(osg::Vec4(static_cast<float>(color.x), static_cast<float>(color.y), static_cast<float>(color.z), static_cast<float>(color.w)));
	}

	std::vector<std::string> OSGGraphicsSystem::GetMaterialNames(std::string resource_group) const
	{
		std::vector<std::string> content;

		ResourceManagerPtr rm = SimEngine::Get().GetResourceManager();
		ResourceGroupVector groups = rm->GetResourceGroups();
		for (size_t i = 0; i < groups.size(); i++)
		{
			ResourceGroupPtr group = groups[i];
			if (group->GetName() == resource_group)
			{
				ResourceLocationVector locations = group->GetResourceLocations();
				for (size_t j = 0; j < locations.size(); j++)
				{
					ResourceLocation::ResourceMap resources = locations[j]->GetResources();
					ResourceLocation::ResourceMap::const_iterator iter = resources.begin();
					while (iter != resources.end())
					{
						std::string mat_name = iter->second->Name();
						content.push_back(mat_name);
						++iter;
					}
				}
			}
		}
		return content;
	}

	void OSGGraphicsSystem::AddMaterial(IGfxMaterialConfig* config, const std::string &name)
	{
		if(auto material = Material::CreateFromConfig(config))
			m_Materials[name] = material;
	}

	bool OSGGraphicsSystem::HasMaterial(const std::string& mat_name) const
	{
		if (m_Materials.find(mat_name) == m_Materials.end())
			return false;
		return true;
	}
	void OSGGraphicsSystem::RemoveMaterial(const std::string& mat_name)
	{
		auto iter = m_Materials.find(mat_name);
		if(iter != m_Materials.end())
			m_Materials.erase(iter);
	}

	osg::ref_ptr<osg::StateSet> OSGGraphicsSystem::GetStateSet(const std::string& material_name)
	{
		return m_Materials[material_name];
	}
}
