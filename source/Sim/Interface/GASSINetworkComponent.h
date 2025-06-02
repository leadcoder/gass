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
#include "Sim/GASSComponent.h"

namespace GASS
{
	class NetworkPackage
	{
	public:
		NetworkPackage() : Id(0)
		{}
		NetworkPackage(int id) : Id(id)
		{}
		virtual ~NetworkPackage() {}
		virtual int GetSize() = 0;
		virtual void Assign(char* data) = 0;
		int Id;
		//NetworkDataPtr Data;
	};
	typedef GASS_SHARED_PTR<NetworkPackage> NetworkPackagePtr;

	class NetworkAddress
	{
	public:
		NetworkAddress(unsigned int address = 0, unsigned int port = 0) : m_Address(address), m_Port(port) {}
		unsigned int m_Address;
		unsigned int m_Port;
	};

	class INetworkComponent : public Reflection<INetworkComponent, Component>
	{
		GASS_DECLARE_CLASS_AS_INTERFACE(INetworkComponent)
	public:
		//indicates if this object is remote or local
		virtual bool IsRemote() const = 0;
		virtual void SerializeToNetwork(NetworkPackagePtr package, unsigned int timeStamp, NetworkAddress address) = 0;
		
	};

	typedef GASS_WEAK_PTR<INetworkComponent> NetworkComponentWeakPtr;
	typedef GASS_SHARED_PTR<INetworkComponent> NetworkComponentPtr;
}
