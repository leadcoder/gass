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

#pragma once

#include "Sim/Common.h"
#include "Sim/Scenario/Scenario.h"
#include "Core/Math/Vector.h"
#include "Core/Reflection/BaseReflectionObject.h"
#include "Core/MessageSystem/Message.h"

class TiXmlElement;

namespace GASS
{
	class ISceneManager;
	class MessageManager;
	class SceneObject;
	class SceneObjectManager;

	typedef boost::shared_ptr<SceneObjectManager> SceneObjectManagerPtr;
	typedef boost::shared_ptr<ISceneManager> SceneManagerPtr;

	class ScenarioUpdateMessage : public Message
	{
	public:
		ScenarioUpdateMessage(int type, int from, double delta_time): Message(type,from) ,m_DeltaTime (delta_time)
		{
			
		}
		virtual ~ScenarioUpdateMessage(){}
		double GetDeltaTime() const {return  m_DeltaTime;}
	private:
		double m_DeltaTime;
	};

	class GASSExport ScenarioScene : public Reflection<ScenarioScene, BaseReflectionObject>
	{
		friend class Scenario;
	public:
		enum ScenarioMessages
		{
			SCENARIO_MESSAGE_LOAD_SCENE_MANAGERS,
			SCENARIO_MESSAGE_UNLOAD_SCENE_MANAGERS,
			SCENARIO_MESSAGE_LOAD_SCENE_OBJECT, // message data: "SceneObject" = SceneObjectPtr 
			SCENARIO_MESSAGE_UNLOAD_SCENE_OBJECT,
			SCENARIO_MESSAGE_UPDATE,
			SCENARIO_MESSAGE_CHANGE_CAMERA
		};

		// Priorities for system loading
		enum
		{ 
			GFX_COMPONENT_LOAD_PRIORITY = 0,
			PHYSICS_COMPONENT_LOAD_PRIORITY = 1,
			GAME_COMPONENT_LOAD_PRIORITY = 2,
			GFX_SYSTEM_LOAD_PRIORITY = 0,
			PHYSICS_SYSTEM_LOAD_PRIORITY = 1,
			GAME_SYSTEM_LOAD_PRIORITY = 1,
		};
	public:
		ScenarioScene();
		virtual ~ScenarioScene();
		static void RegisterReflection();

		Vec3 GetStartPos()const {return m_StartPos;}
		Vec3 GetStartRot()const {return m_StartRot;}
		void SetStartPos(const Vec3 &value) {m_StartPos = value;}
		void SetStartRot(const Vec3 &value) {m_StartRot= value;}
		Vec2 GetRT90Origo() const {return m_RT90Origo;}
		void SetRT90Origo(const Vec2& origo) {m_RT90Origo=origo;}

		Vec3 RT90ToGASSPosition(const Vec3 &pos);
		Vec3 GASSToRT90Position(const Vec3 &pos);

		void LoadXML(TiXmlElement *scene_elem);
		void OnCreate();
		std::string GetName() const {return m_Name;}
		void SetName(const std::string &name) {m_Name = name;}
		void SetOwner(Scenario* scenario) {m_Scenario = scenario;}
		Scenario* GetOwner() {return m_Scenario;}
		//MessageManager* GetMessageManager() {return m_SceneMessageManager;}
		int RegisterForMessage(ScenarioMessages type, int object_id, MessageFunc callback, int priority);
		void UnRegisterForMessage(ScenarioMessages type, int object_id);
		void SendGlobalMessage(MessagePtr message);
		void SendImmediate(MessagePtr message);

		SceneObjectManagerPtr GetObjectManager() {return m_ObjectManager;}
		SceneManagerPtr GetSceneManager(const std::string &type);
		Vec3 GetSceneUp() {return m_Up;}
		Vec3 GetSceneEast() {return m_East;}
		Vec3 GetSceneNorth() {return m_North;}
	protected:

		void SetUpVector(const std::string &value);
		std::string GetUpVector() const;
		void SetEastVector(const std::string &value);
		std::string GetEastVector() const;
		void SetNorthVector(const std::string &value);
		std::string GetNorthVector() const;

		//helpers
		Vec3 GetVector(const std::string &str) const;
		std::string  GetVector(const Vec3 &vec) const;

	
		void OnUpdate(double delta_time);
		void OnLoad();
		SceneManagerPtr LoadSceneManager(TiXmlElement *sm_elem);
		Vec3 m_StartPos;
		Vec3 m_StartRot;
		Vec2 m_RT90Origo;
		std::vector<SceneManagerPtr> m_SceneManagers;
		MessageManager* m_SceneMessageManager;
		Scenario* m_Scenario;
		std::string m_Name;
		SceneObjectManagerPtr m_ObjectManager;

		Vec3 m_Up;
		Vec3 m_North;
		Vec3 m_East;
	};
}

