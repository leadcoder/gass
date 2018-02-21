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

#include "SkyXVolumeCloudComponent.h"
#include "SkyXComponent.h"

#include "Plugins/Ogre/GASSOgreConvert.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/ComponentSystem/GASSComponent.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSScene.h"

namespace GASS
{
	SkyXVolumeCloudComponent::SkyXVolumeCloudComponent(void) :m_CloudManager(NULL), 
		m_WindSpeed(80),
		m_NoiseScale(1),
		m_CloudFieldScale(1.0f),
		m_WindDirection(0),
		m_Weather(1,1,-1),
		m_GlobalOpacity(1.0f),
		m_Height(1000,1500),
		m_Radius(10000),
		m_LightResponse(0.25f,0.2f,1.0f,0.1f),
		m_Autoupdate(false)
	{
				
	}

	SkyXVolumeCloudComponent::~SkyXVolumeCloudComponent(void)
	{

	}

	void SkyXVolumeCloudComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("SkyXVolumeCloudComponent",new Creator<SkyXVolumeCloudComponent, Component>);

		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("SkyXVolumeCloudComponent", OF_VISIBLE )));

		RegisterProperty<float>("NoiseScale", &SkyXVolumeCloudComponent::GetNoiseScale, &SkyXVolumeCloudComponent::SetNoiseScale,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<float>("CloudFieldScale", &SkyXVolumeCloudComponent::GetCloudFieldScale, &SkyXVolumeCloudComponent::SetCloudFieldScale,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<float>("WindDirection", &SkyXVolumeCloudComponent::GetWindDirection, &SkyXVolumeCloudComponent::SetWindDirection,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<float>("WindSpeed", &SkyXVolumeCloudComponent::GetWindSpeed, &SkyXVolumeCloudComponent::SetWindSpeed,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<float>("GlobalOpacity", &SkyXVolumeCloudComponent::GetGlobalOpacity, &SkyXVolumeCloudComponent::SetGlobalOpacity,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<Vec2f>("Height", &SkyXVolumeCloudComponent::GetHeight, &SkyXVolumeCloudComponent::SetHeight,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<float>("Radius", &SkyXVolumeCloudComponent::GetRadius, &SkyXVolumeCloudComponent::SetRadius,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<Vec3f>("Weather", &SkyXVolumeCloudComponent::GetWeather, &SkyXVolumeCloudComponent::SetWeather,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<Vec4f>("LightResponse", &SkyXVolumeCloudComponent::GetLightResponse, &SkyXVolumeCloudComponent::SetLightResponse,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<bool>("Autoupdate", &SkyXVolumeCloudComponent::GetAutoupdate, &SkyXVolumeCloudComponent::SetAutoupdate,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
	}

	void SkyXVolumeCloudComponent::OnInitialize()
	{
		GetSceneObject()->GetScene()->RegisterForMessage(REG_TMESS(SkyXVolumeCloudComponent::OnWeatherRequest,WeatherRequest,0));
	}

	void SkyXVolumeCloudComponent::OnWeatherRequest(WeatherRequestPtr message)
	{
		float cloud_factor = message->GetClouds();
		SetGlobalOpacity(cloud_factor);

		Vec4f ls = GetLightResponse();
		ls.y = 0.1f + (0.9f - cloud_factor-0.1f);
		ls.w = ls.y;
		SetLightResponse(ls);
	}

	void SkyXVolumeCloudComponent::SetAutoupdate(const bool& value)
	{
		m_Autoupdate = value;
		if(m_CloudManager)
			m_CloudManager->setAutoupdate(value);

	}

	bool SkyXVolumeCloudComponent::GetAutoupdate() const
	{
		return m_Autoupdate;
	}

	void SkyXVolumeCloudComponent::SetLightResponse(const Vec4f &value)
	{
		m_LightResponse = value;
		if(m_CloudManager)
			m_CloudManager->getVClouds()->setLightResponse(Ogre::Vector4(value.x, value.y, value.z, value.w));
	}

	Vec4f SkyXVolumeCloudComponent::GetLightResponse() const 
	{
		return m_LightResponse;
	}

	void SkyXVolumeCloudComponent::SetCloudFieldScale(const float &value)
	{
		m_CloudFieldScale = value;
		if(m_CloudManager)
			m_CloudManager->getVClouds()->setCloudFieldScale(value);
	}

	float SkyXVolumeCloudComponent::GetCloudFieldScale() const 
	{
		return m_CloudFieldScale;
	}

	void SkyXVolumeCloudComponent::SetNoiseScale(const float &value)
	{
		m_NoiseScale = value;
		if(m_CloudManager)
			m_CloudManager->getVClouds()->setNoiseScale(m_NoiseScale);
	}

	float SkyXVolumeCloudComponent::GetNoiseScale() const 
	{
		return m_NoiseScale;
	}

	void SkyXVolumeCloudComponent::SetWindDirection(const float &value)
	{
		m_WindDirection = value;
		if(m_CloudManager)
			m_CloudManager->getVClouds()->setWindDirection(Ogre::Radian(m_WindDirection));
	}

	float SkyXVolumeCloudComponent::GetWindDirection() const 
	{
		return m_WindDirection;
	}

	void SkyXVolumeCloudComponent::SetGlobalOpacity(const float &value)
	{
		m_GlobalOpacity = value;
		if(m_CloudManager)
			m_CloudManager->getVClouds()->setGlobalOpacity(value);
	}

	float SkyXVolumeCloudComponent::GetGlobalOpacity() const 
	{
		return m_GlobalOpacity;
	}

	void SkyXVolumeCloudComponent::SetWeather(const Vec3f &value)
	{
		m_Weather = value;
		if(m_CloudManager)
			m_CloudManager->getVClouds()->setWheater(value.x,
													 value.y,
													 value.z > 0);
	}


	Vec3f SkyXVolumeCloudComponent::GetWeather() const 
	{
		return m_Weather;
	}

	void SkyXVolumeCloudComponent::SetHeight(const Vec2f &value)
	{
		m_Height = value;
		if(m_CloudManager)
			CreateVolume();	//reload
	}

	Vec2f SkyXVolumeCloudComponent::GetHeight() const 
	{
		return m_Height;
	}
	
	void SkyXVolumeCloudComponent::SetWindSpeed(const float &value)
	{
		m_WindSpeed = value;
		if(m_CloudManager)
			m_CloudManager->setWindSpeed(value);
	}

	float SkyXVolumeCloudComponent::GetWindSpeed() const 
	{
		return m_WindSpeed;
	}

	void SkyXVolumeCloudComponent::SetRadius(const float &value)
	{
		m_Radius = value;
		if(m_CloudManager)
			CreateVolume();	 //reload
		//	m_CloudManager->getVClouds()->setRadius(value);
	}

	float SkyXVolumeCloudComponent::GetRadius() const 
	{
		return m_Radius;
	}

	void SkyXVolumeCloudComponent::CreateVolume()
	{
		//Get Skyx component
		SkyXComponentPtr skyx = GetSceneObject()->GetFirstComponentByClass<SkyXComponent>();
		if(skyx)
		{
			skyx->GetSkyX()->getVCloudsManager()->remove();

			//float save_clip  = skyx->GetSkyX()->getCamera()->getFarClipDistance();
			//skyx->GetSkyX()->getCamera()->setFarClipDistance(m_Radius);
			m_CloudManager = skyx->GetSkyX()->getVCloudsManager();
			// Volumetric clouds
			m_CloudManager->setHeight(Ogre::Vector2(m_Height.x, m_Height.y));
			m_CloudManager->create(m_Radius);
			//skyx->GetSkyX()->getCamera()->setFarClipDistance(save_clip );


			SetNoiseScale(m_NoiseScale);
			SetCloudFieldScale(m_CloudFieldScale);
			SetWindDirection(m_WindDirection);
			SetWindSpeed(m_WindSpeed);
			SetGlobalOpacity(m_GlobalOpacity);
		
			SetWeather(m_Weather);
			SetLightResponse(m_LightResponse);
			SetAutoupdate(m_Autoupdate);
		}
	}
}


