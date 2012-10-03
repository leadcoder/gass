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

#include "PhysXPhysicsSystem.h"

using namespace physx;
namespace GASS
{

	class PxGASSErrorCallback : public PxErrorCallback
	{
	public:
		PxGASSErrorCallback() :PxErrorCallback(){}
		virtual ~PxGASSErrorCallback(){}

		virtual void PxGASSErrorCallback::reportError(PxErrorCode::Enum e, const char* message, const char* file, int line)
		{
			std::string errorCode;

			switch (e)
			{
			case PxErrorCode::eINVALID_PARAMETER:
				errorCode = "invalid parameter";
				break;
			case PxErrorCode::eINVALID_OPERATION:
				errorCode = "invalid operation";
				break;
			case PxErrorCode::eOUT_OF_MEMORY:
				errorCode = "out of memory";
				break;
			case PxErrorCode::eDEBUG_INFO:
				errorCode = "info";
				break;
			case PxErrorCode::eDEBUG_WARNING:
				errorCode = "warning";
				break;
			default:
				errorCode = "unknown error";
				break;
			}
			std::stringstream ss;
			ss << "PhysX Error Callback:" << file << "(" << line << ") :" << errorCode << " : " << message << "\n";
			GASS::LogManager::getSingleton().stream() << ss.str();

		}
	};

	PhysXPhysicsSystem::PhysXPhysicsSystem(): m_DefaultMaterial(NULL),
		m_PhysicsSDK(NULL),
		m_Foundation(NULL)
	{
		
	}

	PhysXPhysicsSystem::~PhysXPhysicsSystem()
	{
		if(m_PhysicsSDK)
			m_PhysicsSDK->release();
	}

	void PhysXPhysicsSystem::RegisterReflection()
	{
		SystemFactory::GetPtr()->Register("PhysXPhysicsSystem",new GASS::Creator<PhysXPhysicsSystem, ISystem>);
	}

	void PhysXPhysicsSystem::OnCreate()
	{
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(PhysXPhysicsSystem::OnInit,InitSystemMessage,0));
	}
	PxGASSErrorCallback myErrorCallback;
	    

	void PhysXPhysicsSystem::OnInit(InitSystemMessagePtr message)
	{
		bool recordMemoryAllocations = false;
		
		m_Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_DefaultAllocator, myErrorCallback);
		m_PhysicsSDK = PxCreatePhysics(PX_PHYSICS_VERSION, *m_Foundation, physx::PxTolerancesScale(), recordMemoryAllocations );
		PxInitExtensions(*m_PhysicsSDK );
		if(m_PhysicsSDK == NULL) 
		{
			GASS_EXCEPT(Exception::ERR_INTERNAL_ERROR,"Error creating PhysX device!", "PhysXPhysicsSystem::OnInit");
		}
		if(!PxInitExtensions(*m_PhysicsSDK))
			GASS_EXCEPT(Exception::ERR_INTERNAL_ERROR,"PxInitExtensions failed!", "PhysXPhysicsSystem::OnInit");
		
		m_DefaultMaterial = m_PhysicsSDK->createMaterial(0.5,0.5,0.5);
		//physx::PxExtensionVisualDebugger::connect(mSDK->getPvdConnectionManager(), "127.0.0.1", 5425, 10, true,physx::PxGetDefaultDebuggerFlags());
	}

/*
	NxCollisionMesh PhysXPhysicsSystem::CreateCollisionMesh(IMeshComponent* mesh)
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

		unsigned int vertexCount = mesh_data->NumVertex;
		unsigned int indexCount  = mesh_data->NumFaces*3;
		NxVec3* meshVertices = new NxVec3[vertexCount];
		NxU32* meshFaces = new NxU32[indexCount];
		NxMaterialIndex* materials = new NxMaterialIndex[indexCount];

		NxMaterialIndex currentMaterialIndex = 0;
		//		bool use32bitindexes;

		for(unsigned int  i = 0;  i < mesh_data->NumVertex;i++)
		{
			Vec3 pos = mesh_data->VertexVector[i];
			//if(i > 10)
			//	meshVertices[i] = NxVec3(pos.x, 10, pos.z); 
			meshVertices[i] = NxVec3(pos.x, pos.y, pos.z); 
		}

		for(unsigned int  i = 0;  i < indexCount;i++)
		{
			materials[i] = 0;
			meshFaces[i] = mesh_data->FaceVector[i]; 
		}

		NxTriangleMeshDesc mTriangleMeshDescription;

		// Vertices
		//mTriangleMeshDescription.flags					= NX_MF_HARDWARE_MESH;

			mTriangleMeshDescription.numVertices			= vertexCount;
	mTriangleMeshDescription.pointStrideBytes		= sizeof(NxVec3);
	mTriangleMeshDescription.points				= meshVertices;
	mTriangleMeshDescription.numTriangles			= indexCount/3;
	mTriangleMeshDescription.flags					= NX_MF_HARDWARE_MESH;//NX_MF_FLIPNORMALS;
	mTriangleMeshDescription.triangles				= meshFaces;
	mTriangleMeshDescription.triangleStrideBytes	= 3 * sizeof(NxU32);
	


		NxTriangleMesh* trimesh;




	
	
	//Alternative:	see NxMeshFlags
	//triangleMeshDesc->flags				= NX_MF_16_BIT_INDICES
	//triangleMeshDesc->triangles			= indices16;
	//triangleMeshDesc->triangleStrideBytes	= 3 * sizeof(NxU16);

	// The actor has one shape, a triangle mesh
	NxInitCooking();
	MemoryWriteBuffer buf;

    bool status = NxCookTriangleMesh(mTriangleMeshDescription, UserStream("c:/temp/text.bin", false));
	if (status)
	{
		
		trimesh = m_PhysicsSDK->createTriangleMesh(UserStream("c:/temp/text.bin", true));
	}
	else
	{
		assert(false);
		trimesh = NULL;
	}

	NxCloseCooking();


	



		NxCollisionMesh col_mesh;
		col_mesh.NxMesh = trimesh;
		col_mesh.Mesh = mesh_data;
		m_ColMeshMap[col_mesh_name] = col_mesh;

		//delete []meshVertices;
		//delete []meshFaces;
		//delete []materials;
		return col_mesh;
	}

	bool PhysXPhysicsSystem::HasCollisionMesh(const std::string &name)
	{
		CollisionMeshMap::iterator iter;
		iter = m_ColMeshMap.find(name);
		if (iter!= m_ColMeshMap.end()) //in map.
		{
			return true;
		}
		return false;
	}*/
}
