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
#ifndef SPHERE_GEOMETRY_COMPONENT_H
#define SPHERE_GEOMETRY_COMPONENT_H

#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/Interface/GASSIShape.h"
#include "Sim/GASSCommon.h"
#include "Core/Utils/GASSColorRGB.h"
#include "Core/MessageSystem/GASSIMessage.h"

namespace GASS
{
	/**
		Component that create sphere geometry
	*/

	class SphereGeometryComponent : public Reflection<SphereGeometryComponent,BaseSceneComponent> , public IShape
	{
	public:
		SphereGeometryComponent(void);
		~SphereGeometryComponent(void);
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual void OnDelete();

		virtual bool IsPointInside(const Vec3 &point) const;
		virtual Vec3 GetRandomPoint() const;
		Float GetRadius() const;
	protected:
		ADD_PROPERTY(bool, Wireframe);
		void OnChangeName(MessagePtr message);
		void SetRadius(Float value);
		void UpdateMesh();
		void SetColor(const ColorRGBA  &value);
		ColorRGBA GetColor() const {return m_Color;}
	private:
		Float m_Radius;
		ColorRGBA m_Color;
	};
}
#endif
