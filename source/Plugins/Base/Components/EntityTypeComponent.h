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

#pragma once

#include "Sim/GASSCommon.h"
#include "Core/Common.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Math/GASSVector.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/GASSSceneObject.h"

namespace GASS
{
	/**
		Component that hold DIS entity code
	*/

	class EntityTypeComponent;
	using EntityTypeComponentWeakPtr = std::weak_ptr<EntityTypeComponent>;
	class EntityTypeComponent : public Reflection<EntityTypeComponent,BaseSceneComponent>
	{
	public:
		EntityTypeComponent();
		~EntityTypeComponent() override;
		static void RegisterReflection();
		void OnInitialize() override;
		
		int m_EntityKind{0};
		int m_Domain{0};
		int m_CountryCode{0};
		int m_Category{0};
		int m_Subcategory{0};
		int m_Specific{0};
		int m_Extra{0};
	};

	using EntityTypeComponentPtr = std::shared_ptr<EntityTypeComponent>;
	
}

