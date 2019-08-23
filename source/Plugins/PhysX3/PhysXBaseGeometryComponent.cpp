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

#include "Plugins/PhysX3/PhysXBaseGeometryComponent.h"
#include "Plugins/PhysX3/PhysXBodyComponent.h"
#include "Plugins/PhysX3/PhysXPhysicsSceneManager.h"

namespace GASS
{

	PhysXBaseGeometryComponent::PhysXBaseGeometryComponent() :m_StaticActor(NULL),
		m_Shape(NULL),
		m_Offset(0,0,0),
		m_SimulationCollision(true),
		m_SizeFromMesh(true)
	{
	}

	PhysXBaseGeometryComponent::~PhysXBaseGeometryComponent()
	{
		
	}

	void PhysXBaseGeometryComponent::Enable()
	{

	}

	void PhysXBaseGeometryComponent::OnDelete()
	{
		if(!m_Body) // else removed by body?
		{
			if(m_Shape)
				m_Shape->release();
		}
		m_Shape = NULL;
	}

	void PhysXBaseGeometryComponent::RegisterReflection()
	{
		RegisterProperty<Vec3>("Offset", &GASS::PhysXBaseGeometryComponent::GetOffset, &GASS::PhysXBaseGeometryComponent::SetOffset);
		RegisterProperty<bool>("SizeFromMesh", &GASS::PhysXBaseGeometryComponent::GetSizeFromMesh, &GASS::PhysXBaseGeometryComponent::SetSizeFromMesh);
		RegisterMember("SimulationCollision", &GASS::PhysXBaseGeometryComponent::m_SimulationCollision);
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
				GetSceneObject()->RegisterForMessage(typeid(GeometryChangedEvent), MESSAGE_FUNC(PhysXBaseGeometryComponent::OnLoad));
			else
				GetSceneObject()->RegisterForMessage(typeid(PhysicsBodyLoadedEvent), MESSAGE_FUNC(PhysXBaseGeometryComponent::OnLoad));
		}
		else
		{
			if(m_SizeFromMesh && geom)
				GetSceneObject()->RegisterForMessage(typeid(GeometryChangedEvent), MESSAGE_FUNC(PhysXBaseGeometryComponent::OnLoad));
			else
			{
				if(location)
					GetSceneObject()->RegisterForMessage(typeid(LocationLoadedEvent), MESSAGE_FUNC(PhysXBaseGeometryComponent::OnLoad));
				//else
				//	GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXBaseGeometryComponent::OnLoadComponents,LoadComponentsMessage,1));
			}
		}

		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXBaseGeometryComponent::OnTransformationChanged,TransformationChangedEvent, 0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXBaseGeometryComponent::OnCollisionSettings,CollisionSettingsRequest ,0));
	}


	void PhysXBaseGeometryComponent::OnLoad(MessagePtr message)
	{
		if(m_Shape)
			m_Shape->release();
		m_Shape = CreateShape();
		
		if(m_Shape == NULL)
		{
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Failed to create shape","PhysXBaseGeometryComponent::OnLoad");
		}

		m_Shape->userData = this;
		

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

		if(!(geom->GetGeometryFlags() & GEOMETRY_FLAG_TRANSPARENT_OBJECT))
			m_Shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE,m_SimulationCollision);

		if(m_Body)
		{
			physx::PxReal mass = m_Body->GetMass();
			const physx::PxVec3 localPos = PxConvert::ToPx(m_Offset);
			physx::PxRigidBodyExt::setMassAndUpdateInertia(*m_Body->GetPxRigidDynamic(), mass,&localPos);
			//physx::PxRigidBodyExt::updateMassAndInertia(*m_Body->GetPxRigidDynamic(), mass,&localPos);
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
	
	void PhysXBaseGeometryComponent::OnTransformationChanged(TransformationChangedEventPtr message)
	{
		Vec3 pos = message->GetPosition();
		SetPosition(pos);
		Quaternion rot = message->GetRotation();
		SetRotation(rot);
	}

	void PhysXBaseGeometryComponent::OnCollisionSettings(CollisionSettingsRequestPtr message)
	{
		bool value = message->EnableCollision();
		if(value)
			Enable();
		else
			Disable();
	}

	void PhysXBaseGeometryComponent::SetPosition(const Vec3 &pos)
	{
		
	}

	void PhysXBaseGeometryComponent::SetRotation(const Quaternion &rot)
	{
		
	}

	void PhysXBaseGeometryComponent::SetScale(const Vec3 &value)
	{

	}
}
