/****************************************************************************
* This file is part of GASS.                                                *
* See https://github.com/leadcoder/gass                                     *
*                                                                           *
* Copyright (c) 2008-2016 GASS team. See Contributors.txt for details.      *
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

#include "Plugins/PhysX3/PhysXBaseGeometryComponent.h"
#include "PhysXCommon.h"
#include "Sim/GASS.h"
#include "Sim/GASSSceneObjectRef.h"


namespace GASS
{
	class PhysXPhysicsSceneManager;
	typedef GASS_WEAK_PTR<PhysXPhysicsSceneManager> PhysXPhysicsSceneManagerWeakPtr;

	class PhysXBaseJointComponent : public Reflection<PhysXBaseJointComponent,BaseSceneComponent>
	{
	public:
		PhysXBaseJointComponent();
		virtual ~PhysXBaseJointComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual physx::PxJoint* GetJoint() const  = 0;
		//virtual void SceneManagerTick(double delta_time);
	protected:
		void OnBody1Loaded(PhysicsBodyLoadedEventPtr message);
		void OnBody2Loaded(PhysicsBodyLoadedEventPtr message);
		ADD_PROPERTY(bool,Report)

		//get set section
		SceneObjectRef GetBody1() const {return m_Body1;}
		void SetBody1(SceneObjectRef value);
		SceneObjectRef GetBody2()const {return m_Body2;}
		void SetBody2(SceneObjectRef value);
		virtual void CreateJoint() = 0;

		SceneObjectRef m_Body1;
		SceneObjectRef m_Body2;
		bool m_Body1Loaded;
		bool m_Body2Loaded;
	private:
	};
	typedef GASS_SHARED_PTR<PhysXBaseJointComponent> PhysXBaseJointComponentPtr;
}

