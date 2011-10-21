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

#ifndef CORE_SYSTEM_MESSAGES_H
#define CORE_SYSTEM_MESSAGES_H

#include "Sim/Common.h"
#include "Core/MessageSystem/BaseMessage.h"
#include "Core/MessageSystem/IMessage.h"
#include "Core/Math/Vector.h"
#include "Core/Math/Quaternion.h"
#include <string>

namespace GASS
{
	class Scenario;
	class ScenarioScene;
	class SceneObject;

	typedef boost::shared_ptr<ScenarioScene> ScenarioScenePtr;
	typedef boost::shared_ptr<Scenario> ScenarioPtr;
	typedef boost::shared_ptr<SceneObject> SceneObjectPtr;
	

	/**
	Default message used by the SimSystemManager
	Sim system messages are used to communicate with systems and
	used by systems to notify listeners about critcal system events
	To send a SimSystemMessage you have to get hold of the
	SimSystemManager and then post a message, ex:
	SimEngine::Get().GetSimSystemManager()->PostMessage(MessagePtr(new DebugPrintMessage("Testing")))
	*/

	class InitMessage : public BaseMessage
	{
	public:
		InitMessage (SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay)
			{ }
	private:

	};
	typedef boost::shared_ptr<InitMessage> InitMessagePtr;



	/**
	Message that can be posted by anyone to request that a new debug messages should be visualized during one frame.
	*/
	class DebugPrintMessage : public BaseMessage
	{
	public:
		DebugPrintMessage(const std::string &text, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay),
			  m_Text(text)  { }
		  std::string GetText()const {return m_Text;}

	private:
		std::string m_Text;

	};
	typedef boost::shared_ptr<DebugPrintMessage> DebugPrintMessagePtr;

	


	/**
		This message is posted by the Scenario class before the scenario scenes are loaded.
	*/

	class ScenarioAboutToLoadNotifyMessage : public BaseMessage
	{
	public:
		ScenarioAboutToLoadNotifyMessage(ScenarioPtr scenario, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay) ,
			  m_Scenario(scenario){}

		  ScenarioPtr GetScenario() const {return m_Scenario;}
	private:
		ScenarioPtr m_Scenario;
	};

	typedef boost::shared_ptr<ScenarioAboutToLoadNotifyMessage> ScenarioAboutToLoadNotifyMessagePtr;

	/**
		This message is posted by the ScenarioScene class before the scene objects are loaded.
		Suscribe to this message if you want to get hold of sceario scenes before all scene objects are loaded. This
		can be usefull if you want to modify, add or save some objects loaded by the scenario scene.
	*/

	class ScenarioSceneAboutToLoadNotifyMessage : public BaseMessage
	{
	public:
		ScenarioSceneAboutToLoadNotifyMessage(ScenarioScenePtr scenario_scene, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay) ,
			  m_ScenarioScene(scenario_scene){}

		  ScenarioScenePtr GetScenarioScene() const {return m_ScenarioScene;}
	private:
		ScenarioScenePtr m_ScenarioScene;
	};
	typedef boost::shared_ptr<ScenarioSceneAboutToLoadNotifyMessage> ScenarioSceneAboutToLoadNotifyMessagePtr;

	
	/**
		This message is posted by the Scenario class after a scenario scene is loaded.
	*/
	class ScenarioSceneLoadedNotifyMessage : public BaseMessage
	{
	public:
		ScenarioSceneLoadedNotifyMessage(ScenarioScenePtr scenario_scene,SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay) ,
			  m_ScenarioScene(scenario_scene){}

		  ScenarioScenePtr GetScenarioScene() const {return m_ScenarioScene;}
	private:
		ScenarioScenePtr m_ScenarioScene;
	};
	typedef boost::shared_ptr<ScenarioSceneLoadedNotifyMessage> ScenarioSceneLoadedNotifyMessagePtr;


	class ScenarioSceneUnloadNotifyMessage : public BaseMessage
	{
	public:
		ScenarioSceneUnloadNotifyMessage(ScenarioScenePtr scenario_scene, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay) ,
			  m_ScenarioScene(scenario_scene){}

		  ScenarioScenePtr GetScenarioScene() const {return m_ScenarioScene;}
	private:
		ScenarioScenePtr m_ScenarioScene;
	};
	typedef boost::shared_ptr<ScenarioSceneUnloadNotifyMessage> ScenarioSceneUnloadNotifyMessagePtr;


	class RequestTimeStepMessage : public BaseMessage
	{
	public:
		RequestTimeStepMessage  (const double &step, SenderID sender_id = -1, double delay= 0) : BaseMessage(sender_id , delay), m_TimeStep(step)
		{
		}
		double GetTimeStep() const {return m_TimeStep;}
	private:
		double m_TimeStep;
	};
	typedef boost::shared_ptr<RequestTimeStepMessage> RequestTimeStepMessagePtr;

}
#endif
