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

#pragma once
#include "Sim/Common.h"
#include "Core/Math/Vector.h"
#include "Sim/Components/Graphics/ILightComponent.h"
#include "Sim/Components/BaseSceneComponent.h"
#include "Sim/Scenario/Scene/Messages/CoreSceneObjectMessages.h"
#include "Sim/Scenario/Scene/Messages/GraphicsSceneObjectMessages.h"

#include <osg/Light>
#include <osg/LightSource>


namespace GASS
{
	
	class OSGLightComponent : public Reflection<OSGLightComponent,BaseSceneComponent>, public ILightComponent
	{
	public:
		OSGLightComponent();
		virtual ~OSGLightComponent();
		static void RegisterReflection();
		virtual void OnCreate();
		void SetLightType(LightType lt);
		LightType GetLightType()const {return m_LightType;}
		void SetAttenuationParams(const Vec4 &params);
		Vec4 GetAttenuationParams()const {return m_AttenuationParams;}
		void SetSpotParams(const Vec3 &params);
		Vec3 GetSpotParams()const {return m_SpotParams;}
		void SetDiffuse(const Vec3 &diffuse);
		Vec3 GetDiffuse()const {return m_Diffuse;}
		void SetSpecular(const Vec3 &specular);
		Vec3 GetSpecular()const {return m_Specular;}
		void SetAmbient(const Vec3 &ambient);
		Vec3 GetAmbient()const {return m_Ambient;}
		void SetCastShadow(bool value);
		bool GetCastShadow()const {return m_CastShadow;}
		void SetLightId(int value);
		int GetLightId()const {return m_LightId;}
		
	protected:
		
		void OnLoad(LoadGFXComponentsMessagePtr message);
		LightType m_LightType;

		Vec3 m_Diffuse;
		Vec3 m_Specular;
		Vec3 m_Ambient;
		Vec4 m_AttenuationParams;
		bool m_CastShadow;
		Vec3 m_SpotParams;
		int m_LightId;

		osg::ref_ptr<osg::Light> m_OSGLight;
		osg::ref_ptr<osg::LightSource>  m_OSGLightSource;
	};
}

