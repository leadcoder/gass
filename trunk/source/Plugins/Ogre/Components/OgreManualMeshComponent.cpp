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

#include "OgreManualMeshComponent.h"
#include "OgreMeshComponent.h"

#include <OgreSceneNode.h>
#include <OgreMeshManager.h>

#include <OgreEntity.h>
#include <OgreSceneManager.h>
#include <OgreTextureUnitState.h>
#include <OgreMaterialManager.h>
#include <OgreManualObject.h>
#include <OgreCommon.h>
#include <OgreEdgeListBuilder.h>


#include "Core/Utils/GASSLogManager.h"
#include "Core/Utils/GASSFileUtils.h"
#include "Core/Math/GASSVector.h"
#include "Core/Math/GASSQuaternion.h"
#include "Core/Utils/GASSFileUtils.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/ComponentSystem/GASSIComponent.h"

#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSResourceManager.h"
#include "Sim/GASSSimSystemManager.h"


#include "Plugins/Ogre/OgreGraphicsSceneManager.h"
#include "Plugins/Ogre/OgreConvert.h"
#include "Plugins/Ogre/Components/OgreLocationComponent.h"
#include "Plugins/Ogre/OgreMaterialCache.h"

namespace GASS
{
	OgreManualMeshComponent::OgreManualMeshComponent(): m_MeshObject (NULL),
		//m_GeomFlags(GEOMETRY_FLAG_UNKOWN),
		m_GeomFlags(GEOMETRY_FLAG_DYNAMIC_OBJECT),
		
		m_CastShadows(false)
	{

	}

	OgreManualMeshComponent::~OgreManualMeshComponent()
	{

	}

	void OgreManualMeshComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("ManualMeshComponent",new Creator<OgreManualMeshComponent, IComponent>);
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("ManualMeshComponent", OF_VISIBLE)));
		ADD_DEPENDENCY("OgreLocationComponent")
		RegisterProperty<bool>("CastShadow", &GASS::OgreManualMeshComponent::GetCastShadow, &GASS::OgreManualMeshComponent::SetCastShadow,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Should this mesh cast shadows or not",PF_VISIBLE | PF_EDITABLE)));
	}

	void OgreManualMeshComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreManualMeshComponent::OnLocationLoaded,LocationLoadedMessage,1));
		
	}

	void OgreManualMeshComponent::OnLocationLoaded(LocationLoadedMessagePtr message)
	{
		OgreGraphicsSceneManagerPtr ogsm =  GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<OgreGraphicsSceneManager>();
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
		
		OgreLocationComponent * lc = GetSceneObject()->GetFirstComponentByClass<OgreLocationComponent>().get();
		lc->GetOgreNode()->attachObject(m_MeshObject);

		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreManualMeshComponent::OnDataMessage,ManualMeshDataMessage,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreManualMeshComponent::OnClearMessage,ClearManualMeshMessage,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreManualMeshComponent::OnReplaceMaterial,ReplaceMaterialMessage,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreManualMeshComponent::OnTextureMessage,TextureMessage,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreManualMeshComponent::OnResetMaterial,ResetMaterialMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreManualMeshComponent::OnVisibilityMessage,GeometryVisibilityMessage ,0));
	}

	void OgreManualMeshComponent::SetCastShadow(bool castShadow) 
	{
		m_CastShadows = castShadow;
		if(m_MeshObject)
			m_MeshObject->setCastShadows(m_CastShadows);
	}


	void OgreManualMeshComponent::OnDataMessage(ManualMeshDataMessagePtr message)
	{
		GraphicsMeshPtr data = message->GetData();
		CreateMesh(data);
	}

	void OgreManualMeshComponent::OnClearMessage(ClearManualMeshMessagePtr message)
	{
		Clear();
	}

	void OgreManualMeshComponent::Clear()
	{
		if(m_MeshObject)
			m_MeshObject->clear();
	}

	void OgreManualMeshComponent::CreateMesh(GraphicsMeshPtr data)
	{
		if(m_MeshObject)
		{
			m_MeshObject->clear();
			for(size_t i = 0; i < data->SubMeshVector.size() ; i++)
			{
				GraphicsSubMeshPtr sub_mesh =   data->SubMeshVector[i];
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
						m_MeshObject->textureCoord(tex_coord.x,tex_coord.y);
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
			GetSceneObject()->PostMessage(MessagePtr(new GeometryChangedMessage(DYNAMIC_PTR_CAST<IGeometryComponent>(shared_from_this()))));
		}
		
	}

	void OgreManualMeshComponent::OnResetMaterial(ResetMaterialMessagePtr message)
	{
		OgreMaterialCache::Restore(m_MeshObject);
	}

	void OgreManualMeshComponent::OnTextureMessage(TextureMessagePtr message)
	{
		/*if(message->GetTexture() == "")
			return;
		if(m_UserMaterials[0]->getNumTechniques() > 0)
		{
			Ogre::Technique * technique = m_UserMaterials[0]->getTechnique(0);
			if(technique->getNumPasses() > 0)
			{
				Ogre::Pass* pass = technique->getPass(0);
				if(pass->getNumTextureUnitStates() == 0)
				{
					pass->createTextureUnitState();
				}

				if(pass->getNumTextureUnitStates() > 0)
				{
					Ogre::TextureUnitState * textureUnit = pass->getTextureUnitState(0);
					std::string texture_name = message->GetTexture();
					//Check in resource manager

					if(texture_name != "")
					{
						const std::string stripped_name = FileUtils::GetFilename(texture_name);
						textureUnit->setTextureName(stripped_name);
					}
				}
			}
		}*/
	}

	void OgreManualMeshComponent::OnReplaceMaterial(ReplaceMaterialMessagePtr message)
	{
		if(m_MeshObject == NULL)
			return;
		if(m_MeshObject->getNumSections() <= 0)
			return;
	
		if(message->GetSubMeshID() >= 0)
		{
			m_MeshObject->getSection(message->GetSubMeshID())->setMaterialName(message->GetMaterialName());
		}
		else
		{
			for(size_t i= 0;  i< m_MeshObject->getNumSections(); i++)
			{
				m_MeshObject->getSection(i)->setMaterialName(message->GetMaterialName());
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
			box.m_Max = box.m_Max*scale;
			box.m_Min = box.m_Min*scale;
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
			sphere.m_Radius = sphere.m_Radius*Math::Max(scale.x,scale.y,scale.z);
		}
		return sphere;
	}

	GeometryFlags OgreManualMeshComponent::GetGeometryFlags() const
	{
		return m_GeomFlags;
	}

	void OgreManualMeshComponent::SetGeometryFlags(GeometryFlags flags)
	{
		m_GeomFlags = flags;
	}

	void OgreManualMeshComponent::OnVisibilityMessage(GeometryVisibilityMessagePtr message)
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
}
