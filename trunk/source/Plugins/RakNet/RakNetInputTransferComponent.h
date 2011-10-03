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

#ifndef RAK_NET_INPUT_TRANSFER_COMPONENT_H
#define RAK_NET_INPUT_TRANSFER_COMPONENT_H

#include "Sim/Components/Graphics/Geometry/IGeometryComponent.h"
#include "Sim/Components/BaseSceneComponent.h"
#include "Sim/Scenario/Scene/Messages/CoreSceneObjectMessages.h"
#include "Sim/Scenario/Scene/Messages/NetworkSceneObjectMessages.h"
#include "Sim/Systems/Input/ControlSetting.h"
#include "Sim/Scheduling/ITaskListener.h"

#include "Sim/Common.h"
#include "Plugins/RakNet/RakNetMessages.h"
#include "Plugins/RakNet/RakNetPackageFactory.h"
#include "Plugins/Game/GameMessages.h"


namespace GASS
{
	enum
	{
		INPUT_DATA = 0
	};

	class InputPackage : public NetworkPackage
	{
	public:
		InputPackage() 
		{
			
		}
		InputPackage(int id ) : NetworkPackage(id) 
		{
		
		}
		InputPackage(int id, unsigned int time_stamp,int index, float value) : NetworkPackage(id), 
			TimeStamp(time_stamp),
			Index(index),
			Value(value)
			{}
		virtual ~InputPackage(){}
		int GetSize() {return sizeof(InputPackage);}
		void Assign(char* data)
		{
			*this = *(InputPackage*)data;
		}
		int Index;
		float Value;
		unsigned int TimeStamp;
	};

	typedef boost::shared_ptr<InputPackage> InputPackagePtr;

	class SceneObject;
	
	typedef boost::shared_ptr<SceneObject> SceneObjectPtr;
	typedef boost::weak_ptr<SceneObject> SceneObjectWeakPtr;

	class RakNetInputTransferComponent : public Reflection<RakNetInputTransferComponent,BaseSceneComponent>, public ITaskListener
	{
	public:
		RakNetInputTransferComponent();
		virtual ~RakNetInputTransferComponent();
		static void RegisterReflection();
		virtual void OnCreate();
		//int AUTO_RPC_CALLSPEC EnterObject(const char *object, RakNet::AutoRPC* networkCaller);
		void ReceivedInput(int controller, float value);
	private:
		void OnLoad(LoadNetworkComponentsMessagePtr message);
		void OnUnload(UnloadComponentsMessagePtr message);
		void OnDeserialize(NetworkDeserializeMessagePtr message);
		void OnInput(ControllerMessagePtr message);
		void SetControlSetting(const std::string &controlsetting) {m_ControlSettingName = controlsetting;}
		std::string GetControlSetting() const {return m_ControlSettingName;}
		//void OnClientEnterVehicle(ClientEnterVehicleMessagePtr message);
		void OnClientRemoteMessage(ClientRemoteMessagePtr message);
		
		
		//ITaskListener
		void Update(double delta);
		TaskGroup GetTaskGroup() const;

		std::string m_ControlSettingName;
		ControlSetting* m_ControlSetting;
	};
	typedef boost::shared_ptr<RakNetInputTransferComponent> RakNetInputTransferComponentPtr;
}
#endif
