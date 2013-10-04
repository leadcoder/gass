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


#include "Plugins/Ogre/Components/OgreMeshComponent.h"
#include "Plugins/Ogre/OgreGraphicsSceneManager.h"
#include "Plugins/Ogre/Components/OgreLocationComponent.h"
#include "Plugins/Ogre/OgreConvert.h"

#include <OgreBone.h>
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
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSResourceManager.h"
#include "Sim/GASSResourceGroup.h"


using namespace Ogre;

namespace GASS
{

	OgreMeshComponent::MeshMaterialCache OgreMeshComponent::m_MeshMaterialCache;

	std::vector<std::string> OgreMeshEnumerationMetaData::GetEnumeration(BaseReflectionObjectPtr object) const
	{
		std::vector<std::string> content;
		OgreMeshComponentPtr mesh = DYNAMIC_PTR_CAST<OgreMeshComponent>(object);
		if(mesh)
		{
			content = mesh->GetAvailableMeshFiles();
		}
		return content;
	}

	OgreMeshComponent::OgreMeshComponent() : m_OgreEntity(NULL),
		m_CastShadow(true),
		m_ReadyToLoadMesh(false),
		m_UniqueMaterialCreated(false),
		m_GeomFlags(GEOMETRY_FLAG_UNKOWN),
		m_RenderQueue(RENDER_QUEUE_MAIN)
	{

	}

	OgreMeshComponent::~OgreMeshComponent()
	{

	}

	void OgreMeshComponent::RegisterReflection()
	{
		GASS::ComponentFactory::GetPtr()->Register("MeshComponent",new GASS::Creator<OgreMeshComponent, IComponent>);
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("MeshComponent", OF_VISIBLE)));

		RegisterProperty<ResourceHandle>("Filename", &GASS::OgreMeshComponent::GetMeshResource, &GASS::OgreMeshComponent::SetMeshResource,
			OgreMeshEnumerationMetaDataPtr(new OgreMeshEnumerationMetaData("Mesh File",PF_VISIBLE)));
		RegisterProperty<std::string>("EnumerationResourceGroup", &OgreMeshComponent::GetEnumerationResourceGroup, &OgreMeshComponent::SetEnumerationResourceGroup,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("EnumerationResourceGroup",PF_VISIBLE)));
		RegisterProperty<bool>("CastShadow", &GASS::OgreMeshComponent::GetCastShadow, &GASS::OgreMeshComponent::SetCastShadow,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Should this mesh cast shadows or not",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<RenderQueueBinder>("RenderQueue", &GASS::OgreMeshComponent::GetRenderQueue, &GASS::OgreMeshComponent::SetRenderQueue,
			EnumerationProxyPropertyMetaDataPtr(new EnumerationProxyPropertyMetaData("Render Queue",PF_VISIBLE,&RenderQueueBinder::GetStringEnumeration)));
		RegisterProperty<GeometryFlagsBinder>("GeometryFlags", &GetGeometryFlagsBinder, &SetGeometryFlagsBinder,
			EnumerationProxyPropertyMetaDataPtr(new EnumerationProxyPropertyMetaData("Geometry Flags",PF_VISIBLE,&GeometryFlagsBinder::GetStringEnumeration, true)));
	}

	void OgreMeshComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreMeshComponent::OnLocationLoaded,LocationLoadedMessage,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreMeshComponent::OnMeshFileNameMessage,MeshFileMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreMeshComponent::OnTexCoordMessage,TextureCoordinateMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreMeshComponent::OnMaterialMessage,MaterialMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreMeshComponent::OnVisibilityMessage,MeshVisibilityMessage ,0));
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

	void OgreMeshComponent::SetMeshResource(const ResourceHandle &res) 
	{
		m_MeshResource = res;

		if(m_MeshResource.Name() != "" &&  m_ReadyToLoadMesh)
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

			GetSceneObject()->PostMessage(MessagePtr(new GeometryChangedMessage(DYNAMIC_PTR_CAST<IGeometryComponent>(shared_from_this()))));
		}
	}


	void OgreMeshComponent::OnVisibilityMessage(MeshVisibilityMessagePtr message)
	{
		if(m_OgreEntity)
		{
			for(unsigned int i = 0 ; i < m_OgreEntity->getNumSubEntities(); i++)
			{
				Ogre::SubEntity* se = m_OgreEntity->getSubEntity(i);
				se->setVisible(message->GetValue());
			}
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
				//Vec3 bone_pos = OgreConvert::ToGASS(bone_cand->getWorldPosition());
				Vec3 bone_pos = OgreConvert::ToGASS(bone_cand->getPosition());
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
			return OgreConvert::ToGASS(m_OgreEntity->getBoundingBox());
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


	GraphicsMesh OgreMeshComponent::GetMeshData() const
	{
		GraphicsMesh mesh_data;
		if(m_OgreEntity == NULL)
			return mesh_data;
		Ogre::MeshPtr mesh = m_OgreEntity->getMesh();
		CopyMeshToMeshData(mesh, mesh_data);
		return mesh_data;
	}

	void OgreMeshComponent::CopyMeshToMeshData(Ogre::MeshPtr mesh, GraphicsMesh &mesh_data)
	{
		for(unsigned int i = 0;i < mesh->getNumSubMeshes();++i)
		{
			GraphicsSubMeshPtr sub_mesh_data(new GraphicsSubMesh());
			mesh_data.SubMeshVector.push_back(sub_mesh_data);
			SubMesh *sub_mesh = mesh->getSubMesh(i);
			sub_mesh_data->Type = OgreConvert::ToGASS(sub_mesh->operationType);
			sub_mesh_data->MaterialName = sub_mesh->getMaterialName();
			
			Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingletonPtr()->getByName(sub_mesh->getMaterialName());
			if (!mat.isNull()) 
			{
				Ogre::Technique* tech = mat->getBestTechnique();
				//Get last pass and save materials
				if(tech && tech->getNumPasses() > 0)
				{
					Ogre::Pass*  pass = tech->getPass(tech->getNumPasses()-1);

					Ogre::ColourValue ambient =  pass->getAmbient();
					const Ogre::ColourValue diffuse = pass->getDiffuse();
					const Ogre::ColourValue specular = pass->getSpecular();
					const Ogre::ColourValue selfIllumination = pass->getSelfIllumination();
					const Ogre::ColourValue emissive = pass->getEmissive();
					GraphicsMaterial gfx_mat(ColorRGBA(diffuse.r,diffuse.g,diffuse.b,diffuse.a),
						ColorRGB(ambient.r,ambient.g,ambient.b),
						ColorRGB(specular.r,specular.g,specular.b));

					sub_mesh_data->Material = gfx_mat;

					for(unsigned int j = 0 ; j < pass->getNumTextureUnitStates(); j++)
					{
						Ogre::TextureUnitState * textureUnit = pass->getTextureUnitState(j);
						std::string texture_name = textureUnit->getTextureName();
						if(texture_name != "")
							sub_mesh_data->Material.Textures.push_back(texture_name);
					}
				}
			}

			if (!sub_mesh->useSharedVertices)
			{
				AddIndexData(sub_mesh->indexData,0,sub_mesh_data);
				AddVertexData(sub_mesh->vertexData,sub_mesh_data);
			}
			else
			{
				AddIndexData(sub_mesh->indexData,0,sub_mesh_data);
				AddVertexData(mesh->sharedVertexData , sub_mesh_data);
			}
		}
	}

	void OgreMeshComponent::AddVertexData(const Ogre::VertexData *vertex_data,GraphicsSubMeshPtr mesh) 
	{
		if (!vertex_data)
			return;

		// Get the positional buffer element
		const Ogre::VertexElement* posElem = vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);
		if(posElem)
		{
			/*Vec3* tmp_pos_vec = new Vec3[mesh->NumVertex];
			if (mesh->VertexVector)
			{
				memcpy(tmp_pos_vec,mesh->VertexVector,sizeof(Vec3) * prev_size);
				delete[] mesh->VertexVector;
			}
			mesh->VertexVector = tmp_pos_vec;*/


			Ogre::HardwareVertexBufferSharedPtr pbuf = vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());
			const unsigned int vSize = (unsigned int)pbuf->getVertexSize();

			unsigned char* pos_ptr = static_cast<unsigned char*>(pbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
			float* pReal;
			//Vec3 * curPositions = &mesh->VertexVector[prev_size];
			const unsigned int posCount = (unsigned int)vertex_data->vertexCount;
			Vec3 pos;
			for(unsigned int j = 0; j < posCount; ++j)
			{
				posElem->baseVertexPointerToElement(pos_ptr, &pReal);
				pos_ptr += vSize;
				
				pos.x = (*pReal++);
				pos.y = (*pReal++);
				pos.z= (*pReal++);

				mesh->PositionVector.push_back(pos);
				//*curVertices = _transform * (*curVertices);
				//curPositions++;
			}
			pbuf->unlock();
		}

		const Ogre::VertexElement* normElem = vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_NORMAL);
		if(normElem)
		{
			Ogre::HardwareVertexBufferSharedPtr nbuf = vertex_data->vertexBufferBinding->getBuffer(normElem->getSource());
			const unsigned int vSize = (unsigned int)nbuf->getVertexSize();
			unsigned char* normal_ptr = static_cast<unsigned char*>(nbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
			float* pReal;
			const unsigned int normalCount = (unsigned int)vertex_data->vertexCount;
			Vec3 normal;
			for(unsigned int j = 0; j < normalCount; ++j)
			{
				normElem->baseVertexPointerToElement(normal_ptr, &pReal);
				normal_ptr += vSize;

				normal.x = (*pReal++);
				normal.y = (*pReal++);
				normal.z = (*pReal++);
				mesh->NormalVector.push_back(normal);
				//*curVertices = _transform * (*curVertices);
				//curNormals++;
			}
			nbuf->unlock();
		}
		const Ogre::VertexElement* textureElem = vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_TEXTURE_COORDINATES);

		if(textureElem)
		{
			//TODO: support more channels 
			Ogre::HardwareVertexBufferSharedPtr tbuf = vertex_data->vertexBufferBinding->getBuffer(textureElem->getSource());
			const unsigned int tSize = (unsigned int)tbuf->getVertexSize();
			unsigned char* texture_ptr = static_cast<unsigned char*>(tbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
			float* pReal;
			Vec4 tex_coord(0,0,0,0);
			std::vector<Vec4>* tex_coord_vec;
			if(mesh->TexCoordsVector.size() > 0)
				tex_coord_vec = &mesh->TexCoordsVector[0];
			else
			{
				std::vector<Vec4> empty_tex_coord_vec;
				mesh->TexCoordsVector.push_back(empty_tex_coord_vec);
				tex_coord_vec = &mesh->TexCoordsVector[0];
			}
			
			const unsigned int textureCount = (unsigned int)vertex_data->vertexCount;
			for(unsigned int j = 0; j < textureCount; ++j)
			{
				textureElem->baseVertexPointerToElement(texture_ptr, &pReal);
				texture_ptr += tSize;

				tex_coord.x = (*pReal++);
				tex_coord.y = (*pReal++);
				//curTexture->z = (*pReal++);
				//curTexture->w = (*pReal++);
				tex_coord_vec->push_back(tex_coord);

				//*curVertices = _transform * (*curVertices);
				//curTexture++;
			}
			
			tbuf->unlock();
		}
	}


	void OgreMeshComponent::AddIndexData(const Ogre::IndexData *index_data, const unsigned int offset,GraphicsSubMeshPtr mesh) 
	{
		//const unsigned int prev_size = mesh->NumFaces;
		//mesh->NumFaces += (unsigned int)data->indexCount;

		/*unsigned int* tmp_ind = new unsigned int[mesh->NumFaces];
		if (mesh->FaceVector)
		{
			memcpy (tmp_ind, mesh->FaceVector, sizeof(unsigned int) * prev_size);
			delete[] mesh->FaceVector;
		}
		mesh->FaceVector = tmp_ind;*/

		if(index_data->indexCount > 0)
		{

			HardwareIndexBufferSharedPtr ibuf = index_data->indexBuffer;

			const bool use32bitindexes = (ibuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);
			//unsigned int index_offset = mesh->FaceVector.size();

			if (use32bitindexes)
			{
				const unsigned int* pInt = static_cast<unsigned int*>(ibuf->lock(HardwareBuffer::HBL_READ_ONLY));
				for(unsigned int k = 0; k < index_data->indexCount; ++k)
				{
					unsigned int index = pInt[k];
					mesh->IndexVector.push_back(index + offset);
				}
				ibuf->unlock();
			}
			else
			{
				const unsigned short* pShort = static_cast<unsigned short*>(ibuf->lock(HardwareBuffer::HBL_READ_ONLY));
				for(unsigned int k = 0; k < index_data->indexCount; ++k)
				{
					unsigned int index = static_cast<unsigned int> (pShort[k]);
					mesh->IndexVector.push_back(index + offset);
				}
				ibuf->unlock();
			}
		}
	}

	void OgreMeshComponent::RebuildMaterialCache()
	{
		if(m_MeshMaterialCache.find(m_MeshResource.Name()) == m_MeshMaterialCache.end())
		{
			std::vector<std::string> sub_mesh_mats;
			for(unsigned int i = 0 ; i < m_OgreEntity->getNumSubEntities(); i++)
			{
				Ogre::SubEntity* se = m_OgreEntity->getSubEntity(i);
				Ogre::MaterialPtr mat = se->getMaterial();
				sub_mesh_mats.push_back(mat->getName());
			}
			m_MeshMaterialCache[m_MeshResource.Name()] = sub_mesh_mats;
		}
	}

	void OgreMeshComponent::RestoreMaterialsFromCache()
	{
		if(m_MeshMaterialCache.find(m_MeshResource.Name()) != m_MeshMaterialCache.end())
		{
			std::vector<std::string> sub_mesh_mats = m_MeshMaterialCache[m_MeshResource.Name()];
			if(sub_mesh_mats .size() == m_OgreEntity->getNumSubEntities())
			{
				for(unsigned int i = 0 ; i < m_OgreEntity->getNumSubEntities(); i++)
				{
					Ogre::SubEntity* se = m_OgreEntity->getSubEntity(i);
					se->setMaterialName(sub_mesh_mats [i]);
				}
			}
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
		ResourceHandle resource(message->GetFileName());
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

	void OgreMeshComponent::SetGeometryFlagsBinder(GeometryFlagsBinder value)
	{
		SetGeometryFlags(value.GetValue());
	}

	GeometryFlagsBinder OgreMeshComponent::GetGeometryFlagsBinder() const
	{
		return GeometryFlagsBinder(GetGeometryFlags());
	}


	std::vector<std::string> OgreMeshComponent::GetAvailableMeshFiles() const
	{
		std::vector<std::string> content;
		ResourceManagerPtr rm = GASS::SimEngine::Get().GetResourceManager();
		ResourceGroupVector groups = rm->GetResourceGroups();
		std::vector<std::string> values;
		for(size_t i = 0; i < groups.size();i++)
		{
			ResourceGroupPtr group = groups[i];
			if(m_EnumerationResourceGroup == "" || group->GetName() == m_EnumerationResourceGroup)
			{
				ResourceVector res_vec;
				group->GetResourcesByType(res_vec,"MESH");
				for(size_t j = 0; j < res_vec.size();j++)
				{
					content.push_back(res_vec[j]->Name());
				}
			}
		}
		return content;
	}
}
