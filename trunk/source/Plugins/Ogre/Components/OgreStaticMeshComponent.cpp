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
#include "Core/Math/GASSQuaternion.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Plugins/Ogre/Components/OgreStaticMeshComponent.h"
#include "Plugins/Ogre/Components/OgreMeshComponent.h"

#include "Plugins/Ogre/Components/OgreLocationComponent.h"
#include "Plugins/Ogre/OgreConvert.h"

#include <tinyxml.h>
using namespace Ogre;

namespace GASS
{

	OgreStaticMeshComponent::OgreStaticMeshComponent() : m_StaticGeometry(NULL),
		m_CastShadow(true),
		m_ReadyToLoadMesh(false),
		m_UniqueMaterialCreated(false),
		m_RegionSize(300.0),
		m_GeomFlags(GEOMETRY_FLAG_UNKNOWN)
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
		RegisterProperty<Float>("RegionSize", &GASS::OgreStaticMeshComponent::GetRegionSize, &GASS::OgreStaticMeshComponent::SetRegionSize);
	}

	void OgreStaticMeshComponent::OnInitialize()
	{

		OgreGraphicsSceneManagerPtr ogsm =  GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<OgreGraphicsSceneManager>();
		m_OgreSceneManager = ogsm;
		assert(ogsm);


		static unsigned int obj_id = 0;
		obj_id++;
		std::stringstream ss;
		std::string name;
		ss << GetName() << obj_id;
		ss >> name;

		//Use this name to generate unique filename, used by geometry interface. 
		//m_Filename = name;

		m_StaticGeometry  = ogsm->GetOgreSceneManager()->createStaticGeometry(name);
		m_StaticGeometry->setRegionDimensions(Ogre::Vector3(m_RegionSize, m_RegionSize, m_RegionSize));
		m_StaticGeometry->setOrigin(Ogre::Vector3(-m_RegionSize/2, 0, -m_RegionSize/2));

		if(m_RenderQueue == "SkiesLate")
		{
			m_StaticGeometry->setRenderQueueGroup(Ogre::RENDER_QUEUE_SKIES_LATE);
		}
		else if(m_RenderQueue == "SkiesEarly")
		{
			m_StaticGeometry->setRenderQueueGroup(Ogre::RENDER_QUEUE_SKIES_EARLY);
		}
		MeshMap::iterator iter = m_MeshInstances.begin();
		while(iter != m_MeshInstances.end())
		{
			Ogre::Entity* entity = ogsm->GetOgreSceneManager()->createEntity(iter->first,iter->first);
			for(size_t i = 0;  i < iter->second.size(); i++)
			{
				Ogre::Vector3  pos = OgreConvert::ToOgre(iter->second.at(i).m_Position);
				Ogre::Quaternion orientation = OgreConvert::ToOgre(iter->second.at(i).m_Rotation);
				Ogre::Vector3  scale = OgreConvert::ToOgre(iter->second.at(i).m_Scale);
				m_StaticGeometry->addEntity(entity, pos, Ogre::Quaternion::IDENTITY,scale);
			}
			++iter;
		}
		m_StaticGeometry->build();
		GetSceneObject()->PostEvent(GeometryChangedEventPtr(new GeometryChangedEvent(DYNAMIC_PTR_CAST<IGeometryComponent>(shared_from_this()))));
	}


	void OgreStaticMeshComponent::OnDelete()
	{

		if(m_StaticGeometry)
		{
			OgreGraphicsSceneManagerPtr ogsm(m_OgreSceneManager );
			if(ogsm)
				ogsm->GetOgreSceneManager()->destroyStaticGeometry(m_StaticGeometry);
		}
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



	AABox OgreStaticMeshComponent::GetBoundingBox() const
	{
		assert(m_StaticGeometry);
		AABox final_box;

		StaticGeometry::RegionIterator regIt = m_StaticGeometry->getRegionIterator();
		while (regIt.hasMoreElements())
		{
			StaticGeometry::Region* region = regIt.getNext();
			Vec3 center = OgreConvert::ToGASS(region->getCentre());
			AABox box = OgreConvert::ToGASS(region->getBoundingBox());
			final_box.Union(box);
		}
		return final_box;
	}


	Sphere OgreStaticMeshComponent::GetBoundingSphere() const
	{
		Sphere final_sphere;


		bool first_loop = true;

		StaticGeometry::RegionIterator regIt = m_StaticGeometry->getRegionIterator();
		while (regIt.hasMoreElements())
		{
			StaticGeometry::Region* region = regIt.getNext();
			Sphere sphere;


			sphere.m_Radius = region->getBoundingRadius();
			sphere.m_Pos = OgreConvert::ToGASS(region->getCentre());
			if(first_loop)
			{
				final_sphere = sphere;
				first_loop = false;
			}
			else
				final_sphere.Union(sphere);
		}
		return final_sphere;
	}

	GraphicsMesh  OgreStaticMeshComponent::GetMeshData() const
	{
		GraphicsMesh mesh_data;

		StaticGeometry::RegionIterator regIt = m_StaticGeometry->getRegionIterator();
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

					GraphicsSubMeshPtr sub_mesh_data(new GraphicsSubMesh());
					mesh_data.SubMeshVector.push_back(sub_mesh_data);

					while (geomIt.hasMoreElements())
					{
						StaticGeometry::GeometryBucket* geom = geomIt.getNext();

						OgreMeshComponent::AddIndexData(geom->getIndexData(), static_cast<unsigned int>(sub_mesh_data->PositionVector.size()), sub_mesh_data);
						OgreMeshComponent::AddVertexData(geom->getVertexData(), sub_mesh_data);

						//AddIndexData(geom->getIndexData(),mesh_data->NumVertex,mesh_data);
						//AddVertexData(geom->getVertexData(),mesh_data, center);

					}
				}
			}
		}
		return mesh_data;
	}

	/*
	void OgreStaticMeshComponent::AddVertexData(const Ogre::VertexData *vertex_data,MeshDataPtr mesh, const Ogre::Vector3 &offset) const
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

	void OgreStaticMeshComponent::AddIndexData(const Ogre::IndexData *data, const unsigned int offset,MeshDataPtr mesh) const
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
	*/
	GeometryFlags OgreStaticMeshComponent::GetGeometryFlags() const
	{
		return m_GeomFlags;
	}

	void OgreStaticMeshComponent::SetGeometryFlags(GeometryFlags flags)
	{
		m_GeomFlags = flags;
	}
}
