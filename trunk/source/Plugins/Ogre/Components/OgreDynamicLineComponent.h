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

#include "Sim/Common.h"
#include "Sim/Components/Graphics/Geometry/ILineComponent.h"
#include "Sim/Components/Graphics/Geometry/IGeometryComponent.h"
#include "Sim/Components/BaseSceneComponent.h"
#include "Core/MessageSystem/Message.h"


namespace GASS
{
	class DynamicLines;
	class OgreDynamicLineComponent : public Reflection<OgreDynamicLineComponent,BaseSceneComponent> , public ILineComponent , public IGeometryComponent
	{
	public:
		OgreDynamicLineComponent(void);
		~OgreDynamicLineComponent(void);
		static void RegisterReflection();
		virtual void OnCreate();
		virtual void Add(const std::vector<LineData> &line_seg);
		virtual void Clear();
		virtual AABox GetBoundingBox() const;
		virtual Sphere GetBoundingSphere() const;

		void OnLoad(MessagePtr message);
		void SetMaterialName(const std::string &name){m_MaterialName = name;}
		std::string GetMaterialName()const {return m_MaterialName;}
		void SetType(const std::string &name){m_Type = name;}
		std::string GetType()const {return m_Type;}
	protected:
		virtual void UpdateLineFromControlPoints();
		virtual void BuildLineFromControlPoints();
		std::string m_MaterialName;
		Vec2 m_TexScale;
		float m_HeightOffset;
		std::vector<Vec3> m_ControlPoints;
		std::string m_ControlPointList;
		std::string m_Type;
		DynamicLines* m_DynLines;
	};
}

