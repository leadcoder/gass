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

#include "Sim/GASS.h"
#include "Plugins/OSG/Components/OSGCameraComponent.h"
#include "tbb/spin_mutex.h"
#include <osg/Geode>

namespace GASS
{
	class OSGCollisionSystem : public Reflection<OSGCollisionSystem , SimSystem> , public ICollisionSystem, public ITaskListener
	{
	public:
		typedef std::map<CollisionHandle,CollisionRequest> RequestMap;
		typedef std::map<CollisionHandle,CollisionResult> ResultMap;
	public:
		OSGCollisionSystem();
		virtual ~OSGCollisionSystem();
		static void RegisterReflection();
		virtual void Init();
		std::string GetSystemName() const {return "OSGCollisionSystem";}
		CollisionHandle Request(const CollisionRequest &request);
		bool Check(CollisionHandle handle, CollisionResult &result);
		void Force(CollisionRequest &request, CollisionResult &result) const;
		Float GetHeight(ScenePtr scene, const Vec3 &pos, bool absolute=true) const;
		//ITaskListener interface
		void Update(double delta);
		TaskGroup GetTaskGroup() const;
	private:
		void ProcessRaycast(CollisionRequest *request,CollisionResult *result, osg::Node *node) const;
		void OnUnloadScene(SceneUnloadNotifyMessagePtr message);
		void OnLoadScene(SceneAboutToLoadNotifyMessagePtr message);
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

