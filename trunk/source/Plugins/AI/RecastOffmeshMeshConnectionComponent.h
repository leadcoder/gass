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
#include "Sim/GASS.h"
#include "Plugins/Base/CoreMessages.h"
#include "AIMessages.h"

namespace GASS
{
	class RecastOffmeshMeshConnectionComponent : public Reflection<RecastOffmeshMeshConnectionComponent,BaseSceneComponent>
	{
	public:
		RecastOffmeshMeshConnectionComponent();
		virtual ~RecastOffmeshMeshConnectionComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
		Vec3 GetStartPos() const {return m_StartPos;}
		Vec3 GetEndPos() const {return m_EndPos;}
		ADD_ATTRIBUTE(std::string,Mode)
		float GetRadius() const;
	protected:
		void OnStartNodeTransformation(TransformationNotifyMessagePtr message);
		void OnEndNodeTransformation(TransformationNotifyMessagePtr message);
		void UpdateConnectionLine();
		void SetVisible(bool value);
		bool GetVisible() const;
		void SetRadius(float value);
		
		MeshDataPtr m_ConnectionLine;
		bool m_Visible;
		bool m_Initialized;
		float m_Radius;
		Vec3 m_StartPos;
		Vec3 m_EndPos;
		SceneObjectWeakPtr m_EndNode;
	};
	typedef SPTR<RecastOffmeshMeshConnectionComponent> RecastOffmeshMeshConnectionComponentPtr;
	typedef WPTR<RecastOffmeshMeshConnectionComponent> RecastOffmeshMeshConnectionComponentWeakPtr;
}

