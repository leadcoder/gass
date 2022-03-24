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

//common Core includes
#include "Core/Common.h"
#include "Core/Math/GASSVector.h"
#include "Core/Math/GASSMath.h"
#include "Core/Math/GASSQuaternion.h"
#include "Core/Math/GASSMatrix.h"
#include "Core/Serialize/GASSSerialize.h"
#include "Core/Utils/GASSTimer.h"
#include "Core/Utils/GASSFactory.h"
#include "Core/Utils/GASSFilePath.h"
#include "Core/Utils/GASSException.h"
#include "Core/Utils/GASSColorRGB.h"
#include "Core/Utils/GASSColorRGBA.h"
#include "Core/Utils/GASSStringUtils.h"
#include "Core/Utils/GASSFileUtils.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSStaticMessageListener.h"
#include "Sim/GASSSystemFactory.h"
#include "Core/Reflection/GASSIProperty.h"
#include "Core/Reflection/GASSProperty.h"

//common Sim includes
#include "Sim/GASSSimEngine.h"
#include "Sim/Interface/GASSILocationComponent.h"
#include "Sim/Interface/GASSICameraComponent.h"
#include "Sim/Interface/GASSILightComponent.h"
#include "Sim/Interface/GASSIGeometryComponent.h"
#include "Sim/Interface/GASSIMeshComponent.h"
#include "Sim/Interface/GASSIResourceComponent.h"
#include "Sim/Interface/GASSITerrainComponent.h"
#include "Sim/Interface/GASSIShape.h"
#include "Sim/Interface/GASSIGraphicsSystem.h"
#include "Sim/Interface/GASSIInputSystem.h"
#include "Sim/Interface/GASSICollisionSystem.h"
#include "Sim/Interface/GASSICollisionSceneManager.h"
#include "Sim/Interface/GASSIProjectionSceneManager.h"
#include "Sim/Interface/GASSIPhysicsSystem.h"
#include "Sim/Interface/GASSIControlSettingsSystem.h"
#include "Sim/Interface/GASSIWorldLocationComponent.h"

#include "Sim/Messages/GASSScriptSystemMessages.h"
#include "Sim/Messages/GASSCoreSystemMessages.h"
#include "Sim/Messages/GASSGraphicsSystemMessages.h"
#include "Sim/Messages/GASSNetworkSystemMessages.h"

#include "Sim/GASSSimSystem.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSComponentFactory.h"
#include "Sim/GASSComponent.h"
#include "Sim/GASSSceneObjectTemplate.h"
#include "Sim/GASSSceneObjectLink.h"
#include "Sim/GASSBaseSceneManager.h"
#include "Sim/GASSSceneManagerFactory.h"
#include "Sim/GASSResource.h"
#include "Sim/GASSResourceManager.h"
#include "Sim/GASSResourceGroup.h"
#include "Sim/GASSResourceHandle.h"
#include "Sim/GASSSceneObjectRef.h"

#include "Sim/Messages/GASSCoreSceneObjectMessages.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSNetworkSceneObjectMessages.h"
#include "Sim/Messages/GASSPhysicsSceneObjectMessages.h"
#include "Sim/Messages/GASSPhysicsSceneMessages.h"
#include "Sim/Messages/GASSCoreSceneMessages.h"
#include "Sim/Messages/GASSGraphicsSceneMessages.h"
#include "Core/Utils/GASSEnumBinder.h"







