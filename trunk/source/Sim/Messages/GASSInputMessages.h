/****************************************************************************
* This file is part of GASS.                                                *
* See http://code.google.com/p/gass/                                        *
*                                                                           *
* Copyright (c) 2008-2015 GASS team. See Contributors.txt for details.      *
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
#include "Sim/GASSCommon.h"
#include "Core/MessageSystem/GASSBaseMessage.h"
#include "Sim/GASSPlatformType.h"
#include "Sim/GASSPlatformDetectionData.h"
#include "Core/Math/GASSVector.h"
#include "Sim/Interface/GASSIControlSettingsSystem.h"
#include "Sim/Messages/GASSCoreSceneObjectMessages.h"

namespace GASS
{
	class InputRelayEvent : public SceneObjectEventMessage
	{
	public:
		InputRelayEvent(const std::string &settings, const std::string &controller, float value, ControllerType ct, SenderID sender_id = -1, double delay= 0) : 
		  SceneObjectEventMessage(sender_id , delay), m_Controller(controller), 
			  m_Value(value),
			  m_ControllerType(ct),
			  m_Settings(settings)
		  {

		  }
		  std::string GetSettings()const {return m_Settings;}
		  std::string GetController()const {return m_Controller;}
		  float GetValue() const {return m_Value;} 
		  ControllerType GetControllerType() const {return m_ControllerType;}
	private:
		std::string m_Settings;
		std::string m_Controller;
		float m_Value;
		ControllerType m_ControllerType;
	};
	typedef SPTR<InputRelayEvent> InputRelayEventPtr;

}

