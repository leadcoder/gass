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

#pragma once

#include <ode/ode.h>
#include <map>
#include "Core/MessageSystem/Message.h"
#include "Sim/Scenario/Scene/BaseSceneManager.h"

namespace Ogre
{
	class VertexData;
}
namespace GASS
{
	class IMeshComponent;
	struct MeshData;

	struct ODECollisionMesh
	{
		MeshData* Mesh;
		dTriMeshDataID ID;
	};

	class ODEPhysicsSceneManager  : public Reflection<ODEPhysicsSceneManager, BaseSceneManager>
	{
	public:
		typedef std::map<std::string,ODECollisionMesh> CollisionMeshMap;
	public:
		ODEPhysicsSceneManager();
		virtual ~ODEPhysicsSceneManager();
		static void RegisterReflection();
		virtual void OnCreate();
		dSpaceID GetPhysicsSpace(){return m_Space;}
		dSpaceID GetCollisionSpace(){return m_CollisionSpace;}
		ODECollisionMesh CreateCollisionMesh(IMeshComponent* mesh);
		bool HasCollisionMesh(const std::string &name);
		static void CreateODERotationMatrix(const Mat4 &m, dReal *ode_mat);
		static void CreateGASSRotationMatrix(const dReal *ode_mat, Mat4 &m);
		void Update(double delta_time);
		dWorldID GetWorld()const {return m_World;}
	protected:
		void OnLoad(MessagePtr message);
		void OnUnload(MessagePtr message);
		void OnLoadSceneObject(MessagePtr message);
		
		void SetGravity(float gravity);
		float GetGravity() const;
		
	//	virtual void Raycast(RaycastInfo* info,TriangleVector *tri_vec = NULL);
	//	virtual bool SphereCollision(Sphere sphere, CollisionData *coldata = NULL);
	//	bool GetTriDataID(CollisionMesh *cm,dTriMeshDataID &id);
	//	static void ConvertToMatrix44(const dReal pos[3], const dReal R[12], float *m);
		static void NearCallback (void *data, dGeomID o1, dGeomID o2);
		void ProcessCollision(dGeomID o1, dGeomID o2);

		
		//static void SetBodyTransformation(const dBodyID body,ISceneNode* node);
		//static void SetGeomTransformation(const dGeomID geom,ISceneNode* node);
		//static void SetNodeTransformation(const dBodyID body,ISceneNode* node);
	//	bool CheckVisibility(BaseObject* obj_from,BaseObject* obj_to);
		//virtual int ThreadProc();
//		static bool WantsContact(IPhysicsObject *current,  dContact & contact, IPhysicsObject * other, dGeomID you, dGeomID him, bool firstTest);
//		ODECollisionSystem* m_Collision;
		
		
	private:
		dWorldID m_World;
		dSpaceID m_Space;
		dSpaceID m_StaticSpace;
		dSpaceID m_CollisionSpace;
		dJointGroupID m_ContactGroup;
		float m_Gravity;
		bool m_Paused;
		CollisionMeshMap m_ColMeshMap;
		bool m_Init;
		
	};
}

