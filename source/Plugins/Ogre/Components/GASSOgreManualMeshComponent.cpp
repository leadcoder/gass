/****************************************************************************
* This file is part of GASS.                                                *
* See https://github.com/leadcoder/gass                                     *
*                                                                           *
* Copyright (c) 2008-2016 GASS team. See Contributors.txt for details.      *
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

#include "GASSOgreManualMeshComponent.h"
#include "GASSOgreMeshComponent.h"
#include <OgreSceneNode.h>
#include <OgreMeshManager.h>
#include <OgreSceneManager.h>
#include <OgreManualObject.h>
#include <GASSOgreCommon.h>
#include <OgreEdgeListBuilder.h>
#include "Core/Utils/GASSFileUtils.h"
#include "Core/Math/GASSVector.h"
#include "Core/Math/GASSMath.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/ComponentSystem/GASSComponent.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSimEngine.h"
#include "Plugins/Ogre/GASSOgreGraphicsSceneManager.h"
#include "Plugins/Ogre/GASSOgreConvert.h"
#include "Plugins/Ogre/Components/GASSOgreLocationComponent.h"
#include "Plugins/Ogre/GASSOgreMaterialCache.h"

namespace GASS
{
	OgreManualMeshComponent::OgreManualMeshComponent(): m_MeshObject (NULL),
		m_GeometryFlagsBinder(GEOMETRY_FLAG_DYNAMIC_OBJECT),
		m_CastShadows(false)
	{

	}

	OgreManualMeshComponent::~OgreManualMeshComponent()
	{

	}

	void OgreManualMeshComponent::RegisterReflection()
	{
		ComponentFactory::Get().Register<OgreManualMeshComponent>("ManualMeshComponent");
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("ManualMeshComponent", OF_VISIBLE)));
		ADD_DEPENDENCY("OgreLocationComponent")
			RegisterGetSet("CastShadow", &GASS::OgreManualMeshComponent::GetCastShadow, &GASS::OgreManualMeshComponent::SetCastShadow,PF_VISIBLE | PF_EDITABLE,"Should this mesh cast shadows or not");

		RegisterMember("GeometryFlags", &OgreManualMeshComponent::m_GeometryFlagsBinder, PF_VISIBLE, "Geometry Flags",
			EnumerationProxyPropertyMetaDataPtr(new EnumerationProxyPropertyMetaData("Geometry Flags",PF_VISIBLE,&GeometryFlagsBinder::GetStringEnumeration, true)));
	}

	void OgreManualMeshComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreManualMeshComponent::OnLocationLoaded,LocationLoadedEvent,1));
		
		OgreGraphicsSceneManagerPtr ogsm = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<OgreGraphicsSceneManager>();
		assert(ogsm);
		Ogre::SceneManager* sm = ogsm->GetOgreSceneManager();

		static unsigned int obj_id = 0;
		obj_id++;
		std::stringstream ss;
		std::string name;
		ss << GetName() << obj_id;
		ss >> name;

		m_MeshObject = sm->createManualObject(name);
		m_MeshObject->setDynamic(true);
		m_MeshObject->setCastShadows(m_CastShadows);
	}

	void OgreManualMeshComponent::OnLocationLoaded(LocationLoadedEventPtr message)
	{
		OgreLocationComponent * lc = GetSceneObject()->GetFirstComponentByClass<OgreLocationComponent>().get();
		lc->GetOgreNode()->attachObject(m_MeshObject);

		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreManualMeshComponent::OnDataMessage,ManualMeshDataRequest,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreManualMeshComponent::OnClearMessage,ClearManualMeshRequest,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreManualMeshComponent::OnReplaceMaterial,ReplaceMaterialRequest,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreManualMeshComponent::OnResetMaterial,ResetMaterialRequest,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreManualMeshComponent::OnVisibilityMessage,GeometryVisibilityRequest ,0));

		m_Collision = GetSceneObject()->GetFirstComponentByClass<ICollisionComponent>();

		//Hack to get transfer mesh geom 
		if(m_MeshObject->getNumSections() > 0)
			GetSceneObject()->PostEvent(GeometryChangedEventPtr(new GeometryChangedEvent(GASS_DYNAMIC_PTR_CAST<IGeometryComponent>(shared_from_this()),-1,2)));

	}

	void OgreManualMeshComponent::SetCastShadow(bool castShadow) 
	{
		m_CastShadows = castShadow;
		if(m_MeshObject)
			m_MeshObject->setCastShadows(m_CastShadows);
	}

	void OgreManualMeshComponent::OnDataMessage(ManualMeshDataRequestPtr message)
	{
		GraphicsMeshPtr data = message->GetData();
		SetMeshData(*data);
	}

	void OgreManualMeshComponent::OnClearMessage(ClearManualMeshRequestPtr message)
	{
		Clear();
	}

	void OgreManualMeshComponent::Clear()
	{
		if(m_MeshObject)
			m_MeshObject->clear();
	}

	void OgreManualMeshComponent::SetMeshData(const GraphicsMesh &data)
	{
		if(m_MeshObject)
		{
			m_MeshObject->clear();
			for(size_t i = 0; i < data.SubMeshVector.size() ; i++)
			{
				GraphicsSubMeshPtr sub_mesh = data.SubMeshVector[i];
				size_t num_pos = sub_mesh->PositionVector.size();
				if(num_pos == 0)
					continue;

				Ogre::RenderOperation::OperationType op = OgreConvert::ToOgre(sub_mesh->Type);
				//use material name or create new material?
				m_MeshObject->begin(sub_mesh->MaterialName, op);

				bool has_normals = false;
				bool has_tangents = false;
				bool has_colors = false;
				bool has_tex_coords = false;

				if(sub_mesh->NormalVector.size() ==  num_pos)
					has_normals = true;
				if(sub_mesh->TangentVector.size() ==  num_pos)
					has_tangents = true;
				if(sub_mesh->ColorVector.size() ==  num_pos)
					has_colors = true;
				if(sub_mesh->TexCoordsVector.size() > 0 &&  sub_mesh->TexCoordsVector[0].size() ==  num_pos)
					has_tex_coords = true;

				for(size_t j = 0; j < sub_mesh->PositionVector.size() ; j++)
				{
					m_MeshObject->position(OgreConvert::ToOgre(sub_mesh->PositionVector[j]));
					if(has_normals)
						m_MeshObject->normal(OgreConvert::ToOgre(sub_mesh->NormalVector[j]));
					if(has_tangents)
						m_MeshObject->tangent(OgreConvert::ToOgre(sub_mesh->TangentVector[j]));
					if(has_colors)
						m_MeshObject->colour(OgreConvert::ToOgre(sub_mesh->ColorVector[j]));
					if(has_tex_coords)
					{
						Vec4 tex_coord = sub_mesh->TexCoordsVector[0].at(j);
						m_MeshObject->textureCoord(static_cast<Ogre::Real>(tex_coord.x), static_cast<Ogre::Real>(tex_coord.y));
					}
				}
				for(size_t j = 0; j < sub_mesh->IndexVector.size();j++)
				{
					m_MeshObject->index(sub_mesh->IndexVector[j]);
				}
				m_MeshObject->end();
			}
			Ogre::EdgeData::EdgeGroupList::iterator itShadow, itEndShadow;
			if(m_MeshObject->getEdgeList())
				for( itShadow=m_MeshObject->getEdgeList()->edgeGroups.begin(), itEndShadow=m_MeshObject->getEdgeList()->edgeGroups.end(); itShadow!=itEndShadow; itShadow++ )
					const_cast<Ogre::VertexData*>((*itShadow).vertexData)->prepareForShadowVolume();
			OgreMaterialCache::Add(m_MeshObject);
			GetSceneObject()->PostEvent(GeometryChangedEventPtr(new GeometryChangedEvent(GASS_DYNAMIC_PTR_CAST<IGeometryComponent>(shared_from_this()))));
		}
	}

	void OgreManualMeshComponent::OnResetMaterial(ResetMaterialRequestPtr message)
	{
		OgreMaterialCache::Restore(m_MeshObject);
	}

	void OgreManualMeshComponent::OnReplaceMaterial(ReplaceMaterialRequestPtr message)
	{
		SetSubMeshMaterial(message->GetMaterialName(), message->GetSubMeshID());
	}

	void OgreManualMeshComponent::SetSubMeshMaterial(const std::string &material_name, int sub_mesh_index)
	{
		if(m_MeshObject == NULL)
			return;
		if(m_MeshObject->getNumSections() <= 0)
			return;

		if(sub_mesh_index >= 0)
		{
			if(static_cast<int>(m_MeshObject->getNumSections()) > sub_mesh_index)
				m_MeshObject->getSection(sub_mesh_index)->setMaterialName(material_name);
		}
		else
		{
			for(unsigned int i= 0;  i< m_MeshObject->getNumSections(); i++)
			{
				m_MeshObject->getSection(i)->setMaterialName(material_name);
			}
		}
	}

	AABox OgreManualMeshComponent::GetBoundingBox() const
	{
		AABox box;
		//return box;
		assert(m_MeshObject);
		box = OgreConvert::ToGASS(m_MeshObject->getBoundingBox());

		if(m_MeshObject->getParentSceneNode())
		{
			Vec3 scale = OgreConvert::ToGASS(m_MeshObject->getParentSceneNode()->getScale());
			box.Max = box.Max*scale;
			box.Min = box.Min*scale;
		}
		return box;
	}

	Sphere OgreManualMeshComponent::GetBoundingSphere() const
	{
		Sphere sphere;
		assert(m_MeshObject);
		sphere.m_Pos = Vec3(0,0,0);
		sphere.m_Radius = m_MeshObject->getBoundingRadius();
		if(m_MeshObject->getParentSceneNode())
		{
			Vec3 scale = OgreConvert::ToGASS(m_MeshObject->getParentSceneNode()->getScale());
			sphere.m_Radius = sphere.m_Radius * static_cast<float>(std::max(std::max(scale.x,scale.y),scale.z));
		}
		return sphere;
	}

	GeometryFlags OgreManualMeshComponent::GetGeometryFlags() const
	{
		return m_GeometryFlagsBinder.GetValue();
	}

	void OgreManualMeshComponent::SetGeometryFlags(GeometryFlags flags)
	{
		m_GeometryFlagsBinder.SetValue(flags);
		if(GetSceneObject()) 
			GetSceneObject()->PostEvent(GeometryFlagsChangedEventPtr(new GeometryFlagsChangedEvent(flags)));
	}

	void OgreManualMeshComponent::OnVisibilityMessage(GeometryVisibilityRequestPtr message)
	{
		if(m_MeshObject == NULL)
			return;
		m_MeshObject->setVisible(message->GetValue());
	}

	

	GraphicsMesh OgreManualMeshComponent::GetMeshData() const
	{
		GraphicsMesh mesh_data;

		if(m_MeshObject == NULL)
			return mesh_data;
		if(m_MeshObject->getNumSections() == 0)
			return mesh_data;
		Ogre::MeshPtr mesh = m_MeshObject->convertToMesh("ConvertedTempMesh");
		OgreMeshComponent::CopyMeshToMeshData(mesh, mesh_data);
		//remove mesh!
		Ogre::MeshManager::getSingleton().remove("ConvertedTempMesh");
		return mesh_data;
	}

	bool OgreManualMeshComponent::GetCollision() const
	{
		if(m_Collision)
			return m_Collision->GetActive();
		return false;
	}

	void OgreManualMeshComponent::SetCollision(bool value)
	{
		if(m_Collision)
			m_Collision->SetActive(value);
	}

}