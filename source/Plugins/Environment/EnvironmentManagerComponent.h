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
#ifndef ENVIRONMENT_MANAGER_COMPONENT_H
#define ENVIRONMENT_MANAGER_COMPONENT_H

#include "Core/Common.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSGraphicsSystemMessages.h"
#include "Sim/Messages/GASSGraphicsSceneMessages.h"
#include "Sim/Interface/GASSIGeometryComponent.h"
#include "EnvironmentIncludes.h"

namespace GASS
{
	/**
		Manager used to control weather effects like fog, sun light colors(ambient and diffuse), water colors.
		The manager hold color gradients that is mapped to current light direction from SkyX.
	*/
	class EnvironmentManagerComponent : public Reflection<EnvironmentManagerComponent,BaseSceneComponent>, public Ogre::FrameListener
	{
	public:
		EnvironmentManagerComponent(void);
		~EnvironmentManagerComponent(void) override;
		static void RegisterReflection();
		void OnInitialize() override;
		void OnDelete() override;
	protected:
		void UpdateEnvironmentLighting();
		void OnChangeCamera(CameraChangedEventPtr message);
		void OnWeatherRequest(WeatherRequestPtr message);
		bool frameStarted(const Ogre::FrameEvent& evt) override;
		void SetWaterGradient(const std::vector<Vec3f> &value);
		std::vector<Vec3f>  GetWaterGradient() const ;
		void SetWaterGradientWeights(const std::vector<float> &value);
		std::vector<float>  GetWaterGradientWeights() const;

		void SetSunGradient(const std::vector<Vec3f> &value);
		std::vector<Vec3f>  GetSunGradient() const ;
		void SetSunGradientWeights(const std::vector<float> &value);
		std::vector<float>  GetSunGradientWeights() const;

		void SetAmbientGradient(const std::vector<Vec3f> &value);
		std::vector<Vec3f>  GetAmbientGradient() const ;
		void SetAmbientGradientWeights(const std::vector<float> &value);
		std::vector<float>  GetAmbientGradientWeights() const;

		void SetFogGradient(const std::vector<Vec3f> &value);
		std::vector<Vec3f>  GetFogGradient() const ;
		void SetFogGradientWeights(const std::vector<float> &value);
		std::vector<float>  GetFogGradientWeights() const;
		
		float  GetSpecularWeight() const {return m_SpecularWeight;}
		void SetSpecularWeight(float value) {m_SpecularWeight = value;}
	private:
		Ogre::Light *m_SunLight;
		
		std::vector<Vec3f> m_WaterGradientValues;
		std::vector<Vec3f> m_SunGradientValues;
		std::vector<Vec3f> m_AmbientGradientValues;
		std::vector<Vec3f> m_FogGradientValues;
		
		std::vector<float> m_WaterGradientWeights;
		std::vector<float> m_SunGradientWeights;
		std::vector<float> m_AmbientGradientWeights;
		std::vector<float> m_FogGradientWeights;

		float m_SpecularWeight;

		SkyX::ColorGradient m_WaterGradient;
		SkyX::ColorGradient m_SunGradient; 
		SkyX::ColorGradient m_AmbientGradient;
		SkyX::ColorGradient m_FogGradient;

		Hydrax::Hydrax *m_Hydrax;
		Ogre::RenderTarget* m_Target;
		Ogre::Camera * m_CurrentCamera;
		float m_CloudFactor;
	};
}

#endif