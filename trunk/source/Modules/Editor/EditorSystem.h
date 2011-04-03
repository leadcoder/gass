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
#include "Sim/Scheduling/ITaskListener.h"

#include "CEGUI.h"

namespace GASS
{
	class EditorSystem : public Reflection<EditorSystem, SimSystem>, public IMouseListener, public IKeyListener, public GASS::ITaskListener
	{
	public:
		EditorSystem();
		virtual ~EditorSystem();
		static void RegisterReflection();
		virtual void OnCreate();

		//ITaskListener interface
		void Update(double delta);
		GASS::TaskGroup GetTaskGroup() const;
	protected:
		void OnInit(InitMessagePtr  message);
		void OnShutdown(MessagePtr message);
		void OnLoadGUIScript(GUIScriptMessagePtr message);
		void OnLoadScene(ScenarioSceneAboutToLoadNotifyMessagePtr message);
		void OnUnloadScene(ScenarioSceneUnloadNotifyMessagePtr message);
	
		bool HandleLoadScenario(const CEGUI::EventArgs&);
		bool HandleCloseScenario(const CEGUI::EventArgs&);
		bool HandleQuit(const CEGUI::EventArgs&);

		bool HandleScenarioFileDialogOutput(const CEGUI::EventArgs& e);
    	

	

		bool MouseMoved(float x,float y, float z);
		bool MousePressed( int id );
		bool MouseReleased( int id );
		bool KeyPressed(int key, unsigned int text);
		bool KeyReleased( int key, unsigned int text);
		CEGUI::MouseButton ConvertOISButtonToCegui(int buttonID);
		void AddChildren(const std::string &name, CEGUI::Tree* tree,CEGUI::TreeItem* parent);

		std::map<std::string,int> m_Added;
	
	};
	typedef boost::shared_ptr<EditorSystem> EditorSystemPtr;
}

#endif