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
#include "NxPhysics.h"
#include "UserAllocator.h"
#include "Core/Utils/Log.h"
#include "ErrorStream.h"
#include "Stream.h"
#include "Utilities.h"
//#include "cooking.h"

namespace GASS
{
	PhysXPhysicsSystem::PhysXPhysicsSystem(): m_PhysicsSDK(NULL)
	{
		//m_Scene = NULL;
	}
	PhysXPhysicsSystem::~PhysXPhysicsSystem()
	{

	}


	void PhysXPhysicsSystem::OnInit(MessagePtr message)
	{
		// Initialize PhysicsSDK
		NxPhysicsSDKDesc desc;
	UserAllocator*	  gAllocator  = NULL;	
		// Create a memory allocator
    if (!gAllocator)
	{
		gAllocator = new UserAllocator;
		assert(gAllocator);
	}
	
	// Create the physics SDK
	NxSDKCreateError errorCode;
    m_PhysicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, gAllocator,new ErrorStream(), desc, &errorCode);
    if (!m_PhysicsSDK)  return;

	// Set the physics parameters
	m_PhysicsSDK->setParameter(NX_SKIN_WIDTH, 0.01);

	// Set the debug visualization parameters
	m_PhysicsSDK->setParameter(NX_VISUALIZATION_SCALE, 1);
	m_PhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_SHAPES, 1);
	m_PhysicsSDK->setParameter(NX_VISUALIZE_ACTOR_AXES, 1);
	m_PhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_FNORMALS, 1);


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

		m_PhysicsSDK->setParameter(NX_SKIN_WIDTH, 0.05f);

	}

	
	/*void PhysXPhysicsSystem::InitCollisionMesh(CollisionMesh* mesh)
	{
		unsigned int	mVertexCount = mesh->NumVertex, mIndexCount  = mesh->NumFaces;
		NxVec3* mMeshVertices = new NxVec3[mVertexCount];
		NxU32* mMeshFaces = new NxU32[mIndexCount];
		NxMaterialIndex* mMaterials = new NxMaterialIndex[mIndexCount];

		NxMaterialIndex currentMaterialIndex = 0;
//		bool use32bitindexes;

		for(unsigned int  i = 0;  i < mesh->NumVertex;i++)
		{
			Vec3 pos = mesh->VertexVector[i];
			mMeshVertices[i] = NxVec3(pos.x,pos.y,pos.z); 
		}

		for(unsigned int  i = 0;  i < mesh->NumFaces;i++)
		{
			mMaterials[i] = mesh->MatIDVector[i];
			mMeshFaces[i] = mesh->FaceVector[i]; 
		}

		NxTriangleMeshDesc mTriangleMeshDescription;

		// Vertices
		mTriangleMeshDescription.numVertices				= mVertexCount;
		mTriangleMeshDescription.points						= mMeshVertices;							
		mTriangleMeshDescription.pointStrideBytes			= sizeof(NxVec3);
		// Triangles
		mTriangleMeshDescription.numTriangles				= mIndexCount / 3;
		mTriangleMeshDescription.triangles					= mMeshFaces;
		mTriangleMeshDescription.triangleStrideBytes		= 3 * sizeof(NxU32);
		// Materials
		//#if 0
		mTriangleMeshDescription.materialIndexStride		= sizeof(NxMaterialIndex);

		mTriangleMeshDescription.materialIndices			= mMaterials;
		//#endif
		//mTriangleMeshDescription.flags					= NX_MF_HARDWARE_MESH;

		NxTriangleMesh* trimesh;

#ifndef NX_DEBUG

		MemoryWriteBuffer buf;
		if (!NxCookTriangleMesh(mTriangleMeshDescription, buf)) {
			std::stringstream s;
			s	<< "Mesh '"  << "' failed to cook"
				<< "V(" << mMeshVertices << ") F(" << mMeshFaces << ")";

			Log::Error("%s",s.str());//NxThrow_Error(s.str());
		}
		trimesh = m_Scene->getPhysicsSDK().createTriangleMesh(MemoryReadBuffer(buf.data));

#else

		NxString filename;
		if (Ogre::StringUtil::endsWith(meshName, ".mesh")) {
			filename = meshName.substr(0, meshName.length() - 5) + ".TriangleMeshShape.nxs";	
		}
		else {
			filename = meshName + ".TriangleMeshShape.nxs";
		}

		UserStream buf(filename.c_str(),false);

		if (!NxCookTriangleMesh(mTriangleMeshDescription, buf)) {
			std::stringstream s;
			s	<< "Mesh '" << meshName << "' failed to cook"
				<< "V(" << mMeshVertices << ") F(" << mMeshFaces << ")";

			NxThrow_Error(s.str());
		}

		fclose(buf.fp);

		UserStream rbuf(filename.c_str(), true);

		trimesh = scene->getPhysicsSDK().createTriangleMesh(rbuf);

		fclose(rbuf.fp);



#endif
		m_TriMeshMap[mesh] = trimesh;

//		delete []vertices;
//		delete []indices;

		delete []mMeshVertices;
		delete []mMeshFaces;
		delete []mMaterials;

//		return trimesh;

	}

	NxTriangleMesh* PhysXPhysicsSystem::GetTriMesh(CollisionMesh *cm)
	{
		//MeshGeometry* entity = DYNAMIC_CAST(MeshGeometry,geom);
		//if(entity)
		{
			TriMeshMap::iterator iter;
			iter = m_TriMeshMap.find(cm);
			if (iter!= m_TriMeshMap.end()) //in map.
			{
				return m_TriMeshMap[cm];
				
			}
		}
		return NULL;
	}*/
	
	
}
