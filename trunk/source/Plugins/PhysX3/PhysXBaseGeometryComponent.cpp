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

#include "Plugins/PhysX3/PhysXBaseGeometryComponent.h"
#include "Plugins/PhysX3/PhysXBodyComponent.h"
#include "Plugins/PhysX3/PhysXPhysicsSystem.h"
#include "Plugins/PhysX3/PhysXPhysicsSceneManager.h"

namespace GASS
{

	PhysXBaseGeometryComponent::PhysXBaseGeometryComponent() :m_StaticActor(NULL),
		m_Shape(NULL),
		m_Offset(0,0,0)
	{
	}

	PhysXBaseGeometryComponent::~PhysXBaseGeometryComponent()
	{
		if(m_Shape)
			m_Shape->release();
	}

	void PhysXBaseGeometryComponent::Enable()
	{

	}

	void PhysXBaseGeometryComponent::RegisterReflection()
	{
		RegisterProperty<Vec3>("Offset", &GASS::PhysXBaseGeometryComponent::GetOffset, &GASS::PhysXBaseGeometryComponent::SetOffset);
		RegisterProperty<bool>("SizeFromMesh", &GASS::PhysXBaseGeometryComponent::GetSizeFromMesh, &GASS::PhysXBaseGeometryComponent::SetSizeFromMesh);
	//	RegisterProperty<>("Material", &GASS::PhysXBaseGeometryComponent::GetSizeFromMesh, &GASS::PhysXBaseGeometryComponent::SetSizeFromMesh);
		
		//RegisterProperty<bool>("Debug", &GASS::PhysXBaseGeometryComponent::GetDebug, &GASS::PhysXBaseGeometryComponent::SetDebug);
	}

	void PhysXBaseGeometryComponent::OnInitialize()
	{
		//Try to find out when to load
		m_Body = GetSceneObject()->GetFirstComponentByClass<PhysXBodyComponent>();
		LocationComponentPtr location  = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
		GeometryComponentPtr geom  = GetSceneObject()->GetFirstComponentByClass<IGeometryComponent>();
		if(m_Body)
		{
			if(m_SizeFromMesh)
				
				GetSceneObject()->RegisterForMessage(typeid(GeometryChangedMessage), MESSAGE_FUNC(PhysXBaseGeometryComponent::OnLoad));
			else
				GetSceneObject()->RegisterForMessage(typeid(BodyLoadedMessage), MESSAGE_FUNC(PhysXBaseGeometryComponent::OnLoad));
		}
		else
		{
			if(m_SizeFromMesh && geom)
				GetSceneObject()->RegisterForMessage(typeid(GeometryChangedMessage), MESSAGE_FUNC(PhysXBaseGeometryComponent::OnLoad));
			else
			{
				if(location)
					GetSceneObject()->RegisterForMessage(typeid(LocationLoadedMessage), MESSAGE_FUNC(PhysXBaseGeometryComponent::OnLoad));
				//else
				//	GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXBaseGeometryComponent::OnLoadComponents,LoadComponentsMessage,1));
			}
		}

		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXBaseGeometryComponent::OnTransformationChanged,TransformationNotifyMessage, 0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXBaseGeometryComponent::OnCollisionSettings,CollisionSettingsMessage ,0));
	}


	void PhysXBaseGeometryComponent::OnLoad(MessagePtr message)
	{
		if(m_Shape)
			m_Shape->release();
		m_Shape = CreateShape();

		//update collision flags
		GeometryComponentPtr geom  = GetGeometry();
		physx::PxFilterData collFilterData;
		if(geom)
		{
			GeometryFlags against = GeometryFlagManager::GetMask(geom->GetGeometryFlags());
			collFilterData.word0 = geom->GetGeometryFlags();
			collFilterData.word1 = against;
			m_Shape->setSimulationFilterData(collFilterData);
		}
		else //?
		{

		}
		//m_Shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE,true);
		if(m_Body)
		{
			physx::PxReal mass = m_Body->GetMass();
			const physx::PxVec3 localPos = physx::PxVec3(m_Offset.x,m_Offset.y,m_Offset.z);
			physx::PxRigidBodyExt::updateMassAndInertia(*m_Body->GetPxActor(), mass,&localPos);
		}
	}

	bool  PhysXBaseGeometryComponent::GetSizeFromMesh() const
	{
		return m_SizeFromMesh;
	}

	void PhysXBaseGeometryComponent::SetSizeFromMesh(bool value) 
	{
		m_SizeFromMesh = value;
	}

	GeometryComponentPtr PhysXBaseGeometryComponent::GetGeometry() const 
	{
		GeometryComponentPtr geom;
		geom = GetSceneObject()->GetFirstComponentByClass<IGeometryComponent>();
		return geom;
	}

	
	void PhysXBaseGeometryComponent::OnTransformationChanged(TransformationNotifyMessagePtr message)
	{
		Vec3 pos = message->GetPosition();
		SetPosition(pos);
		Quaternion rot = message->GetRotation();
		SetRotation(rot);
	}

	void PhysXBaseGeometryComponent::OnCollisionSettings(CollisionSettingsMessagePtr message)
	{
		bool value = message->EnableCollision();
		if(value)
			Enable();
		else
			Disable();
	}

	void PhysXBaseGeometryComponent::SetPosition(const Vec3 &pos)
	{
		if(m_Body == NULL)
		{

		}
	}

	void PhysXBaseGeometryComponent::SetRotation(const Quaternion &rot)
	{
		if(m_Body == NULL)
		{

		}
	}

	void PhysXBaseGeometryComponent::SetScale(const Vec3 &value)
	{

	}
}
