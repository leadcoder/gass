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

#include "Sim/GASSCommon.h"

//common Core includes


#include "Core/Math/GASSVector.h"
#include "Core/Math/GASSQuaternion.h"
#include "Core/Math/GASSMatrix.h"
#include "Core/Serialize/GASSSerialize.h"
#include "Core/Utils/GASSTimer.h"
#include "Core/Utils/GASSLogManager.h"
#include "Core/Utils/GASSFactory.h"
#include "Core/Utils/GASSFilePath.h"
#include "Core/Utils/GASSException.h"
#include "Core/ComponentSystem/GASSIComponent.h"
#include "Core/ComponentSystem/GASSComponentContainerFactory.h"
#include "Core/ComponentSystem/GASSComponentContainerTemplateFactory.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/ComponentSystem/GASSBaseComponent.h"
#include "Core/ComponentSystem/GASSBaseComponentContainer.h"
#include "Core/ComponentSystem/GASSBaseComponentContainerTemplate.h"
#include "Core/ComponentSystem/GASSBaseComponentContainerTemplateManager.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSStaticMessageListener.h"
#include "Core/System/GASSSystemFactory.h"
#include "Core/Reflection/GASSIProperty.h"
#include "Core/Reflection/GASSProperty.h"


//#include "Core/PluginSystem/GASSPluginManager.h"

//common Sim includes
#include "Sim/GASSSimEngine.h"

#include "Sim/Components/Graphics/GASSILocationComponent.h"
#include "Sim/Components/Graphics/GASSICameraComponent.h"
#include "Sim/Components/Graphics/GASSILightComponent.h"

#include "Sim/Components/GASSBaseSceneComponent.h"


#include "Sim/Components/Graphics/Geometry/GASSIGeometryComponent.h"
#include "Sim/Components/Graphics/Geometry/GASSIMeshComponent.h"
#include "Sim/Components/Graphics/Geometry/GASSITerrainComponent.h"
#include "Sim/Components/Graphics/Geometry/GASSIShape.h"

#include "Sim/Systems/GASSSimSystem.h"
#include "Sim/Systems/Graphics/GASSIGraphicsSystem.h"
#include "Sim/Systems/Input/GASSIInputSystem.h"
#include "Sim/Systems/Collision/GASSICollisionSystem.h"
#include "Sim/Systems/Resource/GASSIResourceSystem.h"
#include "Sim/Systems/GASSIPhysicsSystem.h"

#include "Sim/Systems/GASSSimSystemManager.h"
#include "Sim/Systems/Messages/GASSCoreSystemMessages.h"
#include "Sim/Systems/Messages/GASSGraphicsSystemMessages.h"
#include "Sim/Systems/Messages/GASSNetworkSystemMessages.h"


#include "Sim/Scheduling/GASSIRuntimeController.h"

#include "Sim/Scene/GASSScene.h"
#include "Sim/Scene/GASSSceneObject.h"
#include "Sim/Scene/GASSSceneObjectTemplate.h"
#include "Sim/Scene/GASSSceneObjectLink.h"


#include "Sim/Scene/GASSBaseSceneManager.h"
#include "Sim/Scene/GASSSceneManagerFactory.h"

#include "Sim/Scene/GASSCoreSceneObjectMessages.h"
#include "Sim/Scene/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Scene/GASSNetworkSceneObjectMessages.h"
#include "Sim/Scene/GASSPhysicsSceneObjectMessages.h"
#include "Sim/Scene/GASSPhysicsSceneMessages.h"
#include "Sim/Scene/GASSCoreSceneMessages.h"
#include "Sim/Scene/GASSGraphicsSceneMessages.h"

#include "Sim/Utils/GASSEnumBinder.h"







