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

#include "Plugins/PhysX/PhysXBaseGeometryComponent.h"
#include "Plugins/PhysX/PhysXBodyComponent.h"
#include "Plugins/PhysX/PhysXPhysicsSceneManager.h"

namespace GASS
{

	PhysXBaseGeometryComponent::PhysXBaseGeometryComponent() :
		m_Offset(0,0,0)
		
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
		m_Shape = nullptr;
	}

	void PhysXBaseGeometryComponent::RegisterReflection()
	{
		RegisterGetSet("Offset", &GASS::PhysXBaseGeometryComponent::GetOffset, &GASS::PhysXBaseGeometryComponent::SetOffset);
		RegisterGetSet("SizeFromMesh", &GASS::PhysXBaseGeometryComponent::GetSizeFromMesh, &GASS::PhysXBaseGeometryComponent::SetSizeFromMesh);
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
		
		if(m_Shape == nullptr)
		{
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Failed to create shape","PhysXBaseGeometryComponent::OnLoad");
		}

		m_Shape->userData = this;
		

		//update collision flags
		GeometryComponentPtr geom  = GetGeometry();
		physx::PxFilterData coll_filter_data;
		if(geom)
		{
			GeometryFlags against = GeometryFlagManager::GetMask(geom->GetGeometryFlags());
			coll_filter_data.word0 = geom->GetGeometryFlags();
			coll_filter_data.word1 = against;
			m_Shape->setSimulationFilterData(coll_filter_data);
		}
		else //?
		{

		}

		if(!(geom->GetGeometryFlags() & GEOMETRY_FLAG_TRANSPARENT_OBJECT))
			m_Shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE,m_SimulationCollision);

		if(m_Body)
		{
			physx::PxReal mass = m_Body->GetMass();
			const physx::PxVec3 local_pos = PxConvert::ToPx(m_Offset);
			physx::PxRigidBodyExt::setMassAndUpdateInertia(*m_Body->GetPxRigidDynamic(), mass,&local_pos);
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
