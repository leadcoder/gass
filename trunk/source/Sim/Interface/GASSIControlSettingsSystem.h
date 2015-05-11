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
#include "Core/MessageSystem/GASSBaseMessage.h"
#include "Sim/GASSCommon.h"
#include "Sim/Messages/GASSCoreSystemMessages.h"
#include "Sim/Messages/GASSCoreSceneObjectMessages.h"
#include <map>
#include <string>

namespace GASS
{
	enum ControllerType
	{
		CT_TRIGGER,
		CT_AXIS
	};

	class ControllSettingsMessage : public SystemEventMessage
	{
	public:
		ControllSettingsMessage(const std::string &settings, const std::string &controller, float value, ControllerType ct, SenderID sender_id = -1, double delay= 0) : 
		  SystemEventMessage(sender_id , delay), m_Controller(controller), 
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
	typedef SPTR<ControllSettingsMessage> ControllSettingsMessagePtr;

	/*class InputControllerMessage : public SceneObjectEventMessage
	{
	public:
		InputControllerMessage(const std::string &settings, const std::string &controller, float value, ControllerType ct, SenderID sender_id = -1, double delay= 0) : 
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
	typedef SPTR<InputControllerMessage> InputControllerMessagePtr;*/

	class IControlSettingsSystem 
	{
	public:
		virtual ~IControlSettingsSystem(){};
		virtual void Load(const std::string &filename) = 0;
		virtual std::string GetNameFromIndex(const std::string &settings, int index) = 0;
		virtual int GetIndexFromName(const std::string &settings, const std::string &name) = 0;
	private:
	};
	typedef SPTR<IControlSettingsSystem> ControlSettingsSystemPtr;
}
