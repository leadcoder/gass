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
#ifndef BOX_GEOMETRY_COMPONENT_H
#define BOX_GEOMETRY_COMPONENT_H

#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/Interface/GASSIShape.h"

#include "Sim/GASSCommon.h"
#include "Core/MessageSystem/GASSIMessage.h"

namespace GASS
{
	class BoxGeometryComponent : public Reflection<BoxGeometryComponent,BaseSceneComponent> , public IShape
	{
	public:
		BoxGeometryComponent(void);
		~BoxGeometryComponent(void);
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual bool IsPointInside(const Vec3 &point) const;
		Vec3 GetRandomPoint() const;
	protected:
		void OnLoad(MessagePtr message);
		void OnChangeName(MessagePtr message);
		void UpdateMesh();
		Vec3 GetSize() const;
		void SetSize(const Vec3 &value);
		Vec3 m_Size;
	private:
	};
}
#endif
