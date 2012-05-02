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

#include "Sim/Components/GASSBaseSceneComponent.h"
#include "Sim/Scene/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Scene/GASSCoreSceneObjectMessages.h"
#include "Sim/Scene/GASSGraphicsSceneMessages.h"
#include "Sim/Components/Graphics/Geometry/GASSIGeometryComponent.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include <OgreRenderTargetListener.h>
#include "SkyX.h"



namespace GASS
{
	class SkyXComponent : public Reflection<SkyXComponent,BaseSceneComponent> , public Ogre::FrameListener
	{
	public:
		SkyXComponent(void);
		~SkyXComponent(void);
		static void RegisterReflection();
		virtual void OnInitialize();
		SkyX::SkyX* GetSkyX() const {return m_SkyX;}
	protected:
		bool frameStarted(const Ogre::FrameEvent& evt);
		void OnLoad(LoadComponentsMessagePtr message);
		void OnUnload(UnloadComponentsMessagePtr message);
		void OnTimeOfDayMessage(TimeOfDayMessagePtr message);
		void OnChangeCamera(CameraChangedNotifyMessagePtr message);
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

		bool GetSkyDomeFog() const {return m_SkyDomeFog;}
		void SetSkyDomeFog(bool value) {m_SkyDomeFog=value;}
	private:
		void Init(Ogre::Camera* ocam);
		Ogre::RenderTarget* m_Target;
		double m_TimeMultiplier;
		SkyX::SkyX* m_SkyX;
		Float m_MoonSize;
		SkyX::AtmosphereManager::Options m_SkyXOptions;
		Float m_Radius;
		bool m_SkyDomeFog;
	};
	typedef boost::shared_ptr<SkyXComponent> SkyXComponentPtr;
}

#endif