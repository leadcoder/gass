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

#include "Plugins/RakNet/RakNetNetworkSystem.h"
//#include "Plugins/RakNet/RakNetBase.h"
#include "RakNetLocationTransferComponent.h"


#include "Core/Math/Quaternion.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/IMessage.h"
#include "Core/Utils/Log.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Scenario/Scene/SceneObjectManager.h"

#include "Sim/Systems/Resource/IResourceSystem.h"
#include "Sim/SimEngine.h"
#include "Sim/Systems/SimSystemManager.h"
#include "Sim/Scheduling/IRuntimeController.h"
#include "Sim/Systems/Input/ControlSettingsManager.h"
#include "Sim/Systems/Input/ControlSetting.h"
#include "Sim/Components/Graphics/ICameraComponent.h"

#include "GetTime.h"


//#define _DEBUG_LTC_

namespace GASS
{
	RakNetLocationTransferComponent::RakNetLocationTransferComponent() : m_Velocity(0,0,0),
		m_AngularVelocity(0,0,0),
		m_DeadReckoning(0),
		m_LastSerialize(0),
		m_ParentPos(0,0,0),
		m_RelativeToParent(0),
		m_SendFreq(1.0f/20.0f) // 20fps
	{
		for(int i = 0 ; i < 3; i++)
		{
			m_PositionHistory[i] = Vec3(0,0,0);
			m_RotationHistory[i] = Quaternion::IDENTITY;
			m_TimeStampHistory[i] = 0;
		}
	}

	RakNetLocationTransferComponent::~RakNetLocationTransferComponent()
	{

	}

	void RakNetLocationTransferComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("LocationTransferComponent",new Creator<RakNetLocationTransferComponent, IComponent>);
		GASS::PackageFactory::GetPtr()->Register(TRANSFORMATION_DATA,new GASS::Creator<TransformationPackage, NetworkPackage>);	
		RegisterProperty<float>("SendFrequency", &RakNetLocationTransferComponent::GetSendFrequency, &RakNetLocationTransferComponent::SetSendFrequency);
		RegisterProperty<bool>("RelativeToParent", &RakNetLocationTransferComponent::GetRelativeToParent, &RakNetLocationTransferComponent::SetRelativeToParent);
		
	}

	void RakNetLocationTransferComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(RakNetLocationTransferComponent::OnUnload,UnloadComponentsMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(RakNetLocationTransferComponent::OnLoad,LoadNetworkComponentsMessage,1));

	}

	void RakNetLocationTransferComponent::OnLoad(LoadNetworkComponentsMessagePtr message)
	{
		RakNetNetworkSystemPtr raknet = SimEngine::Get().GetSimSystemManager()->GetFirstSystem<RakNetNetworkSystem>();
		//RakNetNetworkComponentPtr nc = GetSceneObject()->GetFirstComponent<RakNetNetworkComponent>();
		//if(!nc)
		//	Log::Error("RakNetLocationTransferComponent require RakNetNetworkComponent to be present");
		SceneObjectPtr parent = boost::shared_dynamic_cast<SceneObject>(GetSceneObject()->GetParent());
		if(parent && m_RelativeToParent)
		{
			parent->RegisterForMessage(REG_TMESS(RakNetLocationTransferComponent::OnParentTransformationChanged,TransformationNotifyMessage,0));
			std::cout << "---------------------------ehajehj-------------------------" << std::endl;
		}
		if(raknet->IsServer())
		{
			

			GetSceneObject()->RegisterForMessage(REG_TMESS(RakNetLocationTransferComponent::OnTransformationChanged,TransformationNotifyMessage,0));
			GetSceneObject()->RegisterForMessage(REG_TMESS(RakNetLocationTransferComponent::OnVelocityNotify,VelocityNotifyMessage,0));
			SimEngine::GetPtr()->GetRuntimeController()->Register(this);
		}
		else
		{
			GetSceneObject()->RegisterForMessage(REG_TMESS(RakNetLocationTransferComponent::OnDeserialize,NetworkDeserializeMessage,0));

			MessagePtr disable_msg(new PhysicsBodyMessage(PhysicsBodyMessage::DISABLE,Vec3(0,0,0)));
			GetSceneObject()->PostMessage(disable_msg);

			IComponentContainerTemplate::ComponentVector components;
			GetSceneObject()->GetComponentsByClass(components,"ODEBodyComponent");
			for(int i = 0;  i< components.size(); i++)
			{
				BaseSceneComponentPtr comp = boost::shared_dynamic_cast<BaseSceneComponent>(components[i]);
				comp->GetSceneObject()->PostMessage(disable_msg);
			}

			SimEngine::GetPtr()->GetRuntimeController()->Register(this);
		}
	}


	void RakNetLocationTransferComponent::OnVelocityNotify(VelocityNotifyMessagePtr message)
	{
		Mat4 trans;
		trans.Identity();
		m_RotationHistory[0].ToRotationMatrix(trans);
		//trans.SetTranslation(m_PositionHistory[0].x,m_PositionHistory[0].y,m_PositionHistory[0].z);

		//transform
		m_Velocity = trans*message->GetLinearVelocity();
		m_AngularVelocity = trans*message->GetAngularVelocity();
	}


	void RakNetLocationTransferComponent::OnParentTransformationChanged(TransformationNotifyMessagePtr message)
	{
		m_ParentPos = message->GetPosition();
		//std::cout << "pos" << m_ParentPos << std::endl;
	}

	void RakNetLocationTransferComponent::OnTransformationChanged(TransformationNotifyMessagePtr message)
	{
		Vec3 pos = message->GetPosition();

		if(m_RelativeToParent) 
		{
			pos = pos - m_ParentPos;
			std::cout << "pos" << pos << std::endl;
		}
		Quaternion rot = message->GetRotation();

		double current_time = SimEngine::Get().GetTime();
		double delta = current_time - m_TimeStampHistory[0];

		//m_Position[1] = m_LastPosition;
		/*if(pos != m_PositionHistory[0])
		{
		m_Velocity = (pos - m_PositionHistory[0]);
		m_Velocity = m_Velocity*(1.0/delta);
		}

		if(rot != m_RotationHistory[0])
		{
		m_AngularVelocity = (rot - m_RotationHistory[0]);
		m_AngularVelocity = m_AngularVelocity*(1.0/delta);
		}

		std::cout << "Velocity:" << m_Velocity << std::endl;*/

		m_PositionHistory[0] = pos;
		m_RotationHistory[0] = rot;
		m_TimeStampHistory[0] = current_time;

		/*double current_time = SimEngine::Get().GetTime();
		double delta = current_time - m_LastSerialize;


		char debug_text[256];
		sprintf(debug_text,"delta: %f",delta);
		MessagePtr debug_msg2(new DebugPrintMessage(std::string(debug_text)));
		SimEngine::Get().GetSimSystemManager()->SendImmediate(debug_msg2);

		if(delta > m_SendFreq) //serialize transformation at fix freq
		{
		if(delta > 5) //clamp
		delta = 5;
		m_LastSerialize = current_time;


		Vec3 pos = message->GetPosition();
		Quaternion rot = message->GetRotation();

		Vec3 delta_pos(0,0,0);
		Quaternion delta_rot(0,0,0,0);

		//m_Position[1] = m_LastPosition;
		if(pos != m_PositionHistory[0])
		{
		delta_pos = (pos - m_PositionHistory[0]);
		delta_pos = delta_pos*(1.0/delta);
		}

		if(rot != m_RotationHistory[0])
		{
		delta_rot = (rot - m_RotationHistory[0]);
		delta_rot = delta_rot*(1.0/delta);
		}

		m_PositionHistory[0] = pos;
		m_RotationHistory[0] = rot;

		unsigned int time_stamp = RakNet::GetTime();
		//std::cout << "Time stamp:" << time_stamp << " Current time" << current_time << std::endl;

		boost::shared_ptr<TransformationPackage> package(new TransformationPackage(TRANSFORMATION_DATA,time_stamp,pos,delta_pos, rot,delta_rot));
		MessagePtr serialize_message(new NetworkSerializeMessage(0,package));
		GetSceneObject()->SendImmediate(serialize_message);
		}*/

	}

	void RakNetLocationTransferComponent::OnUnload(UnloadComponentsMessagePtr message)
	{

	}


	void RakNetLocationTransferComponent::Update(double delta)
	{


		RakNetNetworkSystemPtr raknet = SimEngine::Get().GetSimSystemManager()->GetFirstSystem<RakNetNetworkSystem>();
		//RakNetNetworkComponentPtr nc = GetSceneObject()->GetFirstComponent<RakNetNetworkComponent>();
		//if(!nc)
		//	Log::Error("RakNetLocationTransferComponent require RakNetNetworkComponent to be present");
		if(raknet->IsServer())
		{
			double current_time = SimEngine::Get().GetTime();
			double delta = current_time - m_LastSerialize;

			if(delta > m_SendFreq) //serialize transformation at fix freq
			{
				m_LastSerialize = current_time;
				unsigned int time_stamp = RakNet::GetTime();
				//std::cout << "Time stamp:" << time_stamp << " Current time" << current_time << std::endl;
				boost::shared_ptr<TransformationPackage> package(new TransformationPackage(TRANSFORMATION_DATA,time_stamp,m_PositionHistory[0],m_Velocity, m_RotationHistory[0],m_AngularVelocity));
				MessagePtr serialize_message(new NetworkSerializeMessage(0,package));
				GetSceneObject()->SendImmediate(serialize_message);
			}

		}
		else
		{
			Quaternion new_rot;
			Vec3 new_pos;

			static RakNetTime  step_back = 100;

#ifdef _DEBUG_LTC_

			if(GetAsyncKeyState(VK_F2))
				step_back--;
			if(GetAsyncKeyState(VK_F3))
				step_back++;
			std::cout << "step_back "<< step_back << std::endl;
#endif

			RakNetTime time = RakNet::GetTime();

			//std::cout << "Time: " << time << " Last Stamp" << m_TimeStampHistory[0] << std::endl; 
			//std::cout << "Time since last data received: " <<(time - m_TimeStampHistory[0]) << std::endl; 
			//Font::DebugPrint("Time since last data recieved : %d",(time -m_UpdateTimeStamp[0]));
			//Font::DebugPrint("inte time: %d",(m_UpdateTimeStamp[0] -m_UpdateTimeStamp[1]));

			time = time - step_back;

#ifdef _DEBUG_LTC_
			char debug_text[256];
#endif
			if(time > m_TimeStampHistory[0])  
			{
				//we have no new data ,extrapolate?
				//Font::DebugPrint("extrapolation Time before: %d",(time -m_UpdateTimeStamp[0]));
				//std::cout << "extrapolation Time before: " <<(time - m_TimeStampHistory[0]) << std::endl; 
				/*float prev_delta_time = float(m_UpdateTimeStamp[0] - m_UpdateTimeStamp[1]);
				float delta_time = time - m_UpdateTimeStamp[0];
				Vec3 delta_dir = m_Pos[0]-m_Pos[1];
				float speed = delta_dir.Length()*(1.0/prev_delta_time);*/
				Float length = m_Velocity.Length();
				if(length > 0.0000001)
				{
					Vec3 delta_dir = m_Velocity;
					delta_dir.Normalize();
					delta_dir = delta_dir*(length*m_DeadReckoning);
					//delta_dir = delta_dir* (1.0/prev_delta_time);
					//delta_dir = delta_dir*delta_time;
					new_pos = m_PositionHistory[0] + delta_dir;
				}
				else 
					new_pos = m_PositionHistory[0];

#ifdef _DEBUG_LTC_
				sprintf(debug_text,"extrapolation Time before: %d Vel %f %f %f Dead time %f",(time -m_TimeStampHistory[0]),m_Velocity.x,m_Velocity.y,m_Velocity.z, m_DeadReckoning);
#endif

				Vec3 ang_vel = m_AngularVelocity*m_DeadReckoning;
				new_rot = Quaternion(Vec3(ang_vel.y,ang_vel.x,ang_vel.z))*m_RotationHistory[0];

				//new_rot.x = m_RotationHistory[0].x  + m_AngularVelocity.x*m_DeadReckoning;
				//new_rot.y = m_RotationHistory[0].y  + m_AngularVelocity.y*m_DeadReckoning;
				//new_rot.z = m_RotationHistory[0].z  + m_AngularVelocity.z*m_DeadReckoning;
				//new_rot.w = m_RotationHistory[0].w  + m_AngularVelocity.w*m_DeadReckoning;

				//new_rot = m_RotationHistory[0];
				//new_pos = m_PositionHistory[0];

			}
			else if(time >= m_TimeStampHistory[1])
			{

				//std::cout << "interpolation 1: " <<(m_TimeStampHistory[0] - time) << std::endl; 
				//Font::DebugPrint("interpolation");
				RakNetTime elapsed = m_TimeStampHistory[0] - time;
				RakNetTime tot = m_TimeStampHistory[0] - m_TimeStampHistory[1];
				double inter = 0;
				if(tot > 0)
					inter = double(elapsed)/double(tot);
				new_pos = (m_PositionHistory[1]*inter) + (m_PositionHistory[0]*(1.0-inter));
				new_rot = Quaternion::Slerp2(inter,m_RotationHistory[0], m_RotationHistory[1]);
#ifdef _DEBUG_LTC_
				sprintf(debug_text,"interpolation 1: Time before: %d inter:%f",(m_TimeStampHistory[0] - time),inter);
#endif
			}
			else if(time >= m_TimeStampHistory[2])
			{
				//sprintf(debug_text,"interpolation 2: Time before: %d",(m_TimeStampHistory[1] - time));
				//std::cout << "interpolation 2: " <<(m_TimeStampHistory[1] - time) << std::endl; 
				RakNetTime elapsed = m_TimeStampHistory[1] - time;
				RakNetTime tot = m_TimeStampHistory[1] - m_TimeStampHistory[2];
				double inter = 0;
				if(tot > 0)
					inter = double(elapsed)/double(tot);
				new_pos = (m_PositionHistory[2]*inter) + (m_PositionHistory[1]*(1.0-inter));
				new_rot = Quaternion::Slerp2(inter,m_RotationHistory[1], m_RotationHistory[2]);
#ifdef _DEBUG_LTC_
				sprintf(debug_text,"interpolation 2: Time before: %d inter %f",(m_TimeStampHistory[1] - time),inter);
#endif
			}
			else
			{
				//Font::DebugPrint("behinde last update: %d",(m_UpdateTimeStamp[1]-time));
#ifdef _DEBUG_LTC_
				sprintf(debug_text,"behinde last update: %d",(m_TimeStampHistory[2] - time));
#endif
				//std::cout << "behinde last update: " <<(m_TimeStampHistory[1] - time) << std::endl; 
				new_rot = m_RotationHistory[2];
				new_pos = m_PositionHistory[2];
			}

			if(m_RelativeToParent)
			{
				std::cout << "pos" << new_pos << std::endl;
				new_pos = new_pos + m_ParentPos;
			}

			m_DeadReckoning += delta;
			GetSceneObject()->PostMessage(MessagePtr(new PositionMessage(new_pos)));
			GetSceneObject()->PostMessage(MessagePtr(new RotationMessage(new_rot)));

			//sprintf(debug_text,"\Time Stamp diff: %d %d",m_TimeStampHistory[0]-m_TimeStampHistory[1],m_TimeStampHistory[1]-m_TimeStampHistory[2]);
#ifdef _DEBUG_LTC_
			MessagePtr debug_msg2(new DebugPrintMessage(std::string(debug_text)));
			SimEngine::Get().GetSimSystemManager()->SendImmediate(debug_msg2);
#endif

		}
	}

	void RakNetLocationTransferComponent::OnDeserialize(NetworkDeserializeMessagePtr message)
	{
		if(message->GetPackage()->Id == TRANSFORMATION_DATA)
		{
			NetworkPackagePtr package = message->GetPackage();
			TransformationPackagePtr trans_package = boost::shared_dynamic_cast<TransformationPackage>(package);

			//if(trans_package->TimeStamp < m_TimeStampHistory[0])
			//	std::cout << "wrong order!!" << std::endl;

			m_Velocity = trans_package->Velocity;
			m_AngularVelocity = trans_package->AngularVelocity;

			m_PositionHistory[2] = m_PositionHistory[1];
			m_PositionHistory[1] = m_PositionHistory[0];
			m_PositionHistory[0] = trans_package->Position;

			m_RotationHistory[2] = m_RotationHistory[1];
			m_RotationHistory[1] = m_RotationHistory[0];
			m_RotationHistory[0] = trans_package->Rotation;

			m_TimeStampHistory[2] = m_TimeStampHistory[1];
			m_TimeStampHistory[1] = m_TimeStampHistory[0];
			m_TimeStampHistory[0] = message->GetTimeStamp();//->TimeStamp;

			m_DeadReckoning = 0;

			RakNetTime time = RakNet::GetTime();
			//std::cout << "Time diff:" <<  (time - message->GetTimeStamp()) <<std::endl; 
			//std::cout << "Time:" <<  time  << " Stamp:" << message->GetTimeStamp() << " Diff:" <<(time - message->GetTimeStamp()) << std::endl; 

		}
	}

	TaskGroup RakNetLocationTransferComponent::GetTaskGroup() const 
	{
		return NETWORK_TASK_GROUP;
	}
}

