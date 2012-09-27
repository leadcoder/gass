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

#ifndef MYGUI_SYSTEM
#define MYGUI_SYSTEM

#include "Sim/GASS.h"
namespace MyGUI
{
	class Gui;
}

namespace GASS
{
	class MyGUISystem : public Reflection<MyGUISystem, SimSystem>, public IMouseListener, public IKeyListener
	{
	public:
		MyGUISystem();
		virtual ~MyGUISystem();
		static void RegisterReflection();
		virtual void OnCreate();
		virtual void Update(double delta_time);
	protected:
		void OnInit(InitSystemMessagePtr  message);
		void OnShutdown(MessagePtr message);
		void OnLoadGUIScript(GUIScriptMessagePtr message);
		bool MouseMoved(const MouseData &data);
		bool MousePressed(const MouseData &data, int id );
		bool MouseReleased(const MouseData &data, int id );
		bool KeyPressed(int key, unsigned int text);
		bool KeyReleased( int key, unsigned int text);

		MyGUI::Gui* mGUI;
	};
	typedef boost::shared_ptr<MyGUISystem> MyGUISystemPtr;
}

#endif