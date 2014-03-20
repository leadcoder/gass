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

#include <stdio.h>
#include "Sim/Messages/GASSScriptSystemMessages.h"
#include "MyGUISystem.h"
#include "MyGUIOSG.h"
#include <MyGUI.h>
#include "MyGUI_LastHeader.h"
#include <MyGUI_OgrePlatform.h>
#include "StatisticInfo.h"
#include "MainMenu.h"

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
	MyGUISystem::MyGUISystem() : mGUI(NULL)
	{
		m_OSGManager = new MYGUIOSGDrawable(this);
	}

	MyGUISystem::~MyGUISystem()
	{

	}

	void MyGUISystem::RegisterReflection()
	{
		SystemFactory::GetPtr()->Register("MyGUISystem",new GASS::Creator<MyGUISystem, ISystem>);
	}

	void MyGUISystem::Init()
	{
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(MyGUISystem::OnLoadGUIScript,GUIScriptRequest,0));
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(MyGUISystem::OnInputSystemLoaded,InputSystemLoadedEvent,0));
		SimEngine::Get().GetRuntimeController()->Register(shared_from_this(),m_TaskNodeName);
		GetSimSystemManager()->RegisterForMessage(REG_TMESS( MyGUISystem::OnCameraChanged,CameraChangedEvent, 0));
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(MyGUISystem::OnPostSceneCreate,PostSceneCreateEvent,0));
	}

	MyGUI::OpenGLPlatform* MyGUISystem::InitializeOpenGLPlatform()
	{
		MyGUI::OpenGLPlatform* gl_platform = new MyGUI::OpenGLPlatform;
		gl_platform->initialise( m_OSGManager.get() );
		gl_platform->getDataManagerPtr()->addResourceLocation("C:/dev/construction-sim/trunk/data/gfx/GUI/MyGUI/Common/Demos", true);
		gl_platform->getDataManagerPtr()->addResourceLocation("C:/dev/construction-sim/trunk/data/gfx/GUI/MyGUI/Common/base", true);
		gl_platform->getDataManagerPtr()->addResourceLocation("C:/dev/construction-sim/trunk/data/gfx/GUI/MyGUI/Common/Themes", true);
		gl_platform->getDataManagerPtr()->addResourceLocation("C:/dev/construction-sim/trunk/data/gfx/GUI/MyGUI/MyGUI_Media", true);
		gl_platform->getDataManagerPtr()->addResourceLocation("C:/dev/construction-sim/trunk/data/gfx/GUI/MyGUI/GASS", true);
		_InitlizeGUI();
		return gl_platform;
	}

	void MyGUISystem::_InitlizeGUI()
	{
		mGUI = new MyGUI::Gui();
		mGUI->initialise("MyGUI_Core.xml");
		MyGUI::ResourceManager::getInstance().load("MyGUI_BlackOrangeTheme.xml");
	//	mInfo = new diagnostic::StatisticInfo();
	//	mInfo->setVisible(true);
		MainMenu* menu = new MainMenu(NULL);
		menu->Init();
	}

	void MyGUISystem::_SetupOSG()
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
	}

	
	//osg::ref_ptr<osg::Camera> m_HUDCamera = new osg::Camera;
	//osg::ref_ptr<osg::Geode> geode;
	//osg::ref_ptr<MYGUIManager>  mm;
	diagnostic::StatisticInfo* mInfo =NULL;

	void MyGUISystem::OnInputSystemLoaded(InputSystemLoadedEventPtr message)
	{
		_SetupOSG();
		//mm = new MYGUIManager();
	/*	geode = new osg::Geode;
		geode->setCullingActive( false );
		geode->addDrawable( m_OSGManager.get() );
		geode->getOrCreateStateSet()->setMode( GL_BLEND, osg::StateAttribute::ON );
		geode->getOrCreateStateSet()->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );

		camera->setReferenceFrame( osg::Transform::ABSOLUTE_RF );
		camera->setRenderOrder( osg::Camera::POST_RENDER );
		camera->setClearMask(GL_DEPTH_BUFFER_BIT);
		camera->setAllowEventFocus( false );
		camera->setProjectionMatrix( osg::Matrix::ortho2D(0.0, 1.0, 0.0, 1.0) );
		camera->addChild( geode.get() );
		
		IOSGGraphicsSystemPtr osg_sys = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<IOSGGraphicsSystem>();

		//IOSGGraphicsSceneManagerPtr osg_sm = message->GetScene()->GetFirstSceneManagerByClass<IOSGGraphicsSceneManager>();
		//osg_sm->GetOSGRootNode()->addChild(camera);
		osgViewer::CompositeViewer* viewer = osg_sys->GetViewer();
		osgViewer::Viewer::Windows windows;
		viewer->getWindows(windows);

		if (windows.empty()) return ;

		// set up cameras to render on the first window available.
		camera->setGraphicsContext(windows[0]);
		camera->setViewport(0,0,windows[0]->getTraits()->width, windows[0]->getTraits()->height);
		osgViewer::View* hudView = new osgViewer::View;
		hudView->setCamera(camera);
		viewer->addView(hudView);
		*/
		return;

		InputSystemPtr input_system = SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystemByClass<IInputSystem>();
		input_system->AddKeyListener(this);
		input_system->AddMouseListener(this);

		MyGUI::OgrePlatform* mPlatform = new MyGUI::OgrePlatform();
		Ogre::SceneManager* sm = Ogre::Root::getSingleton().getSceneManagerIterator().getNext();
		//Ogre::Camera* ocam = sm->getCameraIterator().getNext();
		Ogre::RenderTarget *target = NULL;
		if (Ogre::Root::getSingleton().getRenderSystem()->getRenderTargetIterator().hasMoreElements())
			target = Ogre::Root::getSingleton().getRenderSystem()->getRenderTargetIterator().getNext();
		mPlatform->initialise((Ogre::RenderWindow*)target, sm,"MyGUI");
		mGUI = new MyGUI::Gui();

		mGUI->initialise("MyGUI_Core.xml");
		MyGUI::ResourceManager::getInstance().load("MyGUI_BlackOrangeTheme.xml");

		mInfo = new diagnostic::StatisticInfo();
		mInfo->setVisible(true);

		MainMenu* menu = new MainMenu(NULL);
		menu->Init();
	}

	void MyGUISystem::OnCameraChanged(CameraChangedEventPtr message)
	{
		if(m_HUDCamera.valid())
		{
			CameraComponentPtr camera_comp = message->GetViewport()->GetCamera();
			OSGCameraPtr osg_cam = DYNAMIC_PTR_CAST<IOSGCamera>(camera_comp);
			osgViewer::View* view = dynamic_cast<osgViewer::View*>(osg_cam->GetOSGCamera()->getView());
			view->addEventHandler(new MYGUIOSGEventHandler(m_HUDCamera.get(), m_OSGManager.get()) );
		}
	}

	void MyGUISystem::OnPostSceneCreate(PostSceneCreateEventPtr message)
	{
		//view->addEventHandler( new MYGUIHandler(camera.get(), mygui.get()) );
	}


	void MyGUISystem::OnLoadGUIScript(GUIScriptRequestPtr message)
	{
		return;
		//CEGUI::System::getSingleton().executeScriptFile(message->GetFilename());
		MyGUI::OgrePlatform* mPlatform = new MyGUI::OgrePlatform();
		Ogre::SceneManager* sm = Ogre::Root::getSingleton().getSceneManagerIterator().getNext();
		//Ogre::Camera* ocam = sm->getCameraIterator().getNext();
		Ogre::RenderTarget *target = NULL;
		if (Ogre::Root::getSingleton().getRenderSystem()->getRenderTargetIterator().hasMoreElements())
			target = Ogre::Root::getSingleton().getRenderSystem()->getRenderTargetIterator().getNext();
		mPlatform->initialise((Ogre::RenderWindow*)target, sm);
		mGUI = new MyGUI::Gui();
		mGUI->initialise("MyGUI_Core.xml");
		mInfo = new diagnostic::StatisticInfo();
		mInfo->setVisible(true);

		MainMenu* menu = new MainMenu(NULL);
		menu->Init();


		//load main menu from layout

		/*MyGUI::MenuCtrlPtr menu = mGUI->createWidget<MyGUI::MenuCtrl>("PopupMenu", MyGUI::IntCoord(200, 20, 150, 100), MyGUI::Align::Default, "Overlapped");

		MyGUI::MenuItemPtr item1 = menu->addItem("line1", MyGUI::MenuItemType::Popup);
		MyGUI::MenuItemPtr item2 = menu->addItem("line2", MyGUI::MenuItemType::Normal);
		MyGUI::MenuItemPtr item3 = menu->addItem("line3", MyGUI::MenuItemType::Popup);

		MyGUI::MenuCtrlPtr submenu1 = item1->createItemChild();
		MyGUI::MenuItemPtr subitem1 = submenu1->addItem("subline1", MyGUI::MenuItemType::Normal);
		MyGUI::MenuItemPtr subitem2 = submenu1->addItem("subline2", MyGUI::MenuItemType::Popup);

		MyGUI::MenuCtrlPtr submenu2 = item3->createItemChild();
		MyGUI::MenuItemPtr subitem21 = submenu2->addItem("subline21", MyGUI::MenuItemType::Normal);
		MyGUI::MenuItemPtr subitem22 = submenu2->addItem("subline22", MyGUI::MenuItemType::Normal);

		MyGUI::MenuCtrlPtr submenu21 = subitem2->createItemChild();
		MyGUI::MenuItemPtr subitem31 = submenu21->addItem("subline31", MyGUI::MenuItemType::Normal);

		menu->setVisible(true);*/
	}




	void MyGUISystem::Update(double delta_time)
	{
		if(mInfo )
		{
			mInfo->change("FPS", 10);
			mInfo->change("triangle", 10);
			mInfo->change("batch", 10);
			mInfo->change("batch gui", 10);
			mInfo->update();
		}
	}

	bool MyGUISystem::MouseMoved(const MouseData &data)
	{
		if(mGUI)
		{
			MyGUI::InputManager::getInstance().injectMouseMove(data.XAbs,data.YAbs, data.ZAbs);
		}
		return true;
	}
	bool MyGUISystem::MousePressed(const MouseData &data,MouseButtonId id )
	{
		if(mGUI)
		{
			MyGUI::MouseButton button((MyGUI::MouseButton::Enum) id);
			MyGUI::InputManager::getInstance().injectMousePress(data.XAbs,data.YAbs,button);
		}
		//CEGUI::System::getSingleton().injectMouseButtonDown(ConvertOISButtonToCegui(id));
		return true;
	}
	bool MyGUISystem::MouseReleased(const MouseData &data,MouseButtonId id )
	{
		if(mGUI)
		{
			MyGUI::MouseButton button((MyGUI::MouseButton::Enum) id);
			MyGUI::InputManager::getInstance().injectMouseRelease(data.XAbs,data.YAbs,button);
		}
		//CEGUI::System::getSingleton().injectMouseButtonUp(ConvertOISButtonToCegui(id));
		return true;

	}

	bool MyGUISystem::KeyPressed(int key, unsigned int text)
	{

		// do event injection
		//CEGUI::System& cegui = CEGUI::System::getSingleton();

		// key down
		//cegui.injectKeyDown(key);

		// now character
		//cegui.injectChar(text);
		return true;
	}

	bool MyGUISystem::KeyReleased( int key, unsigned int text)
	{
		//CEGUI::System::getSingleton().injectKeyUp(key);
		return true;
	}

}
