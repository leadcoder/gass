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
#include "Plugins/OSG/OSGImGuiHandler.h"
#include "Plugins/OSG/OSGGraphicsSystem.h"
#include "Plugins/OSG/OSGRenderWindow.h"
#include "Plugins/OSG/Utils/TextBox.h"
#include "Plugins/OSG/Components/OSGCameraComponent.h"
#include "Plugins/OSG/IOSGCameraManipulator.h"
#include "Plugins/OSG/OSGConvert.h"

#include "Core/Utils/GASSFileUtils.h"
#include "Core/Serialize/tinyxml2.h"

namespace GASS
{

	OSGGraphicsSystem::OSGGraphicsSystem(SimSystemManagerWeakPtr manager) : Reflection(manager), 
		m_DebugTextBox(new TextBox()),
		m_Viewer(nullptr),
		m_FlipDDS(false)
	{
		m_UpdateGroup=UGID_POST_SIM;
	}

	OSGGraphicsSystem::~OSGGraphicsSystem(void)
	{
		if(m_Viewer)
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

	void OSGGraphicsSystem::OnSystemInit()
	{
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(OSGGraphicsSystem::OnViewportMovedOrResized,ViewportMovedOrResizedEvent,0));
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(OSGGraphicsSystem::OnInitializeTextBox,CreateTextBoxRequest ,0));

#ifdef WIN32
		osg::DisplaySettings::instance()->setNumMultiSamples(4);
#endif

		m_Viewer = new osgViewer::CompositeViewer();
		m_Viewer->setThreadingModel( osgViewer::Viewer::SingleThreaded);
		m_Viewer->setKeyEventSetsDone(0);
		//m_Viewer->setReleaseContextAtEndOfFrameHint(false);

		m_Viewer->setRealizeOperation(new OSGImGuiHandler::GlewInitOperation);

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

		if(m_FlipDDS)
		{
			const std::string options = opt->getOptionString();
			opt->setOptionString("dds_flip");
			osgDB::Registry::instance()->setOptions(opt);
		}
		osgDB::Registry::instance()->setBuildKdTreesHint(osgDB::Options::BUILD_KDTREES);

		//add default material
		GraphicsMaterial mat_trans;
		mat_trans.Name = "WhiteTransparentNoLighting";
		mat_trans.DepthTest = true;
		mat_trans.DepthWrite = true;
		mat_trans.TrackVertexColor = true;
		mat_trans.Transparent = 11;
		AddMaterial(mat_trans);
		
		GraphicsMaterial mat_opaque;
		mat_opaque.Name = "WhiteNoLighting";
		mat_opaque.DepthTest = true;
		mat_opaque.DepthWrite = true;
		mat_opaque.TrackVertexColor = true;
		mat_opaque.Transparent = 0;
		AddMaterial(mat_opaque);

		GetSimSystemManager()->SendImmediate(SystemMessagePtr(new GraphicsSystemLoadedEvent()));
	}


	RenderWindowVector OSGGraphicsSystem::GetRenderWindows() const
	{
		RenderWindowVector ret;
		for(size_t i = 0; i < m_Windows.size();i++)
		{
			ret.push_back(m_Windows[i]);
		}
		return ret;
	}

	void OSGGraphicsSystem::PrintDebugText(const std::string &message)
	{
		m_DebugVec.push_back(message);
	}

	RenderWindowPtr OSGGraphicsSystem::GetMainRenderWindow() const
	{
		RenderWindowPtr main_win;
		if(m_Windows.size() > 0)
			main_win =  m_Windows[0];
		return main_win;
	}


	RenderWindowPtr OSGGraphicsSystem::CreateRenderWindow(const std::string &name, int width, int height, void* external_handle)
	{

		osg::GraphicsContext::WindowingSystemInterface* wsi = osg::GraphicsContext::getWindowingSystemInterface();
		if (!wsi)
	    {

			GASS_EXCEPT(Exception::ERR_INTERNAL_ERROR,"Failed to get WindowingSystemInterface", "OSGGraphicsSystem::CreateRenderWindow");
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
		
		if(m_Windows.size() > 0)
		{
			traits->sharedContext = m_Windows[0]->GetOSGWindow();
		}

		if(external_handle) //external window
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
			graphics_context->setClearColor(osg::Vec4f(0.0f,0.0f,1.0f,1.0f));
			graphics_context->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			auto* gw = dynamic_cast<osgViewer::GraphicsWindow*>(graphics_context.get());
			gw->getEventQueue()->getCurrentEventState()->setWindowRectangle(0, 0, width, height );
			graphics_context->getState()->setUseModelViewAndProjectionUniforms(true);
		}
		else
		{
			GASS_EXCEPT(Exception::ERR_INTERNAL_ERROR,"Failed to create createGraphicsContext for:" + name, "OSGGraphicsSystem::CreateRenderWindow");
		}

		
		OSGRenderWindowPtr win(new  OSGRenderWindow(this,graphics_context));
		m_Windows.push_back(win);
		return win;
	}

	void OSGGraphicsSystem::OnViewportMovedOrResized(ViewportMovedOrResizedEventPtr message)
	{

		osgViewer::ViewerBase::Views views;
		m_Viewer->getViews(views);
		//set same size in all viewports for the moment
		for(size_t i = 0; i < views.size(); i++)
		{
			if(views[i]->getName() == message->GetViewportName())
			{
				views[i]->getCamera()->setViewport(message->GetPositionX(),message->GetPositionY(),message->GetWidth(), message->GetHeight());
			}
		}
	}

	void OSGGraphicsSystem::OnSystemUpdate(double delta_time)
	{
		//static int tick = 0;
		GetSimSystemManager()->SendImmediate(std::make_shared<PreGraphicsSystemUpdateEvent>(delta_time));
		if(m_Viewer->done())
		{
			return;
		}
		//m_Viewer->setRunMaxFrameRate(100);
		std::string text;
		for (size_t i = 0; i < m_DebugVec.size(); i++)
		{
			text = text + m_DebugVec[i]+ "\n";
		}
		m_DebugTextBox->setText(text);
		m_Viewer->frame(SimEngine::Get().GetSimulationTime());
		m_DebugVec.clear();
		//m_DebugTextBox->setText("");
		//update listeners
		//SimSystem::_UpdateListeners(delta_time);
		GetSimSystemManager()->SendImmediate(std::make_shared<PostGraphicsSystemUpdateEvent>(delta_time));
	}

	void OSGGraphicsSystem::OnInitializeTextBox(CreateTextBoxRequestPtr message)
	{
		if(m_TextBoxes.end() == m_TextBoxes.find(message->m_BoxID))
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

		ResourceManagerPtr rm = GASS::SimEngine::Get().GetResourceManager();
		ResourceGroupVector groups = rm->GetResourceGroups();
		for(size_t i = 0; i < groups.size();i++)
		{
			ResourceGroupPtr group = groups[i];
			if(group->GetName() == resource_group)
			{
				ResourceLocationVector locations = group->GetResourceLocations();
				for(size_t j = 0; j < locations.size(); j++)
				{
					ResourceLocation::ResourceMap resources = locations[j]->GetResources();
					ResourceLocation::ResourceMap::const_iterator iter = resources.begin();
					while(iter != resources.end())
					{
						//remove extension?
						std::string mat_name = FileUtils::RemoveExtension(iter->second->Name());
						content.push_back(mat_name);
						++iter;
					}
				}
			}
		}
		return content;
	}

	void OSGGraphicsSystem::AddMaterial(const GraphicsMaterial &material, const std::string &/*base_mat_name*/)
	{
		osg::ref_ptr<osg::StateSet> state_set = new osg::StateSet();
		SetOSGStateSet(material, state_set);
		m_Materials[material.Name] = state_set;
	}

	bool OSGGraphicsSystem::HasMaterial(const std::string &mat_name) const
	{
		if(m_Materials.find(mat_name) ==m_Materials.end())
			return false;
		return true;
	}
	void OSGGraphicsSystem::RemoveMaterial(const std::string &mat_name)
	{
		m_Materials.erase(m_Materials.find(mat_name));
	}

	GraphicsMaterial OSGGraphicsSystem::GetMaterial(const std::string &mat_name)
	{
		GraphicsMaterial ret;
		SetGASSMaterial(m_Materials[mat_name],ret);
		return ret;
	}

	void OSGGraphicsSystem::SetGASSMaterial(osg::ref_ptr<osg::StateSet> state_set, GraphicsMaterial &material)
	{
		auto* mat  = dynamic_cast<osg::Material*>(state_set->getAttribute(osg::StateAttribute::MATERIAL));
		const osg::Vec4 &ambient = mat->getAmbient(osg::Material::FRONT_AND_BACK);
		const osg::Vec4 &diffuse = mat->getDiffuse(osg::Material::FRONT_AND_BACK);
		const osg::Vec4 &specular = mat->getSpecular(osg::Material::FRONT_AND_BACK);
		const osg::Vec4 &si = mat->getEmission(osg::Material::FRONT_AND_BACK);
		material.Ambient.Set(ambient.x(),ambient.y(),ambient.z());
		material.Diffuse.Set(diffuse.x(),diffuse.y(),diffuse.z(),diffuse.w());
		material.Specular.Set(specular.x(),specular.y(),specular.z());
		material.SelfIllumination.Set(si.x(),si.y(),si.z());
		//TODO: copy textures!
	}

	osg::ref_ptr<osg::StateSet> OSGGraphicsSystem::GetStateSet(const std::string &material_name)
	{
		return m_Materials[material_name];
	}

	void OSGGraphicsSystem::SetOSGStateSet(const GraphicsMaterial &material, osg::ref_ptr<osg::StateSet> state_set)
	{
		ColorRGBA diffuse = material.Diffuse;
		ColorRGB ambient = material.Ambient;
		ColorRGB specular = material.Specular;
		ColorRGB si = material.SelfIllumination;

		if (material.TrackVertexColor) //Use vertex color
		{
			state_set->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
			if (material.Transparent > 0)
			{
				state_set->setAttributeAndModes(new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA));
				state_set->setRenderBinDetails(11, "RenderBin");
			}
		}
		else
		{
			osg::ref_ptr<osg::Material> mat (new osg::Material);
			mat->setDiffuse(osg::Material::FRONT_AND_BACK, OSGConvert::ToOSG(diffuse));
			mat->setAmbient(osg::Material::FRONT_AND_BACK,osg::Vec4(static_cast<float>(ambient.r), static_cast<float>(ambient.g), static_cast<float>(ambient.b),1));
			mat->setSpecular(osg::Material::FRONT_AND_BACK,osg::Vec4(static_cast<float>(specular.r), static_cast<float>(specular.g), static_cast<float>(specular.b),1));
			mat->setShininess(osg::Material::FRONT_AND_BACK,material.Shininess);
			mat->setEmission(osg::Material::FRONT_AND_BACK,osg::Vec4(static_cast<float>(si.r), static_cast<float>(si.g), static_cast<float>(si.b),1));
			state_set->setAttribute(mat);
			state_set->setAttributeAndModes(mat, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
			state_set->setMode(GL_LIGHTING, osg::StateAttribute::ON);
		}

		//mat->setColorMode(osg::Material::ColorMode::DIFFUSE); //Track vertex color

		if(material.DepthTest)
			state_set->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
		else
		{
			state_set->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED | osg::StateAttribute::OVERRIDE);
			state_set->setRenderBinDetails(INT_MAX, "RenderBin");
		}

		//disabling depth write will effect GL_DEPTH_TEST some how,
		//osg::ref_ptr<osg::Depth> depth (new osg::Depth(Depth::LEQUAL,0.0,1.0,material.DepthWrite));
		/*	osg::ref_ptr<osg::Depth> depth (new osg::Depth());
			depth->setWriteMask( material.DepthWrite );
			state_set->setAttributeAndModes( depth, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
		*/

		// Turn on blending
		if (!material.TrackVertexColor && diffuse.a < 1.0) //special handling if we have transparent a material,
		{
			//TODO: provide blending mode in material!
			osg::ref_ptr<osg::BlendFunc> bf (new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA,  osg::BlendFunc::ONE_MINUS_SRC_ALPHA ));
			state_set->setAttributeAndModes(bf);

			// Enable blending, select transparent bin.
			state_set->setMode( GL_BLEND, osg::StateAttribute::ON );
			state_set->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );

			// Enable depth test so that an opaque polygon will occlude a transparent one behind it.
			state_set->setMode( GL_DEPTH_TEST, osg::StateAttribute::ON );

			// Conversely, disable writing to depth buffer so that
			// a transparent polygon will allow polygons behind it to shine through.
			// OSG renders transparent polygons after opaque ones.
			osg::ref_ptr<osg::Depth> depth (new osg::Depth);
			depth->setWriteMask( false );
			state_set->setAttributeAndModes( depth, osg::StateAttribute::ON );
		}
	}
}
