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

#include <OgreSceneNode.h>
#include <OgreEntity.h>
#include <OgreSubEntity.h>
#include <OgreSceneManager.h>
#include <OgreTextureUnitState.h>
#include <OgreSkeletonInstance.h>
#include <OgreSubMesh.h>
#include <OgreMaterialManager.h>

#include "Core/Math/GASSQuaternion.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Plugins/Ogre/OgreGraphicsSceneManager.h"
#include "Plugins/Ogre/Components/OgreMeshComponent.h"
#include "Plugins/Ogre/Components/OgreLocationComponent.h"
#include "Plugins/Ogre/OgreConvert.h"
#include <OgreBone.h>
using namespace Ogre;

namespace GASS
{

	OgreMeshComponent::OgreMeshComponent() : m_OgreEntity(NULL),
		m_CastShadow(true),
		m_ReadyToLoadMesh(false),
		m_UniqueMaterialCreated(false),
		m_GeomFlags(GEOMETRY_FLAG_UNKOWN),
		m_RenderQueue(RENDER_QUEUE_WORLD_GEOMETRY_1)
	{

	}

	OgreMeshComponent::~OgreMeshComponent()
	{


	}

	void OgreMeshComponent::RegisterReflection()
	{
		GASS::ComponentFactory::GetPtr()->Register("MeshComponent",new GASS::Creator<OgreMeshComponent, IComponent>);
		RegisterEnumProperty<RenderQueueBinder>("RenderQueue", &GASS::OgreMeshComponent::GetRenderQueue, &GASS::OgreMeshComponent::SetRenderQueue);
		RegisterEnumProperty<MeshResource>("Filename", &GASS::OgreMeshComponent::GetMeshResource, &GASS::OgreMeshComponent::SetMeshResource);
		RegisterProperty<bool>("CastShadow", &GASS::OgreMeshComponent::GetCastShadow, &GASS::OgreMeshComponent::SetCastShadow);
	}

	void OgreMeshComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreMeshComponent::OnLocationLoaded,LocationLoadedMessage,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreMeshComponent::OnMeshFileNameMessage,MeshFileMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreMeshComponent::OnTexCoordMessage,TextureCoordinateMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreMeshComponent::OnMaterialMessage,MaterialMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreMeshComponent::OnBoneTransformationMessage,BoneTransformationMessage,0));

	}

	void OgreMeshComponent::OnLocationLoaded(LocationLoadedMessagePtr message)
	{
		OgreGraphicsSceneManagerPtr ogsm =  GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<OgreGraphicsSceneManager>();
		assert(ogsm);
		m_ReadyToLoadMesh = true;
		SetMeshResource(m_MeshResource);
	}

	void OgreMeshComponent::OnDelete()
	{
		if(m_OgreEntity && m_UniqueMaterialCreated)
		{
			//relase material
			for(unsigned int i = 0 ; i < m_OgreEntity->getNumSubEntities(); i++)
			{
				Ogre::SubEntity* se = m_OgreEntity->getSubEntity(i);
				Ogre::MaterialPtr mat = se->getMaterial();
				Ogre::MaterialManager::getSingleton().remove(mat->getName());
			}
		}
	}

	void OgreMeshComponent::SetMeshResource(const MeshResource &res) 
	{
		m_MeshResource = res;

		if(m_MeshResource.Valid() && m_ReadyToLoadMesh)
		{
			OgreLocationComponent * lc = GetSceneObject()->GetFirstComponentByClass<OgreLocationComponent>().get();
			if(m_OgreEntity) //release previous mesh
			{
				lc->GetOgreNode()->detachObject(m_OgreEntity);
				lc->GetOgreNode()->getCreator()->destroyEntity(m_OgreEntity);
			}
			static unsigned int obj_id = 0;
			obj_id++;
			std::stringstream ss;
			std::string name;
			ss << GetName() << obj_id;
			ss >> name;

			m_OgreEntity = lc->GetOgreNode()->getCreator()->createEntity(name,m_MeshResource.Name());
			lc->GetOgreNode()->attachObject((Ogre::MovableObject*) m_OgreEntity);
			//m_OgreEntity->setQueryFlags(1);
			//LoadLightmap();
			SetCastShadow(m_CastShadow);
			SetRenderQueue(m_RenderQueue);

			GetSceneObject()->PostMessage(MessagePtr(new GeometryChangedMessage(boost::shared_dynamic_cast<IGeometryComponent>(shared_from_this()))));
		}
	}

	Ogre::Bone* OgreMeshComponent::GetClosestBone(const Vec3 &pos)
	{
		if(!m_OgreEntity)
			return NULL;
		Ogre::Bone* bone = NULL;
		float min_dist = 0;
		if(m_OgreEntity->hasSkeleton())
		{
			Ogre::SkeletonInstance* skeleton = m_OgreEntity->getSkeleton();
			Ogre::Skeleton::BoneIterator bone_iter = skeleton->getBoneIterator();
			while(bone_iter.hasMoreElements())
			{
				Ogre::Bone* bone_cand = bone_iter.getNext();
				//Ogre::Vector3 bone_pos = bone->getWorldPosition();
				//Vec3 bone_pos = Convert::ToGASS(bone_cand->getWorldPosition());
				Vec3 bone_pos = Convert::ToGASS(bone_cand->getPosition());
				//add node pos?
				float  dist = (pos - bone_pos).SquaredLength();
				if(dist < min_dist || bone == NULL)
				{
					min_dist = dist;
					bone = bone_cand;
				}
			}
		}
		return bone;
	}

	bool OgreMeshComponent::HasSkeleton() const
	{
		if(m_OgreEntity)
			return m_OgreEntity->hasSkeleton();
		else
			return false;
	}

	AABox OgreMeshComponent::GetBoundingBox() const
	{
		if(m_OgreEntity)
			return Convert::ToGASS(m_OgreEntity->getBoundingBox());
		else
			return AABox(Vec3(-1,-1,-1),Vec3(1,1,1));
	}


	Sphere OgreMeshComponent::GetBoundingSphere() const
	{
		if(m_OgreEntity)
		{
			Sphere sphere;
			sphere.m_Pos = Vec3(0,0,0);
			sphere.m_Radius = m_OgreEntity->getBoundingRadius();
			return sphere;
		}
		else
			return Sphere(Vec3(0,0,0),1);
	}


	void OgreMeshComponent::GetMeshData(MeshDataPtr mesh_data) const
	{
		mesh_data->NumVertex = 0;
		mesh_data->VertexVector = NULL;
		mesh_data->NumFaces = 0;
		mesh_data->FaceVector = NULL;


		if(m_OgreEntity == NULL)
			return;


		Ogre::MeshPtr mesh = m_OgreEntity->getMesh();

		if(mesh->sharedVertexData)
		{
			AddVertexData(mesh->sharedVertexData,mesh_data);
		}

		for(unsigned int i = 0;i < mesh->getNumSubMeshes();++i)
		{
			SubMesh *sub_mesh = mesh->getSubMesh(i);

			if (!sub_mesh->useSharedVertices)
			{
				AddIndexData(sub_mesh->indexData,mesh_data->NumVertex,mesh_data);
				AddVertexData(sub_mesh->vertexData,mesh_data);
			}
			else
			{
				AddIndexData(sub_mesh->indexData,0,mesh_data);
			}
		}
		mesh_data->NumFaces = mesh_data->NumFaces/3.0;
	}

	void OgreMeshComponent::AddVertexData(const Ogre::VertexData *vertex_data,MeshDataPtr mesh) const
	{
		if (!vertex_data)
			return;

		const VertexData *data = vertex_data;

		const unsigned int prev_size = mesh->NumVertex;
		mesh->NumVertex += (unsigned int)data->vertexCount;

		Vec3* tmp_vert = new Vec3[mesh->NumVertex];
		if (mesh->VertexVector)
		{
			memcpy(tmp_vert,mesh->VertexVector,sizeof(Vec3) * prev_size);
			delete[] mesh->VertexVector;
		}
		mesh->VertexVector = tmp_vert;

		// Get the positional buffer element
		{
			const Ogre::VertexElement* posElem = data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);
			Ogre::HardwareVertexBufferSharedPtr vbuf = data->vertexBufferBinding->getBuffer(posElem->getSource());
			const unsigned int vSize = (unsigned int)vbuf->getVertexSize();

			unsigned char* vertex = static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
			float* pReal;
			Vec3 * curVertices = &mesh->VertexVector[prev_size];
			const unsigned int vertexCount = (unsigned int)data->vertexCount;
			for(unsigned int j = 0; j < vertexCount; ++j)
			{
				posElem->baseVertexPointerToElement(vertex, &pReal);
				vertex += vSize;

				curVertices->x = (*pReal++);
				curVertices->y = (*pReal++);
				curVertices->z = (*pReal++);

				//*curVertices = _transform * (*curVertices);

				curVertices++;
			}
			vbuf->unlock();
		}
	}

	void OgreMeshComponent::AddIndexData(Ogre::IndexData *data, const unsigned int offset,MeshDataPtr mesh) const
	{
		const unsigned int prev_size = mesh->NumFaces;
		mesh->NumFaces += (unsigned int)data->indexCount;

		unsigned int* tmp_ind = new unsigned int[mesh->NumFaces];
		if (mesh->FaceVector)
		{
			memcpy (tmp_ind, mesh->FaceVector, sizeof(unsigned int) * prev_size);
			delete[] mesh->FaceVector;
		}
		mesh->FaceVector = tmp_ind;

		const unsigned int numTris = (unsigned int) data->indexCount / 3;
		HardwareIndexBufferSharedPtr ibuf = data->indexBuffer;
		const bool use32bitindexes = (ibuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);
		unsigned int index_offset = prev_size;

		if (use32bitindexes)
		{
			const unsigned int* pInt = static_cast<unsigned int*>(ibuf->lock(HardwareBuffer::HBL_READ_ONLY));
			for(unsigned int k = 0; k < numTris; ++k)
			{
				mesh->FaceVector[index_offset ++] = offset + *pInt++;
				mesh->FaceVector[index_offset ++] = offset + *pInt++;
				mesh->FaceVector[index_offset ++] = offset + *pInt++;
			}
			ibuf->unlock();
		}
		else
		{
			const unsigned short* pShort = static_cast<unsigned short*>(ibuf->lock(HardwareBuffer::HBL_READ_ONLY));
			for(unsigned int k = 0; k < numTris; ++k)
			{
				mesh->FaceVector[index_offset ++] = offset + static_cast<unsigned int> (*pShort++);
				mesh->FaceVector[index_offset ++] = offset + static_cast<unsigned int> (*pShort++);
				mesh->FaceVector[index_offset ++] = offset + static_cast<unsigned int> (*pShort++);
			}
			ibuf->unlock();
		}
	}

	void OgreMeshComponent::SetTexCoordSpeed(const Vec2 &speed)
	{
		if(!m_OgreEntity)
			return;

		if(!m_UniqueMaterialCreated) //material clone hack to only set texcoord scroll speed for this mesh
		{
			for(unsigned int i = 0 ; i < m_OgreEntity->getNumSubEntities(); i++)
			{
				Ogre::SubEntity* se = m_OgreEntity->getSubEntity(i);
				Ogre::MaterialPtr mat = se->getMaterial();
				mat = mat->clone(m_OgreEntity->getName() + mat->getName());
				se->setMaterial(mat);
			}
			m_UniqueMaterialCreated = true;
		}

		for(unsigned int i = 0 ; i < m_OgreEntity->getNumSubEntities(); i++)
		{

			Ogre::SubEntity* se = m_OgreEntity->getSubEntity(i);

			Ogre::MaterialPtr mat = se->getMaterial();
			if(mat->getNumTechniques() > 0)
			{
				Ogre::Technique * technique = mat->getTechnique(0);
				if(technique->getNumPasses() > 0)
				{
					Ogre::Pass* pass = technique->getPass(0);

					/*static Vec2 uv_offset(0,0);
					uv_offset.x += 0.1*speed.x;
					uv_offset.y += 0.1*speed.y;

					if(pass->getVertexProgram().get())
					{
					pass->getVertexProgramParameters()->setNamedConstant("texOffset",Ogre::Vector3(uv_offset.x ,uv_offset.y,0));
					}
					else*/
					{
						if(pass->getNumTextureUnitStates() > 0)
						{
							Ogre::TextureUnitState * textureUnit = pass->getTextureUnitState(0);
							textureUnit->setTextureScroll(speed.x,speed.y);
						}
					}
				}
			}
		}
	}

	void OgreMeshComponent::OnMeshFileNameMessage(MeshFileMessagePtr message)
	{
		MeshResource resource(message->GetFileName());
		SetMeshResource(resource);
	}

	void OgreMeshComponent::OnTexCoordMessage(TextureCoordinateMessagePtr message)
	{
		Vec2 uv = message->GetTextureCoordinates();
		SetTexCoordSpeed(uv);
	}

	void OgreMeshComponent::OnMaterialMessage(MaterialMessagePtr message)
	{

		if(!m_OgreEntity)
			return;


		if(!m_UniqueMaterialCreated) 
		{
			for(unsigned int i = 0 ; i < m_OgreEntity->getNumSubEntities(); i++)
			{
				Ogre::SubEntity* se = m_OgreEntity->getSubEntity(i);
				Ogre::MaterialPtr mat = se->getMaterial();
				mat = mat->clone(m_OgreEntity->getName() + mat->getName());
				se->setMaterial(mat);
			}
			m_UniqueMaterialCreated = true;
		}
		for(unsigned int i = 0 ; i < m_OgreEntity->getNumSubEntities(); i++)
		{
			Ogre::SubEntity* se = m_OgreEntity->getSubEntity(i);
			Ogre::MaterialPtr mat = se->getMaterial();
			Vec4 diffuse = message->GetDiffuse();
			Vec3 ambient = message->GetAmbient();
			Vec3 specular = message->GetSpecular();
			Vec3 si = message->GetSelfIllumination();

			if(diffuse.w >= 0)
				mat->setDiffuse(diffuse.x,diffuse.y,diffuse.z,diffuse.w);
			if(ambient.x >= 0)
				mat->setAmbient(ambient.x,ambient.y,ambient.z);
			if(specular.x >= 0)
				mat->setSpecular(specular.x,specular.y,specular.z,1);
			if(si.x >= 0)
				mat->setSelfIllumination(si.x,si.y,si.z);
			if(message->GetShininess() >= 0)
				mat->setShininess(message->GetShininess());

			//mat->setAmbient(ambinet.x,ambinet.y,ambinet.z);
			if(diffuse.w < 1.0)
			{
				mat->setDepthWriteEnabled(false);
				mat->setSceneBlending(SBT_TRANSPARENT_ALPHA);
			}
			else
			{
				mat->setDepthWriteEnabled(true);
				mat->setSceneBlending(SBT_REPLACE);
			}
		}
	}


	void OgreMeshComponent::SetCastShadow(bool castShadow) 
	{
		m_CastShadow = castShadow;
		if(m_OgreEntity)
			m_OgreEntity->setCastShadows(m_CastShadow);
	}


	void OgreMeshComponent::SetRenderQueue(const RenderQueueBinder &rq) 
	{
		m_RenderQueue = rq;
		if(m_OgreEntity)
		{
			m_OgreEntity->setRenderQueueGroup(m_RenderQueue.GetValue());
		}
	}

	void OgreMeshComponent::OnBoneTransformationMessage(BoneTransformationMessagePtr message)
	{
		Ogre::Bone* bone;


		if(message->GetName() == "")
		{
			//bone = GetClosestBone(pos);
			return;
		}
		else
		{
			Ogre::SkeletonInstance* skeleton = m_OgreEntity->getSkeleton();
			bone = skeleton->getBone(message->GetName());
		}
		if(bone)
		{

			Vec3 pos = message->GetPosition();
			if(!bone->isManuallyControlled())
			{
				bone->setManuallyControlled(true);
			}
			bone->setPosition(pos.x,pos.y,pos.z);

			/*Ogre::Vector3 worldPos(pos.x, pos.y, pos.z); // desired position in world coords
			Ogre::SceneNode* parent = GetSceneObject()->GetFirstComponentByClass<OgreLocationComponent>().get()->GetOgreNode();
			Ogre::Vector3 parentPos = parent->_getDerivedPosition(); // node local pos
			Ogre::Vector3 parentQuatXbonePos = worldPos - parentPos;
			Ogre::Quaternion parentQuat = parent->_getDerivedOrientation(); // node local ori
			Ogre::Vector3 bonePos = parentQuat.Inverse() * parentQuatXbonePos;
			Ogre::Vector3 inverseParentScale = 1.0 / parent->_getDerivedScale();

			bone->setPosition(bonePos * inverseParentScale);
			Ogre::Quaternion worldQuat(1.0, 0, 0, 0); // desired orientation in world terms
			Ogre::Quaternion boneQuat = worldQuat.Inverse() * parentQuat; // equiv to ("boneQuat = worldQuat / parentQuat")
			bone->setOrientation(boneQuat);*/
		}
	}

	GeometryFlags OgreMeshComponent::GetGeometryFlags() const
	{
		return m_GeomFlags;
	}

	void OgreMeshComponent::SetGeometryFlags(GeometryFlags flags)
	{
		m_GeomFlags = flags;
	}
}
