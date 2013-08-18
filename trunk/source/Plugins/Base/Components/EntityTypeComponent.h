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
#include "Core/Common.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Math/GASSVector.h"
#include "Sim/Interface/GASSICameraComponent.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/GASSSceneObject.h"
#include "Plugins/Base/CoreMessages.h"


namespace GASS
{
	class EntityTypeComponent;
	typedef WPTR<EntityTypeComponent> EntityTypeComponentWeakPtr;
	class EntityTypeComponent : public Reflection<EntityTypeComponent,BaseSceneComponent>
	{
	public:
		EntityTypeComponent();
		virtual ~EntityTypeComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
		
		ADD_ATTRIBUTE(int,EntityKind)
		ADD_ATTRIBUTE(int,Domain)
		ADD_ATTRIBUTE(int,CountryCode)
		ADD_ATTRIBUTE(int,Category)
		ADD_ATTRIBUTE(int,Subcategory)
		ADD_ATTRIBUTE(int,Specific)
		ADD_ATTRIBUTE(int,Extra)
	
	};

	typedef SPTR<EntityTypeComponent> EntityTypeComponentPtr;
	
}

