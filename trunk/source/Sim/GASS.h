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

#include "Sim/Common.h"

//common Core includes


#include "Core/Math/Vector.h"
#include "Core/Math/Quaternion.h"
#include "Core/Math/Matrix.h"
#include "Core/Serialize/Serialize.h"
#include "Core/Utils/Timer.h"
#include "Core/Utils/Log.h"
#include "Core/Utils/Factory.h"
#include "Core/Utils/FilePath.h"
#include "Core/ComponentSystem/IComponent.h"
#include "Core/ComponentSystem/ComponentContainerFactory.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/ComponentSystem/BaseComponent.h"
#include "Core/ComponentSystem/BaseComponentContainer.h"
#include "Core/ComponentSystem/BaseComponentContainerTemplate.h"
#include "Core/ComponentSystem/BaseComponentContainerTemplateManager.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/StaticMessageListener.h"
#include "Core/System/SystemFactory.h"

//#include "Core/PluginSystem/PluginManager.h"

//common Sim includes
#include "Sim/SimEngine.h"

#include "Sim/Components/Graphics/ILocationComponent.h"
#include "Sim/Components/Graphics/ICameraComponent.h"
#include "Sim/Components/Graphics/ILightComponent.h"
#include "Sim/Components/BaseSceneComponent.h"


#include "Sim/Components/Graphics/Geometry/IGeometryComponent.h"
#include "Sim/Components/Graphics/Geometry/IMeshComponent.h"
#include "Sim/Components/Graphics/Geometry/ITerrainComponent.h"

#include "Sim/Systems/SimSystem.h"
#include "Sim/Systems/Graphics/IGraphicsSystem.h"
#include "Sim/Systems/Input/IInputSystem.h"
#include "Sim/Systems/Collision/ICollisionSystem.h"

#include "Sim/Systems/SimSystemManager.h"
#include "Sim/Systems/Messages/CoreSystemMessages.h"
#include "Sim/Systems/Messages/GraphicsSystemMessages.h"
#include "Sim/Systems/Messages/NetworkSystemMessages.h"


#include "Sim/Scheduling/IRuntimeController.h"

#include "Sim/Scenario/Scenario.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Scenario/Scene/SceneObjectManager.h"
#include "Sim/Scenario/Scene/BaseSceneManager.h"
#include "Sim/Scenario/Scene/SceneManagerFactory.h"

#include "Sim/Scenario/Scene/Messages/CoreSceneObjectMessages.h"
#include "Sim/Scenario/Scene/Messages/GraphicsSceneObjectMessages.h"
#include "Sim/Scenario/Scene/Messages/NetworkSceneObjectMessages.h"
#include "Sim/Scenario/Scene/Messages/PhysicsSceneObjectMessages.h"
#include "Sim/Scenario/Scene/Messages/PhysicsScenarioSceneMessages.h"


#include "Sim/Scenario/Scene/Messages/CoreScenarioSceneMessages.h"
#include "Sim/Scenario/Scene/Messages/GraphicsScenarioSceneMessages.h"







