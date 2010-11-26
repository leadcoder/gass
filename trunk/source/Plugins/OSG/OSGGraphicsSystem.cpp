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
#include "Core/System/SystemFactory.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/IMessage.h"
#include "Sim/Scheduling/IRuntimeController.h"
#include "Sim/Systems/Input/IInputSystem.h"
#include "Sim/Systems/SimSystemManager.h"
#include "Sim/Systems/Resource/IResourceSystem.h"
#include "Sim/SimEngine.h"
#include <boost/bind.hpp>
#include <osgDB/ReadFile> 
#include "tinyxml.h"




namespace GASS
{

	int OSGGraphicsSystem::m_ReceivesShadowTraversalMask = 0x40;
	int OSGGraphicsSystem::m_CastsShadowTraversalMask = 0x80;
	OSGGraphicsSystem::OSGGraphicsSystem(void) : m_ShadowSettingsFile("systems.xml"), m_DebugTextBox(new TextBox())
	{


	}

	OSGGraphicsSystem::~OSGGraphicsSystem(void)
	{

	}

	void OSGGraphicsSystem::RegisterReflection()
	{
		SystemFactory::GetPtr()->Register("OSGGraphicsSystem",new GASS::Creator<OSGGraphicsSystem, ISystem>);
		RegisterProperty<bool>("CreateMainWindowOnInit", &GASS::OSGGraphicsSystem::GetCreateMainWindowOnInit, &GASS::OSGGraphicsSystem::SetCreateMainWindowOnInit);
		RegisterProperty<std::string>("ShadowSettingsFile", &GASS::OSGGraphicsSystem::GetShadowSettingsFile, &GASS::OSGGraphicsSystem::SetShadowSettingsFile);
		
		//RegisterProperty<std::string>( "Plugin", NULL, &GASS::OSGGraphicsSystem::AddPlugin);
	}

	void OSGGraphicsSystem::OnCreate()
	{
		SimEngine::GetPtr()->GetRuntimeController()->Register(this);
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(OSGGraphicsSystem::OnInit,InitMessage,0));
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(OSGGraphicsSystem::OnCreateRenderWindow,CreateRenderWindowMessage,0));
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(OSGGraphicsSystem::OnWindowMovedOrResized,MainWindowMovedOrResizedNotifyMessage,0));
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(OSGGraphicsSystem::OnDebugPrint,DebugPrintMessage,0));
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

	void OSGGraphicsSystem::OnInit(MessagePtr message)
	{

		m_Viewer = new osgViewer::CompositeViewer();

		
		std::string full_path;
		ResourceSystemPtr rs = SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystem<IResourceSystem>();
		if(!rs->GetFullPath("arial.ttf",full_path))
		{
			Log::Warning("Failed to find texture:%s",full_path.c_str());
		}
		
		
		m_DebugTextBox->setPosition(osg::Vec3d(0, 400, 0));
		m_DebugTextBox->setFont(full_path);
		m_DebugTextBox->setTextSize(12);
		
		


		if(m_CreateMainWindowOnInit)
		{

			osg::GraphicsContext::WindowingSystemInterface* wsi = osg::GraphicsContext::getWindowingSystemInterface();
			if (!wsi) 
			{
				osg::notify(osg::NOTICE)<<"Error, no WindowSystemInterface available, cannot create windows."<<std::endl;
				return;
			}

			osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
			traits->x = 100;
			traits->y = 100;
			traits->width = 800;
			traits->height = 600;
			traits->windowDecoration = true;
			traits->doubleBuffer = true;
			traits->sharedContext = 0;
			traits->windowName = "OSG Render window";



			m_GraphicsContext = osg::GraphicsContext::createGraphicsContext(traits.get());
			if (m_GraphicsContext.valid())
			{
				osg::notify(osg::INFO)<<"  GraphicsWindow has been created successfully."<<std::endl;

				//need to ensure that the window is cleared make sure that the complete window is set the correct colour
				//rather than just the parts of the window that are under the camera's viewports
				m_GraphicsContext->setClearColor(osg::Vec4f(0.8f,0.0f,0.0f,1.0f));
				m_GraphicsContext->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			}
			else
			{
				osg::notify(osg::NOTICE)<<"  GraphicsWindow has not been created successfully."<<std::endl;
			}

			//osg::ref_ptr<osg::Group> rootNode;
			//rootNode = new osg::Group();
			CreateView(m_Viewer,m_GraphicsContext,  0, 0, traits->width, traits->height);

			m_Viewer->setThreadingModel( osgViewer::Viewer::SingleThreaded );

			m_Viewer->frame();
			size_t windowHnd = 0;

			//there must be a better way to gte the window handle!
#if defined(WIN32) && !defined(__CYGWIN__) 	
			osgViewer::GraphicsWindowWin32* win32_window = (osgViewer::GraphicsWindowWin32*)(m_GraphicsContext.get());
			windowHnd = (size_t) win32_window->getHWND();
#endif
			MessagePtr window_msg(new MainWindowCreatedNotifyMessage((int)windowHnd,(int)windowHnd));
			GetSimSystemManager()->SendImmediate(window_msg);
		}

		//Load shadow settings
		if(m_ShadowSettingsFile != "")
		{
			TiXmlDocument *xmlDoc = new TiXmlDocument(m_ShadowSettingsFile.c_str());
			if (!xmlDoc->LoadFile())
			{
				//Fatal error, cannot load
				Log::Warning("OSGGraphicsSystem::OnInit - Couldn't load shadow settings from: %s", m_ShadowSettingsFile.c_str());
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


		osgDB::ReaderWriter::Options* options = new osgDB::ReaderWriter::Options; 
		options->setOptionString("dds_flip"); 
		osgDB::Registry::instance()->setOptions(options); 


		/*osgDB::ReaderWriter::Options* opt = osgDB::Registry::instance()->getOptions(); 
		if (opt == NULL) { 
		opt = new osgDB::ReaderWriter::Options(); 
		} 
		opt->setObjectCacheHint(osgDB::ReaderWriter::Options::CACHE_ALL); 
		osgDB::Registry::instance()->setOptions(opt); */

		/*	IInputSystem*  is = GetOwner()->GetFirstSystem<IInputSystem>();
		if(is)
		{
		is->SetWindow(windowHnd);
		}*/

		/*boost::shared_ptr<Message> update_msg(new Message(SimSystemManager::SYSTEM_MESSAGE_UPDATE,(int) this));
		update_msg->m_Timer = 1.0/30.0f; //update with 100hz
		GetMessageManager()->PostMessage(update_msg);*/
	}

	void OSGGraphicsSystem::CreateView(osgViewer::CompositeViewer *viewer,
		//osg::ref_ptr<osg::Group> scene,
		osg::ref_ptr<osg::GraphicsContext> gc,
		int x, int y, int width, int height)
	{
		//		double left,right,top,bottom, near_clip;
		//double far_clip;
		//double aspectratio;
		//double frusht, fruswid, fudge;
		//		bool gotfrustum;
		osgViewer::View* view = new osgViewer::View;
		viewer->addView(view);

		//view->setCameraManipulator(Tman);
		//view->setSceneData(scene.get());
		view->getCamera()->setViewport(new osg::Viewport(x, y, width,height));
		view->getCamera()->setComputeNearFarMode(osgUtil::CullVisitor::COMPUTE_NEAR_FAR_USING_BOUNDING_VOLUMES);
		view->setLightingMode(osg::View::SKY_LIGHT); 
		//view->getLight()->setDirection(osg::Vec3(1,1,1));

		/*osgGA::TrackballManipulator *Tman1 = new osgGA::TrackballManipulator();

		view->setCameraManipulator(Tman1);

		osg::ref_ptr<osgGA::StateSetManipulator> statesetManipulator = new osgGA::StateSetManipulator;
		statesetManipulator->setStateSet(view->getCamera()->getOrCreateStateSet());

		view->addEventHandler( statesetManipulator.get() );*/


		/*	view->getCamera()->getProjectionMatrixAsFrustum(left, right, bottom, top, near_clip, far_clip);
		if (gotfrustum) 
		{
		aspectratio = (double) width/ (double) height; 
		frusht = top - bottom;
		fruswid = right - left;
		fudge = frusht*aspectratio/fruswid;
		right = right*fudge;
		left = left*fudge;
		view->getCamera()->setProjectionMatrixAsFrustum(left,right,
		bottom,top,
		near_clip,far_clip);
		}*/
		view->getCamera()->setGraphicsContext(gc.get());
		view->getCamera()->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);

		// add the state manipulator
		//  osg::ref_ptr<osgGA::StateSetManipulator> statesetManipulator = new osgGA::StateSetManipulator;
		// statesetManipulator->setStateSet(view->getCamera()->getOrCreateStateSet());
		// view->addEventHandler( statesetManipulator.get() );


	}

	void OSGGraphicsSystem::OnCreateRenderWindow(CreateRenderWindowMessagePtr message)
	{
		std::string name = message->GetName();
		int height = message->GetHeight();
		int width = message->GetWidth();
		int handel = message->GetHandle();

		if(!m_GraphicsContext) // first window
		{
			int main_handel = message->GetMainHandle();
			// Init the Windata Variable that holds the handle for the Window to display OSG in.
			osg::ref_ptr<osg::Referenced> windata = new osgViewer::GraphicsWindowWin32::WindowData((HWND)handel);
			osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;



			traits->x = 0;
			traits->y = 0;
			traits->width = width;
			traits->height = height;
			traits->windowDecoration = false;
			traits->doubleBuffer = true;
			traits->sharedContext = 0;
			traits->setInheritedWindowPixelFormat = true;
			traits->inheritedWindowData = windata;

			m_GraphicsContext = osg::GraphicsContext::createGraphicsContext(traits.get());
			if (m_GraphicsContext.valid())
			{
				osg::notify(osg::INFO)<<"  GraphicsWindow has been created successfully."<<std::endl;

				//need to ensure that the window is cleared make sure that the complete window is set the correct colour
				//rather than just the parts of the window that are under the camera's viewports
				m_GraphicsContext->setClearColor(osg::Vec4f(0.8f,0.0f,0.0f,1.0f));
				m_GraphicsContext->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			}
			else
			{
				osg::notify(osg::NOTICE)<<"  GraphicsWindow has not been created successfully."<<std::endl;
			}

			

			//osg::ref_ptr<osg::Group> rootNode;
			//rootNode = new osg::Group();
			CreateView(m_Viewer,m_GraphicsContext,  0, 0, traits->width, traits->height);

			m_Viewer->setThreadingModel( osgViewer::Viewer::SingleThreaded );

			m_Viewer->frame();

			MessagePtr window_msg(new MainWindowCreatedNotifyMessage(message->GetHandle(),message->GetMainHandle()));
			GetSimSystemManager()->SendImmediate(window_msg);
		}
	}


	void OSGGraphicsSystem::GetMainWindowInfo(unsigned int &width, unsigned int &height, int &left, int &top)
	{
		//		unsigned int depth;

		if(m_GraphicsContext.get())
		{
			const osg::GraphicsContext::Traits* traits = m_GraphicsContext->getTraits();
			width = traits->width;
			height = traits->height;
			left = traits->x;
			top = traits->y;
		}
	}


	void OSGGraphicsSystem::OnWindowMovedOrResized(MainWindowMovedOrResizedNotifyMessagePtr message)
	{
		//m_Window->windowMovedOrResized();
		osgViewer::ViewerBase::Views views;
		
		
		m_Viewer->getViews(views);
		//set same size in all viewports for the moment
		for(int i = 0; i < views.size(); i++)
		{
			views[i]->getCamera()->setViewport(0,0,message->GetWidth(),message->GetHeight());
		}
	}

	void OSGGraphicsSystem::Update(double delta_time)
	{
		static int tick = 0;
		m_Viewer->frame(delta_time);


		if(m_Viewer->done())
		{
			//Exit
		//	Log::Error("Exit");
		}
		m_DebugTextBox->setText("");
		//WindowEventUtilities::messagePump();
		//m_Root->renderOneFrame();
	}

	TaskGroup OSGGraphicsSystem::GetTaskGroup() const
	{
		return MAIN_TASK_GROUP;
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
				GetViewer()->setThreadingModel(osgViewer::Viewer::SingleThreaded);
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
				GetViewer()->setThreadingModel(osgViewer::Viewer::SingleThreaded);
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




