/****************************************************************************
* This file is part of GASS.                                                *
* See http://code.google.com/p/gass/                                        *
*                                                                           *
* Copyright (c) 2008-2015 GASS team. See Contributors.txt for details.      *
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
#ifndef PLANE_GEOMETRY_COMPONENT_H
#define PLANE_GEOMETRY_COMPONENT_H

#include "Sim/GASSCommon.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Core/MessageSystem/GASSIMessage.h"

namespace GASS
{
	/**
		Component that render plane geometry with texture/material
	*/

	class PlaneGeometryComponent : public Reflection<PlaneGeometryComponent,BaseSceneComponent>
	{
	public:
		PlaneGeometryComponent(void);
		~PlaneGeometryComponent(void);
		static void RegisterReflection();
		virtual void OnInitialize();
		void SetTexture(const std::string &texture_name);
		std::string GetTexture() const;
	protected:
		void GenerateMesh();
		void OnLoad(MessagePtr message);
		void OnChangeName(MessagePtr message);
		Vec2 GetSize() const;
		void SetSize(const Vec2 &value);
		float GetTransparency() const;
		void SetTransparency(float value);

		Vec2 m_Size;
		std::string  m_Texture;
		float m_Transparency;
	private:
	};
}
#endif
