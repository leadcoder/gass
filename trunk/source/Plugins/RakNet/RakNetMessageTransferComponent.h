/****************************************************************************
* This file is part of GASS.                                                *
* See http://code.google.com/p/gass/                                        *
*                                                                           *
* Copyright (c) 2008-2015 GASS team. See Contributors.txt for details.      *
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

#include "Sim/Interface/GASSIGeometryComponent.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/Interface/GASSINetworkComponent.h"
#include "Sim/Messages/GASSCoreSceneObjectMessages.h"
#include "Sim/Messages/GASSNetworkSceneObjectMessages.h"
#include "Sim/Interface/GASSIControlSettingsSystem.h"


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
		virtual void OnDelete();
		void Called(const std::string &message, const std::string &data);
		virtual bool IsRemote() const;
	private:
		void OnDeserialize(NetworkDeserializeRequestPtr message);
		void OnInput(InputRelayEventPtr message);
		void OnClientRemoteMessage(ClientRemoteMessagePtr message);
		void OnOutOfArmor(OutOfArmorMessagePtr message);
		void Call(const std::string &message, const std::string &data);
	};
	typedef SPTR<RakNetMessageTransferComponent> RakNetMessageTransferComponentPtr;
}
#endif
