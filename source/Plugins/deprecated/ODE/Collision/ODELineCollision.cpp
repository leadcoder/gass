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

//#include "Plugins/ODE/Collision/ODEPhysicsSceneManager.h"
#include "Plugins/ODE/Collision/ODELineCollision.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSComponent.h"
#include "Sim/GASSSimEngine.h"

namespace GASS
{
	ODELineCollision::ODELineCollision(const Vec3 &ray_start, const Vec3 &ray_dir, GeometryFlags flags,bool return_first_collision, CollisionResult *result,dGeomID space_id, float segment_length) :m_Result(result),
		m_Space(space_id),
		m_RayGeom(nullptr),
		m_SegmentLength(segment_length),
		m_ReturnFirstCollisionPoint(return_first_collision),
		m_CollisionBits(flags),
		m_RayStart(ray_start),
		m_RayDir(ray_dir)
	{
		m_RayLength = m_RayDir.Length();
		m_RayDir = m_RayDir*(1.0/m_RayLength);
	}

	ODELineCollision::~ODELineCollision()
	{

	}

	void ODELineCollision::Process()
	{
		//ScenePtr scene = SimEngine::Get().GetScenes().getNext();
		//GraphicsSceneManagerPtr gsm = scene->GetFirstSceneManagerByClass<IGraphicsSceneManager>();

		//HACK: split ray into segments to support terrain heightmaps!
		if(m_SegmentLength > 0)
		{
			//calculate segments based on projected ray distance
			Vec3 dir = m_RayDir * m_RayLength;
			dir.y = 0;
			Float l = dir.Length();
			const int segments = static_cast<int>(l / m_SegmentLength);

			dGeomID ray = dCreateRay (nullptr, m_RayLength);
			dGeomSetCollideBits (ray,m_CollisionBits);
			dGeomSetCategoryBits(ray,0);
			double last_ray_length = m_RayLength;
			Float ray_segment = 0;
			if(segments > 0)
			{
				ray_segment = m_RayLength / (Float)segments;
				last_ray_length =  m_RayLength - segments * ray_segment;
				dGeomRaySetLength(ray,ray_segment);
				m_Result->Coll = false;
				m_Result->CollDist = 0;

				for(int i=0 ; i < segments; i++)
				{
					m_Result->Coll = false;
					m_Result->CollDist = 0;
					const Vec3 ray_start = m_RayStart + m_RayDir*(double(i)*ray_segment);
					dGeomRaySet(ray, ray_start.x,ray_start.y,ray_start.z, m_RayDir.x,m_RayDir.y,m_RayDir.z);
					dSpaceCollide2(m_Space,ray,(void*) this,&ODELineCollision::Callback);

					if(m_Result->Coll == true)
					{
						m_Result->CollPosition = ray_start + m_RayDir*m_Result->CollDist;
						m_Result->CollDist = m_Result->CollDist+i*ray_segment;
						dGeomDestroy(ray);
						return;
					}
				}
			}

			if(last_ray_length > 0)
			{
				m_Result->Coll = false;
				m_Result->CollDist = 0;
				const Vec3 ray_start = m_RayStart + m_RayDir*(segments*ray_segment);

				dGeomRaySetLength(ray,last_ray_length);
				dGeomRaySet(ray, ray_start.x,ray_start.y,ray_start.z, m_RayDir.x,m_RayDir.y,m_RayDir.z);
				dSpaceCollide2(m_Space,ray,(void*) this,&ODELineCollision::Callback);
				if(m_Result->Coll == true)
				{
					m_Result->CollPosition = ray_start + m_RayDir*m_Result->CollDist;
					m_Result->CollDist = m_Result->CollDist+segments*ray_segment;
				}
			}


			/*dGeomID ray = dCreateRay (0, m_SegmentLength);
			dGeomSetCollideBits (ray,m_CollisionBits);
			dGeomSetCategoryBits(ray,0);
			const int segments = int (m_RayLength/m_SegmentLength);
			double last_ray_length =  m_RayLength - segments * m_SegmentLength;

			m_Result->Coll = false;
			m_Result->CollDist = 0;

			for(int i=0 ; i < segments; i++)
			{
				m_Result->Coll = false;
				m_Result->CollDist = 0;
				const Vec3 rayStart = m_RayStart + m_RayDir*(double(i)*m_SegmentLength);
				dGeomRaySet(ray, rayStart.x,rayStart.y,rayStart.z, m_RayDir.x,m_RayDir.y,m_RayDir.z);
				dSpaceCollide2(m_Space,ray,(void*) this,&ODELineCollision::Callback);

				if(m_Result->Coll == true)
				{
					m_Result->CollPosition = rayStart + m_RayDir*m_Result->CollDist;
					m_Result->CollDist = m_Result->CollDist+i*m_SegmentLength;
					dGeomDestroy(ray);
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
				dSpaceCollide2(m_Space,ray,(void*) this,&ODELineCollision::Callback);
				if(m_Result->Coll == true)
				{
					m_Result->CollPosition = rayStart + m_RayDir*m_Result->CollDist;
					m_Result->CollDist = m_Result->CollDist+segments*m_SegmentLength;
				}
			}*/
			dGeomDestroy(ray);
		}
		else
		{
			m_RayGeom = dCreateRay (nullptr, m_RayLength);
			dGeomSetCollideBits (m_RayGeom,m_CollisionBits);
			dGeomSetCategoryBits(m_RayGeom,m_CollisionBits);
			dGeomRaySet(m_RayGeom, m_RayStart.x,m_RayStart.y,m_RayStart.z, m_RayDir.x,m_RayDir.y,m_RayDir.z);
			assert(m_Result);
			m_Result->Coll = false;
			m_Result->CollDist = 0;
			auto geom_space = (dGeomID) m_Space;
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
				auto* rs = (ODELineCollision*) data;
				rs->ProcessCallback(o1,o2);
			}
		}
	}

	void ODELineCollision::ProcessCallback(dGeomID o1, dGeomID o2)
	{
		dGeomID ray_geom =nullptr;
		dGeomID other_geom =nullptr;

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

		auto* pobj = static_cast<Component*>(dGeomGetData(other_geom));
		SceneObjectPtr scene_object = pobj->GetSceneObject();

		int num_contact_points = 10;
		dContact contact[10];

		if(m_ReturnFirstCollisionPoint)
		{
			num_contact_points = 1;
		}

		//dGeomID tri_mesh_geom = 0;
		//if(dGeomGetClass(other_geom) == dTriMeshClass)
		//	tri_mesh_geom = other_geom;

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
}
