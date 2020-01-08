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

//Raknet includes

#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4244)
#endif

#include "AutoRPC.h"
#include "Replica.h"
#include "StringTable.h"
#include "BitStream.h"
#include "PacketPriority.h"
#include "MessageIdentifiers.h"
#include "NetworkIDManager.h"
#include "ReplicaEnums.h"
#include "ReplicaManager.h"
#include "PacketLogger.h"
#include "RakPeerInterface.h"
#include "RakNetworkFactory.h"
#include "GetTime.h"
#include "AutoRPC.h"

#ifdef _MSC_VER
#pragma warning (pop)
#endif