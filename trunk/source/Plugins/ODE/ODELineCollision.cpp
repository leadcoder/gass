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

#include "Plugins/ODE/ODEPhysicsSceneManager.h"
#include "Plugins/ODE/ODELineCollision.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Components/BaseSceneComponent.h"


namespace GASS
{


	ODELineCollision::ODELineCollision(CollisionRequest *request,CollisionResult *result,ODEPhysicsSceneManagerPtr ode_scene, float segment_length) :	m_Request(request),
		m_Result(result),
		m_SceneManager(ode_scene),
		m_RayGeom(0),
		m_SegmentLength(segment_length)
	{
		
		m_RayDir = request->LineEnd - request->LineStart;
		m_RayStart = request->LineStart;
		m_RayLength = m_RayDir.Length();
		m_RayDir = m_RayDir*(1.0/m_RayLength);
	}

	ODELineCollision::~ODELineCollision()
	{
		
	}

	void ODELineCollision::Process()
	{
		//split ray into segments
		
		if(m_SegmentLength > 0)
		{
			dGeomID geom_space = (dGeomID) ODEPhysicsSceneManagerPtr(m_SceneManager)->GetPhysicsSpace();
			dGeomID ray = dCreateRay (0, m_SegmentLength);
			dGeomSetCollideBits (ray,m_Request->CollisionBits);
			dGeomSetCategoryBits(ray,m_Request->CollisionBits);
			const int segments = int (m_RayLength/m_SegmentLength);
			double last_ray_length =  m_RayLength - segments * m_SegmentLength;

			m_Result->Coll = false;
			m_Result->CollDist = 0;

			for(int i=0 ; i < segments; i++)
			{
				m_Result->Coll = false;
				m_Result->CollDist = 0;
				const Vec3 rayStart = m_RayStart + m_RayDir*(i*m_SegmentLength);
				dGeomRaySet(ray, rayStart.x,rayStart.y,rayStart.z, m_RayDir.x,m_RayDir.y,m_RayDir.z);
				dSpaceCollide2(geom_space,ray,(void*) this,&ODELineCollision::Callback);
				if(m_Result->Coll == true)
				{
					dGeomDestroy(ray);
					m_Result->CollPosition = rayStart + m_RayDir*m_Result->CollDist;
					return;
				}
			}
			if(last_ray_length > 0)
			{
				m_Result->Coll = false;
				m_Result->CollDist = 0;
				const Vec3 rayStart = m_RayStart + m_RayDir*(segments*m_SegmentLength);
				dGeomRaySetLength(ray,last_ray_length);
				dGeomRaySet(ray, rayStart.x,rayStart.y,rayStart.z, m_RayDir.x,m_RayDir.y,m_RayDir.z);
				dSpaceCollide2(geom_space,ray,(void*) this,&ODELineCollision::Callback);
				if(m_Result->Coll == true)
				{
					m_Result->CollPosition = rayStart + m_RayDir*m_Result->CollDist;
				}
			}
			dGeomDestroy(ray);
		}
		else
		{
			m_RayGeom = dCreateRay (0, m_RayLength);
			dGeomSetCollideBits (m_RayGeom,m_Request->CollisionBits);
			dGeomSetCategoryBits(m_RayGeom,m_Request->CollisionBits);
			dGeomRaySet(m_RayGeom, m_RayStart.x,m_RayStart.y,m_RayStart.z, m_RayDir.x,m_RayDir.y,m_RayDir.z);
			assert(m_Result);
			m_Result->Coll = false;
			m_Result->CollDist = 0;
			dGeomID geom_space = (dGeomID) ODEPhysicsSceneManagerPtr(m_SceneManager)->GetPhysicsSpace();
			dSpaceCollide2(geom_space,m_RayGeom,(void*) this,&ODELineCollision::Callback);
			if(m_Result->Coll == true)
			{
				m_Result->CollPosition = m_RayStart + m_RayDir*m_Result->CollDist;
			}
			dGeomDestroy(m_RayGeom);
		}
	}

	void ODELineCollision::Callback(void *data, dGeomID o1, dGeomID o2)
	{
		if(dGeomIsSpace(o1) ||  dGeomIsSpace(o2))
		{
			//dSpaceCollide2 (o1,o2,data,&RaycastCallback);
			// collide all geoms internal to the space(s)
			if (dGeomIsSpace (o1)) dSpaceCollide2(o1,o2,data,&ODELineCollision::Callback);
			if (dGeomIsSpace (o2)) dSpaceCollide2(o2,o1,data,&ODELineCollision::Callback);
		}
		else
		{
			long int cat1 = dGeomGetCategoryBits (o1);
			long int cat2 = dGeomGetCategoryBits (o2);
			long int col1 = dGeomGetCollideBits (o1);
			long int col2 = dGeomGetCollideBits (o2);
			if ((cat1 & col2) || (cat2 & col1)) 
			{
				ODELineCollision* rs = (ODELineCollision*) data;
				rs->ProcessCallback(o1,o2);
			}
		}
	}

	void ODELineCollision::ProcessCallback(dGeomID o1, dGeomID o2)
	{


		dGeomID ray_geom =0;
		dGeomID other_geom =0;

		if(dGeomGetClass(o1) == dRayClass) 
		{
			ray_geom = o1;
			other_geom = o2;
		}
		else if(dGeomGetClass(o2) == dRayClass) 
		{
			ray_geom = o2;
			other_geom = o1;
		}

		if(!dGeomIsEnabled(other_geom)) return;

		BaseSceneComponent* pobj = static_cast<BaseSceneComponent*>(dGeomGetData(other_geom));
		SceneObjectPtr scene_object = pobj->GetSceneObject();

		int num_contact_points = 10;
		dContact contact[10];

		if(m_Request->ReturnFirstCollisionPoint)
		{
			num_contact_points = 1;
		}

		dGeomID tri_mesh_geom = 0;
		if(dGeomGetClass(other_geom) == dTriMeshClass) 
			tri_mesh_geom = other_geom;

		/*if(tri_mesh_geom && m_Request->ReturnTriangles) 
		{
		dGeomTriMeshSetRayCallback(tri_mesh_geom, (dTriRayCallback*) TriangleCallback);
		}*/
		int num_contact = dCollide(ray_geom,other_geom,num_contact_points,&contact[0].geom, sizeof(dContact));
		//if(tri_mesh_geom && m_Request->ReturnTriangles) dGeomTriMeshSetRayCallback(tri_mesh_geom, 0);
		//double isect_dist = 0;
		
		for(int i = 0 ; i < num_contact; i++)
		{
			//pos - This is the point at which the ray intersects the surface of the other geom, regardless of whether the ray starts from inside or outside the geom.
			//normal - This is the surface normal of the other geom at the contact point. if dCollide is passed the ray as its first geom then the normal will be oriented correctly for ray reflection from that surface (otherwise it will have the opposite sign).
			//depth
			if(m_Result->Coll == false || contact[i].geom.depth < m_Result->CollDist)
			{
				m_Result->CollDist = contact[i].geom.depth;
				m_Result->Coll = true;
				//m_Result->CollPosition = m_RayStart + m_RayDir*isect_dist;
				m_Result->CollSceneObject = scene_object;	
				m_Result->CollNormal = Vec3(contact[i].geom.normal[i],contact[i].geom.normal[1],contact[i].geom.normal[2]);
			}
		}
	}

	/*void ODELineCollision::TriangleCallback(dGeomID tri_mesh, dGeomID ray, int triangle_index, dReal u, dReal v)
	{

	}*/
}
