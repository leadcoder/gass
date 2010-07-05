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
#ifndef SKY_X_VOLUME_CLOUD_COMPONENT_H
#define SKY_X_VOLUME_CLOUD_COMPONENT_H

#include "Core/Common.h"
#include "Sim/Components/BaseSceneComponent.h"
#include "Sim/Scenario/Scene/SceneObjectMessages.h"
#include "Sim/Components/Graphics/Geometry/IGeometryComponent.h"
#include "Core/MessageSystem/IMessage.h"
#include <OgreRenderTargetListener.h>
#include "SkyX/SkyX.h"

namespace GASS
{
	class SkyXVolumeCloudComponent : public Reflection<SkyXVolumeCloudComponent,BaseSceneComponent>
	{
	public:
		SkyXVolumeCloudComponent(void);
		~SkyXVolumeCloudComponent(void);
		static void RegisterReflection();
		virtual void OnCreate();
	protected:
		void OnLoad(LoadGFXComponentsMessagePtr message);
		void OnUnload(UnloadComponentsMessagePtr message);
		void SetNoiseScale(const Float &value);
		Float GetNoiseScale() const;
		void SetCloudFieldScale(const Float &value);
		Float GetCloudFieldScale() const;
		void SetWindDirection(const Float &value);
		Float GetWindDirection() const ;
		void SetWindSpeed(const Float &value);
		Float GetWindSpeed() const;
		void SetGlobalOpacity(const Float &value);
		Float GetGlobalOpacity() const;
		void SetWeather(const Vec3 &value);
		Vec3 GetWeather() const;
		void SetHeight(const Vec2 &value);
		Vec2 GetHeight() const;
		void SetRadius(const Float &value);
		Float GetRadius() const;
		Vec4 GetLightResponse() const;
		void SetLightResponse(const Vec4 &value);
		void SetAutoupdate(const bool& value);
		bool GetAutoupdate() const;
	
		void Load();
	private:
		
		SkyX::VCloudsManager* m_CloudManager;
		Float  m_WindDirection;
		Float m_NoiseScale;
		Float m_CloudFieldScale;
		Float m_WindSpeed;
		Vec3 m_Weather;
		Float m_GlobalOpacity;
		Float m_Radius;
		Vec2 m_Height;
		Vec4 m_LightResponse;
		bool m_Autoupdate;
		
	
	};
}

#endif