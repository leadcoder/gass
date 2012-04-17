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
#include "Core/MessageSystem/BaseMessage.h"

#include <map>
#include <string>


namespace GASS
{

	class IInputSystem;
	
	
	class MessageManager;
	class Controller;


	enum ControllerType
	{
		CT_TRIGGER,
		CT_AXIS
	};


	class ControllerMessage : public BaseMessage
	{
	public:
		ControllerMessage(const std::string &controller, float value, ControllerType ct, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_Controller(controller), m_Value(value),m_ControllerType(ct)
		  {

		  }
		  std::string GetController()const {return m_Controller;}
		  float GetValue() const {return m_Value;} 
		  ControllerType GetControllerType() const {return m_ControllerType;}
	private:
		std::string m_Controller;
		float m_Value;
		ControllerType m_ControllerType;
	};

	typedef boost::shared_ptr<ControllerMessage> ControllerMessagePtr;
	


	class GASSExport ControlSetting
	{
	public:
		ControlSetting(IInputSystem* input);
		~ControlSetting(void);
		void Update(double delta_time);
		void AddController(Controller *controller, const std::string &name,int action);
		virtual Controller* GetController(const std::string &input) const;
		IInputSystem* GetInputSystem() const {return m_Input;}
		MessageManager* GetMessageManager() const {return m_MM;}

		typedef std::map<std::string,Controller*> ControllerMap;
		
		std::map<std::string,int> m_NameToIndex;
		std::map<int,std::string> m_IndexToName;
		ControllerMap m_ControllerMap;
		IInputSystem *m_Input;
		MessageManager* m_MM;
	};
}

