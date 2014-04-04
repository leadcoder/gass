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

#include "Plugins/PhysX3/PhysXWheelComponent.h"
#include "Plugins/PhysX3/PhysXBodyComponent.h"
#include "Plugins/PhysX3/PhysXPhysicsSceneManager.h"
#include "Plugins/PhysX3/PhysXPhysicsSystem.h"
#include "Plugins/PhysX3/PhysXConvexGeometryComponent.h"
#include "Sim/GASS.h"
using namespace physx;
namespace GASS
{
	PhysXWheelComponent::PhysXWheelComponent() :
		m_Mass(20),
		m_MaxHandBrakeTorque(4000.0f),
		m_MaxBrakeTorque(1500.0f),
		m_MaxSteer(60.0f),
		m_DampingRate(0.25f),
		m_SuspensionMaxCompression(0.3f),
		m_SuspensionMaxDroop(0.1f),
		m_SuspensionSpringStrength(35000.0f),
		m_SuspensionSpringDamperRate(4500.0f),
		m_TireLongitudinalStiffnessPerUnitGravity(1000.0f),
		m_Initialized(false),
		m_TireType("SLICKS")
	{
	
	}

	PhysXWheelComponent::~PhysXWheelComponent()
	{

	}

	void PhysXWheelComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("PhysXWheelComponent",new Creator<PhysXWheelComponent, Component>);
		REG_PROPERTY(float,Mass,PhysXWheelComponent)
		REG_PROPERTY(float,MaxBrakeTorque,PhysXWheelComponent)
		REG_PROPERTY(float,MaxHandBrakeTorque,PhysXWheelComponent)
		REG_PROPERTY(float,MaxSteer,PhysXWheelComponent)
		REG_PROPERTY(float,DampingRate,PhysXWheelComponent)
		REG_PROPERTY(float,SuspensionMaxCompression,PhysXWheelComponent)
		REG_PROPERTY(float,SuspensionMaxDroop,PhysXWheelComponent)
		REG_PROPERTY(float,SuspensionSpringStrength,PhysXWheelComponent)
		REG_PROPERTY(float,SuspensionSpringDamperRate,PhysXWheelComponent)
		REG_PROPERTY(float,TireLongitudinalStiffnessPerUnitGravity,PhysXWheelComponent)
		REG_PROPERTY(std::string,TireType,PhysXWheelComponent)
	}

	void PhysXWheelComponent::ComputeWheelDim(PxConvexMesh* wheel_convex_mesh, PxF32 &wheel_width, PxF32 &wheel_rad)
	{
		const PxU32 num_verts = wheel_convex_mesh->getNbVertices();
		const PxVec3* verts = wheel_convex_mesh->getVertices();
		PxVec3 wheel_min(PX_MAX_F32,PX_MAX_F32,PX_MAX_F32);
		PxVec3 wheel_max(-PX_MAX_F32,-PX_MAX_F32,-PX_MAX_F32);
		for(PxU32 i=0; i < num_verts;i++)
		{
			wheel_min.x=PxMin(wheel_min.x,verts[i].x);
			wheel_min.y=PxMin(wheel_min.y,verts[i].y);
			wheel_min.z=PxMin(wheel_min.z,verts[i].z);
			wheel_max.x=PxMax(wheel_max.x,verts[i].x);
			wheel_max.y=PxMax(wheel_max.y,verts[i].y);
			wheel_max.z=PxMax(wheel_max.z,verts[i].z);
		}
		wheel_width = wheel_max.x - wheel_min.x;
		wheel_rad = PxMax(wheel_max.y,wheel_max.z);
	}

	void PhysXWheelComponent::OnInitialize()
	{
		//set priority to 1, convex geometry has 0 and will be cooked when we are called
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXWheelComponent::OnGeometryChanged,GeometryChangedEvent,1));
	}

	void PhysXWheelComponent::OnGeometryChanged(GeometryChangedEventPtr message)
	{
		m_WheelData.mMass = m_Mass;
		m_WheelData.mMaxBrakeTorque = m_MaxBrakeTorque;
		m_WheelData.mMaxHandBrakeTorque = m_MaxHandBrakeTorque;
		m_WheelData.mMaxSteer  = Math::Deg2Rad(m_MaxSteer);
		m_WheelData.mDampingRate = m_DampingRate;
		m_SuspensionData.mMaxCompression = m_SuspensionMaxCompression;
		m_SuspensionData.mMaxDroop = m_SuspensionMaxDroop;
		m_SuspensionData.mSpringStrength = m_SuspensionSpringStrength ;
		m_SuspensionData.mSpringDamperRate = m_SuspensionSpringDamperRate;
		m_TireData.mLongitudinalStiffnessPerUnitGravity = m_TireLongitudinalStiffnessPerUnitGravity;
		//m_SuspensionData.mSprungMass = 1.0;
		
		PhysXConvexGeometryComponentPtr cgc = GetSceneObject()->GetFirstComponentByClass<PhysXConvexGeometryComponent>();
		PxF32 wheel_width;
		PxF32 wheel_rad;
		ComputeWheelDim(cgc->GetConvexMesh().m_ConvexMesh,wheel_width,wheel_rad);
		PxF32 wheel_MOI = 0.5f*m_WheelData.mMass*wheel_rad*wheel_rad;
	
		m_WheelData.mRadius = wheel_rad;
		m_WheelData.mMOI = wheel_MOI;
		m_WheelData.mWidth = wheel_width;
		
		PhysXPhysicsSystemPtr system = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<PhysXPhysicsSystem>();
		m_TireData.mType = system->GetTireIDFromName(m_TireType);
		m_Initialized = true;
	}
}
