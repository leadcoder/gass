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

#include "SkyXCloudLayerComponent.h"
#include "SkyXComponent.h"

#include "Plugins/Ogre/GASSOgreConvert.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/ComponentSystem/GASSComponent.h"
#include "Sim/GASSSceneObject.h"

namespace GASS
{
	SkyXCloudLayerComponent::SkyXCloudLayerComponent(void) :m_CloudLayer(NULL)
	{
				
	}

	SkyXCloudLayerComponent::~SkyXCloudLayerComponent(void)
	{

	}

	void SkyXCloudLayerComponent::RegisterReflection()
	{
		ComponentFactory::Get().Register<SkyXCloudLayerComponent>();
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("SkyXCloudLayerComponent", OF_VISIBLE )));
		RegisterGetSet("Scale", &SkyXCloudLayerComponent::GetScale, &SkyXCloudLayerComponent::SetScale,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("Height", &SkyXCloudLayerComponent::GetHeight, &SkyXCloudLayerComponent::SetHeight,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("WindDirection", &SkyXCloudLayerComponent::GetWindDirection, &SkyXCloudLayerComponent::SetWindDirection,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("TimeMultiplier", &SkyXCloudLayerComponent::GetTimeMultiplier, &SkyXCloudLayerComponent::SetTimeMultiplier,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("DistanceAttenuation", &SkyXCloudLayerComponent::GetDistanceAttenuation, &SkyXCloudLayerComponent::SetDistanceAttenuation,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("DetailAttenuation", &SkyXCloudLayerComponent::GetDetailAttenuation, &SkyXCloudLayerComponent::SetDetailAttenuation,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("NormalMultiplier", &SkyXCloudLayerComponent::GetNormalMultiplier, &SkyXCloudLayerComponent::SetNormalMultiplier,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("HeightVolume", &SkyXCloudLayerComponent::GetHeightVolume, &SkyXCloudLayerComponent::SetHeightVolume,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("VolumetricDisplacement", &SkyXCloudLayerComponent::GetVolumetricDisplacement, &SkyXCloudLayerComponent::SetVolumetricDisplacement,PF_VISIBLE | PF_EDITABLE,"");
	}

	void SkyXCloudLayerComponent::OnInitialize()
	{
		
	}
	
	void SkyXCloudLayerComponent::SetScale(const float &value)
	{
		m_Options.Scale = value;
		UpdateOptions();
	}

	float SkyXCloudLayerComponent::GetScale() const
	{
		return m_Options.Scale;
	}

	void SkyXCloudLayerComponent::SetVolumetricDisplacement(const float &value)
	{
		m_Options.VolumetricDisplacement = value;
		UpdateOptions();
	}

	float SkyXCloudLayerComponent::GetVolumetricDisplacement() const
	{
		return m_Options.VolumetricDisplacement;
	}

	void SkyXCloudLayerComponent::SetHeightVolume(const float &value)
	{
		m_Options.HeightVolume = value;
		UpdateOptions();
	}

	float SkyXCloudLayerComponent::GetHeightVolume() const
	{
		return m_Options.HeightVolume;
	}

	void SkyXCloudLayerComponent::SetNormalMultiplier(const float &/*value*/)
	{
		//m_Options.NormalMultiplier = value;
		//UpdateOptions();
	}

	float SkyXCloudLayerComponent::GetNormalMultiplier() const
	{
		return 0;//m_Options.NormalMultiplier;
	}
	
	void SkyXCloudLayerComponent::SetDetailAttenuation(const float &value)
	{
		m_Options.DetailAttenuation = value;
		UpdateOptions();
	}

	float SkyXCloudLayerComponent::GetDetailAttenuation() const
	{
		return m_Options.DetailAttenuation;
	}


	void SkyXCloudLayerComponent::SetDistanceAttenuation(const float &value)
	{
		m_Options.DistanceAttenuation = value;
		UpdateOptions();
	}

	float SkyXCloudLayerComponent::GetDistanceAttenuation() const 
	{
		return m_Options.DistanceAttenuation;
	}

	void SkyXCloudLayerComponent::SetTimeMultiplier(const float &value)
	{
		m_Options.TimeMultiplier = value;
		UpdateOptions();
	}

	float SkyXCloudLayerComponent::GetTimeMultiplier() const
	{
		return m_Options.TimeMultiplier;
	}

	void SkyXCloudLayerComponent::SetWindDirection(const Vec2f &value)
	{
		m_Options.WindDirection.x = value.x;
		m_Options.WindDirection.y = value.y;
		UpdateOptions();
	}

	Vec2f SkyXCloudLayerComponent::GetWindDirection() const 
	{
		return Vec2f(m_Options.WindDirection.x, m_Options.WindDirection.y);
	}

	void SkyXCloudLayerComponent::SetHeight(const float &value)
	{
		m_Options.Height = value;
		UpdateOptions();
	}

	float SkyXCloudLayerComponent::GetHeight() const
	{
		return m_Options.Height;
	}

	void SkyXCloudLayerComponent::UpdateOptions()
	{
		if(m_CloudLayer)
		{
			m_CloudLayer->setOptions(m_Options);
			
		}
	}

	void SkyXCloudLayerComponent::CreateLayer()
	{
		//Get Skyx component
		SkyXComponentPtr skyx = GetSceneObject()->GetFirstComponentByClass<SkyXComponent>();
		if(skyx)
			m_CloudLayer = skyx->GetSkyX()->getCloudsManager()->add(m_Options);
	}
}


