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

#ifndef CE_GUI_MANAGER
#define CE_GUI_MANAGER

#include "Core/MessageSystem/IMessage.h"
#include "Sim/Scenario/Scene/BaseSceneManager.h"
#include "Sim/Systems/SimSystem.h"
#include "Sim/Systems/SimSystemMessages.h"
#include "Sim/Systems/Input/IInputSystem.h"

#include "CEGUI.h"

namespace GASS
{
	class CEGUISystem : public Reflection<CEGUISystem, SimSystem>, public IMouseListener, public IKeyListener
	{
	public:
		CEGUISystem();
		virtual ~CEGUISystem();
		static void RegisterReflection();
		virtual void OnCreate();
	protected:
		void OnInit(InitMessagePtr  message);
		void OnShutdown(MessagePtr message);
		void OnLoadGUIScript(GUIScriptMessagePtr message);
		bool MouseMoved(float x,float y, float z);
		bool MousePressed( int id );
		bool MouseReleased( int id );
		bool KeyPressed(int key, unsigned int text);
		bool KeyReleased( int key, unsigned int text);
		CEGUI::MouseButton ConvertOISButtonToCegui(int buttonID);
	};
	typedef boost::shared_ptr<CEGUISystem> CEGUISystemPtr;
}

#endif