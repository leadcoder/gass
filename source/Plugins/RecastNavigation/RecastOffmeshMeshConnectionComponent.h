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
#include "Sim/GASS.h"
#include "Plugins/Base/CoreMessages.h"
#include "RecastNavigationMessages.h"

namespace GASS
{
	class RecastOffmeshMeshConnectionComponent : public Reflection<RecastOffmeshMeshConnectionComponent,Component>
	{
	public:
		RecastOffmeshMeshConnectionComponent();
		~RecastOffmeshMeshConnectionComponent() override;
		static void RegisterReflection();
		void OnInitialize() override;
		Vec3 GetStartPos() const {return m_StartPos;}
		Vec3 GetEndPos() const {return m_EndPos;}
		float GetRadius() const;
		std::string GetMode() const {return m_Mode;}
	protected:
		void OnStartNodeTransformation(TransformationChangedEventPtr message);
		void OnEndNodeTransformation(TransformationChangedEventPtr message);
		void UpdateConnectionLine();
		void SetVisible(bool value);
		bool GetVisible() const;
		void SetRadius(float value);
		
		GraphicsMeshPtr m_ConnectionLine;
		bool m_Visible{true};
		bool m_Initialized{false};
		float m_Radius{1.0};
		Vec3 m_StartPos;
		Vec3 m_EndPos;
		SceneObjectWeakPtr m_EndNode;
		std::string m_Mode;
	};
	using RecastOffmeshMeshConnectionComponentPtr = std::shared_ptr<RecastOffmeshMeshConnectionComponent>;
	using RecastOffmeshMeshConnectionComponentWeakPtr = std::weak_ptr<RecastOffmeshMeshConnectionComponent>;
}

