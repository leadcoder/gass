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


namespace GASS
{
	class CEGUISystem : public Reflection<CEGUISystem, SimSystem>
	{
	public:
		CEGUISystem();
		virtual ~CEGUISystem();
		static void RegisterReflection();
		virtual void OnCreate();
	protected:
		void OnInit(MessagePtr message);
		void OnShutdown(MessagePtr message);
	};

	typedef boost::shared_ptr<CEGUISystem> CEGUISystemPtr;
}

#endif