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

#include "Sim/GASSCommon.h"
#include "Core/MessageSystem/GASSBaseMessage.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Math/GASSVector.h"
#include "Core/Math/GASSQuaternion.h"

namespace GASS
{
	class Scene;
	class SceneObject;

	
	typedef boost::shared_ptr<Scene> ScenePtr;
	typedef boost::shared_ptr<SceneObject> SceneObjectPtr;
	

	/**
	Default message used by the SimSystemManager
	Sim system messages are used to communicate with systems and
	used by systems to notify listeners about critcal system events
	To send a SimSystemMessage you have to get hold of the
	SimSystemManager and then post a message, ex:
	SimEngine::Get().GetSimSystemManager()->PostMessage(MessagePtr(new DebugPrintMessage("Testing")))
	*/

	class InitSystemMessage : public BaseMessage
	{
	public:
		InitSystemMessage (SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay)
			{ }
	private:

	};
	typedef boost::shared_ptr<InitSystemMessage> InitSystemMessagePtr;



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
		This message is posted by the Scene class before the scenes are loaded.
	*/

	class SceneAboutToLoadNotifyMessage : public BaseMessage
	{
	public:
		SceneAboutToLoadNotifyMessage(ScenePtr scene, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay) ,
			  m_Scene(scene){}

		  ScenePtr GetScene() const {return m_Scene;}
	private:
		ScenePtr m_Scene;
	};

	typedef boost::shared_ptr<SceneAboutToLoadNotifyMessage> SceneAboutToLoadNotifyMessagePtr;

	
	
	/**
		This message is posted by the Scene class after a scene is loaded.
	*/
	class SceneLoadedNotifyMessage : public BaseMessage
	{
	public:
		SceneLoadedNotifyMessage(ScenePtr scene,SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay) ,
			  m_Scene(scene){}

		  ScenePtr GetScene() const {return m_Scene;}
	private:
		ScenePtr m_Scene;
	};
	typedef boost::shared_ptr<SceneLoadedNotifyMessage> SceneLoadedNotifyMessagePtr;


	class SceneUnloadNotifyMessage : public BaseMessage
	{
	public:
		SceneUnloadNotifyMessage(ScenePtr scene, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay) ,
			  m_Scene(scene){}

		  ScenePtr GetScene() const {return m_Scene;}
	private:
		ScenePtr m_Scene;
	};
	typedef boost::shared_ptr<SceneUnloadNotifyMessage> SceneUnloadNotifyMessagePtr;


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
