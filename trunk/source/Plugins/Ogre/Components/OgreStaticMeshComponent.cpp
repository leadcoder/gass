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
#include <OgreBone.h>
#include <OgreStaticGeometry.h>
#include "Core/Math/Quaternion.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/IMessage.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Plugins/Ogre/OgreGraphicsSceneManager.h"
#include "Plugins/Ogre/Components/OgreStaticMeshComponent.h"
#include "Plugins/Ogre/Components/OgreLocationComponent.h"
#include "Plugins/Ogre/OgreConvert.h"

#include <tinyxml.h>
using namespace Ogre;

namespace GASS
{

	OgreStaticMeshComponent::OgreStaticMeshComponent() : m_OgreEntity(NULL),
		m_CastShadow(true),
		m_ReadyToLoadMesh(false),
		m_UniqueMaterialCreated(false)
	{

	}

	OgreStaticMeshComponent::~OgreStaticMeshComponent()
	{


	}

	void OgreStaticMeshComponent::RegisterReflection()
	{
		GASS::ComponentFactory::GetPtr()->Register("StaticMeshComponent",new GASS::Creator<OgreStaticMeshComponent, IComponent>);
		RegisterProperty<std::string>("RenderQueue", &GASS::OgreStaticMeshComponent::GetRenderQueue, &GASS::OgreStaticMeshComponent::SetRenderQueue);
		//RegisterProperty<std::string>("Filename", &GASS::OgreStaticMeshComponent::GetFilename, &GASS::OgreStaticMeshComponent::SetFilename);
		RegisterProperty<bool>("CastShadow", &GASS::OgreStaticMeshComponent::GetCastShadow, &GASS::OgreStaticMeshComponent::SetCastShadow);
	}

	void OgreStaticMeshComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreStaticMeshComponent::OnLoad,LoadGFXComponentsMessage,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreStaticMeshComponent::OnUnload,UnloadComponentsMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreStaticMeshComponent::OnMeshFileNameMessage,MeshFileMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreStaticMeshComponent::OnTexCoordMessage,TextureCoordinateMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreStaticMeshComponent::OnMaterialMessage,MaterialMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreStaticMeshComponent::OnBoneTransformationMessage,BoneTransformationMessage,0));

	}

	void OgreStaticMeshComponent::LoadXML(TiXmlElement *elem)
	{
		TiXmlElement *prop_elem = elem->FirstChildElement();
		while(prop_elem)
		{
			std::string prop_name = prop_elem->Value();
			if(prop_name == "MeshInstance")
			{
				MeshInstance instance;
				instance.m_Position = Vec3(0,0,0);
				instance.m_Rotation = Quaternion();
				instance.m_Scale = Vec3(1,1,1);
				TiXmlElement *file_elem = prop_elem->FirstChildElement("Filename");
				const std::string mesh_name = file_elem->Attribute("value");
				instance.m_Filename = mesh_name;
				//sort by mesh name
				TiXmlElement *pos_elem = prop_elem->FirstChildElement("Position");
				if(pos_elem)
				{
					const std::string pos_str = pos_elem->Attribute("value");
					std::stringstream(pos_str) >> instance.m_Position;
				}
				TiXmlElement *rot_elem = prop_elem->FirstChildElement("Quaternion");
				if(rot_elem)
				{
					const std::string rot_str = rot_elem->Attribute("value");
					std::stringstream(rot_str) >> instance.m_Rotation;
		
				}

				TiXmlElement *scale_elem = prop_elem->FirstChildElement("Scale");
				if(scale_elem)
				{
					const std::string scale_str = scale_elem->Attribute("value");
					std::stringstream(scale_str) >> instance.m_Scale;
		
				}
				m_MeshInstances[mesh_name].push_back(instance);
				//const std::string mesh_pos = 
			}
			else
			{
				std::string prop_val = prop_elem->FirstAttribute()->Value();
				SetPropertyByString(prop_name,prop_val);
			}
			prop_elem  = prop_elem->NextSiblingElement();
		}
	}

	void OgreStaticMeshComponent::OnLoad(LoadGFXComponentsMessagePtr message)
	{
		OgreGraphicsSceneManagerPtr ogsm = boost::shared_static_cast<OgreGraphicsSceneManager>(message->GetGFXSceneManager());
		assert(ogsm);
		m_ReadyToLoadMesh = true;

		static unsigned int obj_id = 0;
		obj_id++;
		std::stringstream ss;
		std::string name;
		ss << GetName() << obj_id;
		ss >> name;


		Ogre::StaticGeometry *sg = ogsm->GetSceneManger()->createStaticGeometry(name);
		float size = 300;
		sg->setRegionDimensions(Ogre::Vector3(size, size, size));
        sg->setOrigin(Ogre::Vector3(-size/2, 0, -size/2));
		


		if(m_RenderQueue == "SkiesLate")
		{
				sg->setRenderQueueGroup(Ogre::RENDER_QUEUE_SKIES_LATE);
		}
		else if(m_RenderQueue == "SkiesEarly")
		{
			sg->setRenderQueueGroup(Ogre::RENDER_QUEUE_SKIES_EARLY);
		}
		MeshMap::iterator iter = m_MeshInstances.begin();
		while(iter != m_MeshInstances.end())
		{
			Ogre::Entity* entity = ogsm->GetSceneManger()->createEntity(iter->first,iter->first);
			for(size_t i = 0;  i < iter->second.size(); i++)
			{
				Ogre::Vector3  pos = Convert::ToOgre(iter->second.at(i).m_Position);
				Ogre::Quaternion orientation = Convert::ToOgre(iter->second.at(i).m_Rotation);
				Ogre::Vector3  scale = Convert::ToOgre(iter->second.at(i).m_Scale);
				sg->addEntity(entity, pos, Ogre::Quaternion::IDENTITY,scale);
			}
			iter++;
		}
		sg->build();
		//
	//	SetFilename(m_Filename);
	}


	void OgreStaticMeshComponent::OnUnload(UnloadComponentsMessagePtr message)
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


	void OgreStaticMeshComponent::SetFilename(const std::string &filename) 
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
			GetSceneObject()->PostMessage(MessagePtr(new GeometryChangedMessage(shared_from_this())));
		}
	}

	/*	void OgreStaticMeshComponent::TryLoadLightmap()
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



	Ogre::Bone* OgreStaticMeshComponent::GetClosestBone(const Vec3 &pos)
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

	bool OgreStaticMeshComponent::HasSkeleton() const
	{
		assert(m_OgreEntity);
		return m_OgreEntity->hasSkeleton();
	}

	AABox OgreStaticMeshComponent::GetBoundingBox() const
	{
		assert(m_OgreEntity);
		return Convert::ToGASS(m_OgreEntity->getBoundingBox());
	}


	Sphere OgreStaticMeshComponent::GetBoundingSphere() const
	{
		Sphere sphere;
		assert(m_OgreEntity);
		sphere.m_Pos = Vec3(0,0,0);
		sphere.m_Radius = m_OgreEntity->getBoundingRadius();
		return sphere;
	}


	void OgreStaticMeshComponent::GetMeshData(MeshDataPtr mesh_data)
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

	void OgreStaticMeshComponent::AddVertexData(const Ogre::VertexData *vertex_data,MeshDataPtr mesh)
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

	void OgreStaticMeshComponent::AddIndexData(Ogre::IndexData *data, const unsigned int offset,MeshDataPtr mesh)
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

	void OgreStaticMeshComponent::SetTexCoordSpeed(const Vec2 &speed)
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

	void OgreStaticMeshComponent::OnMeshFileNameMessage(MeshFileMessagePtr message)
	{
		std::string name = message->GetFileName();
		SetFilename(name);
	}

	void OgreStaticMeshComponent::OnTexCoordMessage(TextureCoordinateMessagePtr message)
	{
		Vec2 uv = message->GetTextureCoordinates();
		SetTexCoordSpeed(uv);
	}

	void OgreStaticMeshComponent::OnMaterialMessage(MaterialMessagePtr message)
	{


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

			mat->setDiffuse(diffuse.x,diffuse.y,diffuse.z,diffuse.w);
			mat->setAmbient(ambient.x,ambient.y,ambient.z);
			mat->setSpecular(specular.x,specular.y,specular.z,1);
			mat->setSelfIllumination(si.x,si.y,si.z);
			mat->setShininess(message->GetShininess());

			//mat->setAmbient(ambinet.x,ambinet.y,ambinet.z);
			if(diffuse.w < 1.0)
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

	void OgreStaticMeshComponent::OnBoneTransformationMessage(BoneTransformationMessagePtr message)
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
			Ogre::SceneNode* parent = GetSceneObject()->GetFirstComponent<OgreLocationComponent>().get()->GetOgreNode();
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
}
