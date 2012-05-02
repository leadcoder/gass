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

#ifndef RAK_NET_MESSAGE_TRANSFER_COMPONENT_H
#define RAK_NET_MESSAGE_TRANSFER_COMPONENT_H

#include "Sim/Components/Graphics/Geometry/GASSIGeometryComponent.h"
#include "Sim/Components/GASSBaseSceneComponent.h"
#include "Sim/Components/Network/GASSINetworkComponent.h"
#include "Sim/Scene/GASSCoreSceneObjectMessages.h"
#include "Sim/Scene/GASSNetworkSceneObjectMessages.h"
#include "Sim/Systems/Input/GASSControlSetting.h"
#include "Sim/Scheduling/GASSITaskListener.h"

#include "Sim/GASSCommon.h"
#include "Plugins/RakNet/RakNetMessages.h"
#include "Plugins/RakNet/RakNetPackageFactory.h"
#include "Plugins/Game/GameMessages.h"


namespace GASS
{
	class RakNetMessageTransferComponent : public Reflection<RakNetMessageTransferComponent,BaseSceneComponent>, public INetworkComponent
	{
	public:
		RakNetMessageTransferComponent();
		virtual ~RakNetMessageTransferComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
		void Called(const std::string &message, const std::string &data);
		virtual bool IsRemote() const;
	private:
		void OnLoad(LoadComponentsMessagePtr message);
		void OnUnload(UnloadComponentsMessagePtr message);
		void OnDeserialize(NetworkDeserializeMessagePtr message);
		void OnInput(ControllerMessagePtr message);
		void OnClientRemoteMessage(ClientRemoteMessagePtr message);
		void OnOutOfArmor(OutOfArmorMessagePtr message);
		void Call(const std::string &message, const std::string &data);
	};
	typedef boost::shared_ptr<RakNetMessageTransferComponent> RakNetMessageTransferComponentPtr;
}
#endif
