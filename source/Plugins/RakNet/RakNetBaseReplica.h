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

#pragma once

#include "Sim/GASSCommon.h"
#include "RakNetCommon.h"
#include "Core/Math/GASSVector.h"

class ReplicaManager;

namespace GASS
{
	class SceneObject;
	using SceneObjectPtr = std::shared_ptr<SceneObject>;

	class RakNetBaseReplica  : public Replica
	{
	public:
		RakNetBaseReplica();
		~RakNetBaseReplica() override;

		void SetOwner(SceneObjectPtr object) {m_Owner = object;}

		//Remote calls
		int AUTO_RPC_CALLSPEC RemoteMessage(const char *client_address, const char *message, RakNet::AutoRPC* networkCaller);
		int AUTO_RPC_CALLSPEC RemoteMessageWithData(const char *message, const char *data, RakNet::AutoRPC* networkCaller);
		int AUTO_RPC_CALLSPEC RemoteInput(SystemAddress input_source, int controller, float value, RakNet::AutoRPC* networkCaller); 
		
		void ProcessMessages();
	protected:
		
		SceneObjectPtr m_Owner;
		std::vector<std::string> m_MessageBuffer;
	};
}
