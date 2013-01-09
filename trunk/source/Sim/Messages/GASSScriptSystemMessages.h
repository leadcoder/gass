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

#ifndef SCRIPT_SYSTEM_MESSAGES_H
#define SCRIPT_SYSTEM_MESSAGES_H

#include "Sim/GASSCommon.h"
#include "Core/MessageSystem/GASSBaseMessage.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Math/GASSVector.h"
#include "Core/Math/GASSQuaternion.h"
#include "Sim/Messages/GASSCoreSystemMessages.h"
#include <string>

namespace GASS
{
	class Scene;
	class Scene;
	class SceneObject;

	typedef boost::shared_ptr<Scene> ScenePtr;
	typedef boost::shared_ptr<Scene> ScenePtr;
	typedef boost::shared_ptr<SceneObject> SceneObjectPtr;


	//Script messages
	/**
	Lua script state is distributed through this message
	*/
	class LuaScriptStateMessage : public BaseMessage
	{
	public:
		LuaScriptStateMessage(void *state, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay) ,
			  m_State(state){}
		 void* GetState() const {return m_State;}
	private:
		void *m_State;
	};
	typedef boost::shared_ptr<LuaScriptStateMessage> LuaScriptStateMessagePtr;

	/**
	Load a gui script file	
	*/
	class GUIScriptRequest : public SystemRequestMessage
	{
	public:
		GUIScriptRequest(const std::string &filename, SenderID sender_id = -1, double delay= 0) :
		  SystemRequestMessage(sender_id , delay) ,
			  m_Name(filename){}

		 std::string GetFilename() const {return m_Name;}
	private:
		std::string m_Name;
	};
	typedef boost::shared_ptr<GUIScriptRequest> GUIScriptRequestPtr;
}
#endif
