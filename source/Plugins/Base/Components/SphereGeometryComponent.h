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

#include "Sim/GASSComponent.h"
#include "Sim/Interface/GASSIShape.h"
#include "Sim/GASSCommon.h"
#include "Core/Utils/GASSColorRGBA.h"
#include "Core/MessageSystem/GASSIMessage.h"

namespace GASS
{
	/**
		Component that create sphere geometry
	*/

	class SphereGeometryComponent : public Reflection<SphereGeometryComponent,Component> , public IShape
	{
	public:
		SphereGeometryComponent(void);
		~SphereGeometryComponent(void) override;
		static void RegisterReflection();
		void OnPostInitialize() override;
		void OnDelete() override;
		bool IsPointInside(const Vec3 &point) const override;
		Vec3 GetRandomPoint() const override;
		Float GetRadius() const;
	protected:
		void SetRadius(Float value);
		void UpdateMesh();
		void SetColor(const ColorRGBA  &value);
		ColorRGBA GetColor() const {return m_Color;}
	private:
		bool m_Wireframe{true};
		Float m_Radius{1};
		ColorRGBA m_Color;
	};
}

