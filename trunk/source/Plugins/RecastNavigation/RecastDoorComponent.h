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
#include "RecastNavigationMeshComponent.h"
#include "RecastNavigationMessages.h"
#include "DetourNavMesh.h"

namespace GASS
{
	#define MAX_REF_POLYS 256
	class RecastDoorComponent : public Reflection<RecastDoorComponent,BaseSceneComponent>
	{
	public:
		RecastDoorComponent();
		virtual ~RecastDoorComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
	protected:
		void OnTransformation(TransformationChangedEventPtr message);
		void OnLoad(LocationLoadedEventPtr message);
		void OnDoorMessage(DoorMessagePtr message);
		void UpdatePolyState(bool value);
		void SetOpen(bool value);
		bool GetOpen() const;
	
		bool m_Initialized;
		bool m_Open;
		Vec3 m_Pos;
		RecastNavigationMeshComponentWeakPtr m_NavMeshComp;
		dtPolyRef m_PolyRefs[MAX_REF_POLYS];
		int m_PolyRefCount;
	};
	typedef SPTR<RecastDoorComponent> RecastDoorComponentPtr;
	typedef WPTR<RecastDoorComponent> RecastDoorComponentWeakPtr;
}

