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
	PhysXWheelComponent::PhysXWheelComponent() 
	{
		m_WheelData.mMass = 20;
		m_WheelData.mMaxHandBrakeTorque = 4000.0f;
		m_WheelData.mMaxSteer  = PxPi*0.3333f;
		m_SuspensionData.mMaxCompression = 0.3f;
		m_SuspensionData.mMaxDroop = 0.1f;
		m_SuspensionData.mSpringStrength = 35000.0f;
		m_SuspensionData.mSpringDamperRate = 4500.0f;
	}

	PhysXWheelComponent::~PhysXWheelComponent()
	{

	}

	void PhysXWheelComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("PhysXWheelComponent",new Creator<PhysXWheelComponent, IComponent>);
		//RegisterProperty<float>("Damping", &GASS::PhysXWheelComponent::GetDamping, &GASS::PhysXWheelComponent::SetDamping);
		//RegisterProperty<float>("Strength", &GASS::PhysXWheelComponent::GetStrength, &GASS::PhysXWheelComponent::SetStrength);
		RegisterProperty<float>("Mass", &GASS::PhysXWheelComponent::GetMass, &GASS::PhysXWheelComponent::SetMass);
	}

	float PhysXWheelComponent::GetMass() const
	{
		return m_WheelData.mMass;
	}

	void PhysXWheelComponent::SetMass(float mass)
	{
		m_WheelData.mMass = mass;
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
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysXWheelComponent::OnGeometryChanged,GeometryChangedMessage,1));
	}

	void PhysXWheelComponent::OnGeometryChanged(GeometryChangedMessagePtr message)
	{
		PhysXConvexGeometryComponentPtr cgc = GetSceneObject()->GetFirstComponentByClass<PhysXConvexGeometryComponent>();
		PxF32 wheel_width;
		PxF32 wheel_rad;
		ComputeWheelDim(cgc->GetConvexMesh().m_ConvexMesh,wheel_width,wheel_rad);
		PxF32 wheel_MOI = 0.5f*m_WheelData.mMass*wheel_rad*wheel_rad;
	
		m_WheelData.mRadius = wheel_rad;
		m_WheelData.mMOI = wheel_MOI;
		m_WheelData.mWidth = wheel_width;
		
		PhysXPhysicsSystemPtr system = SimEngine::Get().GetSimSystemManager()->GetFirstSystem<PhysXPhysicsSystem>();
		m_TireData.mType = system->GetTireIDFromName("SLICKS");
	}
}
