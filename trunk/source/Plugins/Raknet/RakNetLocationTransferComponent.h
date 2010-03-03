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

#ifndef RAK_NET_LOCATION_TRANSFER_COMPONENT_H
#define RAK_NET_LOCATION_TRANSFER_COMPONENT_H

#include "Sim/Components/Graphics/Geometry/IGeometryComponent.h"
#include "Sim/Components/BaseSceneComponent.h"
#include "Sim/Scenario/Scene/SceneObjectMessages.h"
#include "Sim/Systems/Input/ControlSetting.h"
#include "Sim/Common.h"
#include "Plugins/Game/GameMessages.h"
#include "Plugins/RakNet/RakNetMessages.h"

namespace GASS
{
	enum
	{
		TRANSFORMATION_DATA = 0
	};
	class SceneObject;
	class RakNetBase;
	typedef boost::shared_ptr<SceneObject> SceneObjectPtr;
	typedef boost::weak_ptr<SceneObject> SceneObjectWeakPtr;

	class RakNetLocationTransferComponent : public Reflection<RakNetLocationTransferComponent,BaseSceneComponent>
	{
	public:
		RakNetLocationTransferComponent();
		virtual ~RakNetLocationTransferComponent();
		static void RegisterReflection();
		virtual void OnCreate();
	private:
		void OnLoad(LoadGameComponentsMessagePtr message);
		void OnUnload(UnloadComponentsMessagePtr message);
		void OnTransformationChanged(TransformationNotifyMessagePtr message);
		void OnSerialize(NetworkSerializeMessagePtr message);
	
	};
	typedef boost::shared_ptr<RakNetLocationTransferComponent> RakNetLocationTransferComponentPtr;
}
#endif
