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
#ifndef SKY_X_COMPONENT_H
#define SKY_X_COMPONENT_H

#include "Sim/Components/BaseSceneComponent.h"
#include "Sim/Scenario/Scene/SceneObjectMessages.h"
#include "Sim/Components/Graphics/Geometry/IGeometryComponent.h"
#include "Core/MessageSystem/IMessage.h"
#include <OgreRenderTargetListener.h>
#include "SkyX/SkyX.h"



namespace GASS
{

	class SkyXComponent : public Reflection<SkyXComponent,BaseSceneComponent> , public Ogre::RenderTargetListener
	{
	public:
		SkyXComponent(void);
		~SkyXComponent(void);
		static void RegisterReflection();
		virtual void OnCreate();
		SkyX::SkyX* GetSkyX() const {return m_SkyX;}
	protected:
		virtual void preViewportUpdate(const Ogre::RenderTargetViewportEvent& evt);
		void OnLoad(LoadGFXComponentsMessagePtr message);
		void OnUnload(UnloadComponentsMessagePtr message);
		void SetInnerRadius(const Float &value);
		Float GetInnerRadius() const;
		void SetOuterRadius(const Float &value);
		Float GetOuterRadius() const ;
		void SetExposure(const Float &value);
		Float GetExposure() const ;
		void SetHeightPosition(const Float &value);
		Float GetHeightPosition() const ;
		void SetRayleighMultiplier(const Float &value);
		Float GetRayleighMultiplier() const ;
		Float GetMieMultiplier() const ;
		void SetMieMultiplier(const Float &value);
		int GetNumberOfSamples() const ;
		void SetNumberOfSamples(const int &value);
		void SetTimeMultiplier(const double &value);
		double GetTimeMultiplier() const;
		void SetMoonSize(const Float &value);
		Float GetMoonSize() const ;


		void SetTime(const Vec3 &value);
		Vec3 GetTime() const;

		void UpdateOptions();
	private:
		Ogre::Viewport* m_Viewport;
		double m_TimeMultiplier;
		SkyX::SkyX* m_SkyX;
		Float m_MoonSize;
		SkyX::AtmosphereManager::Options m_SkyXOptions;
		Float m_Radius;
	};
	typedef boost::shared_ptr<SkyXComponent> SkyXComponentPtr;
}

#endif