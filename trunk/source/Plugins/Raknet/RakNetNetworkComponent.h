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

#ifndef RAK_NET_NETWORK_COMPONENT_H
#define RAK_NET_NETWORK_COMPONENT_H

#include "Sim/Components/Graphics/Geometry/IGeometryComponent.h"
#include "Sim/Components/BaseSceneComponent.h"
#include "Sim/Scenario/Scene/SceneObjectMessages.h"
#include "Sim/Systems/Input/ControlSetting.h"
#include "Sim/Common.h"
#include "Plugins/Game/GameMessages.h"
#include "Plugins/RakNet/RakNetMessages.h"

namespace GASS
{
	class SceneObject;
	class RakNetBase;
	typedef boost::shared_ptr<SceneObject> SceneObjectPtr;
	typedef boost::weak_ptr<SceneObject> SceneObjectWeakPtr;

	class RakNetNetworkComponent : public Reflection<RakNetNetworkComponent,BaseSceneComponent>
	{
	public:
		RakNetNetworkComponent();
		virtual ~RakNetNetworkComponent();
		static void RegisterReflection();
		virtual void OnCreate();
		void OnLoad(LoadGameComponentsMessagePtr message);
		void OnUnload(UnloadComponentsMessagePtr message);
		RakNetBase* GetReplica() const {return m_Replica;}
		void SetReplica(RakNetBase* replica) {m_Replica=replica;}
		void SetPartId(int id) {m_PartId = id;}
		int GetPartId() const {return m_PartId;}
	private:
		void OnNewReplica(ReplicaCreatedMessagePtr message);
		RakNetBase* m_Replica;
		int m_PartId;
	};

	typedef boost::shared_ptr<RakNetNetworkComponent> RakNetNetworkComponentPtr;
}
#endif
