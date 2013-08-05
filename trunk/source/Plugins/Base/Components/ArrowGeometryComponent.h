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
#ifndef ARROW_GEOMETRY_COMPONENT_H
#define ARROW_GEOMETRY_COMPONENT_H

#include "Sim/GASSCommon.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/GASSResourceHandle.h"
#include "Core/MessageSystem/GASSIMessage.h"

namespace GASS
{
	class ArrowGeometryComponent : public Reflection<ArrowGeometryComponent,BaseSceneComponent>
	{
	public:
		ArrowGeometryComponent(void);
		~ArrowGeometryComponent(void);
		static void RegisterReflection();
		virtual void OnInitialize();
		void SetTexture(const ResourceHandle &texture_name);
		ResourceHandle GetTexture() const;
	protected:
		void GenerateMesh();
		void OnLoad(MessagePtr message);
		void OnChangeName(MessagePtr message);
		Vec2 GetSize() const;
		void SetSize(const Vec2 &value);
		Vec4 GetColor() const{return m_Color;}
		void SetColor(const Vec4 &value){m_Color =value;}
		Vec2 m_Size;
		ResourceHandle m_Texture;
		Vec4 m_Color;
	private:
	};
}
#endif
