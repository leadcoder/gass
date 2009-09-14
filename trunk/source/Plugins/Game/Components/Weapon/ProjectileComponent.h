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

#ifndef PROJECTILE_COMPONENT_H
#define PROJECTILE_COMPONENT_H

#include "Sim/Components/BaseSceneComponent.h"
#include "Sim/Common.h"
#include "Core/Math/Quaternion.h"
#include "Core/MessageSystem/Message.h"
#include "Sim/Systems/Collision/ICollisionSystem.h"
#include "Sim/Scheduling/ITaskListener.h"

namespace GASS
{
	class SceneObject;
	typedef boost::shared_ptr<SceneObject> SceneObjectPtr;
	typedef boost::weak_ptr<SceneObject> SceneObjectWeakPtr;

	class ProjectileComponent :  public Reflection<ProjectileComponent,BaseSceneComponent>, ITaskListener
	{
	public:
		ProjectileComponent();
		virtual ~ProjectileComponent();
		static void RegisterReflection();
		virtual void OnCreate();

		//ITaskListener interface
		void Update(double delta);
		TaskGroup GetTaskGroup() const;

	private:
		void OnLoad(MessagePtr message);
		void OnUnload(MessagePtr message);
		void StepPhysics(double time);
		void OnPositionMessage(MessagePtr message);
		void OnRotationMessage(MessagePtr message);
		void OnPhysicsParameterMessage(MessagePtr message);
		
		void SpawnEffect(const std::string &effect);	

		
		Vec3 m_Velocity;
		float m_TimeToLive;
		float m_TimeLeft;
		std::string m_StartEffectTemplateName;
		std::string m_EndEffectTemplateName;
		int m_DamageType;
		int m_Material;
		int m_HasCollisionEffect;
		int m_MatID;
		int m_DieAfterColl;
		bool m_Hit;
		bool m_HasColHandle;
		float m_TotSquaredDist;
		float m_DamgeRadius;
		float m_ExplodeNearEnemyDistance;
		double m_PhysicsDeltaTime;
		CollisionHandle m_ColHandle;
		CollisionSystemPtr m_ColSys;
		Vec3 m_Pos;
		Quaternion m_Rot;

	};
}
#endif
