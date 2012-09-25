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


#if !defined PHYSX_COMMON_H
#define PHYSX_COMMON_H
//#define  _ITERATOR_DEBUG_LEVEL 0

#include <PxPhysicsAPI.h> 
#include <PxPhysics.h>
#include <PxRigidActor.h>
#include <foundation/PxErrorCallback.h>
#include <extensions/PxExtensionsAPI.h>
#include <extensions/PxDefaultErrorCallback.h>
#include <extensions/PxDefaultAllocator.h>
#include <extensions/PxShapeExt.h>
#include <extensions/PxSimpleFactory.h>
#include <extensions/PxDefaultSimulationFilterShader.h>
#include <extensions/PxJoint.h>
#include <extensions/PxDistanceJoint.h>
#include <extensions/PxSphericalJoint.h>

#include <geometry/PxPlaneGeometry.h>
#include <geometry/PxSphereGeometry.h>
#include <geometry/PxBoxGeometry.h>
#include <geometry/PxCapsuleGeometry.h>
#include <geometry/PxConvexMeshGeometry.h>
#include <geometry/PxTriangleMeshGeometry.h>

#include "Core/System/GASSSystemFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/Utils/GASSException.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/Scene/GASSBaseSceneManager.h"
#include "Sim/Scene/GASSCoreSceneObjectMessages.h"
#include "Sim/Scene/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Scene/GASSPhysicsSceneObjectMessages.h"
#include "Sim/Scene/GASSPhysicsSceneMessages.h"
#include "Sim/Scene/GASSCoreSceneMessages.h"
#include "Sim/Scene/GASSGraphicsSceneMessages.h"
#include "Sim/Scene/GASSSceneManagerFactory.h"
#include "Sim/Components/GASSBaseSceneComponent.h"
#include "Sim/Components/Graphics/GASSILocationComponent.h"
#include "Sim/Components/Graphics/Geometry/GASSIGeometryComponent.h"
#include "Sim/Components/Graphics/Geometry/GASSIMeshComponent.h"
#include "Sim/Components/Graphics/Geometry/GASSITerrainComponent.h"
#include "Sim/Components/Graphics/Geometry/GASSIShape.h"
#include "Sim/Components/Physics/GASSIPhysicsGeometryComponent.h"
#include "Sim/Systems/GASSSimSystem.h"
#include "Sim/Systems/GASSSimSystemManager.h"
#include "Sim/Systems/Messages/GASSCoreSystemMessages.h"
#include "Sim/GASSCommon.h"
#include "Plugins/PhysX3/PhysXConvert.h"

#endif 
