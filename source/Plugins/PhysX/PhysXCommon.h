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


#if !defined PHYSX_COMMON_H
#define PHYSX_COMMON_H

#include "Sim/GASSCommon.h"


#ifdef _MSC_VER
	#pragma warning (push)
	#pragma warning (disable : 4244)
#endif

#include <PxScene.h>
#include <PxPhysicsAPI.h> 
#include <PxPhysics.h>
#include <PxRigidActor.h>
#include <PxBatchQueryDesc.h>


#include <foundation/PxErrorCallback.h>
#include <foundation/PxPreprocessor.h>
#include <foundation/PxAssert.h>
#include <foundation/PxMath.h>
#include <foundation/PxIO.h>

#include <common/PxPhysXCommonConfig.h>

#include <vehicle/PxVehicleSDK.h>
#include <vehicle/PxVehicleUpdate.h>
#include <vehicle/PxVehicleSDK.h>

#include <cooking/PxCooking.h>

#include <extensions/PxExtensionsAPI.h>
#include <extensions/PxDefaultErrorCallback.h>
#include <extensions/PxDefaultAllocator.h>
#include <extensions/PxShapeExt.h>
#include <extensions/PxSimpleFactory.h>
#include <extensions/PxDefaultSimulationFilterShader.h>
#include <extensions/PxJoint.h>
#include <extensions/PxDistanceJoint.h>
#include <extensions/PxSphericalJoint.h>
#include <extensions/PxRigidActorExt.h>
#include <geometry/PxPlaneGeometry.h>
#include <geometry/PxSphereGeometry.h>
#include <geometry/PxBoxGeometry.h>
#include <geometry/PxCapsuleGeometry.h>
#include <geometry/PxConvexMeshGeometry.h>
#include <geometry/PxTriangleMeshGeometry.h>

#include <characterkinematic/PxCapsuleController.h>
#include <characterkinematic/PxControllerManager.h>


#ifdef _MSC_VER
	#pragma warning (pop)
#endif

#include "Sim/GASSSystemFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/GASSComponentFactory.h"
#include "Core/Utils/GASSException.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSBaseSceneManager.h"
#include "Sim/GASSPhysicsMesh.h"
#include "Sim/Messages/GASSCoreSceneObjectMessages.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSPhysicsSceneObjectMessages.h"
#include "Sim/Messages/GASSPhysicsSceneMessages.h"
#include "Sim/Messages/GASSCoreSceneMessages.h"
#include "Sim/Messages/GASSGraphicsSceneMessages.h"
#include "Sim/GASSSceneManagerFactory.h"
#include "Sim/GASSComponent.h"
#include "Sim/Interface/GASSILocationComponent.h"
#include "Sim/Interface/GASSIGeometryComponent.h"
#include "Sim/Interface/GASSIMeshComponent.h"
#include "Sim/Interface/GASSITerrainComponent.h"
#include "Sim/Interface/GASSIShape.h"
#include "Sim/Interface/GASSIPhysicsGeometryComponent.h"
#include "Sim/GASSSimSystem.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/Messages/GASSCoreSystemMessages.h"
#include "Plugins/PhysX/PhysXConvert.h"

#endif 
