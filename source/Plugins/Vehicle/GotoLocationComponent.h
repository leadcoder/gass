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

#ifndef GO_TO_LOCATION_COMPONENT_H
#define GO_TO_LOCATION_COMPONENT_H

#include "Sim/GASSCommon.h"
#include "Sim/Interface/GASSITerrainComponent.h"
#include "Sim/Interface/GASSIGeometryComponent.h"
#include "Sim/GASSComponent.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/GASSSceneObjectRef.h"
#include "Sim/Messages/GASSPlatformMessages.h"

namespace GASS
{
	class GoToLocationComponent :  public Reflection<GoToLocationComponent,Component>
	{
	public:
		GoToLocationComponent();
		~GoToLocationComponent() override;
		static void RegisterReflection();
		void OnInitialize() override;
		void SceneManagerTick(double delta) override;
		std::vector<SceneObjectRef>  GetNavigationEnumeration() const;
	private:
		void OnTransformationMessage(TransformationChangedEventPtr message);
		void OnPathfindToLocation(PathfindToPositionMessagePtr message);
		SceneObjectRef m_NavigationObject;
		Vec3 m_CurrentLocation;
		Vec3 m_DestinationLocation;
		std::vector<Vec3> m_Path;
		
	};
}
#endif
