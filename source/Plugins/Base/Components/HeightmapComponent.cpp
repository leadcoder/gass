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
#include "Core/Utils/GASSHeightField.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/Interface/GASSIGeometryComponent.h"
#include "Sim/Interface/GASSIMeshComponent.h"
#include "Sim/GASSGraphicsMesh.h"
#include "Sim/Interface/GASSILocationComponent.h"
#include "Sim/Interface/GASSITerrainSceneManager.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Utils/GASSCollisionHelper.h"

namespace GASS
{
	HeightmapComponent::HeightmapComponent(void) : m_HM(nullptr),
		m_Extent({0, 0, 0}, { 200, 200, 200 }),
		m_SampleStep(1.0),
		m_AutoBBoxGeneration(false),
		m_Debug(false)
	{

	}

	HeightmapComponent::~HeightmapComponent(void)
	{
		delete m_HM;
	}

	void HeightmapComponent::RegisterReflection()
	{
		ComponentFactory::Get().Register<HeightmapComponent>();
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("HeightmapComponent", OF_VISIBLE)));

		RegisterGetSet("Extent", &GASS::HeightmapComponent::GetExtent, &GASS::HeightmapComponent::SetExtent, PF_VISIBLE | PF_EDITABLE,"Relative heightfield extent: <min-x min-y min-z> <max-x max-y max-z> [m]");
		RegisterMember("SampleStep", &GASS::HeightmapComponent::m_SampleStep, PF_VISIBLE | PF_EDITABLE,"[m]");
		RegisterGetSet("Update", &GASS::HeightmapComponent::GetUpdate, &GASS::HeightmapComponent::SetUpdate,PF_VISIBLE | PF_EDITABLE | PF_RUNTIME,"Update height values from scene geometry");
		RegisterGetSet("UpdateExtentFromGeometry", &GASS::HeightmapComponent::GetUpdateExtentFromGeometry, &GASS::HeightmapComponent::SetUpdateExtentFromGeometry,PF_VISIBLE | PF_EDITABLE | PF_RUNTIME,"Get bounding box from terrain");
		RegisterGetSet("Debug", &GASS::HeightmapComponent::GetDebug, &GASS::HeightmapComponent::SetDebug, PF_VISIBLE | PF_EDITABLE | PF_RUNTIME, "Show/hide bounds");
	}

	void HeightmapComponent::OnInitialize()
	{
		//Try to load from file!	
		FilePath full_path = _GetFilePath();
		if(full_path.Exist()) //Check if file exist
		{
			m_HM = new HeightField();
			m_HM->Load(full_path.GetFullPath());
			//m_SampleStep = m_HM->GetBoundingBox().GetSize().x / static_cast<double>(m_HM->GetNumSamplesH());
			SetExtent(m_HM->GetBoundingBox());
			GetSceneObject()->PostEvent(GeometryChangedEventPtr(new GeometryChangedEvent(GASS_DYNAMIC_PTR_CAST<IGeometryComponent>(shared_from_this()))));
			
		}
	}

	FilePath HeightmapComponent::_GetFilePath() const
	{
		ScenePtr scene = GetSceneObject()->GetScene();
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

	AABoxd HeightmapComponent::GetExtent() const
	{
		return m_Extent;
	}

	void HeightmapComponent::SetExtent(const AABoxd& extent)
	{
		_UpdateDebugObject(extent);
		m_Extent = extent;
	}

	void HeightmapComponent::_UpdateData()
	{
		if(!GetSceneObject())
			return;

		const AABoxd bbox = GetExtent();
		//_UpdateDebugObject(bbox);

		const Vec3 bbsize = bbox.GetSize();
		
		const double inv_sample_step = 1.0/m_SampleStep;
		const unsigned int px_width = static_cast<unsigned int>(bbsize.x * inv_sample_step);
		const unsigned int pz_height = static_cast<unsigned int>(bbsize.z * inv_sample_step);
		
		//GEOMETRY_FLAG_GROUND
		GeometryFlags flags =  static_cast<GeometryFlags>(GEOMETRY_FLAG_SCENE_OBJECTS | GEOMETRY_FLAG_PAGED_LOD);
		ScenePtr scene = GetSceneObject()->GetScene();

		TerrainSceneManagerPtr terrain_sm = scene->GetFirstSceneManagerByClass<ITerrainSceneManager>(true);
		
		//delete previous hm
		delete m_HM;

		if (terrain_sm)
		{
			m_HM = new HeightField(bbox.Min, bbox.Max, px_width, pz_height);

			GASS_LOG(LINFO) << "START building heightmap\n";
		
		
			GASS_LOG(LINFO) << "OE Terrain\n";
			const Vec3 world_pos = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->GetWorldPosition();
			Quaternion world_rot = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->GetWorldRotation();
			Mat4 transform(world_rot, world_pos);
		
			for (unsigned int i = 0; i < px_width; i++)
			{
				GASS_LOG(LINFO) << "row:" << i << " of:" << px_width - 1 << "\n";
				for (unsigned int j = 0; j < pz_height; j++)
				{
					const Vec3d local_sample_pos(bbox.Min.x + i * m_SampleStep, 0, bbox.Min.z + j * m_SampleStep);
					const Vec3d world_sample_pos = transform * local_sample_pos;
					double height;
					terrain_sm->GetTerrainHeight(world_sample_pos, height, flags);
					//float h = static_cast<float>(CollisionHelper::GetHeightAtPosition(scene, pos, flags, true));
					m_HM->SetHeightAtSample(i, j, static_cast<float>(height));
				}
			}
		}

		else
		{
			//add some padding to support runtime height change
			//bbox.Min.y -= 100;
			//bbox.Max.y += 100;

			const Vec3 world_pos = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->GetWorldPosition();
			Quaternion world_rot = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->GetWorldRotation();
			Mat4 transform(world_rot, world_pos);
			const float hae = world_pos.y;
			//const float offset = static_cast<float>(CollisionHelper::GetHeightAtPosition(scene, world_pos, flags, true));
			m_HM = new HeightField(bbox.Min, bbox.Max, px_width, pz_height);

			for (unsigned int i = 0; i < px_width; i++)
			{
				GASS_LOG(LINFO) << "row:" << i << " of:" << px_width - 1 << "\n";
				for (unsigned int j = 0; j < pz_height; j++)
				{
					const Vec3d local_pos(bbox.Min.x + i * m_SampleStep, 0.0, bbox.Min.z + j * m_SampleStep);
					const Vec3d world_sample_pos = transform * local_pos;
					float h = static_cast<float>(CollisionHelper::GetHeightAtPosition(scene, world_sample_pos, flags, true));
					h = h - hae;
					m_HM->SetHeightAtSample(i, j, h);
				}
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

	bool HeightmapComponent::GetUpdateExtentFromGeometry()const
	{
		return false;
	}

	void HeightmapComponent::SetUpdateExtentFromGeometry(bool value)
	{
		if (GetSceneObject())
		{
			MeshComponentPtr mesh = GetSceneObject()->GetFirstParentComponentByClass<IMeshComponent>();
			if (mesh)
			{
				GeometryComponentPtr geom = GASS_DYNAMIC_PTR_CAST<IGeometryComponent>(mesh);
				if (geom)// && geom->GetGeometryFlags()  & (GEOMETRY_FLAG_GROUND | GEOMETRY_FLAG_STATIC_OBJECT))
				{
					const AABox mesh_box = geom->GetBoundingBox();
					SetExtent(mesh_box);
				}
				else //no geometry
				{
					GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Failed updating extent, no IGeometryComponent found", "HeightmapComponent::SetUpdateExtentFromGeometry");
				}
			}
			else //no mesh found
			{
				GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Failed updating extent, no mesh found", "HeightmapComponent::SetUpdateExtentFromGeometry");
			}
		}
	}

	bool HeightmapComponent::GetCollision() const
	{
		return true;
	}

	void HeightmapComponent::SetCollision(bool /*value*/)
	{

	}

	void HeightmapComponent::SetDebug(bool value)
	{
		if (m_Debug && !value)
		{
			SceneObjectPtr obj = _GetOrCreateDebugObject();
			obj->GetFirstComponentByClass<ILocationComponent>()->SetVisible(false);
		}

		m_Debug = value;
	
		_UpdateDebugObject(m_Extent);
	}

	bool HeightmapComponent::GetDebug() const
	{
		return m_Debug;
	}

	void HeightmapComponent::_UpdateDebugObject(const AABoxd& extent)
	{
		if (!m_Debug)
			return;
		if (!GetSceneObject())
			return;
		SceneObjectPtr obj = _GetOrCreateDebugObject();
		obj->GetFirstComponentByClass<ILocationComponent>()->SetVisible(m_Debug);
		const Vec3d center = extent.Min + (extent.Max - extent.Min) * 0.5;
		obj->GetFirstComponentByClass<ILocationComponent>()->SetPosition(center);
		BaseSceneComponent *box_comp = obj->GetComponentByClassName("BoxGeometryComponent");
		if (box_comp)
		{
			box_comp->SetPropertyValue("Size", extent.GetSize());
		}
	}

	SceneObjectPtr HeightmapComponent::_GetOrCreateDebugObject()
	{
		const std::string obj_id = "DEBUG_OBJECT";
		SceneObjectPtr scene_object = GetSceneObject()->GetChildByID(obj_id);
		if (!scene_object)
		{
			scene_object = std::make_shared<SceneObject>();
			scene_object->SetSerialize(false);
			scene_object->SetID(obj_id);
			scene_object->SetName("DebugObject");
			ComponentPtr location_comp = ComponentFactory::Get().Create("LocationComponent");
			location_comp->SetPropertyValue("AttachToParent", true);
			ComponentPtr mesh_comp = ComponentFactory::Get().Create("ManualMeshComponent");
			mesh_comp->SetPropertyValue("CastShadow", false);
			ComponentPtr box_comp = ComponentFactory::Get().Create("BoxGeometryComponent");
			scene_object->AddComponent(location_comp);
			scene_object->AddComponent(mesh_comp);
			scene_object->AddComponent(box_comp);
			GetSceneObject()->AddChildSceneObject(scene_object, true);
		}
		return scene_object;
	}
}