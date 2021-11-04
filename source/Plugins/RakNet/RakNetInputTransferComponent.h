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

#ifndef RAK_NET_INPUT_TRANSFER_COMPONENT_H
#define RAK_NET_INPUT_TRANSFER_COMPONENT_H

#include "Sim/GASSCommon.h"
#include "Sim/GASSComponent.h"
#include "Sim/Messages/GASSNetworkSceneObjectMessages.h"
#include "Plugins/RakNet/RakNetPackageFactory.h"
#include "Sim/Messages/GASSInputMessages.h"
#include "RakNetCommon.h"


namespace GASS
{
	enum
	{
		INPUT_DATA = 0
	};

	class InputPackage : public NetworkPackage
	{
	public:
		InputPackage() : NetworkPackage(0)
			
		{
			
		}
		InputPackage(int id ) : NetworkPackage(id)
			
		{
		
		}
		InputPackage(int id, int generated_by, unsigned int time_stamp,int index, float value) : NetworkPackage(id), 
			TimeStamp(time_stamp),
			Index(index),
			Value(value),
			Generator(generated_by)
			{}
		~InputPackage() override{}
		int GetSize() override {return sizeof(InputPackage);}
		void Assign(char* data) override
		{
			*this = *(InputPackage*)data;
		}
		int Index{0};
		float Value{0};
		unsigned int Generator{0};
		unsigned int TimeStamp{0};
	};

	using InputPackagePtr = std::shared_ptr<InputPackage>;

	class SceneObject;
	
	using SceneObjectPtr = std::shared_ptr<SceneObject>;
	using SceneObjectWeakPtr = std::weak_ptr<SceneObject>;

	class RakNetInputTransferComponent : public Reflection<RakNetInputTransferComponent,Component>
	{
	public:
		RakNetInputTransferComponent();
		~RakNetInputTransferComponent() override;
		static void RegisterReflection();
		void OnInitialize() override;
		void OnDelete() override;
		void ReceivedInput(int controller, float value);
	private:
		void OnDeserialize(NetworkDeserializeEventPtr message);
		void OnInput(InputRelayEventPtr message);
		void SetControlSetting(const std::string &controlsetting) {m_ControlSettingName = controlsetting;}
		std::string GetControlSetting() const {return m_ControlSettingName;}
		void OnClientRemoteMessage(ClientRemoteMessagePtr message);
		
		std::string m_ControlSettingName;
		using InputHistoryMap = std::map<int, float>;
		InputHistoryMap m_InputHistory;
	};
	using RakNetInputTransferComponentPtr = std::shared_ptr<RakNetInputTransferComponent>;
}
#endif
