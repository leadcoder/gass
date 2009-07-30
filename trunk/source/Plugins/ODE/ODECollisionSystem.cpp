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

#include "Plugins/ODE/ODECollisionSystem.h"
#include "Plugins/ODE/ODEPhysicsSceneManager.h"
#include "Plugins/ODE/ODELineCollision.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Systems/SimSystemManager.h"
#include "Core/System/SystemFactory.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/Message.h"

namespace GASS
{

	ODECollisionSystem::ODECollisionSystem()
	{
		m_HandleCount = 5;
		

	}

	ODECollisionSystem::~ODECollisionSystem()
	{

	}
	
	CollisionHandle ODECollisionSystem::Request(const CollisionRequest &request)
	{
		assert(request.Scene);
		CollisionHandle handle = ( m_HandleCount + 1 ) % 0xFFFFFFFE;
		m_RequestMap[handle] = request;
		//Process this direct for now;
		Process();
		return handle;
	}

	void ODECollisionSystem::Process()
	{
		//double buffer this when running threaded
		RequestMap::iterator iter;
		for(iter = m_RequestMap.begin(); iter != m_RequestMap.end(); iter++)
		{
			CollisionRequest request =  iter->second;
			CollisionHandle handle = iter->first;
			ODEPhysicsSceneManager* ode_scene = dynamic_cast<ODEPhysicsSceneManager*>(request.Scene->GetSceneManager("ODEPhysicsSceneManager").get());
			if(request.Type == COL_LINE)
			{
				CollisionResult result;
				ODELineCollision raycast(&request,&result,ode_scene);
				raycast.Process();
				m_ResultMap[handle] = result;
			}
			/*else(request.Type == COL_SPHERE)
			{

			}*/
		}
		m_RequestMap.clear();
	}

	bool ODECollisionSystem::Check(CollisionHandle handle, CollisionResult &result)
	{
		ResultMap::iterator iter = m_ResultMap.find(handle);
		if(iter != m_ResultMap.end())
		{
			result = m_ResultMap[handle];
			m_ResultMap.erase(iter);
			return true;
		}
		return false;
	}

	void ODECollisionSystem::RegisterReflection()
	{
		SystemFactory::GetPtr()->Register("ODECollisionSystem",new GASS::Creator<ODECollisionSystem, ISystem>);
	}

	void ODECollisionSystem::OnCreate()
	{
		int address = (int) this;
		//m_Owner->GetMessageManager()->RegisterForMessage(SystemManager::SYSTEM_RM_UPDATE, address,  boost::bind( &ODECollisionSystem::OnUpdate, this, _1 ),0);
	}

	/*void ODECollisionSystem::OnUpdate(MessagePtr message)
	{
		Process();
	}*/

	/*void ODECollisionSystem::GetTriangleCollision(Sphere sphere, TriangleVector *tri_vec)
	{
		m_TriVec = tri_vec;
		dGeomID geom_space = (dGeomID) ((ODEPhysicsManager*)Root::Get().GetPhysicsManager())->m_CollisionSpace;
		dGeomID ode_sphere = dCreateSphere(0,sphere.m_Radius);
		dGeomSetPosition(ode_sphere,sphere.m_Pos.x,sphere.m_Pos.y,sphere.m_Pos.z);
		dSpaceCollide2(geom_space, ode_sphere, 0, &TriangleCollsionCallback);
		dGeomDestroy(ode_sphere);
		m_TriVec = NULL;
	}

	bool ODECollisionSystem::SphereCollision(Sphere sphere, CollisionData *coldata)
	{
		dGeomID geom_space = (dGeomID) ((ODEPhysicsManager*)Root::Get().GetPhysicsManager())->m_CollisionSpace;
		dGeomID ode_sphere = dCreateSphere(0,sphere.m_Radius);
		dGeomSetPosition(ode_sphere,sphere.m_Pos.x,sphere.m_Pos.y,sphere.m_Pos.z);
		if(coldata == NULL)
		{
			CollisionData temp_col_data = CollisionData(true,false);
			dSpaceCollide2(geom_space, ode_sphere, (void*) &temp_col_data,GenericCollisionCallback);
			dGeomDestroy(ode_sphere);
			return temp_col_data.GetCollision();
		}
		else
		{
			dSpaceCollide2(geom_space, ode_sphere, (void*) coldata,GenericCollisionCallback);
			dGeomDestroy(ode_sphere);
			return coldata->GetCollision();
		}
	}

	
	void ODECollisionSystem::TriangleCollsionCallback(void *data, dGeomID o1, dGeomID o2)
	{
		{
			PROFILE("TriangleCollsionCallback")

				if(!dGeomIsSpace(o1) &&  !dGeomIsSpace(o2))
				{
					dGeomID tri_mesh_geom =0;
					dGeomID other_geom =0;
					//if(dGeomGetClass(o1) == dSphereClass) sphere_geom = o1;
					//if(dGeomGetClass(o2) == dSphereClass) sphere_geom = o2;
					if(dGeomGetClass(o1) == dTriMeshClass && dGeomGetClass(o2) != dTriMeshClass) 
					{
						tri_mesh_geom = o1;
						other_geom = o2;
					}
					else if(dGeomGetClass(o2) == dTriMeshClass && dGeomGetClass(o1) != dTriMeshClass) 
					{
						tri_mesh_geom = o2;
						other_geom = o1;
					}

					if(tri_mesh_geom && other_geom && dGeomIsEnabled(tri_mesh_geom))
					{
						dGeomTriMeshSetArrayCallback(tri_mesh_geom, TriArrayCallback);
						dContact contact[1];
						int num_contact;
						num_contact = dCollide(other_geom,tri_mesh_geom,1,&contact[0].geom, sizeof(dContact));
						//num_contact = dCollide(sphere_geom,tri_mesh_geom,0,0,0);
						dGeomTriMeshSetArrayCallback(tri_mesh_geom, 0);
					}
				}
		}
	}


	void ODECollisionSystem::GenericCollisionCallback(void *data, dGeomID o1, dGeomID o2)
	{
		{
			PROFILE("GenericCollsionCallback")
				assert(data);
			CollisionData* coldata = (CollisionData*) data;
			if(dGeomIsSpace(o1) ||  dGeomIsSpace(o2))
			{
				//dSpaceCollide2 (o1,o2,data,&RaycastCallback);
				// collide all geoms internal to the space(s)
				if (dGeomIsSpace (o1)) dSpaceCollide2(o1,o2,data,&GenericCollisionCallback);
				if (dGeomIsSpace (o2)) dSpaceCollide2(o2,o1,data,&GenericCollisionCallback);
			}
			else
				//if(!dGeomIsSpace(o1) &&  !dGeomIsSpace(o2))
			{
	
				if(dGeomIsEnabled(o1) && dGeomIsEnabled(o2))
					//if(tri_mesh_geom && other_geom && dGeomIsEnabled(tri_mesh_geom))
				{
					dContact contact[1];
					int num_contact;
					num_contact = dCollide(o1,o2,1,&contact[0].geom, sizeof(dContact));
					if(num_contact > 0)
					{
						if(coldata->WantObjectInfo())
						{
							IPhysicsObject* pobj = (IPhysicsObject*) dGeomGetData(o1);
							if(pobj == NULL) pobj = (IPhysicsObject*) dGeomGetData(o2);
							if(pobj)
							{
								coldata->AddCollisionObject(pobj->GetOwner());
							}
						}
						coldata->SetCollision(true);

					}
				}
			}
		}
	}

	void ODECollisionSystem::TriArrayCallback(dGeomID TriMesh, dGeomID RefObject,	const int* TriIndices, int TriCount)
	{
		//dVector3 v1,v2,v3;
		TriangleVector* tri_vec = ((ODEPhysicsManager*)Root::Get().GetPhysicsManager())->m_Collision->m_TriVec;

		if(!tri_vec) return;
		CollisionTriangle tri;

		/*SimpleObject* so = (SimpleObject*) dGeomGetData(TriMesh);
		IGeometry* ig = so->GetFirstGeometry();
		CollisionMesh* cm = NULL;

		if(ig->IsExactClass(&StandardMesh::m_RTTI))
		{
		StandardMesh* sm = (StandardMesh*) ig;
		cm = sm->GetMesh()->m_Mesh.GetCollisionMesh(0);
		}

		for(int i  = 0; i < TriCount; i++)
		{
		//dGeomTriMeshGetTriangle(TriMesh, i, &v1.x,&v2.x,&v3.x);
		dGeomTriMeshGetTriangle(TriMesh, TriIndices[i], &v1,&v2,&v3);
		int id = 0;
		if(cm)
		{
		id = cm->MatIDVector[TriIndices[i]];
		}

		tri.p1.Set(v1[0],v1[1],v1[2]); 
		tri.p2.Set(v2[0],v2[1],v2[2]);
		tri.p3.Set(v3[0],v3[1],v3[2]);

		tri_vec->push_back(tri);

		/*glDisable(GL_TEXTURE_2D);
		glBegin(GL_TRIANGLES);
		glVertex3f(v1[0],v1[1]+0.1,v1[2]);
		glVertex3f(v2[0],v2[1]+0.1,v2[2]);
		glVertex3f(v3[0],v3[1]+0.1,v3[2]);

		glEnd();*/
		//}
	//}

	
}
