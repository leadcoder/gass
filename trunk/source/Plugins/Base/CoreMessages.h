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

#ifndef CORE_MESSAGES_H
#define CORE_MESSAGES_H

#include "Core/MessageSystem/BaseMessage.h"
#include "CoreSceneManager.h"

namespace GASS
{
	class LoadCoreComponentsMessage : public BaseMessage
	{
	public:
		LoadCoreComponentsMessage(CoreSceneManagerPtr core_scene_manager, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_CoreSceneManager(core_scene_manager){}
		  CoreSceneManagerPtr GetCoreSceneManager() const {return m_CoreSceneManager;}
	private:
		CoreSceneManagerPtr m_CoreSceneManager;
	};
	typedef boost::shared_ptr<LoadCoreComponentsMessage> LoadCoreComponentsMessagePtr;
}
#endif
