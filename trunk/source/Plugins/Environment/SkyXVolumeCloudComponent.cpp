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

#include "SkyXVolumeCloudComponent.h"
#include "SkyXComponent.h"

#include "Plugins/Ogre/OgreConvert.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/ComponentSystem/IComponent.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/IMessage.h"
#include "Sim/SimEngine.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include <Ogre.h>


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
		ComponentFactory::GetPtr()->Register("SkyXVolumeCloudComponent",new Creator<SkyXVolumeCloudComponent, IComponent>);
		RegisterProperty<Float>("NoiseScale", &SkyXVolumeCloudComponent::GetNoiseScale, &SkyXVolumeCloudComponent::SetNoiseScale);
		RegisterProperty<Float>("CloudFieldScale", &SkyXVolumeCloudComponent::GetCloudFieldScale, &SkyXVolumeCloudComponent::SetCloudFieldScale);
		RegisterProperty<Float>("WindDirection", &SkyXVolumeCloudComponent::GetWindDirection, &SkyXVolumeCloudComponent::SetWindDirection);
		RegisterProperty<Float>("WindSpeed", &SkyXVolumeCloudComponent::GetWindSpeed, &SkyXVolumeCloudComponent::SetWindSpeed);
		RegisterProperty<Float>("GlobalOpacity", &SkyXVolumeCloudComponent::GetGlobalOpacity, &SkyXVolumeCloudComponent::SetGlobalOpacity);
		RegisterProperty<Vec2>("Height", &SkyXVolumeCloudComponent::GetHeight, &SkyXVolumeCloudComponent::SetHeight);
		RegisterProperty<Float>("Radius", &SkyXVolumeCloudComponent::GetRadius, &SkyXVolumeCloudComponent::SetRadius);
		RegisterProperty<Vec3>("Weather", &SkyXVolumeCloudComponent::GetWeather, &SkyXVolumeCloudComponent::SetWeather);
		RegisterProperty<Vec4>("LightResponse", &SkyXVolumeCloudComponent::GetLightResponse, &SkyXVolumeCloudComponent::SetLightResponse);
		RegisterProperty<bool>("Autoupdate", &SkyXVolumeCloudComponent::GetAutoupdate, &SkyXVolumeCloudComponent::SetAutoupdate);
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

	void SkyXVolumeCloudComponent::SetLightResponse(const Vec4 &value)
	{
		m_LightResponse = value;
		if(m_CloudManager)
			m_CloudManager->getVClouds()->setLightResponse(Convert::ToOgre(value));
	}

	Vec4 SkyXVolumeCloudComponent::GetLightResponse() const 
	{
		return m_LightResponse;
	}

	void SkyXVolumeCloudComponent::SetCloudFieldScale(const Float &value)
	{
		m_CloudFieldScale = value;
		if(m_CloudManager)
			m_CloudManager->getVClouds()->setCloudFieldScale(value);
	}

	Float SkyXVolumeCloudComponent::GetCloudFieldScale() const 
	{
		return m_CloudFieldScale;
	}

	void SkyXVolumeCloudComponent::SetNoiseScale(const Float &value)
	{
		m_NoiseScale = value;
		if(m_CloudManager)
			m_CloudManager->getVClouds()->setNoiseScale(m_NoiseScale);
	}

	Float SkyXVolumeCloudComponent::GetNoiseScale() const 
	{
		return m_NoiseScale;
	}

	void SkyXVolumeCloudComponent::SetWindDirection(const Float &value)
	{
		m_WindDirection = value;
		if(m_CloudManager)
			m_CloudManager->getVClouds()->setWindDirection(Ogre::Radian(m_WindDirection));
	}

	Float SkyXVolumeCloudComponent::GetWindDirection() const 
	{
		return m_WindDirection;
	}

	void SkyXVolumeCloudComponent::SetGlobalOpacity(const Float &value)
	{
		m_GlobalOpacity = value;
		if(m_CloudManager)
			m_CloudManager->getVClouds()->setGlobalOpacity(value);
	}

	Float SkyXVolumeCloudComponent::GetGlobalOpacity() const 
	{
		return m_GlobalOpacity;
	}

	void SkyXVolumeCloudComponent::SetWeather(const Vec3 &value)
	{
		m_Weather = value;
		if(m_CloudManager)
			m_CloudManager->getVClouds()->setWheater(value.x,value.y,value.z);
	}


	Vec3 SkyXVolumeCloudComponent::GetWeather() const 
	{
		return m_Weather;
	}

	void SkyXVolumeCloudComponent::SetHeight(const Vec2 &value)
	{
		m_Height = value;
		if(m_CloudManager)
			Load();	//reload
	}

	Vec2 SkyXVolumeCloudComponent::GetHeight() const 
	{
		return m_Height;
	}
	
	void SkyXVolumeCloudComponent::SetWindSpeed(const Float &value)
	{
		m_WindSpeed = value;
		if(m_CloudManager)
			m_CloudManager->setWindSpeed(value);
	}

	Float SkyXVolumeCloudComponent::GetWindSpeed() const 
	{
		return m_WindSpeed;
	}

	void SkyXVolumeCloudComponent::SetRadius(const Float &value)
	{
		m_Radius = value;
		if(m_CloudManager)
			Load();	 //reload
		//	m_CloudManager->getVClouds()->setRadius(value);
	}

	Float SkyXVolumeCloudComponent::GetRadius() const 
	{
		return m_Radius;
	}

	void SkyXVolumeCloudComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(SkyXVolumeCloudComponent::OnLoad,LoadGFXComponentsMessage,3));
		GetSceneObject()->RegisterForMessage(REG_TMESS(SkyXVolumeCloudComponent::OnUnload,UnloadComponentsMessage,0));
	}

	void SkyXVolumeCloudComponent::OnUnload(UnloadComponentsMessagePtr message)
	{

	}

	void SkyXVolumeCloudComponent::OnLoad(LoadGFXComponentsMessagePtr message)
	{
		//Get Skyx component
		Load();
	}


	void SkyXVolumeCloudComponent::Load()
	{
		//Get Skyx component
		SkyXComponentPtr skyx = GetSceneObject()->GetFirstComponent<SkyXComponent>();
		if(skyx)
		{
			skyx->GetSkyX()->getVCloudsManager()->remove();

			float save_clip  = skyx->GetSkyX()->getCamera()->getFarClipDistance();
			skyx->GetSkyX()->getCamera()->setFarClipDistance(m_Radius);
			m_CloudManager = skyx->GetSkyX()->getVCloudsManager();
			// Volumetric clouds
			m_CloudManager->setHeight(Convert::ToOgre(m_Height));
			m_CloudManager->create();
			skyx->GetSkyX()->getCamera()->setFarClipDistance(save_clip );
		}
	}
}


