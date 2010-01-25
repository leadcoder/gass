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

#include "Core/Math/Quaternion.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/IMessage.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"
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
		m_UniqueMaterialCreated(false)
	{

	}

	OgreMeshComponent::~OgreMeshComponent()
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

	void OgreMeshComponent::RegisterReflection()
	{
		GASS::ComponentFactory::GetPtr()->Register("MeshComponent",new GASS::Creator<OgreMeshComponent, IComponent>);
		RegisterProperty<std::string>("RenderQueue", &GASS::OgreMeshComponent::GetRenderQueue, &GASS::OgreMeshComponent::SetRenderQueue);
		RegisterProperty<std::string>("Filename", &GASS::OgreMeshComponent::GetFilename, &GASS::OgreMeshComponent::SetFilename);
		RegisterProperty<bool>("CastShadow", &GASS::OgreMeshComponent::GetCastShadow, &GASS::OgreMeshComponent::SetCastShadow);
	}

	void OgreMeshComponent::OnCreate()
	{
		REGISTER_OBJECT_MESSAGE_CLASS(OgreMeshComponent::OnLoad,LoadGFXComponentsMessage,1);
		REGISTER_OBJECT_MESSAGE_CLASS(OgreMeshComponent::OnMeshFileNameMessage,MeshFileMessage,0);
		REGISTER_OBJECT_MESSAGE_CLASS(OgreMeshComponent::OnTexCoordMessage,TextureCoordinateMessage,0);
		REGISTER_OBJECT_MESSAGE_CLASS(OgreMeshComponent::OnColorMessage,ColorMessage,0);
	}

	void OgreMeshComponent::OnLoad(LoadGFXComponentsMessagePtr message)
	{
		OgreGraphicsSceneManagerPtr ogsm = boost::shared_static_cast<OgreGraphicsSceneManager>(message->GetGFXSceneManager());
		assert(ogsm);
		m_ReadyToLoadMesh = true;
		SetFilename(m_Filename);
	}

	void OgreMeshComponent::SetFilename(const std::string &filename) 
	{
		m_Filename = filename;
		if(m_ReadyToLoadMesh)
		{
			OgreLocationComponent * lc = GetSceneObject()->GetFirstComponent<OgreLocationComponent>().get();
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


			m_OgreEntity = lc->GetOgreNode()->getCreator()->createEntity(name,m_Filename);
			lc->GetOgreNode()->attachObject((Ogre::MovableObject*) m_OgreEntity);
			//LoadLightmap();
			if(m_CastShadow)
			{
				m_OgreEntity->setCastShadows(true);
				//??
			}
			else
			{
				m_OgreEntity->setCastShadows(false);
			}

			if(m_RenderQueue == "SkiesLate")
			{
				m_OgreEntity->setRenderQueueGroup(Ogre::RENDER_QUEUE_SKIES_LATE);
			}
			else if(m_RenderQueue == "SkiesEarly")
			{
				m_OgreEntity->setRenderQueueGroup(Ogre::RENDER_QUEUE_SKIES_EARLY);
			}
		}
	}

	/*	void OgreMeshComponent::TryLoadLightmap()
	{
	std::string lightmap = Root::GetPtr()->GetLevel()->GetPath() + "/3dmodels/";
	char temp[255];
	Vec3 pos = m_SceneNode->GetAbsoluteTransformation().GetTranslation();
	std::string base_name = Misc::RemoveExtension(m_Mesh->m_Name);
	sprintf(temp,"%s_%d-%d-%d.tga",base_name.c_str(),(int) pos.x,(int) pos.y,(int) -pos.z);
	lightmap += temp;
	FILE* fp = fopen(lightmap.c_str(),"rb");
	if(fp)
	{
	fclose(fp);
	//Change material of enity
	for(unsigned int i = 0;i < m_OgreEntity->getNumSubEntities();i++)
	{
	Ogre::SubEntity* se = m_OgreEntity->getSubEntity(i);
	Ogre::MaterialPtr mat = se->getMaterial();
	if(!mat.isNull())
	{
	std::string new_name = mat->getName() + lightmap;
	Ogre::MaterialPtr new_mat = mat->clone(new_name);
	Ogre::Technique * technique = new_mat->getTechnique(0);
	Ogre::Pass* pass = technique->getPass(0);
	pass->setLightingEnabled(false);
	Ogre::TextureUnitState * textureUnit = pass->createTextureUnitState(lightmap,1);
	textureUnit->setColourOperationEx(Ogre::LBX_MODULATE_X2);
	se->setMaterialName(new_name);
	//Ogre::MaterialManager::getSingleton().
	}
	}
	}
	}*/



	Ogre::Bone* OgreMeshComponent::GetClosestBone(const Vec3 &pos)
	{
		assert(m_OgreEntity);
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
				Vec3 bone_pos = Convert::ToGASS(bone_cand->_getDerivedPosition());
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
		assert(m_OgreEntity);
		return m_OgreEntity->hasSkeleton();
	}

	AABox OgreMeshComponent::GetBoundingBox() const
	{
		assert(m_OgreEntity);
		return Convert::ToGASS(m_OgreEntity->getBoundingBox());

	}
	Sphere OgreMeshComponent::GetBoundingSphere() const
	{
		Sphere sphere;
		assert(m_OgreEntity);
		sphere.m_Pos = Vec3(0,0,0);
		sphere.m_Radius = m_OgreEntity->getBoundingRadius();
		return sphere;
	}


	void OgreMeshComponent::GetMeshData(MeshDataPtr mesh_data)
	{
		mesh_data->NumVertex = 0;
		mesh_data->VertexVector = NULL;
		mesh_data->NumFaces = 0;
		mesh_data->FaceVector = NULL;

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

	void OgreMeshComponent::AddVertexData(const Ogre::VertexData *vertex_data,MeshDataPtr mesh)
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

	void OgreMeshComponent::AddIndexData(Ogre::IndexData *data, const unsigned int offset,MeshDataPtr mesh)
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
			Ogre::Technique * technique = mat->getTechnique(0);
			Ogre::Pass* pass = technique->getPass(0);
			Ogre::TextureUnitState * textureUnit = pass->getTextureUnitState(0);
			textureUnit->setTextureScroll(speed.x,speed.y);
		}
	}

	void OgreMeshComponent::OnMeshFileNameMessage(MeshFileMessagePtr message)
	{
		std::string name = message->GetFileName();
		SetFilename(name);
	}

	void OgreMeshComponent::OnTexCoordMessage(TextureCoordinateMessagePtr message)
	{
		Vec2 uv = message->GetTextureCoordinates();
		SetTexCoordSpeed(uv);
	}

	void OgreMeshComponent::OnColorMessage(ColorMessagePtr message)
	{
		Vec4 color = message->GetColor();

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
			mat->setDiffuse(color.x,color.y,color.z,color.w);
			if(color.w < 1.0)
			{
				mat->setDepthCheckEnabled(false);
				mat->setSceneBlending(SBT_TRANSPARENT_ALPHA);
			}
			else
			{
				mat->setDepthCheckEnabled(true);
				mat->setSceneBlending(SBT_REPLACE);
			}
		}
	}
}
