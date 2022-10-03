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

#pragma once

#include "Sim/GASSCommon.h"
//#include "AutoRPC.h"
#include "ReplicaManager3.h"
#include "PacketPriority.h"
#include "StringTable.h"
#include "BitStream.h"
#include "GetTime.h"
#include "Core/Utils/GASSLogManager.h"
#include "Core/Math/GASSVector.h"
#include "Core/Math/GASSQuaternion.h"

class ReplicaManager;


namespace GASS
{
	class SceneObject;
	typedef SPTR<SceneObject> SceneObjectPtr;

	class RakNetBaseReplica  : public RakNet::Replica3
	{
	public:
		RakNetBaseReplica();
		virtual ~RakNetBaseReplica();

		void SetOwner(SceneObjectPtr object) {m_Owner = object;}

		//Remote calls
		//int AUTO_RPC_CALLSPEC EnterVehicle(const char *client_address, RakNet::AutoRPC* networkCaller);
		//int AUTO_RPC_CALLSPEC ExitVehicle(const char *client_address, RakNet::AutoRPC* networkCaller);
		
		//R4 int AUTO_RPC_CALLSPEC RemoteMessage(const char *client_address, const char *message, RakNet::AutoRPC* networkCaller);
		//R4 int AUTO_RPC_CALLSPEC RemoteMessageWithData(const char *message, const char *data, RakNet::AutoRPC* networkCaller);
		//R4 int AUTO_RPC_CALLSPEC RemoteInput(SystemAddress input_source, int controller, float value, RakNet::AutoRPC* networkCaller); 
		
		void ProcessMessages();
	protected:
		
		SceneObjectPtr m_Owner;
		std::vector<std::string> m_MessageBuffer;
	};
}
