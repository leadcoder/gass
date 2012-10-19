/****************************************************************************
* This file is part of GASS.                                                *
* See http://sourceforge.net/projects/gass/                                 *
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

#include "SkyXCloudLayerComponent.h"
#include "SkyXComponent.h"

#include "Plugins/Ogre/OgreConvert.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/ComponentSystem/GASSIComponent.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSceneObject.h"
#include <Ogre.h>


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
		ComponentFactory::GetPtr()->Register("SkyXCloudLayerComponent",new Creator<SkyXCloudLayerComponent, IComponent>);
		RegisterProperty<Float>("Scale", &SkyXCloudLayerComponent::GetScale, &SkyXCloudLayerComponent::SetScale);
		RegisterProperty<Float>("Height", &SkyXCloudLayerComponent::GetHeight, &SkyXCloudLayerComponent::SetHeight);
		RegisterProperty<Vec2>("WindDirection", &SkyXCloudLayerComponent::GetWindDirection, &SkyXCloudLayerComponent::SetWindDirection);
		RegisterProperty<Float>("TimeMultiplier", &SkyXCloudLayerComponent::GetTimeMultiplier, &SkyXCloudLayerComponent::SetTimeMultiplier);
		RegisterProperty<Float>("DistanceAttenuation", &SkyXCloudLayerComponent::GetDistanceAttenuation, &SkyXCloudLayerComponent::SetDistanceAttenuation);
		RegisterProperty<Float>("DetailAttenuation", &SkyXCloudLayerComponent::GetDetailAttenuation, &SkyXCloudLayerComponent::SetDetailAttenuation);
		RegisterProperty<Float>("NormalMultiplier", &SkyXCloudLayerComponent::GetNormalMultiplier, &SkyXCloudLayerComponent::SetNormalMultiplier);
		RegisterProperty<Float>("HeightVolume", &SkyXCloudLayerComponent::GetHeightVolume, &SkyXCloudLayerComponent::SetHeightVolume);
		RegisterProperty<Float>("VolumetricDisplacement", &SkyXCloudLayerComponent::GetVolumetricDisplacement, &SkyXCloudLayerComponent::SetVolumetricDisplacement);
	}


	void SkyXCloudLayerComponent::OnInitialize()
	{
		
	}

	
	void SkyXCloudLayerComponent::SetScale(const Float &value)
	{
		m_Options.Scale = value;
		UpdateOptions();
	}

	Float SkyXCloudLayerComponent::GetScale() const 
	{
		return m_Options.Scale;
	}

	void SkyXCloudLayerComponent::SetVolumetricDisplacement(const Float &value)
	{
		m_Options.VolumetricDisplacement = value;
		UpdateOptions();
	}

	Float SkyXCloudLayerComponent::GetVolumetricDisplacement() const 
	{
		return m_Options.VolumetricDisplacement;
	}

	void SkyXCloudLayerComponent::SetHeightVolume(const Float &value)
	{
		m_Options.HeightVolume = value;
		UpdateOptions();
	}

	Float SkyXCloudLayerComponent::GetHeightVolume() const 
	{
		return m_Options.HeightVolume;
	}

	void SkyXCloudLayerComponent::SetNormalMultiplier(const Float &value)
	{
		m_Options.NormalMultiplier = value;
		UpdateOptions();
	}

	Float SkyXCloudLayerComponent::GetNormalMultiplier() const 
	{
		return m_Options.NormalMultiplier;
	}
	
	void SkyXCloudLayerComponent::SetDetailAttenuation(const Float &value)
	{
		m_Options.DetailAttenuation = value;
		UpdateOptions();
	}

	Float SkyXCloudLayerComponent::GetDetailAttenuation() const 
	{
		return m_Options.DetailAttenuation;
	}


	void SkyXCloudLayerComponent::SetDistanceAttenuation(const Float &value)
	{
		m_Options.DistanceAttenuation = value;
		UpdateOptions();
	}

	Float SkyXCloudLayerComponent::GetDistanceAttenuation() const 
	{
		return m_Options.DistanceAttenuation;
	}

	void SkyXCloudLayerComponent::SetTimeMultiplier(const Float &value)
	{
		m_Options.TimeMultiplier = value;
		UpdateOptions();
	}

	Float SkyXCloudLayerComponent::GetTimeMultiplier() const 
	{
		return m_Options.TimeMultiplier;
	}


	void SkyXCloudLayerComponent::SetWindDirection(const Vec2 &value)
	{
		m_Options.WindDirection = Convert::ToOgre(value);
		UpdateOptions();
	}

	Vec2 SkyXCloudLayerComponent::GetWindDirection() const 
	{
		return Convert::ToGASS(m_Options.WindDirection);
	}


	void SkyXCloudLayerComponent::SetHeight(const Float &value)
	{
		m_Options.Height = value;
		UpdateOptions();
	}

	Float SkyXCloudLayerComponent::GetHeight() const 
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


