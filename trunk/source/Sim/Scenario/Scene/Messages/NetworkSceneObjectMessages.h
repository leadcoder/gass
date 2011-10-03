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

#include "Sim/Common.h"
#include "Core/MessageSystem/BaseMessage.h"
#include "Core/MessageSystem/IMessage.h"
#include "Core/Math/Vector.h"
#include "Core/Math/Quaternion.h"

namespace GASS
{
	class SimSceneManager;
	class ISceneManager;
	class IGeometryComponent;
	struct ManualMeshData;
	typedef boost::shared_ptr<ManualMeshData> ManualMeshDataPtr;
	typedef boost::shared_ptr<ISceneManager> SceneManagerPtr;
	typedef boost::shared_ptr<IGeometryComponent> GeometryComponentPtr;


	class LoadNetworkComponentsMessage : public BaseMessage
	{
	public:
		LoadNetworkComponentsMessage(SceneManagerPtr network_scene_manager, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage( sender_id , delay), m_NetworkSceneManager(network_scene_manager){}
		  SceneManagerPtr GetNetworkSceneManager() const {return m_NetworkSceneManager;}
	private:
		SceneManagerPtr m_NetworkSceneManager;
	};
	typedef boost::shared_ptr<LoadNetworkComponentsMessage> LoadNetworkComponentsMessagePtr;


	//typedef boost::shared_ptr<char> NetworkDataPtr;
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
	typedef boost::shared_ptr<NetworkPackage> NetworkPackagePtr;

	class NetworkAddress
	{
	public:
		NetworkAddress(unsigned int address = 0, unsigned int port = 0) : m_Address(address),m_Port(port) {}
		unsigned int m_Address;
		unsigned int m_Port;
	};

	class NetworkSerializeMessage : public BaseMessage
	{

	public:
		NetworkSerializeMessage(const NetworkAddress &address, unsigned int time_stamp, NetworkPackagePtr package, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage( sender_id , delay),
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
	typedef boost::shared_ptr<NetworkSerializeMessage> NetworkSerializeMessagePtr;



	class NetworkDeserializeMessage : public BaseMessage
	{
	public:
		NetworkDeserializeMessage(const NetworkAddress &address, unsigned int time_stamp, NetworkPackagePtr package, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage( sender_id , delay),
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
	typedef boost::shared_ptr<NetworkDeserializeMessage> NetworkDeserializeMessagePtr;
	
}
