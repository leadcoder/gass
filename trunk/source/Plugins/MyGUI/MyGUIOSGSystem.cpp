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

#include "MyGUIOSGSystem.h"
#include <stdio.h>
#include "Sim/Messages/GASSScriptSystemMessages.h"
#include "MyGUIOSG.h"
#include <MyGUI.h>
#include "MyGUI_LastHeader.h"

#include <osg/Texture2D>
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osgDB/ReadFile>
#include <osgGA/StateSetManipulator>
#include <osgGA/TrackballManipulator>
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/Viewer>
#include <osgViewer/View>
#include <osgViewer/CompositeViewer>

#include "Plugins/OSG/IOSGGraphicsSceneManager.h"
#include "Plugins/OSG/IOSGGraphicsSystem.h"
#include "Plugins/OSG/IOSGCamera.h"
#include "Sim/Interface/GASSIViewport.h"

namespace GASS
{
	MyGUIOSGSystem::MyGUIOSGSystem() 
	{
		m_OSGManager = new MYGUIOSGPlatformProxy(this);
	}

	MyGUIOSGSystem::~MyGUIOSGSystem()
	{
		//delete m_OSGManager;
	}

	void MyGUIOSGSystem::RegisterReflection()
	{
		SystemFactory::GetPtr()->Register("MyGUIOSGSystem",new GASS::Creator<MyGUIOSGSystem, ISystem>);
	}

	void MyGUIOSGSystem::Init()
	{
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(MyGUIOSGSystem::OnCameraChanged,CameraChangedEvent, 0));
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(MyGUIOSGSystem::OnGraphicsSystemLoaded,GraphicsSystemLoadedEvent ,0));
		MyGUISystem::Init();
	}

	MyGUI::OpenGLPlatform* MyGUIOSGSystem::InitializeOpenGLPlatform()
	{
		MyGUI::OpenGLPlatform* gl_platform = new MyGUI::OpenGLPlatform;
		gl_platform->initialise( m_OSGManager.get() );
		//reflect MYGUI resource group to platform
		ResourceManagerPtr rm = SimEngine::Get().GetResourceManager();
		
		ResourceGroupPtr group = rm->GetFirstResourceGroupByName("MyGUI");
		ResourceLocationVector locations = group->GetResourceLocations();
		for(int i = 0; i < locations.size(); i++)
		{
			const std::string path = locations[i]->GetPath().GetFullPath();
			gl_platform->getDataManagerPtr()->addResourceLocation(path, true);
		}

		/*gl_platform->getDataManagerPtr()->addResourceLocation("C:/dev/construction-sim/trunk/data/gfx/GUI/MyGUI/Common/Demos", true);
		gl_platform->getDataManagerPtr()->addResourceLocation("C:/dev/construction-sim/trunk/data/gfx/GUI/MyGUI/Common/base", true);
		gl_platform->getDataManagerPtr()->addResourceLocation("C:/dev/construction-sim/trunk/data/gfx/GUI/MyGUI/Common/Themes", true);
		gl_platform->getDataManagerPtr()->addResourceLocation("C:/dev/construction-sim/trunk/data/gfx/GUI/MyGUI/MyGUI_Media", true);
		gl_platform->getDataManagerPtr()->addResourceLocation("C:/dev/construction-sim/trunk/data/gfx/GUI/MyGUI/GASS", true);
		*/
		InitGUI();
		return gl_platform;
	}

	void MyGUIOSGSystem::_SetupOSG(osgViewer::View* view)
	{
		m_HUDCamera = new osg::Camera;
		osg::ref_ptr<osg::Geode> geode = new osg::Geode;
		geode->setCullingActive( false );
		geode->addDrawable( m_OSGManager.get() );
		geode->getOrCreateStateSet()->setMode( GL_BLEND, osg::StateAttribute::ON );
		geode->getOrCreateStateSet()->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
		
		m_HUDCamera->setReferenceFrame( osg::Transform::ABSOLUTE_RF );
		m_HUDCamera->setRenderOrder( osg::Camera::POST_RENDER );
		m_HUDCamera->setClearMask(GL_DEPTH_BUFFER_BIT);
		m_HUDCamera->setAllowEventFocus( false );
		m_HUDCamera->setProjectionMatrix( osg::Matrix::ortho2D(0.0, 1.0, 0.0, 1.0) );
		m_HUDCamera->addChild( geode.get() );
		
		IOSGGraphicsSystemPtr osg_sys = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<IOSGGraphicsSystem>();
		osgViewer::CompositeViewer* viewer = osg_sys->GetViewer();
		osgViewer::Viewer::Windows windows;
		viewer->getWindows(windows);

		if (windows.empty()) 
			return;
		// set up cameras to render on the first window available.
		m_HUDCamera->setGraphicsContext(windows[0]);
		m_HUDCamera->setViewport(0,0, windows[0]->getTraits()->width, windows[0]->getTraits()->height);
		osgViewer::View* hudView = new osgViewer::View;
		hudView->setCamera(m_HUDCamera);
		viewer->addView(hudView);
		view->addEventHandler(new MYGUIOSGEventHandler(m_HUDCamera.get(), m_OSGManager.get()) );
		osgViewer::GraphicsWindow* gw = dynamic_cast<osgViewer::GraphicsWindow*>( windows[0] );
		if ( gw )
		{
			// Send window size for MyGUI to initialize
			int x, y, w, h; gw->getWindowRectangle( x, y, w, h );
			view->getEventQueue()->windowResize( x, y, w, h );
		}
		
	}

	void MyGUIOSGSystem::OnGraphicsSystemLoaded(GraphicsSystemLoadedEventPtr message)
	{
		//_SetupOSG();
	}

	void MyGUIOSGSystem::OnCameraChanged(CameraChangedEventPtr message)
	{
		CameraComponentPtr camera_comp = message->GetViewport()->GetCamera();
		OSGCameraPtr osg_cam = DYNAMIC_PTR_CAST<IOSGCamera>(camera_comp);
		osgViewer::View* view = dynamic_cast<osgViewer::View*>(osg_cam->GetOSGCamera()->getView());

		if(!m_HUDCamera.valid()) //create hud camera
		{
			_SetupOSG(view);
		}
		
		
	}
}
