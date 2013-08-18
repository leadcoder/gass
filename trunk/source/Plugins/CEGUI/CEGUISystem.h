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

#ifndef CE_GUI_MANAGER
#define CE_GUI_MANAGER

#include "Core/MessageSystem/IMessage.h"
#include "Sim/Scenario/Scene/BaseSceneManager.h"
#include "Sim/Systems/SimSystem.h"
#include "Sim/Systems/SimSystemMessages.h"
#include "Sim/Systems/Input/IInputSystem.h"

#include "CEGUI.h"

namespace GASS
{
	class CEGUISystem : public Reflection<CEGUISystem, SimSystem>, public IMouseListener, public IKeyListener
	{
	public:
		CEGUISystem();
		virtual ~CEGUISystem();
		static void RegisterReflection();
		virtual void OnCreate();
	protected:
		void OnInit(InitMessagePtr  message);
		void OnShutdown(MessagePtr message);
		void OnLoadGUIScript(GUIScriptMessagePtr message);
		bool MouseMoved(float x,float y, float z);
		bool MousePressed( int id );
		bool MouseReleased( int id );
		bool KeyPressed(int key, unsigned int text);
		bool KeyReleased( int key, unsigned int text);
		CEGUI::MouseButton ConvertOISButtonToCegui(int buttonID);
	};
	typedef boost::shared_ptr<CEGUISystem> CEGUISystemPtr;
}

#endif