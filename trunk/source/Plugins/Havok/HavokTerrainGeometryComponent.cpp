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

#ifdef WIN32
#define NOMINMAX
#include <algorithm>
#endif

#include "Plugins/Havok/HavokTerrainGeometryComponent.h"
#include "Plugins/Havok/HavokPhysicsSceneManager.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/ComponentSystem/BaseComponentContainerTemplateManager.h"

#include "Core/MessageSystem/MessageManager.h"
#include "Core/Math/AABox.h"
#include "Core/Utils/Log.h"
#include "Sim/Scenario/Scenario.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Scenario/Scene/SceneObjectManager.h"
#include "Sim/Scenario/Scene/SceneObjectTemplate.h"
#include "Sim/Components/Graphics/Geometry/IGeometryComponent.h"
#include "Sim/Components/Graphics/Geometry/IMeshComponent.h"
#include "Sim/Components/Graphics/Geometry/ITerrainComponent.h"
#include "Sim/Components/Graphics/ILocationComponent.h"
#include "Sim/SimEngine.h"
#include <boost/bind.hpp>

namespace GASS
{

	class CoarseSampledHeightFieldShape: public hkpSampledHeightFieldShape
	{
	public:
		CoarseSampledHeightFieldShape( const hkpSampledHeightFieldBaseCinfo& ci, HavokTerrainGeometryComponent* tgc )
			:	hkpSampledHeightFieldShape(ci),
			m_TGC(tgc)
		{
		}

		// Generate a rough terrain
		HK_FORCE_INLINE hkReal getHeightAtImpl( int x, int z ) const
		{
			// Lookup data and return a float
			// We scale the data artificially by 20.0f to make it look interesting
			return m_TGC->GetTerrainHeight(x,z);
			//return 20.0f * hkReal(m_data[x * m_zRes + z]) / hkReal( hkUint16(-1) );
		}

		// Assuming each heightfield quad is defined as four points { 00, 01, 11, 10 },
		// this should return true if the two triangles share the edge p00-p11.
		// Otherwise it should return false if the triangles share the edge p01-p10
		HK_FORCE_INLINE hkBool getTriangleFlipImpl() const
		{	
			return false;
		}

		virtual void collideSpheres( const CollideSpheresInput& input, SphereCollisionOutput* outputArray) const
		{
			hkSampledHeightFieldShape_collideSpheres(*this, input, outputArray);
		}

	private:
		HavokTerrainGeometryComponent* m_TGC;
	};

	HavokTerrainGeometryComponent::HavokTerrainGeometryComponent():
	m_Friction(1),
		m_CollisionCategory(1),
		m_CollisionBits(1),
		m_Debug(false),
		m_Shape(NULL)
	{

	}

	HavokTerrainGeometryComponent::~HavokTerrainGeometryComponent()
	{
		if(m_Shape)
			m_Shape->removeReference();
	}

	void HavokTerrainGeometryComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("PhysicsTerrainGeometryComponent",new Creator<HavokTerrainGeometryComponent, IComponent>);
		RegisterProperty<long int>("CollisionBits", &GASS::HavokTerrainGeometryComponent::GetCollisionBits, &GASS::HavokTerrainGeometryComponent::SetCollisionBits);
		RegisterProperty<long int>("CollisionCategory", &GASS::HavokTerrainGeometryComponent::GetCollisionCategory, &GASS::HavokTerrainGeometryComponent::SetCollisionCategory);
	}

	void HavokTerrainGeometryComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(HavokTerrainGeometryComponent::OnLoad,LoadPhysicsComponentsMessage ,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(HavokTerrainGeometryComponent::OnCollisionSettings,CollisionSettingsMessage ,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(HavokTerrainGeometryComponent::OnGeometryChanged,GeometryChangedMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(HavokTerrainGeometryComponent::OnPhysicsDebug,PhysicsDebugMessage,0));
	}

	void HavokTerrainGeometryComponent::OnLoad(LoadPhysicsComponentsMessagePtr message)
	{
		HavokPhysicsSceneManagerPtr scene_manager = boost::shared_static_cast<HavokPhysicsSceneManager> (message->GetPhysicsSceneManager());
		assert(scene_manager);
		m_SceneManager = scene_manager;
	}

	void HavokTerrainGeometryComponent::OnGeometryChanged(GeometryChangedMessagePtr message)
	{
		TerrainComponentPtr terrain = GetTerrainComponent();
		GeometryComponentPtr geom = boost::shared_dynamic_cast<IGeometryComponent>(terrain);

		//save raw point for fast height access, not thread safe!!
		m_TerrainGeom = terrain.get();

		SetCollisionBits(m_CollisionBits);
		SetCollisionCategory(m_CollisionCategory);


		if(terrain)
		{
			m_TerrainBounds = geom->GetBoundingBox();
			int samples_x = terrain->GetSamplesX();
			int samples_z = terrain->GetSamplesZ();
			Float size_x = m_TerrainBounds.m_Max.x - m_TerrainBounds.m_Min.x;
			Float size_z = m_TerrainBounds.m_Max.z - m_TerrainBounds.m_Min.z;
			m_SampleWidth = size_x/(samples_x-1);
			m_SampleHeight = size_z/(samples_z-1);

			HavokPhysicsSceneManagerPtr(m_SceneManager)->GetWorld()->lock();

			
			hkpSampledHeightFieldBaseCinfo ci;
			ci.m_xRes = samples_x;
			ci.m_zRes = samples_z;
			ci.m_scale.set(size_x/(samples_x-1),1.0f,size_z/(samples_z-1));

			m_Shape = new CoarseSampledHeightFieldShape( ci , this);

			// Now that we have a shape all we need is the fixed body to represent it in the 
			// the simulation. Standard rigid body setup.
			{
					hkpRigidBodyCinfo rci;
					rci.m_motionType = hkpMotion::MOTION_FIXED;
					rci.m_shape = m_Shape;
					rci.m_friction = 1.0f;

					hkpRigidBody* body = new hkpRigidBody( rci );

					HavokPhysicsSceneManagerPtr(m_SceneManager)->GetWorld()->addEntity(body);
					body->removeReference();
			}

			// Build the coarse tree
			int coarseness = 3;
			m_Shape->buildCoarseMinMaxTree(coarseness);
			HavokPhysicsSceneManagerPtr(m_SceneManager)->GetWorld()->unlock();
		}
	}

	TerrainComponentPtr HavokTerrainGeometryComponent::GetTerrainComponent() const 
	{
		TerrainComponentPtr geom;
		if(m_GeometryTemplate != "")
		{
			geom = boost::shared_dynamic_cast<ITerrainComponent>(GetSceneObject()->GetComponent(m_GeometryTemplate));
		}
		else geom = GetSceneObject()->GetFirstComponentByClass<ITerrainComponent>();
		return geom;
	}





	Float HavokTerrainGeometryComponent::GetTerrainHeight(unsigned int x,unsigned int z)
	{
		Float world_x = x * m_SampleWidth + m_TerrainBounds.m_Min.x;
		Float world_z = z * m_SampleWidth + m_TerrainBounds.m_Min.z;
		Float h = m_TerrainGeom->GetHeight(world_x,world_z);
		return h;
	}

	long int HavokTerrainGeometryComponent::GetCollisionBits() const 
	{
		return m_CollisionBits;
	}

	void HavokTerrainGeometryComponent::SetCollisionBits(long int value)
	{
		m_CollisionBits = value;

	}

	void HavokTerrainGeometryComponent::OnCollisionSettings(CollisionSettingsMessagePtr message)
	{
		bool value = message->EnableCollision();
		if(value)
			Enable();
		else
			Disable();
	}

	void HavokTerrainGeometryComponent::Disable()
	{

	}

	void HavokTerrainGeometryComponent::Enable()
	{

	}

	long int HavokTerrainGeometryComponent::GetCollisionCategory() const 
	{
		return m_CollisionCategory;
	}

	void HavokTerrainGeometryComponent::SetCollisionCategory(long int value)
	{
		m_CollisionCategory =value;

	}

	void HavokTerrainGeometryComponent::OnPhysicsDebug(PhysicsDebugMessagePtr message)
	{
		//SetDebug(message->DebugGeometry());
	}
}

