/****************************************************************************
*                                                                           *
* HiFiEngine                                                                *
* Copyright (C)2003 - 2005 Johan Hedstrom                                   *
* Email: hifiengine@gmail.com                                               *
* Web page: http://n00b.dyndns.org/HiFiEngine                               *
*                                                                           *
* HiFiEngine is only used with knowledge from the author. This software     *
* is not allowed to redistribute without permission from the author.        *
* For further license information, please turn to the product home page or  *
* contact author. Abuse against the HiFiEngine license is prohibited by law.*
*                                                                           *
*****************************************************************************/ 

#include "PhysXPhysicsSystem.h"
#include "PhysXGeometry.h"

#include "Sim/Components/Graphics/Geometry/IMeshComponent.h"
#include "Core/System/SystemFactory.h"
#include "Sim/Scenario/Scene/SceneManagerFactory.h"
#include "Sim/Systems/SimSystemManager.h"


#include "NxPhysics.h"
#include "SDKUtils/UserAllocator.h"
#include "Core/Utils/Log.h"
#include "SDKUtils/ErrorStream.h"
#include "SDKUtils/Stream.h"
#include "SDKUtils/Utilities.h"
#include "SDKUtils/cooking.h"

namespace GASS
{
	PhysXPhysicsSystem::PhysXPhysicsSystem(): m_PhysicsSDK(NULL)
	{
		//m_Scene = NULL;
	}

	PhysXPhysicsSystem::~PhysXPhysicsSystem()
	{

	}

	void PhysXPhysicsSystem::RegisterReflection()
	{
		SystemFactory::GetPtr()->Register("PhysXPhysicsSystem",new GASS::Creator<PhysXPhysicsSystem, ISystem>);
	}

	void PhysXPhysicsSystem::OnCreate()
	{
		GetSimSystemManager()->RegisterForMessage(SYSTEM_RM_INIT, MESSAGE_FUNC( PhysXPhysicsSystem::OnInit));
	}

	void PhysXPhysicsSystem::OnInit(MessagePtr message)
	{
		// Initialize PhysicsSDK
		NxPhysicsSDKDesc desc;
		
		/*UserAllocator*	  gAllocator  = NULL;	
		// Create a memory allocator
		if (!gAllocator)
		{
		gAllocator = new UserAllocator;
		assert(gAllocator);
		}*/

		// Create the physics SDK
		NxSDKCreateError errorCode;
		m_PhysicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, NULL,new ErrorStream(), desc, &errorCode);
		//if (!m_PhysicsSDK)  
		{
			switch(errorCode)
			{
			case NXCE_NO_ERROR:
				Log::Print("No errors occurred when creating the Physics SDK");
				break;
			case NXCE_PHYSX_NOT_FOUND:
				Log::Error("Unable to find the PhysX libraries. The PhysX drivers are not installed correctly.");
				break;

			case NXCE_WRONG_VERSION:
				Log::Error("The application supplied a version number that does not match with the libraries.");
				break;

			case NXCE_DESCRIPTOR_INVALID:
				Log::Error("The supplied SDK descriptor is invalid.");
				break;

			case NXCE_CONNECTION_ERROR:
				Log::Error("A PhysX card was found, but there are problems when communicating with the card.");
				break;

			case NXCE_RESET_ERROR:
				Log::Error("A PhysX card was found, but it did not reset (or initialize) properly.");
				break;

			case NXCE_IN_USE_ERROR:
				Log::Error("A PhysX card was found, but it is already in use by another application.");
				break;

			case NXCE_BUNDLE_ERROR:
				Log::Error("A PhysX card was found, but there are issues with loading the firmware.");
				break;
			}
		}
		
		// Set the debug visualization parameters
		/*m_PhysicsSDK->setParameter(NX_VISUALIZATION_SCALE, 1);
		m_PhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_SHAPES, 1);
		m_PhysicsSDK->setParameter(NX_VISUALIZE_ACTOR_AXES, 1);
		m_PhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_FNORMALS, 1);*/


		//m_PhysicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, NULL, new ErrorStream(), desc, &errorCode);
		if(m_PhysicsSDK == NULL) 
		{
			Log::Error("\nSDK create error (%d - %s).\nUnable to initialize the PhysX SDK, exiting the sample.\n\n", errorCode, getNxSDKCreateError(errorCode));
			return ;
		}
#if SAMPLES_USE_VRD
		// The settings for the VRD host and port are found in SampleCommonCode/SamplesVRDSettings.h
		if (m_PhysicsSDK->getFoundationSDK().getRemoteDebugger())
			m_PhysicsSDK->getFoundationSDK().getRemoteDebugger()->connect(SAMPLES_VRD_HOST, SAMPLES_VRD_PORT, SAMPLES_VRD_EVENTMASK);
#endif

		m_PhysicsSDK->setParameter(NX_SKIN_WIDTH, 0.01f);

		m_PhysicsSDK->getFoundationSDK().getRemoteDebugger()->connect ("localhost", 5425);
 


	}

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
	/*	mTriangleMeshDescription.numVertices				= vertexCount;
		mTriangleMeshDescription.points						= meshVertices;							
		mTriangleMeshDescription.pointStrideBytes			= sizeof(NxVec3);
		// Triangles
		mTriangleMeshDescription.numTriangles				= indexCount / 3;
		mTriangleMeshDescription.triangles					= meshFaces;
		mTriangleMeshDescription.triangleStrideBytes		= 3 * sizeof(NxU32);
		// Materials
		//#if 0
		mTriangleMeshDescription.materialIndexStride		= sizeof(NxMaterialIndex);
		mTriangleMeshDescription.materialIndices			= materials;
	*/	//#endif
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

	/*bool status = NxCookTriangleMesh(mTriangleMeshDescription, buf);
	if (status)
	{
		trimesh = m_PhysicsSDK->createTriangleMesh(MemoryReadBuffer(buf.data));
	}
	else
	{
		assert(false);
		trimesh = NULL;
	}*/

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






	/*	MemoryWriteBuffer buf;
		if (!NxCookTriangleMesh(mTriangleMeshDescription, buf)) 
		{
			std::stringstream s;
			s	<< "Mesh '"  << "' failed to cook"
				<< "V(" << meshVertices << ") F(" << meshFaces << ")";

			Log::Error("%s",s.str().c_str());//NxThrow_Error(s.str());
		}
		trimesh = m_PhysicsSDK->createTriangleMesh(MemoryReadBuffer(buf.data));*/




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
	}
}
