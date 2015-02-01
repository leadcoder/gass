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

#ifndef MY_GUI_MESSAGES_H
#define MY_GUI_MESSAGES_H

#include "Sim/GASSCommon.h"
#include "Core/MessageSystem/GASSBaseMessage.h"
#include "Core/Math/GASSVector.h"
#include "Sim/Messages/GASSCoreSceneObjectMessages.h"
#include <MyGUI.h>

namespace GASS
{
	class LayoutLoadedEvent : public SceneObjectEventMessage
	{
	public:
		LayoutLoadedEvent(MyGUI::VectorWidgetPtr widgets, SenderID sender_id = -1, double delay= 0) : 
		  SceneObjectEventMessage(sender_id , delay), m_Widgets(widgets)
		  {

		  }
		  MyGUI::VectorWidgetPtr m_Widgets;
	private:
		
	};
	typedef SPTR<LayoutLoadedEvent> LayoutLoadedEventPtr;
}
#endif
