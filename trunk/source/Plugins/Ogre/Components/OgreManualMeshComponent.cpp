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

namespace GASS
{
	OgreManualMeshComponent::OgreManualMeshComponent(): m_MeshObject (NULL),
		m_UniqueMaterialCreated(false),
		m_GeomFlags(GEOMETRY_FLAG_UNKOWN),
		m_CastShadows(false)
	{

	}

	OgreManualMeshComponent::~OgreManualMeshComponent()
	{

	}

	void OgreManualMeshComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("ManualMeshComponent",new Creator<OgreManualMeshComponent, IComponent>);

		RegisterProperty<bool>("CastShadow", &GASS::OgreManualMeshComponent::GetCastShadow, &GASS::OgreManualMeshComponent::SetCastShadow,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Should this mesh cast shadows or not",PF_VISIBLE | PF_EDITABLE)));
	}

	void OgreManualMeshComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreManualMeshComponent::OnLocationLoaded,LocationLoadedMessage,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreManualMeshComponent::OnDataMessage,ManualMeshDataMessage,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreManualMeshComponent::OnClearMessage,ClearManualMeshMessage,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreManualMeshComponent::OnMaterialMessage,MaterialMessage,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreManualMeshComponent::OnTextureMessage,TextureMessage,1));


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
	}

	void OgreManualMeshComponent::SetCastShadow(bool castShadow) 
	{
		m_CastShadows = castShadow;
		if(m_MeshObject)
			m_MeshObject->setCastShadows(m_CastShadows);
	}


	void OgreManualMeshComponent::OnDataMessage(ManualMeshDataMessagePtr message)
	{
		MeshDataPtr data = message->GetData();
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


	void OgreManualMeshComponent::CreateMesh(MeshDataPtr data)
	{
		if(m_MeshObject)
		{
			m_MeshObject->clear();
			for(size_t i = 0; i < data->SubMeshVector.size() ; i++)
			{
				SubMeshDataPtr sub_mesh =   data->SubMeshVector[i];
				Ogre::RenderOperation::OperationType op = OgreConvert::ToOgre(sub_mesh->Type);
				//use material name or create new material?
				m_MeshObject->begin(sub_mesh->MaterialName, op);

				size_t num_pos = sub_mesh->PositionVector.size();

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
		}
		GetSceneObject()->PostMessage(MessagePtr(new GeometryChangedMessage(DYNAMIC_PTR_CAST<IGeometryComponent>(shared_from_this()))));
	}

	/*void OgreManualMeshComponent::CreateMesh(ManualMeshDataPtr data)
	{
		if(m_MeshObject)
		{
			m_MeshObject->clear();

			if(data->ScreenSpace)
			{
				// Use identity view/projection matrices
				m_MeshObject->setUseIdentityProjection(true);
				m_MeshObject->setUseIdentityView(true);

				// Use infinite AAB to always stay visible
				Ogre::AxisAlignedBox aabInf;
				aabInf.setInfinite();
				m_MeshObject->setBoundingBox(aabInf);

				// Render just before overlays
				m_MeshObject->setRenderQueueGroup(Ogre::RENDER_QUEUE_OVERLAY - 1);
				//Set up flags not to trigger ray cast detection
				//m_MeshObject->setQueryFlags();
				//setQueryFlagsNoHit(m_MeshObject);
			}

			Ogre::RenderOperation::OperationType op = Ogre::RenderOperation::OT_LINE_LIST;

			op = OgreConvert::ToOgre(data->Type);
			{
			case LINE_LIST:
				op = Ogre::RenderOperation::OT_LINE_LIST;
				break;
			case POINT_LIST:
				op = Ogre::RenderOperation::OT_POINT_LIST;
				break;
			case LINE_STRIP:
				op = Ogre::RenderOperation::OT_LINE_STRIP;
				break;
			case TRIANGLE_FAN:
				op = Ogre::RenderOperation::OT_TRIANGLE_FAN;
				break;
			case TRIANGLE_LIST:
				op = Ogre::RenderOperation::OT_TRIANGLE_LIST;
				break;
			case TRIANGLE_STRIP:
				op = Ogre::RenderOperation::OT_TRIANGLE_STRIP;
				break;
			}
			m_CurrentOP = op;
			m_MeshObject->begin(data->Material, op);
			for(int i = 0; i < data->VertexVector.size(); i++)
			{
				Vec3 pos = data->VertexVector[i].Pos;
				Vec3 normal  = data->VertexVector[i].Normal;
				Vec3 tangent  = data->VertexVector[i].Tangent;
				Vec2 tex_coord  = data->VertexVector[i].TexCoord;
				Vec4 color  = data->VertexVector[i].Color;


				m_MeshObject->position(pos.x, pos.y, pos.z);
				m_MeshObject->normal(normal.x,normal.y,normal.z);
				m_MeshObject->tangent(tangent.x,tangent.y,tangent.z);
				m_MeshObject->textureCoord(tex_coord.x,tex_coord.y);
				Ogre::ColourValue col;
				col.r = color.x;
				col.g = color.y;
				col.b = color.z;
				col.a = color.w;
				m_MeshObject->colour(col);
			}

			for(int i = 0; i < data->IndexVector.size();i++)
			{
				m_MeshObject->index(data->IndexVector[i]);
			}
			m_MeshObject->end();
			GetSceneObject()->PostMessage(MessagePtr(new GeometryChangedMessage(DYNAMIC_PTR_CAST<IGeometryComponent>(shared_from_this()))));
		}
	}
	*/

	void OgreManualMeshComponent::OnTextureMessage(TextureMessagePtr message)
	{
		if(message->GetTexture() == "")
			return;

		if(!m_UniqueMaterialCreated) 
		{
			Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingletonPtr()->getByName(m_MeshObject->getSection(0)->getMaterialName());
			std::string mat_name = m_MeshObject->getName() + mat->getName();
			mat = mat->clone(mat_name);
			m_MeshObject->getSection(0)->setMaterialName(mat_name);
			m_UniqueMaterialCreated = true;
		}
		Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingletonPtr()->getByName(m_MeshObject->getSection(0)->getMaterialName());

		if(mat->getNumTechniques() > 0)
		{
			Ogre::Technique * technique = mat->getTechnique(0);
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
		}
	}


	void OgreManualMeshComponent::OnMaterialMessage(MaterialMessagePtr message)
	{

		if(m_MeshObject->getNumSections() <= 0)
			return;
		if(!m_UniqueMaterialCreated) 
		{
			Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingletonPtr()->getByName(m_MeshObject->getSection(0)->getMaterialName());
			if(mat.isNull()) 
				return;
			std::string mat_name = m_MeshObject->getName() + mat->getName();
			mat = mat->clone(mat_name);
			m_MeshObject->getSection(0)->setMaterialName(mat_name);
			m_UniqueMaterialCreated = true;
		}

		Vec4 diffuse = message->GetDiffuse();
		Vec3 ambient = message->GetAmbient();
		Vec3 specular = message->GetSpecular();
		Vec3 si = message->GetSelfIllumination();
		Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingletonPtr()->getByName(m_MeshObject->getSection(0)->getMaterialName());
		if(mat.isNull()) 
			return;

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

		/*if(diffuse.w < 1.0)
		{
		mat->setDepthCheckEnabled(false);
		mat->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
		}
		else
		{
		//mat->setDepthCheckEnabled(true);
		mat->setSceneBlending(Ogre::SBT_REPLACE);
		}*/
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


	MeshData OgreManualMeshComponent::GetMeshData() const
	{
		MeshData mesh_data;

		if(m_MeshObject == NULL)
			return mesh_data;
		Ogre::MeshPtr mesh = m_MeshObject->convertToMesh("ConvertedTempMesh");
		OgreMeshComponent::CopyMeshToMeshData(mesh, mesh_data);
		//remove mesh!
		Ogre::MeshManager::getSingleton().remove("ConvertedTempMesh");


		return mesh_data;
	}
}
