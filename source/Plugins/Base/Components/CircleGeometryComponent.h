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
#ifndef CIRCLE_GEOMETRY_COMPONENT_H
#define CIRCLE_GEOMETRY_COMPONENT_H

#include "Sim/GASSCommon.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/Interface/GASSIShape.h"

namespace GASS
{
	/**
		Component that create  circle geometry
	*/

	class CircleGeometryComponent : public Reflection<CircleGeometryComponent,BaseSceneComponent> , public IShape
	{
	public:
		CircleGeometryComponent(void);
		~CircleGeometryComponent(void) override;
		static void RegisterReflection();
		void OnInitialize() override;
		void OnDelete() override;
		bool IsPointInside(const Vec3 &point) const override;
		Vec3 GetRandomPoint() const override;
		Float GetRadius() const;
	protected:
		Vec3 m_Color;
		bool m_Dashed;
		void SetRadius(Float value);
		void UpdateMesh();
	private:
		Float m_Radius;
	};
}
#endif
