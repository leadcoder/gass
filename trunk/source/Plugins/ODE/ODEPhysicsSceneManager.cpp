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
#include <boost/bind.hpp>
#include "Core/Utils/Log.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/Message.h"
#include "Sim/Scenario/Scene/SceneManagerFactory.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"

#include "Sim/Scenario/Scene/SceneObjectManager.h"
#include "Sim/SimEngine.h"
#include "Sim/Scheduling/IRuntimeController.h"

#include "Sim/Components/Graphics/Geometry/IMeshComponent.h"
#include "Plugins/ODE/ODEPhysicsSceneManager.h"
//#include "Main/Root.h"
#include "Plugins/ODE/ODEBody.h"


namespace GASS
{

	ODEPhysicsSceneManager::ODEPhysicsSceneManager() :
		m_Space(0),
		m_StaticSpace(0),
		m_World(0),
		m_CollisionSpace(0),
		m_ContactGroup (0),
		m_Paused(false),
		m_Gravity(-9.81f)
	{

	}

	ODEPhysicsSceneManager::~ODEPhysicsSceneManager()
	{
	}

	void ODEPhysicsSceneManager::RegisterReflection()
	{
		SceneManagerFactory::GetPtr()->Register("ODEPhysicsSceneManager",new GASS::Creator<ODEPhysicsSceneManager, ISceneManager>);
		RegisterProperty<float>( "Gravity", &GetGravity, &SetGravity);
	}

	void ODEPhysicsSceneManager::SetGravity(float gravity)
	{
		m_Gravity = gravity;
	}

	float ODEPhysicsSceneManager::GetGravity() const
	{
		return m_Gravity;
	}



	void ODEPhysicsSceneManager::OnCreate()
	{
		int address = (int) this;
		SimEngine::GetPtr()->GetRuntimeController()->Register(boost::bind( &ODEPhysicsSceneManager::Update, this, _1 ),false);

		m_Scene->RegisterForMessage(ScenarioScene::SCENARIO_MESSAGE_LOAD_SCENE_MANAGERS, address,  boost::bind( &ODEPhysicsSceneManager::OnLoad, this, _1 ),0);
		//m_Scene->GetMessageManager()->RegisterForMessage(ScenarioScene::SCENARIO_MESSAGE_UPDATE, address,  boost::bind( &ODEPhysicsSceneManager::OnUpdate, this, _1 ),0);
		m_Scene->RegisterForMessage(ScenarioScene::SCENARIO_MESSAGE_UNLOAD_SCENE_MANAGERS, address,  boost::bind( &ODEPhysicsSceneManager::OnUnload, this, _1 ),0);
		m_Scene->RegisterForMessage(ScenarioScene::SCENARIO_MESSAGE_LOAD_SCENE_OBJECT, address,  boost::bind( &ODEPhysicsSceneManager::OnLoadSceneObject, this, _1 ),ScenarioScene::PHYSICS_COMPONENT_LOAD_PRIORITY);
	}

	void ODEPhysicsSceneManager::OnLoadSceneObject(MessagePtr message)
	{
		//Initlize all gfx components and send scene mananger as argument
		SceneObjectPtr obj = boost::any_cast<SceneObjectPtr>(message->GetData("SceneObject"));
		assert(obj);
		MessagePtr phy_msg(new Message(SceneObject::OBJECT_MESSAGE_LOAD_PHYSICS_COMPONENTS,(int) this));
		phy_msg->SetData("PhysicsSceneManager",boost::any(this));
		obj->GetMessageManager()->SendImmediate(phy_msg);
	}

	
	void ODEPhysicsSceneManager::Update(double delta_time)
	{
		
		if (m_Paused)
			return;
		dSpaceCollide2((dGeomID) m_Space,(dGeomID)m_Space,this,&NearCallback);
		dSpaceCollide2((dGeomID) m_Space,(dGeomID)m_StaticSpace,this,&NearCallback);
		dWorldQuickStep(m_World, delta_time);
		dJointGroupEmpty(m_ContactGroup);
	}

	void ODEPhysicsSceneManager::OnLoad(MessagePtr message)
	{
		ScenarioScene* scene = boost::any_cast<ScenarioScene*>(message->GetData("ScenarioScene"));

		dInitODE2(0);
		m_Space = 0;
		m_StaticSpace = 0;
		m_World = dWorldCreate();
		m_Space = dHashSpaceCreate(m_Space);
		m_StaticSpace = dHashSpaceCreate(m_StaticSpace);

		m_CollisionSpace = dHashSpaceCreate(m_CollisionSpace);
		m_ContactGroup = dJointGroupCreate(0);

		Vec3 gravity_vec = scene->GetSceneUp()*m_Gravity;

		dWorldSetGravity(m_World, gravity_vec.x,gravity_vec.y, gravity_vec.z);
		dWorldSetAutoDisableFlag(m_World, 1);
		dWorldSetAutoDisableLinearThreshold(m_World, 0.01);
		dWorldSetAutoDisableAngularThreshold(m_World, 0.01);
		dWorldSetAutoDisableSteps(m_World, 10);
		dWorldSetAutoDisableTime(m_World, 0);
		dAllocateODEDataForThread(dAllocateMaskAll);
		
		m_Init = true;
	}

	void ODEPhysicsSceneManager::OnUnload(MessagePtr message)
	{
		dJointGroupDestroy (m_ContactGroup);
		dSpaceDestroy (m_CollisionSpace);
		dSpaceDestroy (m_StaticSpace);
		dWorldDestroy (m_World);
		dCloseODE();

		int address = (int) this;
		SimEngine::GetPtr()->GetRuntimeController()->Unregister(boost::bind( &ODEPhysicsSceneManager::Update, this, _1 ));
	}


#define MAX_CONTACTS 25		// maximum number of contact points per body
	void ODEPhysicsSceneManager::NearCallback(void *data, dGeomID o1, dGeomID o2)
	{
		ODEPhysicsSceneManager* manager = (ODEPhysicsSceneManager*) data;
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
			ODEGeometry* geom1 = static_cast<ODEGeometry*>(dGeomGetData(o1));
			ODEGeometry* geom2 = static_cast<ODEGeometry*>(dGeomGetData(o2));

			if(!(geom1 && geom2)) 
				return;

			if((geom1 == geom2)) 
				return;

		/*	if((geom1->GetSceneObject()->GetParent() && geom2->GetSceneObject()->GetParent()) && geom1->GetSceneObject()->GetParent() == geom2->GetSceneObject()->GetParent()) 
				return;


			*/

			// check if part of same object

			if(geom1->GetSceneObject()->GetObjectUnderRoot() == geom2->GetSceneObject()->GetObjectUnderRoot()) 
			{
				return;
			}

			

			dBodyID b1 = dGeomGetBody(o1);
			dBodyID b2 = dGeomGetBody(o2);
			if(!b1 && !b2) return;
			if(b1 && b2 && dAreConnectedExcluding(b1,b2,dJointTypeContact)) return;


			dContact contact;
			contact.fdir1[0] = 0;
			contact.fdir1[1] = 0;
			contact.fdir1[2] = -1;
			contact.surface.mode = 0;
			contact.surface.mu = geom1->GetFriction()*geom2->GetFriction();
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
					dJointID tempJoint = dJointCreateContact( m_World, m_ContactGroup, &contact );
					dJointAttach( tempJoint, b1, b2 );
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
		ode_mat[0] = m.m_Data2[0];
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
	}

	void ODEPhysicsSceneManager::CreateGASSRotationMatrix(const dReal *R, Mat4 &m)
	{
		m.m_Data2[0] = R[0];
		m.m_Data2[1] = R[4];
		m.m_Data2[2] = R[8];
		m.m_Data2[3] = 0;
		m.m_Data2[4] = R[1];
		m.m_Data2[5] = R[5];
		m.m_Data2[6]= R[9];
		m.m_Data2[7]= 0;
		m.m_Data2[8] = R[2];
		m.m_Data2[9] = R[6];
		m.m_Data2[10] = R[10];
		m.m_Data2[11] = 0;
		m.m_Data2[12] = 0;
		m.m_Data2[13] = 0;
		m.m_Data2[14] = 0;
		m.m_Data2[15] = 1;

	}


	/*bool ODEPhysicsSceneManager::WantsContact(IPhysicsObject *current,  dContact & contact, IPhysicsObject * other, dGeomID you, dGeomID him, bool firstTest)
	{
	ODEGeometry* odegeom = DYNAMIC_CAST(ODEGeometry, current);
	if(odegeom)
	{
	return odegeom->WantsContact(contact,other,you,him,firstTest);
	}
	return true;
	}*/

	/*	void ODEPhysicsSceneManager::SetNodeTransformation(const dBodyID body,ISceneNode* node)
	{
	Mat4 trans;
	ODEPhysicsSceneManager::ConvertToMatrix44(dBodyGetPosition(body),dBodyGetRotation(body),trans.m_Data2);
	node->SetAbsoluteTransformation(trans);
	}

	void ODEPhysicsSceneManager::SetBodyTransformation(const dBodyID body,ISceneNode* node)
	{
	dReal ode_rot_mat[12];
	Vec3 abs_pos = node->GetAbsoluteTransformation().GetTranslation();
	float *m = node->GetAbsoluteTransformation().m_Data2;
	MakeODERotationMatrix(node->GetAbsoluteTransformation(),(float*)ode_rot_mat);
	//dBodySetPosition(body, abs_pos.x - offset.x, abs_pos.y - offset.y, abs_pos.z - offset.z);
	dBodySetPosition(body, abs_pos.x, abs_pos.y, abs_pos.z);
	dBodySetRotation(body, ode_rot_mat);
	}

	void ODEPhysicsSceneManager::SetGeomTransformation(const dGeomID geom,ISceneNode* node)
	{
	dReal ode_rot_mat[12];
	Vec3 abs_pos = node->GetAbsoluteTransformation().GetTranslation();
	float *m = node->GetAbsoluteTransformation().m_Data2;
	MakeODERotationMatrix(node->GetAbsoluteTransformation(),(float*)ode_rot_mat);
	//dBodySetPosition(body, abs_pos.x - offset.x, abs_pos.y - offset.y, abs_pos.z - offset.z);
	dGeomSetPosition(geom, abs_pos.x, abs_pos.y, abs_pos.z);
	dGeomSetRotation(geom, ode_rot_mat);
	}


	void ODEPhysicsSceneManager::MakeODERotationMatrix(const Mat4 &m, float *ode_mat)
	{
	//Make ODE rotation matrix 
	ode_mat[0] = m.m_Data2[0];
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
	}

	void ODEPhysicsSceneManager::ConvertToMatrix44(const dReal pos[3], const dReal R[12], float *m)
	{

	m[0] = R[0];
	m[1] = R[4];
	m[2] = R[8];
	m[3] = 0;
	m[4] = R[1];
	m[5] = R[5];
	m[6]= R[9];
	m[7]= 0;
	m[8] = R[2];
	m[9] = R[6];
	m[10] = R[10];
	m[11] = 0;
	m[12] = pos[0];
	m[13] = pos[1];
	m[14] = pos[2];
	m[15] = 1;

	}*/

	//TODO: move this to physics system instead of scenatio manager, mesh data should be same between scenes and therefore shareable
	ODECollisionMesh ODEPhysicsSceneManager::CreateCollisionMesh(IMeshComponent* mesh)
	{
		std::string col_mesh_name = mesh->GetFilename();
		if(HasCollisionMesh(col_mesh_name))
		{
			return m_ColMeshMap[col_mesh_name];
		}
		//not loaded, load it!

		MeshDataPtr mesh_data = new MeshData;
		mesh->GetMeshData(mesh_data);

		if(mesh_data->NumVertex < 1 || mesh_data->NumFaces < 1)
		{
			//Log::Error("No verticies found for this mesh")
		}
		// This should equal above code, but without Opcode dependency and no duplicating data
		dTriMeshDataID id = dGeomTriMeshDataCreate();
		dGeomTriMeshDataBuildSingle(id,
			&(mesh_data->VertexVector[0]),
			sizeof(float)*3,
			mesh_data->NumVertex,
			(unsigned int*)&mesh_data->FaceVector[0], 
			mesh_data->NumFaces*3, 
			3 * sizeof(unsigned int));
		//Save id for this collision mesh

		ODECollisionMesh col_mesh;
		col_mesh.ID = id;
		col_mesh.Mesh = mesh_data;
		m_ColMeshMap[col_mesh_name] = col_mesh;
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
