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

#pragma once

#include "Sim/GASSCommon.h"
#include "Core/MessageSystem/GASSBaseMessage.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Math/GASSVector.h"
#include "Core/Math/GASSQuaternion.h"

namespace GASS
{
	class SimSceneManager;
	class ISceneManager;
	class IGeometryComponent;
	typedef SPTR<ISceneManager> SceneManagerPtr;
	typedef SPTR<IGeometryComponent> GeometryComponentPtr;



	//*********************************************************
	// ALL MESSAGES IN THIS SECTION CAN BE POSTED BY USER
	//*********************************************************

	//*********************************************************
	// ALL MESSAGES BELOW SHOULD ONLY BE POSTED GASS INTERNALS
	//*********************************************************

	class LoadNetworkComponentsMessage : public SceneObjectRequestMessage
	{
	public:
		LoadNetworkComponentsMessage(SceneManagerPtr network_scene_manager, SenderID sender_id = -1, double delay= 0) :
		  SceneObjectRequestMessage( sender_id , delay), m_NetworkSceneManager(network_scene_manager){}
		  SceneManagerPtr GetNetworkSceneManager() const {return m_NetworkSceneManager;}
	private:
		SceneManagerPtr m_NetworkSceneManager;
	};
	typedef SPTR<LoadNetworkComponentsMessage> LoadNetworkComponentsMessagePtr;

	
	

	//typedef SPTR<char> NetworkDataPtr;
	class NetworkPackage
	{
	public:
		NetworkPackage()
		{}
		NetworkPackage(int id) : Id(id)
		{}
		virtual ~NetworkPackage(){}
		virtual int GetSize() = 0;
		virtual void Assign(char* data) = 0;
		int Id;
		//NetworkDataPtr Data;
	};
	typedef SPTR<NetworkPackage> NetworkPackagePtr;

	class NetworkAddress
	{
	public:
		NetworkAddress(unsigned int address = 0, unsigned int port = 0) : m_Address(address),m_Port(port) {}
		unsigned int m_Address;
		unsigned int m_Port;
	};

	class NetworkSerializeMessage : public SceneObjectRequestMessage
	{

	public:
		NetworkSerializeMessage(const NetworkAddress &address, unsigned int time_stamp, NetworkPackagePtr package, SenderID sender_id = -1, double delay= 0) :
		  SceneObjectRequestMessage( sender_id , delay),
			  m_Package(package),
			  m_TimeStamp(time_stamp),
			  m_Address(address)
		  {
		  }
		  NetworkPackagePtr GetPackage() const {return m_Package;}
		  unsigned int GetTimeStamp() const {return m_TimeStamp;}
		  NetworkAddress GetAddress() const {return m_Address;}
	private:
		NetworkPackagePtr m_Package;
		unsigned int m_TimeStamp;
		NetworkAddress m_Address;

	};
	typedef SPTR<NetworkSerializeMessage> NetworkSerializeMessagePtr;



	class NetworkDeserializeMessage : public SceneObjectRequestMessage
	{
	public:
		NetworkDeserializeMessage(const NetworkAddress &address, unsigned int time_stamp, NetworkPackagePtr package, SenderID sender_id = -1, double delay= 0) :
		  SceneObjectRequestMessage( sender_id , delay),
			  m_Package(package),
			  m_TimeStamp(time_stamp),
			  m_Address(address)
		  {
		  }
		  NetworkPackagePtr GetPackage() const {return m_Package;}
		  unsigned int GetTimeStamp() const {return m_TimeStamp;}
		  NetworkAddress GetAddress() const {return m_Address;}
	private:
		NetworkPackagePtr m_Package;
		unsigned int m_TimeStamp;
		NetworkAddress m_Address;

	};
	typedef SPTR<NetworkDeserializeMessage> NetworkDeserializeMessagePtr;



	
}
