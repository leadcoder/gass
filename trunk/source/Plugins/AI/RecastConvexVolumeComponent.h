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
#include "AIMessages.h"
#include "Detour/DetourNavMesh.h"

namespace GASS
{
	
	class RecastConvexVolumeComponent : public Reflection<RecastConvexVolumeComponent,BaseSceneComponent>
	{
	public:
		RecastConvexVolumeComponent();
		virtual ~RecastConvexVolumeComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
		ADD_PROPERTY(LandCoverTypeBinder,LandCoverType)
	protected:
		void OnTransformation(TransformationNotifyMessagePtr message);
		void OnLoad(LocationLoadedMessagePtr message);
		bool m_Initialized;
	};
	typedef SPTR<RecastConvexVolumeComponent> RecastConvexVolumeComponentPtr;
	typedef WPTR<RecastConvexVolumeComponent> RecastConvexVolumeComponentWeakPtr;
}

