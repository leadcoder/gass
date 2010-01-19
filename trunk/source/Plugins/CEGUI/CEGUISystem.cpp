/****************************************************************************
*                                                                           *
* HiFiEngine                                                                *
* Copyright (C)2003 - 2005 Johan Hedstrom                                   *
* Email: hifiengine@gmail.com                                               *
* Web page: http://n00b.dyndns.org/HiFiEngine                               *
*                                                                           *
* HiFiEngine is only used with knowledge from the author. This software     *
* is not allowed to redistribute without permission from the author.        *
* For further license information, please turn to the product home page or  *
* contact author. Abuse against the HiFiEngine license is prohibited by law.*
*                                                                           *
*****************************************************************************/ 

#include "CEGUISystem.h"
#include "Core/System/SystemFactory.h"
#include "Sim/Scenario/Scene/SceneManagerFactory.h"
#include "Sim/Systems/SimSystemManager.h"

#include "CEGUI.h"
#include "RendererModules/Ogre/CEGUIOgreRenderer.h"

namespace GASS
{
	CEGUISystem::CEGUISystem()
	{
		
	}

	CEGUISystem::~CEGUISystem()
	{

	}

	void CEGUISystem::RegisterReflection()
	{
		SystemFactory::GetPtr()->Register("CEGUISystem",new GASS::Creator<CEGUISystem, ISystem>);
	}

	void CEGUISystem::OnCreate()
	{
		GetSimSystemManager()->RegisterForMessage(SYSTEM_RM_INIT, MESSAGE_FUNC( CEGUISystem::OnInit));
	}

	void CEGUISystem::OnInit(MessagePtr message)
	{
		
    // initialise GUI system using the new automagic function
    CEGUI::OgreRenderer* d_renderer = &CEGUI::OgreRenderer::bootstrapSystem();

	using namespace CEGUI;

    // CEGUI relies on various systems being set-up, so this is what we do
    // here first.
    //
    // The first thing to do is load a CEGUI 'scheme' this is basically a file
    // that groups all the required resources and definitions for a particular
    // skin so they can be loaded / initialised easily
    //
    // So, we use the SchemeManager singleton to load in a scheme that loads the
    // imagery and registers widgets for the TaharezLook skin.  This scheme also
    // loads in a font that gets used as the system default.
    SchemeManager::getSingleton().create("TaharezLook.scheme");

    // The next thing we do is to set a default mouse cursor image.  This is
    // not strictly essential, although it is nice to always have a visible
    // cursor if a window or widget does not explicitly set one of its own.
    //
    // The TaharezLook Imageset contains an Image named "MouseArrow" which is
    // the ideal thing to have as a defult mouse cursor image.
    System::getSingleton().setDefaultMouseCursor("TaharezLook", "MouseArrow");

    // Now the system is initialised, we can actually create some UI elements, for
    // this first example, a full-screen 'root' window is set as the active GUI
    // sheet, and then a simple frame window will be created and attached to it.

    // All windows and widgets are created via the WindowManager singleton.
    WindowManager& winMgr = WindowManager::getSingleton();

    // Here we create a "DeafultWindow".  This is a native type, that is, it does
    // not have to be loaded via a scheme, it is always available.  One common use
    // for the DefaultWindow is as a generic container for other windows.  Its
    // size defaults to 1.0f x 1.0f using the Relative metrics mode, which means
    // when it is set as the root GUI sheet window, it will cover the entire display.
    // The DefaultWindow does not perform any rendering of its own, so is invisible.
    //
    // Create a DefaultWindow called 'Root'.
    DefaultWindow* root = (DefaultWindow*)winMgr.createWindow("DefaultWindow", "Root");

    // set the GUI root window (also known as the GUI "sheet"), so the gui we set up
    // will be visible.
    System::getSingleton().setGUISheet(root);

    // A FrameWindow is a window with a frame and a titlebar which may be moved around
    // and resized.
    //
    // Create a FrameWindow in the TaharezLook style, and name it 'Demo Window'
    FrameWindow* wnd = (FrameWindow*)winMgr.createWindow("TaharezLook/FrameWindow", "Demo Window");

    // Here we attach the newly created FrameWindow to the previously created
    // DefaultWindow which we will be using as the root of the displayed gui.
    root->addChildWindow(wnd);

    // Windows are in Relative metrics mode by default.  This means that we can
    // specify sizes and positions without having to know the exact pixel size
    // of the elements in advance.  The relative metrics mode co-ordinates are
    // relative to the parent of the window where the co-ordinates are being set.
    // This means that if 0.5f is specified as the width for a window, that window
    // will be half as its parent window.
    //
    // Here we set the FrameWindow so that it is half the size of the display,
    // and centered within the display.
    wnd->setPosition(UVector2(cegui_reldim(0.25f), cegui_reldim( 0.25f)));
    wnd->setSize(UVector2(cegui_reldim(0.5f), cegui_reldim( 0.5f)));

    // now we set the maximum and minum sizes for the new window.  These are
    // specified using relative co-ordinates, but the important thing to note
    // is that these settings are aways relative to the display rather than the
    // parent window.
    //
    // here we set a maximum size for the FrameWindow which is equal to the size
    // of the display, and a minimum size of one tenth of the display.
    wnd->setMaxSize(UVector2(cegui_reldim(1.0f), cegui_reldim( 1.0f)));
    wnd->setMinSize(UVector2(cegui_reldim(0.1f), cegui_reldim( 0.1f)));

    // As a final step in the initialisation of our sample window, we set the window's
    // text to "Hello World!", so that this text will appear as the caption in the
    // FrameWindow's titlebar.
    wnd->setText("Hello World!");

    // return true so that the samples framework knows that initialisation was a
    // success, and that it should now run the sample.
   


	}

	
}
