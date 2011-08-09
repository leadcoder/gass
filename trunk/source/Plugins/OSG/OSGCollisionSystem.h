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


#include <map>

#include "Sim/Systems/Collision/ICollisionSystem.h"
#include "Sim/Scenario/Scene/SceneObjectMessages.h"
#include "Sim/Systems/SimSystemMessages.h"
#include "Sim/Scenario/Scene/ScenarioSceneMessages.h"
#include "Sim/Scheduling/ITaskListener.h"
#include "Core/System/BaseSystem.h"
#include "Plugins/OSG/Components/OSGCameraComponent.h"
#include "tbb/spin_mutex.h"
#include <osg/Geode>

namespace GASS
{
	class OSGCollisionSystem : public Reflection<OSGCollisionSystem , BaseSystem> , public boost::enable_shared_from_this<OSGCollisionSystem>, public ICollisionSystem, public ITaskListener, public IMessageListener
	{
	public:
		typedef std::map<CollisionHandle,CollisionRequest> RequestMap;
		typedef std::map<CollisionHandle,CollisionResult> ResultMap;
	public:
		OSGCollisionSystem();
		virtual ~OSGCollisionSystem();
		static void RegisterReflection();
		virtual void OnCreate();
		SystemType GetSystemType() const {return "CollisionSystem";}
		CollisionHandle Request(const CollisionRequest &request);
		bool Check(CollisionHandle handle, CollisionResult &result);
		void Force(CollisionRequest &request, CollisionResult &result);
		Float GetHeight(ScenarioScenePtr scene, const Vec3 &pos, bool absolute=true) const;
		//void Process();

		//ITaskListener interface
		void Update(double delta);
		TaskGroup GetTaskGroup() const;

	private:
		void ProcessRaycast(CollisionRequest *request,CollisionResult *result, osg::Node *node) const;
		void OnUnloadScene(ScenarioSceneUnloadNotifyMessagePtr message);
		void OnLoadScene(ScenarioSceneAboutToLoadNotifyMessagePtr message);
		void OnChangeCamera(ChangeCameraMessagePtr message);
		RequestMap m_RequestMap;
		ResultMap m_ResultMap;
		unsigned int m_HandleCount;
		tbb::spin_mutex m_RequestMutex;
		tbb::spin_mutex m_ResultMutex;
		OSGCameraComponentWeakPtr m_CurrentCamera;
	};
	typedef boost::shared_ptr<OSGCollisionSystem> OSGCollisionSystemPtr;


}

