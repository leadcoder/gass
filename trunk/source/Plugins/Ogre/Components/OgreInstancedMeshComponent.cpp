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
#include "Plugins/Ogre/Components/OgreInstancedMeshComponent.h"
#include "Plugins/Ogre/Components/OgreLocationComponent.h"
#include "Plugins/Ogre/OgreConvert.h"

#include <tinyxml.h>
using namespace Ogre;

namespace GASS
{

	OgreInstancedMeshComponent::OgreInstancedMeshComponent() : m_CastShadow(true),
		m_ReadyToLoadMesh(false),
		m_UniqueMaterialCreated(false),
		m_RegionSize(300.0),
		m_GlobalScale(1,1,1)
	{

	}

	OgreInstancedMeshComponent::~OgreInstancedMeshComponent()
	{


	}

	void OgreInstancedMeshComponent::RegisterReflection()
	{
		GASS::ComponentFactory::GetPtr()->Register("InstancedMeshComponent",new GASS::Creator<OgreInstancedMeshComponent, IComponent>);
		RegisterProperty<std::string>("RenderQueue", &GASS::OgreInstancedMeshComponent::GetRenderQueue, &GASS::OgreInstancedMeshComponent::SetRenderQueue);
		RegisterProperty<bool>("CastShadow", &GASS::OgreInstancedMeshComponent::GetCastShadow, &GASS::OgreInstancedMeshComponent::SetCastShadow);
		RegisterProperty<Float>("RegionSize", &GASS::OgreInstancedMeshComponent::GetRegionSize, &GASS::OgreInstancedMeshComponent::SetRegionSize);
		RegisterProperty<Vec3>("GlobalScale", &GASS::OgreInstancedMeshComponent::GetGlobalScale, &GASS::OgreInstancedMeshComponent::SetGlobalScale);
	}

	void OgreInstancedMeshComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreInstancedMeshComponent::OnLoad,LoadGFXComponentsMessage,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreInstancedMeshComponent::OnUnload,UnloadComponentsMessage,0));
	}

	void OgreInstancedMeshComponent::LoadXML(TiXmlElement *elem)
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
					std::stringstream ss(pos_str);
					ss >> instance.m_Position;
				}
				TiXmlElement *rot_elem = prop_elem->FirstChildElement("Quaternion");
				if(rot_elem)
				{
					const std::string rot_str = rot_elem->Attribute("value");
					std::stringstream ss(rot_str);
					ss >> instance.m_Rotation;

				}

				TiXmlElement *scale_elem = prop_elem->FirstChildElement("Scale");
				if(scale_elem)
				{
					const std::string scale_str = scale_elem->Attribute("value");
					std::stringstream ss(scale_str);
					ss >> instance.m_Scale;

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

	void OgreInstancedMeshComponent::OnLoad(LoadGFXComponentsMessagePtr message)
	{
		OgreGraphicsSceneManagerPtr ogsm = boost::shared_static_cast<OgreGraphicsSceneManager>(message->GetGFXSceneManager());
		m_OgreSceneManager = ogsm;
		assert(ogsm);



		static unsigned int obj_id = 0;
		obj_id++;
		std::stringstream ss;
		std::string name;
		ss << GetName() << obj_id;
		ss >> name;

		//Use this name to generate unique filename, used by geometry interface. 
		m_Filename = name;

		/*m_InstancedGeometry = new InstancedGeometry(mSceneMgr,m_Filename);

		m_InstancedGeometry->setBatchInstanceDimensions (Ogre::Vector3(m_RegionSize, m_RegionSize, m_RegionSize));
		m_InstancedGeometry->setOrigin(Ogre::Vector3(-m_RegionSize/2, 0, -m_RegionSize/2));


		if(m_RenderQueue == "SkiesLate")
		{
		m_InstancedGeometry->setRenderQueueGroup(Ogre::RENDER_QUEUE_SKIES_LATE);
		}
		else if(m_RenderQueue == "SkiesEarly")
		{
		m_InstancedGeometry->setRenderQueueGroup(Ogre::RENDER_QUEUE_SKIES_EARLY);
		}*/
		const int  maxObjectsPerBatch = 80;
		MeshMap::iterator iter = m_MeshInstances.begin();
		while(iter != m_MeshInstances.end())
		{
			Ogre::InstancedGeometry* instancedGeometry = new InstancedGeometry(ogsm->GetSceneManger(),iter->first + "Inst" );
			instancedGeometry->setBatchInstanceDimensions (Ogre::Vector3(m_RegionSize, m_RegionSize, m_RegionSize));

			if(m_RenderQueue == "SkiesLate")
			{
				instancedGeometry->setRenderQueueGroup(Ogre::RENDER_QUEUE_SKIES_LATE);
			}
			else if(m_RenderQueue == "SkiesEarly")
			{
				instancedGeometry->setRenderQueueGroup(Ogre::RENDER_QUEUE_SKIES_EARLY);
			}

			//m_InstancedGeometry->setOrigin(Ogre::Vector3(-m_RegionSize/2, 0, -m_RegionSize/2));
			//instancedGeometry->setBatchInstanceDimensions (Vector3(300, 300, 300));
			Ogre::Entity* entity = ogsm->GetSceneManger()->createEntity(iter->first,iter->first);
			setupInstancedMaterialToEntity(entity);

			//calulate number of entities in batch
			const size_t batchSize = (iter->second.size() > maxObjectsPerBatch) ? maxObjectsPerBatch : iter->second.size();
			//for(size_t i = 0; i < iter->second.size(); i++)
			for(size_t i = 0; i < batchSize; i++)
			{
				instancedGeometry->addEntity(entity, Vector3::ZERO);
			}

			instancedGeometry->setOrigin(Vector3::ZERO);
			instancedGeometry->build();

			const int num_batches = (iter->second.size() / maxObjectsPerBatch); 

			for (size_t k = 0; k < num_batches; k++)
			{
				instancedGeometry->addBatchInstance();
			}

			InstancedGeometry::BatchInstanceIterator regIt = instancedGeometry->getBatchInstanceIterator();
			unsigned int index = 0;
			while (regIt.hasMoreElements ())
			{
				InstancedGeometry::BatchInstance *r = regIt.getNext();
				int pos_x = 0;
				InstancedGeometry::BatchInstance::InstancedObjectIterator bit = r->getObjectIterator();
				while(bit.hasMoreElements())
				{
					InstancedGeometry::InstancedObject* obj = bit.getNext();

					if(index < iter->second.size()-1)
					{
						Ogre::Vector3  pos = Convert::ToOgre(iter->second.at(index).m_Position);
						//Ogre::Quaternion orientation = Convert::ToOgre(iter->second.at(index).m_Rotation);
						Ogre::Vector3  scale = Convert::ToOgre(iter->second.at(index).m_Scale);
						scale.x *= m_GlobalScale.x;
						scale.y *= m_GlobalScale.y;//0.07;
						scale.z *= m_GlobalScale.z;//0.2;
						obj->setPosition(pos);
						obj->setScale(scale);
						//obj->setOrientation(orientation);
					}
					//obj->setPosition(Ogre::Vector3(pos_x*10,0,pos_z*10));


					index++;
				}

			}
			instancedGeometry->setVisible(true);
			ogsm->GetSceneManger()->destroyEntity (entity);
			m_InstancedGeometries.push_back(instancedGeometry);
			iter++;
			/*for(size_t i = 0;  i < iter->second.size(); i++)
			{
			Ogre::Vector3  pos = Convert::ToOgre(iter->second.at(i).m_Position);
			Ogre::Quaternion orientation = Convert::ToOgre(iter->second.at(i).m_Rotation);
			Ogre::Vector3  scale = Convert::ToOgre(iter->second.at(i).m_Scale);
			//m_StaticGeometry->addEntity(entity, pos, Ogre::Quaternion::IDENTITY,scale);
			}
			iter++;*/
		}
		//m_StaticGeometry->build();
		GetSceneObject()->PostMessage(MessagePtr(new GeometryChangedMessage(boost::shared_dynamic_cast<IGeometryComponent>(shared_from_this()))));
	}

	void OgreInstancedMeshComponent::setupInstancedMaterialToEntity(Entity*ent)
	{
		for (Ogre::uint i = 0; i < ent->getNumSubEntities(); ++i)
		{
			SubEntity* se = ent->getSubEntity(i);
			String materialName= se->getMaterialName();
			se->setMaterialName(buildInstancedMaterial(materialName));
		}
	}
	String OgreInstancedMeshComponent::buildInstancedMaterial(const String &originalMaterialName)
	{

		// already instanced ?
		if (StringUtil::endsWith (originalMaterialName, "/instanced"))
			return originalMaterialName;

		MaterialPtr originalMaterial = MaterialManager::getSingleton ().getByName (originalMaterialName);

		// if originalMat doesn't exists use "Instancing" material name
		const String instancedMaterialName (originalMaterial.isNull() ? "Instancing" : originalMaterialName + "/Instanced");
		MaterialPtr  instancedMaterial = MaterialManager::getSingleton ().getByName (instancedMaterialName);

		// already exists ?
		if (instancedMaterial.isNull())
		{
			instancedMaterial = originalMaterial->clone(instancedMaterialName);
			instancedMaterial->load();
			Technique::PassIterator pIt = instancedMaterial->getBestTechnique ()->getPassIterator();
			while (pIt.hasMoreElements())
			{

				Pass * const p = pIt.getNext();
				p->setVertexProgram("Instancing", false);
				//p->setShadowCasterVertexProgram("InstancingShadowCaster");


			}
		}
		instancedMaterial->load();
		return instancedMaterialName;


	}


	void OgreInstancedMeshComponent::OnUnload(UnloadComponentsMessagePtr message)
	{

		OgreGraphicsSceneManagerPtr ogsm(m_OgreSceneManager );
		if(ogsm)
		{
			for(size_t i = 0; i < m_InstancedGeometries.size(); i++)
			{
				ogsm->GetSceneManger()->destroyInstancedGeometry(m_InstancedGeometries[i]);
			}
		}
	}


	AABox OgreInstancedMeshComponent::GetBoundingBox() const
	{
		//assert(m_InstancedGeometry);
		AABox final_box;

		/*	StaticGeometry::RegionIterator regIt = m_StaticGeometry->getRegionIterator();
		while (regIt.hasMoreElements())
		{
		StaticGeometry::Region* region = regIt.getNext();
		Vec3 center = Convert::ToGASS(region->getCentre());
		AABox box = Convert::ToGASS(region->getBoundingBox());
		final_box.Union(box);
		}*/
		return final_box;
	}


	Sphere OgreInstancedMeshComponent::GetBoundingSphere() const
	{
		Sphere final_sphere;


		bool first_loop = true;

		/*StaticGeometry::RegionIterator regIt = m_StaticGeometry->getRegionIterator();
		while (regIt.hasMoreElements())
		{
		StaticGeometry::Region* region = regIt.getNext();
		Sphere sphere;


		sphere.m_Radius = region->getBoundingRadius();
		sphere.m_Pos = Convert::ToGASS(region->getCentre());
		if(first_loop)
		{
		final_sphere = sphere;
		first_loop = false;
		}
		else
		final_sphere.Union(sphere);
		}*/
		return final_sphere;
	}

	void OgreInstancedMeshComponent::GetMeshData(MeshDataPtr mesh_data)
	{
		mesh_data->NumVertex = 0;
		mesh_data->VertexVector = NULL;
		mesh_data->NumFaces = 0;
		mesh_data->FaceVector = NULL;

		/*	StaticGeometry::RegionIterator regIt = m_StaticGeometry->getRegionIterator();
		while (regIt.hasMoreElements())
		{
		StaticGeometry::Region* region = regIt.getNext();
		Ogre::Vector3 center = region->getCentre();

		StaticGeometry::Region::LODIterator lodIt = region->getLODIterator();

		float sqdist = 1e24;
		Ogre::StaticGeometry::LODBucket *theBucket = 0;
		while (lodIt.hasMoreElements()) 
		{

		Ogre::StaticGeometry::LODBucket *b = lodIt.getNext();
		if (!theBucket || b->getLodValue() < sqdist) {
		sqdist = b->getLodValue();
		theBucket = b;
		}
		}

		//while (lodIt.hasMoreElements())
		{

		//StaticGeometry::LODBucket* bucket = lodIt.getNext();

		StaticGeometry::LODBucket::MaterialIterator matIt = theBucket->getMaterialIterator();
		while (matIt.hasMoreElements())
		{
		StaticGeometry::MaterialBucket* mat = matIt.getNext();
		StaticGeometry::MaterialBucket::GeometryIterator geomIt = mat->getGeometryIterator();
		while (geomIt.hasMoreElements())
		{
		StaticGeometry::GeometryBucket* geom = geomIt.getNext();

		AddIndexData(geom->getIndexData(),mesh_data->NumVertex,mesh_data);
		AddVertexData(geom->getVertexData(),mesh_data, center);

		}
		}
		}
		}	
		mesh_data->NumFaces = mesh_data->NumFaces/3.0;*/
	}


	void OgreInstancedMeshComponent::AddVertexData(const Ogre::VertexData *vertex_data,MeshDataPtr mesh, const Ogre::Vector3 &offset)
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

				curVertices->x = (*pReal++) + offset.x;
				curVertices->y = (*pReal++) + offset.y;
				curVertices->z = (*pReal++) + offset.z;

				//*curVertices = _transform * (*curVertices);

				curVertices++;
			}
			vbuf->unlock();
		}
	}

	void OgreInstancedMeshComponent::AddIndexData(const Ogre::IndexData *data, const unsigned int offset,MeshDataPtr mesh)
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


	GeometryCategory OgreInstancedMeshComponent::GetGeometryCategory() const
	{
		return GeometryCategory(GT_REGULAR);
	}

}
