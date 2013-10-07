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
#include <tbb/spin_mutex.h>
#include <osg/Geode>

namespace GASS
{
	class OSGCollisionSceneManager : public Reflection<OSGCollisionSceneManager, BaseSceneManager> , public ICollisionSceneManager
	{
	public:
		typedef std::map<CollisionHandle,CollisionRequest> RequestMap;
		typedef std::map<CollisionHandle,CollisionResult> ResultMap;
	public:
		OSGCollisionSceneManager();
		virtual ~OSGCollisionSceneManager();
		static void RegisterReflection();
		virtual void OnCreate();
		virtual void OnInit();
		virtual void OnShutdown();
		virtual void SystemTick(double delta_time);
		virtual bool GetSerialize() const {return false;}

		//ICollisionSceneManager
		virtual CollisionHandle Request(const CollisionRequest &request);
		virtual bool Check(CollisionHandle handle, CollisionResult &result);
		virtual void Force(CollisionRequest &request, CollisionResult &result) const;
		//virtual Float GetHeight(const Vec3 &pos, bool absolute=true) const;
	private:
		void ProcessRaycast(CollisionRequest *request,CollisionResult *result, osg::Node *node) const;
		RequestMap m_RequestMap;
		ResultMap m_ResultMap;
		unsigned int m_HandleCount;
		tbb::spin_mutex m_RequestMutex;
		tbb::spin_mutex m_ResultMutex;
	};
	typedef SPTR<OSGCollisionSceneManager> OSGCollisionSceneManagerPtr;


}

