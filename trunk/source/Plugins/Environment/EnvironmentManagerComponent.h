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
#ifndef ENVIRONMENT_MANAGER_COMPONENT_H
#define ENVIRONMENT_MANAGER_COMPONENT_H

#include "Core/Common.h"
#include "Sim/Components/BaseSceneComponent.h"
#include "Sim/Scenario/Scene/SceneObjectMessages.h"
#include "Sim/Components/Graphics/Geometry/IGeometryComponent.h"
#include "Core/MessageSystem/IMessage.h"
#include <OgreRenderTargetListener.h>
#include <OgreLight.h>

#include "Hydrax/Hydrax.h"
#include "SkyX/SkyX.h"

namespace GASS
{
	class EnvironmentManagerComponent : public Reflection<EnvironmentManagerComponent,BaseSceneComponent>, public Ogre::FrameListener
	{
	public:
		EnvironmentManagerComponent(void);
		~EnvironmentManagerComponent(void);
		static void RegisterReflection();
		virtual void OnCreate();
	protected:
		void UpdateEnvironmentLighting();
		void OnLoad(LoadGFXComponentsMessagePtr message);
		void OnUnload(UnloadComponentsMessagePtr message);
		bool frameStarted(const Ogre::FrameEvent& evt);
		void SetWaterGradient(const std::vector<Vec3> &value);
		std::vector<Vec3>  GetWaterGradient() const ;
		void SetWaterGradientWeights(const std::vector<float> &value);
		std::vector<float>  GetWaterGradientWeights() const;
	private:
		Ogre::Light *m_SunLight;
		SkyX::ColorGradient m_WaterGradient;
		std::vector<Vec3> m_WaterGradientValues;
		std::vector<float> m_WaterGradientWeights;
		SkyX::ColorGradient m_SunGradient; 
		SkyX::ColorGradient m_AmbientGradient;
		Hydrax::Hydrax *m_Hydrax;
		SkyX::SkyX *m_SkyX;
		Ogre::RenderTarget* m_Target;
	};
}

#endif