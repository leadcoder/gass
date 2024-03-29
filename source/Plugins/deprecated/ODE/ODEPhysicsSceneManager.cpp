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

#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Utils/GASSException.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSPhysicsMesh.h"
#include "Sim/Interface/GASSIMeshComponent.h"
#include "Sim/GASSSimSystemManager.h"
#include "Plugins/ODE/ODEPhysicsSceneManager.h"
#include "Plugins/ODE/ODEBodyComponent.h"
#include "Plugins/ODE/ODEPhysicsSystem.h"
#include "Sim/Interface/GASSIGroupComponent.h"
#include "Plugins/ODE/IODEGeometryComponent.h"

namespace GASS
{
	bool ODEPhysicsSceneManager::m_ZUp = true;

	void ODEPhysicsSceneManager::RegisterReflection()
	{
		RegisterGetSet("Gravity", &GASS::ODEPhysicsSceneManager::GetGravity, &GASS::ODEPhysicsSceneManager::SetGravity);
	}

	ODEPhysicsSceneManager::ODEPhysicsSceneManager(SceneWeakPtr scene) : Reflection(scene),
		m_Space(nullptr),
		m_World(nullptr),
		m_CollisionSpace(nullptr),
		m_ContactGroup (nullptr),
		m_Active(true),
		m_Gravity(-9.81f),
		m_SimulationUpdateInterval(1.0/60.0), //Locked to 60hz, if this value is changed the
											  //behavior of simulation is effected and values for
											  //bodies and joints must be re-trimmed
		m_TimeToProcess(0),
		m_MaxSimSteps(4),
		m_Init(false)
	{

	}

	void ODEPhysicsSceneManager::OnPostConstruction()
	{
		ScenePtr scene = GetScene();
		RegisterForPreUpdate<ODEPhysicsSystem>();

		m_Space = nullptr;
		m_World = dWorldCreate();
		m_Space = dHashSpaceCreate(m_Space);

		m_CollisionSpace = dHashSpaceCreate(m_CollisionSpace);
		m_ContactGroup = dJointGroupCreate(0);

		Vec3 gravity_vec = Vec3(0, m_Gravity, 0);

		dWorldSetGravity(m_World, gravity_vec.x, gravity_vec.y, gravity_vec.z);
		dWorldSetAutoDisableFlag(m_World, 1);
		dWorldSetAutoDisableLinearThreshold(m_World, 0.01);
		dWorldSetAutoDisableAngularThreshold(m_World, 0.01);
		dWorldSetAutoDisableSteps(m_World, 10);
		dWorldSetAutoDisableTime(m_World, 0);
		m_Init = true;
	}

	void ODEPhysicsSceneManager::OnSceneShutdown()
	{
		if (m_ContactGroup) dJointGroupDestroy(m_ContactGroup);
		if (m_CollisionSpace) dSpaceDestroy(m_CollisionSpace);
		if (m_World) dWorldDestroy(m_World);
		/*ODEPhysicsSystemPtr system =  SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystemByClass<ODEPhysicsSystem>();
		SystemListenerPtr listener = shared_from_this();
		system->UnregisterListener(listener);*/
	}

	ODEPhysicsSceneManager::~ODEPhysicsSceneManager()
	{
	}

	

	void ODEPhysicsSceneManager::SetGravity(float gravity)
	{
		m_Gravity = gravity;
	}

	float ODEPhysicsSceneManager::GetGravity() const
	{
		return m_Gravity;
	}
	
	void ODEPhysicsSceneManager::OnSceneCreated()
	{
		
	}

	void ODEPhysicsSceneManager::OnUpdate(double delta_time)
	{
		if (m_Active)
		{
			dSpaceCollide2((dGeomID) m_Space,(dGeomID)m_Space,this,&NearCallback);
			//dSpaceCollide2((dGeomID) m_Space,(dGeomID)m_StaticSpace,this,&NearCallback);
			if(delta_time > 0.1)
				dWorldQuickStep(m_World, 0.1);
			else
				dWorldQuickStep(m_World, delta_time);
			dJointGroupEmpty(m_ContactGroup);
			//do some time slicing
			/*m_TimeToProcess += delta_time;
			int num_steps = (int) (m_TimeToProcess / m_SimulationUpdateInterval);
			int clamp_num_steps = num_steps;

			//Take max 4 simulation step each frame
			if(num_steps > m_MaxSimSteps) clamp_num_steps = m_MaxSimSteps;

			for (int i = 0; i < clamp_num_steps; ++i)
			{
				dSpaceCollide2((dGeomID) m_Space,(dGeomID)m_Space,this,&NearCallback);
				dSpaceCollide2((dGeomID) m_Space,(dGeomID)m_StaticSpace,this,&NearCallback);
				dWorldQuickStep(m_World, m_SimulationUpdateInterval);
				dJointGroupEmpty(m_ContactGroup);
			}
			//std::cout << "Steps:" <<  clamp_num_steps << std::endl;
			m_TimeToProcess -= m_SimulationUpdateInterval * num_steps;*/
		}

		//Temp: move this to ODEPhysicsSystem
		/*ODECollisionSystem* col_sys = dynamic_cast<ODECollisionSystem*>(SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystemByClass<GASS::ICollisionSystem>().get());
		if(col_sys)
		{
			col_sys->Process();
		}*/
	}

#define MAX_CONTACTS 25		// maximum number of contact points per body
	void ODEPhysicsSceneManager::NearCallback(void *data, dGeomID o1, dGeomID o2)
	{
		auto* manager = (ODEPhysicsSceneManager*) data;
		manager->ProcessCollision(o1,o2);
	}

	void ODEPhysicsSceneManager::ProcessCollision(dGeomID o1, dGeomID o2)
	{
		int i;
		// if (o1->body && o2->body) return;
		if(dGeomIsSpace(o1) &&  dGeomIsSpace(o2) && o1 == o2)
		{
			return;
		}

		if(dGeomIsSpace(o1) ||  dGeomIsSpace(o2))
		{
			dSpaceCollide2(o1,o2,this,&NearCallback);
		}
		else
		{
			// check that both bodies has a geometry

			auto* geom1 = static_cast<Component*>(dGeomGetData(o1));
			auto* geom2 = static_cast<Component*>(dGeomGetData(o2));


			if(!(geom1 && geom2))
				return;

			if((geom1 == geom2))
				return;

			/*	if((geom1->GetSceneObject()->GetParent() && geom2->GetSceneObject()->GetParent()) && geom1->GetSceneObject()->GetParent() == geom2->GetSceneObject()->GetParent())
			return;


			*/

			// check if part of same object
			//get top body!
			GroupComponentPtr group_1 = geom1->GetSceneObject()->GetFirstParentComponentByClass<IGroupComponent>();
			GroupComponentPtr group_2 = geom2->GetSceneObject()->GetFirstParentComponentByClass<IGroupComponent>();
			if(group_1 && group_2 &&( group_1 == group_2))
			{
				return;
			}



			dBodyID b1 = dGeomGetBody(o1);
			dBodyID b2 = dGeomGetBody(o2);
			if(!b1 && !b2) return;
			if(b1 && b2 && dAreConnectedExcluding(b1,b2,dJointTypeContact)) return;


			auto* physics_geom1 = dynamic_cast<IODEGeometryComponent*>(geom1);
			auto* physics_geom2 = dynamic_cast<IODEGeometryComponent*>(geom2);


			dContact contact;
			contact.fdir1[0] = 0;
			contact.fdir1[1] = 0;
			contact.fdir1[2] = -1;
			contact.surface.mode = 0;
			contact.surface.mu = physics_geom1->GetFriction()*physics_geom2->GetFriction();
			contact.surface.mu2 = contact.surface.mu;


			//reset?
			contact.surface.soft_erp = 0;
			contact.surface.motion1 = 0;
			contact.surface.motion2 = 0;
			contact.surface.slip1 = 0;
			contact.surface.slip2 = 0;
			contact.surface.mode = dContactSoftCFM | dContactApprox1;
			//contact.surface.mu = Math::Max(geom1->GetFriction() , geom2->GetFriction());
			contact.surface.soft_cfm = 0.01;

			//dContact contact[MAX_CONTACTS];
			dContactGeom contacts[MAX_CONTACTS];
			int numc = dCollide(o1,o2,MAX_CONTACTS,contacts,sizeof(dContactGeom));

			for (i=0; i < numc; i++)
			{
				contact.geom = contacts[i];

				/*if( (WantsContact(geom1, contact, geom2, o1, o2, true )) &&
				(WantsContact(geom2, contact, geom1, o2, o1, true )) )
				*/{
					dJointID temp_joint = dJointCreateContact( m_World, m_ContactGroup, &contact );
					dJointAttach( temp_joint, b1, b2 );
				}
			}
			/*if(numc > 0)
			{
			Vec3 pos = Vec3(contacts[0].pos[0],contacts[0].pos[1],contacts[0].pos[2]);
			Vec3 normal = Vec3(contacts[0].normal[0],contacts[0].normal[1],contacts[0].normal[2]);
			geom1->CallCollsionListeners(pos,normal);
			geom2->CallCollsionListeners(pos,normal);
			}*/
		}

	}


	void ODEPhysicsSceneManager::CreateODERotationMatrix(const Mat4 &m, dReal *ode_mat)
	{
		//Make ODE rotation matrix
		/*ode_mat[0] = m.m_Data2[0];
		ode_mat[1] = m.m_Data2[4];
		ode_mat[2] = m.m_Data2[8];
		ode_mat[3] = 0;
		ode_mat[4] = m.m_Data2[1];
		ode_mat[5] = m.m_Data2[5];
		ode_mat[6] = m.m_Data2[9];
		ode_mat[7] = 0;
		ode_mat[8] = m.m_Data2[2];
		ode_mat[9] = m.m_Data2[6];
		ode_mat[10]= m.m_Data2[10];
		ode_mat[11] = 0;
		*/

		ode_mat[0] = m.m_Data2[0];
		ode_mat[1] = m.m_Data2[1];
		ode_mat[2] = m.m_Data2[2];
		ode_mat[3] = 0;
		ode_mat[4] = m.m_Data2[4];
		ode_mat[5] = m.m_Data2[5];
		ode_mat[6] = m.m_Data2[6];
		ode_mat[7] = 0;
		ode_mat[8] = m.m_Data2[8];
		ode_mat[9] = m.m_Data2[9];
		ode_mat[10]= m.m_Data2[10];
		ode_mat[11] = 0;
	}

	void ODEPhysicsSceneManager::CreateGASSRotationMatrix(const dReal *R, Mat4 &m)
	{
		m.m_Data2[0]  = R[0];
		m.m_Data2[1]  = R[1];
		m.m_Data2[2]  = R[2];
		m.m_Data2[3]  = 0;
		m.m_Data2[4]  = R[4];
		m.m_Data2[5]  = R[5];
		m.m_Data2[6]  = R[6];
		m.m_Data2[7]  = 0;
		m.m_Data2[8]  = R[8];
		m.m_Data2[9]  = R[9];
		m.m_Data2[10] = R[10];
		m.m_Data2[11] = 0;
		m.m_Data2[12] = 0;
		m.m_Data2[13] = 0;
		m.m_Data2[14] = 0;
		m.m_Data2[15] = 1;

	}

	ODEPhysicsCollisionMesh ODEPhysicsSceneManager::CreateCollisionMesh(const std::string &col_mesh_id, MeshComponentPtr mesh)
	{
		if(HasCollisionMesh(col_mesh_id))
		{
			return m_ColMeshMap[col_mesh_id];
		}
		//not loaded, load it!
		GraphicsMesh gfx_mesh_data = mesh->GetMeshData();
		PhysicsMeshPtr physics_mesh(new PhysicsMesh(gfx_mesh_data));

		if(physics_mesh->PositionVector.size() < 1 || physics_mesh->IndexVector.size() < 1)
		{
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"No vertex or face data for mesh", "ODECollisionSystem::CreateCollisionMesh");
		}

		// This should equal above code, but without Opcode dependency and no duplicating data
		dTriMeshDataID id = dGeomTriMeshDataCreate();


		int float_size = sizeof(Float);
		if(float_size == 8) //double precision
		{
			dGeomTriMeshDataBuildDouble(id,
			&(physics_mesh->PositionVector[0]),
			sizeof(Float)*3,
			static_cast<int>(physics_mesh->PositionVector.size()),
			(unsigned int*)&physics_mesh->IndexVector[0],
			static_cast<int>(physics_mesh->IndexVector.size()),
			3 * sizeof(unsigned int));
		}
		else
		{
			dGeomTriMeshDataBuildSingle(id,
			&(physics_mesh->PositionVector[0]),
			sizeof(Float)*3,
			static_cast<int>(physics_mesh->PositionVector.size()),
			(unsigned int*)&physics_mesh->IndexVector[0],
			static_cast<int>(physics_mesh->IndexVector.size()),
			3 * sizeof(unsigned int));
		}
		//Save id for this collision mesh

		ODEPhysicsCollisionMesh col_mesh;
		col_mesh.ID = id;
		col_mesh.Mesh = physics_mesh;
		m_ColMeshMap[col_mesh_id] = col_mesh;
		return col_mesh;
	}

	bool ODEPhysicsSceneManager::HasCollisionMesh(const std::string &name)
	{
		CollisionMeshMap::iterator iter;
		iter = m_ColMeshMap.find(name);
		if (iter!= m_ColMeshMap.end()) //in map.
		{
			return true;
		}
		return false;
	}
}
