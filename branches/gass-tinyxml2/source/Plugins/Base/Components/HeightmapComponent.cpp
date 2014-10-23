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

#include "HeightmapComponent.h"
#include "Plugins/Base/CoreMessages.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Utils/GASSLogManager.h"
#include "Core/Utils/GASSHeightmap.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/Interface/GASSIGeometryComponent.h"
#include "Sim/GASSGraphicsMesh.h"
#include "Sim/Interface/GASSILocationComponent.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Utils/GASSCollisionHelper.h"

namespace GASS
{
	HeightmapComponent::HeightmapComponent(void) : m_HM(NULL),
		m_Size(200,200),
		m_Resolution(1.0)
	{

	}

	HeightmapComponent::~HeightmapComponent(void)
	{

	}

	void HeightmapComponent::RegisterReflection()
	{
		GASS::ComponentFactory::GetPtr()->Register("HeightmapComponent",new GASS::Creator<HeightmapComponent, Component>);
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("HeightmapComponent", OF_VISIBLE)));

		RegisterProperty<Vec2>("Size", &GASS::HeightmapComponent::GetSize, &GASS::HeightmapComponent::SetSize,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Size of hightmap in [m]",PF_VISIBLE | PF_EDITABLE)));

		RegisterProperty<Float>("Resolution", &GASS::HeightmapComponent::GetResolution, &GASS::HeightmapComponent::SetResolution,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("[samples/m]",PF_VISIBLE | PF_EDITABLE)));

		RegisterProperty<bool>("Update", &GASS::HeightmapComponent::GetUpdate, &GASS::HeightmapComponent::SetUpdate,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Update height values from scene geometry",PF_VISIBLE | PF_EDITABLE)));
	}

	void HeightmapComponent::OnInitialize()
	{
		//Try to load from file!	
		FilePath full_path = _GetFilePath();
		if(full_path.Exist()) //Check if file exist
		{
			m_HM = new Heightmap();
			m_HM->Load(full_path.GetFullPath());
			GetSceneObject()->PostEvent(GeometryChangedEventPtr(new GeometryChangedEvent(DYNAMIC_PTR_CAST<IGeometryComponent>(shared_from_this()))));
		}
		//try to load present file
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

	void HeightmapComponent::SetUpdate(bool value)
	{
		_UpdateData();
	}

	bool  HeightmapComponent::GetUpdate() const
	{
		return false;
	}

	void HeightmapComponent::_UpdateData()
	{
		if(!GetSceneObject())
			return;
		//Get current location component
		Vec3 pos = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->GetWorldPosition();

		Float half_width = m_Size.x*0.5;
		Float half_height = m_Size.y*0.5;

		Vec3 min_bound(pos.x-half_width,0,pos.z-half_width);
		Vec3 max_bound(pos.x+half_height,0,pos.z+half_height);


		const unsigned int px_width = m_Size.x*m_Resolution;
		const unsigned int pz_height = m_Size.y*m_Resolution;

		Float inv_sample_ratio = 1.0/m_Resolution;

		//GEOMETRY_FLAG_GROUND
		GeometryFlags flags =  GEOMETRY_FLAG_SCENE_OBJECTS;
		ScenePtr scene = GetSceneObject()->GetScene();

		delete m_HM;
		m_HM = new Heightmap(min_bound,max_bound,px_width,pz_height);

		LogManager::Get().stream() << "START building heightmap\n";
		for(unsigned int i = 0; i <  px_width; i++)
		{
			LogManager::Get().stream() << "row" << i << "\n";
			for(unsigned int j = 0; j <  pz_height; j++)
			{
				Vec3 pos(min_bound.x + i*inv_sample_ratio, 0, min_bound.z + j*inv_sample_ratio); 
				Float h = CollisionHelper::GetHeightAtPosition(scene, pos, flags, true);
				m_HM->SetHeight(i,j,h);
			}	
		}
		m_HM->Save(_GetFilePath().GetFullPath());
		GetSceneObject()->PostEvent(GeometryChangedEventPtr(new GeometryChangedEvent(DYNAMIC_PTR_CAST<IGeometryComponent>(shared_from_this()))));
	}


	Float HeightmapComponent::GetHeightAtPoint(int x, int z) const
	{
		if(m_HM)
			return m_HM->GetHeight(x,z);
		return 0;
	}
	Float HeightmapComponent::GetHeightAtWorldLocation(Float x, Float z) const
	{
		if(m_HM)
			return m_HM->GetInterpolatedHeight(x,z);
		return 0;
	}

	unsigned int HeightmapComponent::GetSamples() const
	{
		if(m_HM)
			return m_HM->GetWidth();
		return 0;
	}

	float* HeightmapComponent::GetHeightData() const
	{
		if(m_HM)
			return m_HM->GetData();
		return NULL;
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
}
