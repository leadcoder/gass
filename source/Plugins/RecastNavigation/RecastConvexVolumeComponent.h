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
#include "RecastNavigationMeshComponent.h"

namespace GASS
{
	
	class RecastConvexVolumeComponent : public Reflection<RecastConvexVolumeComponent,BaseSceneComponent>
	{
	public:
		RecastConvexVolumeComponent();
		~RecastConvexVolumeComponent() override;
		static void RegisterReflection();
		void OnInitialize() override;
		LandCoverTypeBinder GetLandCoverType() const { return m_LandCoverType; }
	protected:
		bool m_Initialized{false};
		LandCoverTypeBinder m_LandCoverType;
	};
	using RecastConvexVolumeComponentPtr = std::shared_ptr<RecastConvexVolumeComponent>;
	using RecastConvexVolumeComponentWeakPtr = std::weak_ptr<RecastConvexVolumeComponent>;
}

