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

#include "RecastConvexVolumeComponent.h"

#include <memory>
#include "Core/Serialize/tinyxml2.h"

namespace GASS
{
	RecastConvexVolumeComponent::RecastConvexVolumeComponent() :  
		m_LandCoverType(LAND_COVER_DEFAULT)
	{

	}

	RecastConvexVolumeComponent::~RecastConvexVolumeComponent()
	{
		
	}

	void RecastConvexVolumeComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register<RecastConvexVolumeComponent>();
		GetClassRTTI()->SetMetaData(std::make_shared<ClassMetaData>("RecastConvexVolumeComponent", OF_VISIBLE));
		RegisterMember("LandCoverType", &RecastConvexVolumeComponent::m_LandCoverType, PF_VISIBLE, "Land Cover Type");
	}

	/*void RecastConvexVolumeComponent::SetAreaType(const std::string value)
	{
		m_Area = GetAreaFromName(value);
	}

	std::string RecastConvexVolumeComponent::GetAreaType() const
	{
		return GetAreaName(m_Area);
	}*/

	void RecastConvexVolumeComponent::OnInitialize()
	{
	}
}
