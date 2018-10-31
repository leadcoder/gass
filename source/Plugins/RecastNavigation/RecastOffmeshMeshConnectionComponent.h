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
	class RecastOffmeshMeshConnectionComponent : public Reflection<RecastOffmeshMeshConnectionComponent,BaseSceneComponent>
	{
	public:
		RecastOffmeshMeshConnectionComponent();
		~RecastOffmeshMeshConnectionComponent() override;
		static void RegisterReflection();
		void OnInitialize() override;
		Vec3 GetStartPos() const {return m_StartPos;}
		Vec3 GetEndPos() const {return m_EndPos;}
		ADD_PROPERTY(std::string,Mode)
		float GetRadius() const;
	protected:
		void OnStartNodeTransformation(TransformationChangedEventPtr message);
		void OnEndNodeTransformation(TransformationChangedEventPtr message);
		void UpdateConnectionLine();
		void SetVisible(bool value);
		bool GetVisible() const;
		void SetRadius(float value);
		
		GraphicsMeshPtr m_ConnectionLine;
		bool m_Visible;
		bool m_Initialized;
		float m_Radius;
		Vec3 m_StartPos;
		Vec3 m_EndPos;
		SceneObjectWeakPtr m_EndNode;
	};
	typedef GASS_SHARED_PTR<RecastOffmeshMeshConnectionComponent> RecastOffmeshMeshConnectionComponentPtr;
	typedef GASS_WEAK_PTR<RecastOffmeshMeshConnectionComponent> RecastOffmeshMeshConnectionComponentWeakPtr;
}

