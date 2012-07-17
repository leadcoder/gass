/****************************************************************************
* This file is part of GASS.                                                *
* See http://sourceforge.net/projects/gass/                                 *
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

#   pragma warning (disable : 4541)
#include <osgDB/ReadFile>
#include <osgSim/MultiSwitch>
#include <osg/MatrixTransform>
#include <osg/CullSettings>
#include <osg/PositionAttitudeTransform>
#include <osgGA/TrackballManipulator>
#include <osgViewer/CompositeViewer>
#include <osgGA/StateSetManipulator>


#include <osgShadow/ShadowedScene>
#include <osgShadow/ShadowVolume>
#include <osgShadow/ShadowTexture>
#include <osgShadow/ShadowMap>
#include <osgShadow/SoftShadowMap>
#include <osgShadow/ParallelSplitShadowMap>
#include <osgShadow/LightSpacePerspectiveShadowMap>
#include <osgShadow/StandardShadowMap>
#include <osgGA/StateSetManipulator>
#include <osgGA/GUIEventHandler>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>




#if defined(WIN32) && !defined(__CYGWIN__) 

#include <osgViewer/api/Win32/GraphicsWindowWin32>

typedef HWND WindowHandle; 
typedef osgViewer::GraphicsWindowWin32::WindowData WindowData; 
#elif defined(__APPLE__) // Assume using Carbon on Mac. 
#include <osgViewer/api/Carbon/GraphicsWindowCarbon> 
typedef WindowRef WindowHandle; 
typedef osgViewer::GraphicsWindowCarbon::WindowData WindowData; 
#else // all other unix 
#include <osgViewer/api/X11/GraphicsWindowX11> 
typedef Window WindowHandle; 
typedef osgViewer::GraphicsWindowX11::WindowData WindowData; 
#endif 

#include "Plugins/OSG/OSGGraphicsSystem.h"
#include "Plugins/OSG/Utils/TextBox.h"
#include "Plugins/OSG/Components/OSGCameraComponent.h"
#include "Plugins/OSG/Components/OSGCameraManipulatorComponent.h"
#include "Plugins/OSG/IOSGCameraManipulator.h"
#include "Sim/GASS.h"

#include <osgDB/ReadFile> 
#include "tinyxml.h"


namespace GASS
{

	OSGGraphicsSystem::OSGGraphicsSystem(void) : m_ShadowSettingsFile("systems.xml"), m_DebugTextBox(new TextBox()),m_Viewer(NULL)
	{

	}

	OSGGraphicsSystem::~OSGGraphicsSystem(void)
	{
		if(m_Viewer)
			m_Viewer->stopThreading();
		
		delete m_Viewer;
	}

	void OSGGraphicsSystem::RegisterReflection()
	{
		SystemFactory::GetPtr()->Register("OSGGraphicsSystem",new GASS::Creator<OSGGraphicsSystem, ISystem>);
		RegisterProperty<bool>("CreateMainWindowOnInit", &GASS::OSGGraphicsSystem::GetCreateMainWindowOnInit, &GASS::OSGGraphicsSystem::SetCreateMainWindowOnInit);
		RegisterProperty<std::string>("ShadowSettingsFile", &GASS::OSGGraphicsSystem::GetShadowSettingsFile, &GASS::OSGGraphicsSystem::SetShadowSettingsFile);
	}

	void OSGGraphicsSystem::OnCreate()
	{
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(OSGGraphicsSystem::OnInit,InitSystemMessage,0));
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(OSGGraphicsSystem::OnViewportMovedOrResized,ViewportMovedOrResizedNotifyMessage,0));
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(OSGGraphicsSystem::OnDebugPrint,DebugPrintMessage,0));
	}

	void OSGGraphicsSystem::OnInit(InitSystemMessagePtr message)
	{
		m_Viewer = new osgViewer::CompositeViewer();
		m_Viewer->setThreadingModel( osgViewer::Viewer::SingleThreaded);
		m_Viewer->setKeyEventSetsDone(0);
		std::string full_path;
		
		ResourceSystemPtr rs = SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystem<IResourceSystem>();
		if(!rs->GetFullPath("arial.ttf",full_path))
		{
			GASS_EXCEPT(Exception::ERR_FILE_NOT_FOUND,"Failed to find texture" + full_path,"OSGGraphicsSystem::OnInit");
		}
		
		m_DebugTextBox->setPosition(osg::Vec3d(0, 400, 0));
		m_DebugTextBox->setFont(full_path);
		m_DebugTextBox->setTextSize(12);
		
		
		if(m_CreateMainWindowOnInit)
		{
			osg::GraphicsContext::WindowingSystemInterface* wsi = osg::GraphicsContext::getWindowingSystemInterface();
			if (!wsi) 
			{
				GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"No WindowSystemInterface available, cannot create windows","OSGGraphicsSystem::OnInit");
				//osg::notify(osg::NOTICE)<<"Error, no WindowSystemInterface available, cannot create windows."<<std::endl;
				return;
			}

			CreateRenderWindow("MainWindow",800, 600,0);
			CreateViewport("MainViewport","MainWindow", 0, 0, 800, 600);
		}

		
		//Load shadow settings
		if(m_ShadowSettingsFile != "")
		{
			TiXmlDocument *xmlDoc = new TiXmlDocument(m_ShadowSettingsFile.c_str());
			if (!xmlDoc->LoadFile())
			{
				//Fatal error, cannot load
				LogManager::getSingleton().stream() << "WARNING: OSGGraphicsSystem::OnInit - Couldn't load shadow settings from: " <<  m_ShadowSettingsFile;
			}
			else
			{
				//TiXmlElement *ss= xmlDoc->FirstChildElement("ShadowSettings");
				TiXmlElement *ss= xmlDoc->FirstChildElement("Systems");
				if(ss)
				{
					ss = ss->FirstChildElement("OSGGraphicsSystem");
					if(ss)
						ss = ss->FirstChildElement("ShadowSettings");
				}
				LoadShadowSettings(ss);
			}
		}

		//osgDB::ReaderWriter::Options* options = new osgDB::ReaderWriter::Options; 
		
		osgDB::ReaderWriter::Options* opt = osgDB::Registry::instance()->getOptions(); 
		if (opt == NULL) 
		{ 
			opt = new osgDB::ReaderWriter::Options(); 
		} 
		
		const std::string options = opt->getOptionString();
		opt->setOptionString("dds_flip"); 
		osgDB::Registry::instance()->setOptions(opt); 

		//opt->setObjectCacheHint(osgDB::ReaderWriter::Options::CACHE_ALL); 
		//osgDB::Registry::instance()->setOptions(opt); 
	}

	void OSGGraphicsSystem::OnDebugPrint(DebugPrintMessagePtr message)
	{
		std::string debug_text = message->GetText();
		
		m_DebugTextBox->setText(m_DebugTextBox->getText() + "\n" + debug_text);

	}

	void OSGGraphicsSystem::SetActiveData(osg::Group* root)
	{

		osgViewer::ViewerBase::Views views;
		m_Viewer->getViews(views);

		root->addChild(&m_DebugTextBox->getGroup());

		//set same scene in all viewports for the moment
		for(int i = 0; i < views.size(); i++)
		{
			views[i]->setSceneData(root);
		}
		//m_Viewer->realize();
	}

	void OSGGraphicsSystem::CreateRenderWindow(const std::string &name, int width, int height, void* handle, void* main_handle)
	{
		osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
			
		traits->x = 0;
		traits->y = 0;
		traits->width = width;
		traits->height = height;
		traits->doubleBuffer = true;
		traits->sharedContext = 0;
		if(m_Windows.size() > 0)
			traits->sharedContext = m_Windows.begin()->second;
		

		if(handle) //external window
		{
			osg::ref_ptr<osg::Referenced> windata = new osgViewer::GraphicsWindowWin32::WindowData((HWND)handle);
			traits->windowDecoration = false;
			traits->setInheritedWindowPixelFormat = true;
			traits->inheritedWindowData = windata;
			
		}
		else //create window here
		{
			traits->windowDecoration = true;
			traits->windowName = name;
		}
		
		osg::ref_ptr<osg::GraphicsContext> graphics_context = osg::GraphicsContext::createGraphicsContext(traits.get());
		if (graphics_context.valid())
		{
			osg::notify(osg::INFO)<<"  GraphicsWindow has been created successfully."<<std::endl;
			//need to ensure that the window is cleared make sure that the complete window is set the correct colour
			//rather than just the parts of the window that are under the camera's viewports
			graphics_context->setClearColor(osg::Vec4f(0.8f,0.0f,0.0f,1.0f));
			graphics_context->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
		else
		{
			osg::notify(osg::NOTICE)<<"  GraphicsWindow has not been created successfully."<<std::endl;
		}
		m_Windows[name] = graphics_context;

		

		if(m_Windows.size() == 1) //first window created?
		{
			if(handle == 0) //internal window
			{
				#if defined(WIN32) && !defined(__CYGWIN__) 	
				osgViewer::GraphicsWindowWin32* win32_window = (osgViewer::GraphicsWindowWin32*)(graphics_context.get());
				main_handle = (void*) win32_window->getHWND();
				handle = main_handle;
				#endif
			}
			MessagePtr window_msg(new MainWindowCreatedNotifyMessage(handle,main_handle));
			GetSimSystemManager()->SendImmediate(window_msg);
			
		}
	}

	void OSGGraphicsSystem::CreateViewport(const std::string &name, const std::string &render_window, float  left, float top, float width, float height)
	//void OSGGraphicsSystem::CreateViewport(const std::string &name, const std::string &render_window, int pos_x, int pos_y, int width, int height)
	{
		if(m_Windows.find(render_window) != m_Windows.end())
		{
			//
			osgViewer::View* view = new osgViewer::View;
			view->setName(name);
		
			int win_w = m_Windows[render_window]->getTraits()->width;
			int win_h = m_Windows[render_window]->getTraits()->height;

			int p_left = win_w*left;
			int p_top = win_h*top;

			int p_width = win_w*width;
			int p_height = win_h*height;

			m_Viewer->addView(view);

			view->getCamera()->setViewport(new osg::Viewport(p_left, p_top, p_width,p_height));
			view->getCamera()->setComputeNearFarMode(osgUtil::CullVisitor::COMPUTE_NEAR_FAR_USING_BOUNDING_VOLUMES);
			view->setLightingMode(osg::View::SKY_LIGHT); 
			view->getDatabasePager()->setDoPreCompile( true );
			view->getDatabasePager()->setTargetMaximumNumberOfPageLOD(100);
    		// add some stock OSG handlers:
			
			osgViewer::StatsHandler* stats = new osgViewer::StatsHandler();
			stats->setKeyEventTogglesOnScreenStats('y');
			stats->setKeyEventPrintsOutStats(0);

			view->addEventHandler(stats);

			view->addEventHandler(new osgViewer::WindowSizeHandler());
			view->addEventHandler(new osgViewer::ThreadingHandler());
			view->addEventHandler(new osgViewer::LODScaleHandler());

			osgGA::StateSetManipulator* ssm =  new osgGA::StateSetManipulator(view->getCamera()->getOrCreateStateSet());
			ssm->setKeyEventCyclePolygonMode('u');
			view->addEventHandler(ssm);
			//    view->addEventHandler(new osgViewer::HelpHandler(arguments.getApplicationUsage()));
			view->getCamera()->setGraphicsContext(m_Windows[render_window]);
			view->getCamera()->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
		}
	}

	void OSGGraphicsSystem::ChangeCamera(const std::string &viewport, OSGCameraComponentPtr cam_comp)
	{
		osgViewer::ViewerBase::Views views;
		GetViewer()->getViews(views);
		
		for(int i = 0; i < views.size(); i++)
		{
			if(views[i]->getName() == viewport || viewport == "ALL")
			{
				cam_comp->SetOSGCamera( views[i]->getCamera());
				//set manipulator
				OSGCameraManipulatorPtr man = cam_comp->GetSceneObject()->GetFirstComponentByClass<IOSGCameraManipulator>();
				if(man)
					views[i]->setCameraManipulator(man->GetManipulator());
				else 
					views[i]->setCameraManipulator(NULL);
			}
		}
	}


	void OSGGraphicsSystem::GetMainWindowInfo(unsigned int &width, unsigned int &height, int &left, int &top) const
	{
		if(m_Windows.size() > 0)
		{
			const osg::GraphicsContext::Traits* traits = m_Windows.begin()->second->getTraits();
			width = traits->width;
			height = traits->height;
			left = traits->x;
			top = traits->y;
		}
	}


	void OSGGraphicsSystem::OnViewportMovedOrResized(ViewportMovedOrResizedNotifyMessagePtr message)
	{
		//m_Window->windowMovedOrResized();
		osgViewer::ViewerBase::Views views;
		
		m_Viewer->getViews(views);
		//set same size in all viewports for the moment
		for(int i = 0; i < views.size(); i++)
		{
			if(views[i]->getName() == message->GetViewportName())
			{
				views[i]->getCamera()->setViewport(message->GetPositionX(),message->GetPositionY(),message->GetWidth(), message->GetHeight());
			}
		}
	}

	void OSGGraphicsSystem::Update(double delta_time)
	{
		static int tick = 0;
		
		if(m_Viewer->done())
		{
			return;
		}
		m_Viewer->frame(delta_time);
		m_DebugTextBox->setText("");
		//update listeners
		SimSystem::Update(delta_time);
	}


	void OSGGraphicsSystem::LoadShadowSettings(TiXmlElement *shadow_elem)
	{
		//Load shadow settings
		//TiXmlElement *shadow_elem= elem->FirstChildElement("ShadowSettings");

		if(shadow_elem)
		{
			std::string type = shadow_elem->Attribute("type");
			
			if(type == "ShadowVolume")
			{
				//GetViewer()->setThreadingModel(osgViewer::Viewer::SingleThreaded);
				// hint to tell viewer to request stencil buffer when setting up windows
				osg::DisplaySettings::instance()->setMinimumNumStencilBits(8);
				osg::ref_ptr<osgShadow::ShadowVolume> sv = new osgShadow::ShadowVolume;
				sv->setDynamicShadowVolumes(true);
				//sv->setDrawMode(osgShadow::ShadowVolumeGeometry::STENCIL_TWO_SIDED);
				//sv->setDrawMode(osgShadow::ShadowVolumeGeometry::STENCIL_TWO_PASS);
				m_ShadowTechnique  = sv;
			}
			else if(type == "ShadowTexture")
			{
				osg::ref_ptr<osgShadow::ShadowTexture> st = new osgShadow::ShadowTexture;
				m_ShadowTechnique  = st;
			}
			else if(type == "ShadowMap")
			{
				osg::ref_ptr<osgShadow::ShadowMap> sm = new osgShadow::ShadowMap;
				m_ShadowTechnique = sm;
			}
			else if(type == "StandardShadowMap")
			{
				osg::ref_ptr<osgShadow::StandardShadowMap> ssm = new osgShadow::StandardShadowMap;
				m_ShadowTechnique = ssm;
			}
			else if(type == "ParallelSplitShadowMap")
			{
				// pssm isn't yet thread safe
				//GetViewer()->setThreadingModel(osgViewer::Viewer::SingleThreaded);
				int mapcount = 4;
				int mapres = 1024;
				float maxFarPlane = 200;
				float minNearDistanceForSplits = 10;
				float moveVCamBehindRCamFactor = 1;
				
				shadow_elem->QueryFloatAttribute("MaxFarDistance",&maxFarPlane);
				shadow_elem->QueryIntAttribute("TextureSize",&mapres);
				shadow_elem->QueryIntAttribute("NumCount",&mapcount);
				shadow_elem->QueryFloatAttribute("MinNearDistanceForSplits",&minNearDistanceForSplits);
				shadow_elem->QueryFloatAttribute("MoveVCamBehindRCamFactor",&moveVCamBehindRCamFactor);
				
				osg::ref_ptr<osgShadow::ParallelSplitShadowMap> pssm = new osgShadow::ParallelSplitShadowMap(NULL,mapcount);
			
				pssm->setTextureResolution(mapres);
				pssm->setMaxFarDistance(maxFarPlane);
				pssm->setMoveVCamBehindRCamFactor(moveVCamBehindRCamFactor);
				pssm->setMinNearDistanceForSplits(minNearDistanceForSplits);

				double polyoffsetfactor = pssm->getPolygonOffset().x();
				double polyoffsetunit   = pssm->getPolygonOffset().y();
	
				shadow_elem->QueryDoubleAttribute("PolyOffsetFactor",&polyoffsetfactor);
				shadow_elem->QueryDoubleAttribute("PolyOffsetUnit",&polyoffsetunit);
				
				pssm->setPolygonOffset(osg::Vec2(polyoffsetfactor ,polyoffsetunit )); 
				m_ShadowTechnique = pssm;
			}
			else if(type == "LightSpacePerspectiveShadowMap")
			{
				osg::ref_ptr<osgShadow::MinimalShadowMap> sm = NULL;
				
				std::string sub_type = shadow_elem->Attribute("SubType");
				if(sub_type == "Draw")
				{
					sm = new osgShadow::LightSpacePerspectiveShadowMapDB;
					
				}
				else if(sub_type == "Cull")
					sm = new osgShadow::LightSpacePerspectiveShadowMapCB;
				else if(sub_type == "Frustum")
					sm = new osgShadow::LightSpacePerspectiveShadowMapVB;
				else
					sm = new osgShadow::LightSpacePerspectiveShadowMapDB;
				
				if( sm.valid() ) 
				{
					//sm->setMainVertexShader( NULL ); 
					//sm->setShadowVertexShader(NULL);
					
					float minLightMargin = 20.f;
					float maxFarPlane = 500;
					int texSize = 1024;
					int baseTexUnit = 0;
					int shadowTexUnit = 1;

					shadow_elem->QueryFloatAttribute("MinLightMargin",&minLightMargin);
					shadow_elem->QueryFloatAttribute("MaxFarPlane",&maxFarPlane);
					shadow_elem->QueryIntAttribute("TextureSize",&texSize);
					shadow_elem->QueryIntAttribute("BaseTextureUnit",&baseTexUnit);
					shadow_elem->QueryIntAttribute("ShadowTextureUnit",&shadowTexUnit);
					
					sm->setMinLightMargin( minLightMargin );
					sm->setMaxFarPlane( maxFarPlane );
					sm->setTextureSize( osg::Vec2s( texSize, texSize ) );
					sm->setShadowTextureCoordIndex( shadowTexUnit );
					sm->setShadowTextureUnit( shadowTexUnit );
					sm->setBaseTextureCoordIndex( baseTexUnit );
					sm->setBaseTextureUnit( baseTexUnit );
					m_ShadowTechnique = sm;
				} 
			}
		}
	}
}









