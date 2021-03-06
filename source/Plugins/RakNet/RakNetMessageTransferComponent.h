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

#ifndef RAK_NET_MESSAGE_TRANSFER_COMPONENT_H
#define RAK_NET_MESSAGE_TRANSFER_COMPONENT_H

#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/Interface/GASSINetworkComponent.h"
#include "Sim/GASSCommon.h"
#include "Sim/Messages/GASSWeaponMessages.h"
#include "RakNetCommon.h"

namespace GASS
{
	class RakNetMessageTransferComponent : public Reflection<RakNetMessageTransferComponent,BaseSceneComponent>, public INetworkComponent
	{
	public:
		RakNetMessageTransferComponent();
		~RakNetMessageTransferComponent() override;
		static void RegisterReflection();
		void OnInitialize() override;
		void OnDelete() override;
		void Called(const std::string &message, const std::string &data);
		bool IsRemote() const override;
	private:
		void OnOutOfArmor(OutOfArmorMessagePtr message);
		void Call(const std::string &message, const std::string &data);
	};
	typedef GASS_SHARED_PTR<RakNetMessageTransferComponent> RakNetMessageTransferComponentPtr;
}
#endif
