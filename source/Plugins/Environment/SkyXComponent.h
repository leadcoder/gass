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
#ifndef SKY_X_COMPONENT_H
#define SKY_X_COMPONENT_H

#include "Sim/GASSCommon.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSGraphicsSystemMessages.h"
#include "Sim/Messages/GASSGraphicsSceneMessages.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "EnvironmentIncludes.h"

namespace GASS
{
	class SkyXComponent : public Reflection<SkyXComponent,BaseSceneComponent>// , public Ogre::FrameListener
	{
	public:
		SkyXComponent(void);
		~SkyXComponent(void);
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual void OnDelete();
		SkyX::SkyX* GetSkyX() const {return m_SkyX;}
	protected:
		void OnTimeOfDayRequest(TimeOfDayRequestPtr message);
		void SetInnerRadius(const float &value);
		float GetInnerRadius() const;
		void SetOuterRadius(const float &value);
		float GetOuterRadius() const ;
		void SetExposure(const float &value);
		float GetExposure() const ;
		void SetHeightPosition(const float &value);
		float GetHeightPosition() const ;
		void SetRayleighMultiplier(const float &value);
		float GetRayleighMultiplier() const ;
		float GetMieMultiplier() const ;
		void SetMieMultiplier(const float &value);
		int GetNumberOfSamples() const ;
		void SetNumberOfSamples(const int &value);
		void SetTimeMultiplier(const float &value);
		float GetTimeMultiplier() const;
		void SetMoonSize(const float &value);
		float GetMoonSize() const ;

		/** Set time
		    @param value Time, where x = time in [0, 24]h range, y = sunrise hour in [0, 24]h range, z = sunset hour in [0, 24] range
		 */
		void SetTime(const Vec3f &value);
		Vec3f GetTime() const;

		void UpdateOptions();

		bool GetSkyDomeFog() const {return m_SkyDomeFog;}
		void SetSkyDomeFog(bool value);// {m_SkyDomeFog=value;}
	private:
		void _Init();
		Ogre::RenderTarget* m_Target;
		float m_TimeMultiplier;
		SkyX::SkyX* m_SkyX;
		float m_MoonSize;
		SkyX::AtmosphereManager::Options m_SkyXOptions;
		float m_Radius;
		bool m_SkyDomeFog;
		bool m_Initialized;
		SkyX::BasicController* m_BasicController;
		Vec3f m_Time;
	};
	typedef GASS_SHARED_PTR<SkyXComponent> SkyXComponentPtr;


	class SkyXComponentLoadedMessage : public BaseMessage
	{
	public:
		SkyXComponentLoadedMessage(SenderID sender_id = -1, double delay= 0) : BaseMessage(sender_id , delay){}
	};
	typedef GASS_SHARED_PTR<SkyXComponentLoadedMessage> SkyXComponentLoadedMessagePtr;
}

#endif