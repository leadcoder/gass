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

#pragma once
#include "Sim/GASS.h"
#include "Plugins/OSG/OSGCommon.h"

namespace GASS
{
	class OSGLightComponent : public Reflection<OSGLightComponent,Component>, public ILightComponent
	{
	public:
		OSGLightComponent();
		~OSGLightComponent() override;
		static void RegisterReflection();
		void OnInitialize() override;
		void SetLightType(LightTypeBinder lt);
		LightTypeBinder GetLightType()const {return m_LightType;}
		void SetAttenuationParams(const Vec4 &params);
		Vec4 GetAttenuationParams()const {return m_AttenuationParams;}
		void SetSpotParams(const Vec3 &params);
		Vec3 GetSpotParams()const {return m_SpotParams;}
		void SetDiffuse(const ColorRGB &diffuse);
		ColorRGB GetDiffuse()const {return m_Diffuse;}
		void SetSpecular(const ColorRGB &specular);
		ColorRGB GetSpecular()const {return m_Specular;}
		void SetAmbient(const ColorRGB &ambient);
		ColorRGB GetAmbient()const {return m_Ambient;}
		void SetCastShadow(bool value);
		bool GetCastShadow()const {return m_CastShadow;}
		void SetLightId(int value);
		int GetLightId()const {return m_LightId;}
	protected:
		void OnLocationLoaded(LocationLoadedEventPtr message);

		LightTypeBinder m_LightType;
		ColorRGB m_Diffuse;
		ColorRGB m_Specular;
		ColorRGB m_Ambient;
		Vec4 m_AttenuationParams;
		bool m_CastShadow{true};
		Vec3 m_SpotParams;
		int m_LightId{0};
		osg::ref_ptr<osg::Light> m_OSGLight;
		osg::ref_ptr<osg::LightSource>  m_OSGLightSource;
	};
}

