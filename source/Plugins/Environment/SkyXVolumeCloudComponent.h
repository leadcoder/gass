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
#ifndef SKY_X_VOLUME_CLOUD_COMPONENT_H
#define SKY_X_VOLUME_CLOUD_COMPONENT_H

#include "Core/Common.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSGraphicsSceneMessages.h"
#include "EnvironmentIncludes.h"

namespace GASS
{

	/**
		GASS Wrapper for volume cloud layer used by the  SkyXcomponent
	*/

	class SkyXVolumeCloudComponent : public Reflection<SkyXVolumeCloudComponent,BaseSceneComponent>
	{
	public:
		SkyXVolumeCloudComponent(void);
		~SkyXVolumeCloudComponent(void);
		static void RegisterReflection();
		virtual void OnInitialize();
		void CreateVolume();
	protected:
		void OnWeatherRequest(WeatherRequestPtr message);
		void SetNoiseScale(const float &value);
		float GetNoiseScale() const;
		void SetCloudFieldScale(const float &value);
		float GetCloudFieldScale() const;
		void SetWindDirection(const float &value);
		float GetWindDirection() const ;
		void SetWindSpeed(const float &value);
		float GetWindSpeed() const;
		void SetGlobalOpacity(const float &value);
		float GetGlobalOpacity() const;
		void SetWeather(const Vec3f &value);
		Vec3f GetWeather() const;
		void SetHeight(const Vec2f &value);
		Vec2f GetHeight() const;
		void SetRadius(const float &value);
		float GetRadius() const;
		Vec4f GetLightResponse() const;
		void SetLightResponse(const Vec4f &value);
		void SetAutoupdate(const bool& value);
		bool GetAutoupdate() const;
		
	private:
		SkyX::VCloudsManager* m_CloudManager;
		float  m_WindDirection;
		float m_NoiseScale;
		float m_CloudFieldScale;
		float m_WindSpeed;
		Vec3f m_Weather;
		float m_GlobalOpacity;
		float m_Radius;
		Vec2f m_Height;
		Vec4f m_LightResponse;
		bool m_Autoupdate;
	};
	typedef GASS_SHARED_PTR<SkyXVolumeCloudComponent> SkyXVolumeCloudComponentPtr; 
}

#endif