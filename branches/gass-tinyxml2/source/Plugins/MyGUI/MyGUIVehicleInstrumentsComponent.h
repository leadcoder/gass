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

#include "Sim/GASSCommon.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSScriptSystemMessages.h"
#include "MyGUIMessages.h"
#include <osg/ref_ptr>


namespace GASS
{
	class MyGUIVehicleInstrumentsComponent : public Reflection<MyGUIVehicleInstrumentsComponent,BaseSceneComponent>
	{
	public:
		MyGUIVehicleInstrumentsComponent(void);
		~MyGUIVehicleInstrumentsComponent(void);
		static void RegisterReflection();
		virtual void OnInitialize();
	protected:
		void OnLayoutLoaded(LayoutLoadedEventPtr message);
		};
	typedef SPTR<MyGUIVehicleInstrumentsComponent> MyGUIVehicleInstrumentsComponentPtr;
}

