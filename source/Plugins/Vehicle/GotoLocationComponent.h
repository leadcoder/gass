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
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSPhysicsSceneObjectMessages.h"
#include "Sim/Messages/GASSCoreSceneObjectMessages.h"
#include "Sim/GASSSceneObjectRef.h"

#include "Sim/Messages/GASSPlatformMessages.h"
#include "Plugins/Base/CoreMessages.h"


namespace GASS
{
	class GoToLocationComponent :  public Reflection<GoToLocationComponent,BaseSceneComponent>
	{
	public:
		GoToLocationComponent();
		virtual ~GoToLocationComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
		void SceneManagerTick(double delta);
		std::vector<SceneObjectPtr>  GetNavigationEnumeration() const;
	private:
		void OnTransformationMessage(TransformationChangedEventPtr message);
		void OnPathfindToLocation(PathfindToPositionMessagePtr message);
		ADD_PROPERTY(SceneObjectRef,NavigationObject);
		Vec3 m_CurrentLocation;
		Vec3 m_DestinationLocation;
		std::vector<Vec3> m_Path;
		
	};
}
#endif
