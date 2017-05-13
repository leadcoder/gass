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

#include "HeightmapComponent.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/Utils/GASSLogManager.h"
#include "Core/Utils/GASSHeightField.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/Interface/GASSIGeometryComponent.h"
#include "Sim/Interface/GASSIMeshComponent.h"
#include "Sim/GASSGraphicsMesh.h"
#include "Sim/Interface/GASSILocationComponent.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Utils/GASSCollisionHelper.h"

namespace GASS
{
	HeightmapComponent::HeightmapComponent(void) : m_HM(NULL),
		m_Size(200,200),
		m_Resolution(1.0),
		m_AutoBBoxGeneration(false)
	{

	}

	HeightmapComponent::~HeightmapComponent(void)
	{
		delete m_HM;
	}

	void HeightmapComponent::RegisterReflection()
	{
		GASS::ComponentFactory::GetPtr()->Register("HeightmapComponent",new GASS::Creator<HeightmapComponent, Component>);
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("HeightmapComponent", OF_VISIBLE)));

		RegisterProperty<Vec2>("Size", &GASS::HeightmapComponent::GetSize, &GASS::HeightmapComponent::SetSize,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Size of heighttmap in [m]",PF_VISIBLE | PF_EDITABLE)));

		RegisterProperty<Float>("Resolution", &GASS::HeightmapComponent::GetResolution, &GASS::HeightmapComponent::SetResolution,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("[samples/m]",PF_VISIBLE | PF_EDITABLE)));

		RegisterProperty<bool>("Update", &GASS::HeightmapComponent::GetUpdate, &GASS::HeightmapComponent::SetUpdate,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Update height values from scene geometry",PF_VISIBLE | PF_EDITABLE)));

		RegisterProperty<bool>("AutoBBoxGeneration", &GASS::HeightmapComponent::GetAutoBBoxGeneration, &GASS::HeightmapComponent::SetAutoBBoxGeneration,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Get bounding box from terrain",PF_VISIBLE | PF_EDITABLE)));
	}

	void HeightmapComponent::OnInitialize()
	{
		//Try to load from file!	
		FilePath full_path = _GetFilePath();
		if(full_path.Exist()) //Check if file exist
		{
			m_HM = new HeightField();
			m_HM->Load(full_path.GetFullPath());
			GetSceneObject()->PostEvent(GeometryChangedEventPtr(new GeometryChangedEvent(GASS_DYNAMIC_PTR_CAST<IGeometryComponent>(shared_from_this()))));
		}
	}

	FilePath HeightmapComponent::_GetFilePath() const
	{
		ScenePtr  scene = GetSceneObject()->GetScene();
		std::string scene_path = scene->GetSceneFolder().GetFullPath();
		std::string filename = "heightmap_" + GetName() + ".hm";
		FilePath full_path(scene_path + "/" + filename);
		return full_path;
	}

	void HeightmapComponent::SaveXML(tinyxml2::XMLElement *obj_elem)
	{
		BaseSceneComponent::SaveXML(obj_elem);
		if(m_HM)
		{
			m_HM->Save(_GetFilePath().GetFullPath());
		}
	}

	void HeightmapComponent::SetUpdate(bool)
	{
		_UpdateData();
	}

	bool  HeightmapComponent::GetUpdate() const
	{
		return false;
	}

	AABox HeightmapComponent::_GetTerrainBoundingBox() const
	{
		if(!GetSceneObject())
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Failed generate bounding box, no scene object found","HeightmapComponent::_GetTerrainBoundingBox");

		AABox box;
		if(m_AutoBBoxGeneration)
		{
			std::vector<SceneObjectPtr> objs;
			ComponentContainer::ComponentVector components;
			GetSceneObject()->GetScene()->GetRootSceneObject()->GetComponentsByClass<IMeshComponent>(components, true);
			for(size_t i = 0; i < components.size() ; i++)
			{
				BaseSceneComponentPtr comp = GASS_DYNAMIC_PTR_CAST<BaseSceneComponent>(components[i]);
				objs.push_back(comp->GetSceneObject());
			}
			if(objs.size() > 0)
			{
				for(size_t i = 0;  i <  objs.size(); i++)
				{
					SceneObjectPtr obj = objs[i];
					MeshComponentPtr mesh = obj->GetFirstComponentByClass<IMeshComponent>();
					GeometryComponentPtr geom = obj->GetFirstComponentByClass<IGeometryComponent>();
					if(geom && geom->GetGeometryFlags() & (GEOMETRY_FLAG_GROUND | GEOMETRY_FLAG_STATIC_OBJECT))
					{
						AABox mesh_box = geom->GetBoundingBox();
						LocationComponentPtr lc = obj->GetFirstComponentByClass<ILocationComponent>();
						if(lc)
						{
							Vec3 world_pos = lc->GetWorldPosition();
							Mat4 trans_mat = Mat4::CreateTransformationSRT(lc->GetScale(),lc->GetWorldRotation(), world_pos);
							mesh_box.Transform(trans_mat);
						}

						box.Union(mesh_box);
					}
				}
			}
			else //no terrain data in scene
			{
				GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Failed generate bounding box, no terrain found (GEOMETRY_FLAG_GROUND)","HeightmapComponent::_GetTerrainBoundingBox");
			}
		}
		else
		{
			Vec3 pos = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->GetWorldPosition();
			Float half_width = m_Size.x*0.5;
			Float half_height = m_Size.y*0.5;
			box.m_Min.Set(pos.x - half_width, -500, pos.z - half_height);
			box.m_Max.Set(pos.x + half_width, 500, pos.z + half_height);
		}
		return box;
	}

	void HeightmapComponent::_UpdateData()
	{
		if(!GetSceneObject())
			return;
		AABox bbox = _GetTerrainBoundingBox();

		Vec3 bbsize = bbox.GetSize();
		const unsigned int px_width = static_cast<unsigned int>(bbsize.x * m_Resolution);
		const unsigned int pz_height = static_cast<unsigned int>(bbsize.z * m_Resolution);
		Float inv_sample_ratio = 1.0/m_Resolution;
		//GEOMETRY_FLAG_GROUND
		GeometryFlags flags =  static_cast<GeometryFlags>(GEOMETRY_FLAG_SCENE_OBJECTS | GEOMETRY_FLAG_PAGED_LOD);
		ScenePtr scene = GetSceneObject()->GetScene();

		//delete previous hm
		delete m_HM;
		//add some padding to support runtime height change
		bbox.m_Min.y -= 100;
		bbox.m_Max.y += 100;
		m_HM = new HeightField(bbox.m_Min, bbox.m_Max, px_width, pz_height);

		LogManager::Get().stream() << "START building heightmap\n";
		for(unsigned int i = 0; i <  px_width; i++)
		{
			LogManager::Get().stream() << "row:" << i << " of:" << px_width-1 << "\n";
			for(unsigned int j = 0; j <  pz_height; j++)
			{
				Vec3 pos(bbox.m_Min.x + i*inv_sample_ratio, 0, bbox.m_Min.z + j*inv_sample_ratio); 
				float h = static_cast<float>(CollisionHelper::GetHeightAtPosition(scene, pos, flags, true));
				m_HM->SetHeightAtSample(i, j, h);
			}	
		}
		m_HM->Save(_GetFilePath().GetFullPath());
		GetSceneObject()->PostEvent(GeometryChangedEventPtr(new GeometryChangedEvent(GASS_DYNAMIC_PTR_CAST<IGeometryComponent>(shared_from_this()))));
	}

	Float HeightmapComponent::GetHeightAtSample(int x, int z) const
	{
		if(m_HM)
			return m_HM->GetHeightAtSample(x,z);
		return 0;
	}
	Float HeightmapComponent::GetHeightAtWorldLocation(Float x, Float z) const
	{
		if(m_HM)
			return m_HM->GetInterpolatedHeight(x,z);
		return 0;
	}

	unsigned int HeightmapComponent::GetNumSamplesW() const
	{
		if(m_HM)
			return m_HM->GetNumSamplesW();
		return 0;
	}

	unsigned int HeightmapComponent::GetNumSamplesH() const
	{
		if(m_HM)
			return m_HM->GetNumSamplesH();
		return 0;
	}

	AABox HeightmapComponent::GetBoundingBox() const
	{
		AABox bounds;
		if(m_HM)
		{
			bounds = m_HM->GetBoundingBox();
		}
		return bounds;
	}

	Sphere HeightmapComponent::GetBoundingSphere() const
	{
		return GetBoundingBox().GetBoundingSphere();
	}

	GeometryFlags HeightmapComponent::GetGeometryFlags() const
	{
		return GEOMETRY_FLAG_GROUND;
	}

	bool HeightmapComponent::GetAutoBBoxGeneration()const
	{
		return m_AutoBBoxGeneration;
	}

	void HeightmapComponent::SetAutoBBoxGeneration(bool value)
	{
		m_AutoBBoxGeneration = value;
	}

	bool HeightmapComponent::GetCollision() const
	{
		return true;
	}

	void HeightmapComponent::SetCollision(bool /*value*/)
	{

	}
}