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


#include "RakNetLocationTransferComponent.h"
#include "Plugins/RakNet/RakNetNetworkSystem.h"
#include "Plugins/RakNet/RakNetNetworkSceneManager.h"
#include "Plugins/RakNet/RakNetPackageFactory.h"
#include "Core/Math/GASSQuaternion.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/ComponentSystem/GASSComponentContainerTemplate.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/Interface/GASSILocationComponent.h"
#include "Sim/Interface/GASSIPhysicsBodyComponent.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/Messages/GASSPhysicsSceneObjectMessages.h"

//#define _DEBUG_LTC_

namespace GASS
{
	RakNetLocationTransferComponent::RakNetLocationTransferComponent() : m_Velocity(0,0,0),
		m_AngularVelocity(0,0,0),
		m_DeadReckoning(0),
		m_LastSerialize(0),
		m_ParentPos(0,0,0),
		m_UpdatePosition(true),
		m_UpdateRotation(true),
		m_SendFreq(0),
		m_NumHistoryFrames(6),
		m_ExtrapolatePosition(true),
		m_ExtrapolateRotation(true),
		m_ClientLocationMode(UNCHANGED)
	{
		m_LocationHistory.resize(m_NumHistoryFrames);
	}

	RakNetLocationTransferComponent::~RakNetLocationTransferComponent()
	{

	}

	void RakNetLocationTransferComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register<RakNetLocationTransferComponent>("LocationTransferComponent");
		GASS::PackageFactory::GetPtr()->Register(TRANSFORMATION_DATA,new GASS::Creator<TransformationPackage, NetworkPackage>);
		RegisterGetSet("SendFrequency", &RakNetLocationTransferComponent::GetSendFrequency, &RakNetLocationTransferComponent::SetSendFrequency);
		RegisterMember("ClientLocationMode", &RakNetLocationTransferComponent::m_ClientLocationMode);
		RegisterGetSet("UpdatePosition", &RakNetLocationTransferComponent::GetUpdatePosition, &RakNetLocationTransferComponent::SetUpdatePosition);
		RegisterGetSet("UpdateRotation", &RakNetLocationTransferComponent::GetUpdateRotation, &RakNetLocationTransferComponent::SetUpdateRotation);
		RegisterGetSet("ExtrapolatePosition", &RakNetLocationTransferComponent::GetExtrapolatePosition, &RakNetLocationTransferComponent::SetExtrapolatePosition);
		RegisterGetSet("ExtrapolateRotation", &RakNetLocationTransferComponent::GetExtrapolateRotation, &RakNetLocationTransferComponent::SetExtrapolateRotation);
	}

	void RakNetLocationTransferComponent::OnInitialize()
	{

		RakNetNetworkSystemPtr raknet = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<RakNetNetworkSystem>();
		if(!raknet->IsActive())
			return;

		RegisterForPreUpdate<RaknetNetworkSceneManager>();
		
		SceneObjectPtr parent = GASS_DYNAMIC_PTR_CAST<SceneObject>(GetSceneObject()->GetParent());
		if(parent && m_ClientLocationMode == FORCE_ATTACHED_TO_PARENT_AND_SEND_RELATIVE)
		{
			parent->RegisterForMessage(REG_TMESS(RakNetLocationTransferComponent::OnParentTransformationChanged,TransformationChangedEvent,0));
		}
		if(raknet->IsServer())
		{
			GetSceneObject()->RegisterForMessage(REG_TMESS(RakNetLocationTransferComponent::OnTransformationChanged,TransformationChangedEvent,0));
			GetSceneObject()->RegisterForMessage(REG_TMESS(RakNetLocationTransferComponent::OnVelocityNotify,PhysicsVelocityEvent,0));
		}
		else
		{
			GetSceneObject()->RegisterForMessage(REG_TMESS(RakNetLocationTransferComponent::OnDeserialize,NetworkDeserializeRequest,0));

			PhysicsBodyComponentPtr body = GetSceneObject()->GetFirstComponentByClass<IPhysicsBodyComponent>();
			if(body)
				body->SetActive(false);

			ComponentContainerTemplate::ComponentVector components;
			GetSceneObject()->GetComponentsByClass<IPhysicsBodyComponent>(components,true);
			for(size_t i = 0;  i< components.size(); i++)
			{
				PhysicsBodyComponentPtr comp = GASS_DYNAMIC_PTR_CAST<IPhysicsBodyComponent>(components[i]);
				comp->SetActive(false);
			}

			if(m_ClientLocationMode == FORCE_ATTACHED_TO_PARENT_AND_SEND_RELATIVE ||
				m_ClientLocationMode == FORCE_ATTACHED_TO_PARENT_AND_SEND_WORLD)
			{
				//attach this to parent node
				LocationComponentPtr location = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
				BaseSceneComponentPtr base = GASS_DYNAMIC_PTR_CAST<BaseSceneComponent>(location);
				base->SetPropertyValue("AttachToParent", true);
			}
		}
	}

	void RakNetLocationTransferComponent::OnVelocityNotify(PhysicsVelocityEventPtr message)
	{
		m_Velocity = message->GetLinearVelocity();
		m_AngularVelocity = message->GetAngularVelocity();
	}

	void RakNetLocationTransferComponent::OnParentTransformationChanged(TransformationChangedEventPtr message)
	{
		m_ParentPos = message->GetPosition();
		m_ParentRot = message->GetRotation();
	}

	void RakNetLocationTransferComponent::OnTransformationChanged(TransformationChangedEventPtr message)
	{
		Vec3 pos = message->GetPosition();
		Quaternion rot = message->GetRotation();

		if(m_ClientLocationMode == FORCE_ATTACHED_TO_PARENT_AND_SEND_RELATIVE)
		{
			Mat4 parent_transform(m_ParentRot, m_ParentPos);
			Mat4 inv_parent = parent_transform.GetInvert();
			pos = inv_parent * pos;
			Mat4 rot_mat = rot.GetRotationMatrix();

			//remove translation in inverted....is this OK??
			inv_parent.SetTranslation(Vec3(0, 0, 0));
			//transform.GetInvert(); What was intended??? 
			
			//get relativ rotation matrix
			rot_mat = inv_parent*rot_mat;

			//update quaternion
			rot.FromRotationMatrix(rot_mat);
		}

		unsigned int current_time = RakNet::GetTime();
		m_LocationHistory[0].Position = pos;
		m_LocationHistory[0].Rotation = rot;
		m_LocationHistory[0].Time = current_time;
	}

	void RakNetLocationTransferComponent::OnDelete()
	{

		SceneObjectPtr parent = GASS_DYNAMIC_PTR_CAST<SceneObject>(GetSceneObject()->GetParent());
		if(parent && m_ClientLocationMode == FORCE_ATTACHED_TO_PARENT_AND_SEND_RELATIVE)
		{
			parent->UnregisterForMessage(UNREG_TMESS(RakNetLocationTransferComponent::OnParentTransformationChanged,TransformationChangedEvent));
		}
	}

	bool RakNetLocationTransferComponent::IsRemote() const
	{
		RakNetNetworkSystemPtr raknet = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<RakNetNetworkSystem>();
		if(raknet && raknet->IsActive())
			return !raknet->IsServer();
		return false;
	}

	void RakNetLocationTransferComponent::SceneManagerTick(double /*delta*/)
	{
		RakNetNetworkSystemPtr raknet = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<RakNetNetworkSystem>();

		if(raknet->IsServer())
		{
			double current_time = SimEngine::Get().GetTime();
			double delta = current_time - m_LastSerialize;
			double send_intervall = 1.0/raknet->GetLocationSendFrequency();

			if(m_SendFreq > 0) //override?
			{
				send_intervall = 1.0/m_SendFreq;
			}

			if(delta > send_intervall) //serialize transformation at fix freq
			{
				m_LastSerialize = current_time;
				unsigned int time_stamp = RakNet::GetTime();
				//std::cout << "Time stamp:" << time_stamp << " Current time" << current_time << std::endl;
				SystemAddress address = UNASSIGNED_SYSTEM_ADDRESS;
				GASS_SHARED_PTR<TransformationPackage> package(new TransformationPackage(TRANSFORMATION_DATA,time_stamp,m_LocationHistory[0].Position,m_Velocity, m_LocationHistory[0].Rotation,m_AngularVelocity));
				NetworkSerializeRequestPtr serialize_message(new NetworkSerializeRequest(NetworkAddress(address.binaryAddress,address.port),0,package));
				GetSceneObject()->SendImmediateRequest(serialize_message);
			}
		}
		else //client
		{
			Quaternion new_rot;
			Vec3 new_pos;
			static RakNetTime  step_back = static_cast<RakNetTime>(raknet->GetInterpolationLag());
#ifdef _DEBUG_LTC_
			if(GetAsyncKeyState(VK_F2))
				step_back--;
			if(GetAsyncKeyState(VK_F3))
				step_back++;
		//	std::cout << "step_back "<< step_back << std::endl;
		//  std::cout << "Local vel"<< m_LocalVelocity << " " << m_LocalAngularVelocity << std::endl;
#endif

			RakNetTime time = RakNet::GetTime();

			//std::cout << "Time: " << time << " Last Stamp" << m_TimeStampHistory[0] << std::endl;
			//std::cout << "Time since last data received: " <<(time - m_TimeStampHistory[0]) << std::endl;
			//Font::DebugPrint("Time since last data recieved : %d",(time -m_UpdateTimeStamp[0]));
			//Font::DebugPrint("inte time: %d",(m_UpdateTimeStamp[0] -m_UpdateTimeStamp[1]));
			time = time - step_back;


			/*char debug_text[256];
			sprintf(debug_text,"Time: %d  %d %d",(time - m_LocationHistory[0].Time),m_LocationHistory[0].Time, time);
			GASS_PRINT(std::string(debug_text))
			*/

#ifdef _DEBUG_LTC_
			char debug_text[256];
#endif
			if(time > m_LocationHistory[0].Time)
			{
				m_DeadReckoning = static_cast<double>(time - m_LocationHistory[0].Time)/1000.0;

				new_pos = m_LocationHistory[0].Position;
				new_rot = m_LocationHistory[0].Rotation;

				//we have no new data ,extrapolate?
				if(m_ExtrapolatePosition || m_ExtrapolateRotation)
				{
					Mat4 trans(m_LocationHistory[0].Rotation);
					Vec3 local_velocity = trans*m_Velocity;
					Vec3 local_angular_velocity = trans*m_AngularVelocity;
					Float length = local_velocity.Length();
					if(m_ExtrapolatePosition && length > 0.0000001)
					{
						Vec3 delta_dir = local_velocity;
						delta_dir.Normalize();
						delta_dir = delta_dir*(length*m_DeadReckoning);
						new_pos = m_LocationHistory[0].Position + delta_dir;
					}
					if(m_ExtrapolateRotation)
					{
						Vec3 ang_vel = local_angular_velocity*m_DeadReckoning;
						new_rot = m_LocationHistory[0].Rotation * Quaternion::CreateFromEulerYXZ(ang_vel);
					}
#ifdef _DEBUG_LTC_
					sprintf(debug_text,"extrapolation Time before: %d Vel %f %f %f Dead time %f",(time -m_LocationHistory[0].Time),m_Velocity.x,m_Velocity.y,m_Velocity.z, m_DeadReckoning);
#endif
				}
				else
				{
					#ifdef _DEBUG_LTC_
					sprintf(debug_text,"extrapolation disabled Time before: %d Vel %f %f %f Dead time %f",(time -m_LocationHistory[0].Time),m_Velocity.x,m_Velocity.y,m_Velocity.z, m_DeadReckoning);
					#endif
				}



			}
			else
			{
				//pick last element as default

				new_pos = m_LocationHistory[m_LocationHistory.size()-1].Position;
				new_rot = m_LocationHistory[m_LocationHistory.size()-1].Rotation;
#ifdef _DEBUG_LTC_
				sprintf(debug_text,"Too much behinde, no history available, time: %d",(m_LocationHistory[m_LocationHistory.size()-1].Time - time));
#endif
				for(size_t i = 1; i < m_LocationHistory.size(); i++)
				{
					if(time >= m_LocationHistory[i].Time)
					{

						RakNetTime elapsed = m_LocationHistory[i-1].Time - time;
						RakNetTime tot = m_LocationHistory[i-1].Time - m_LocationHistory[i].Time;
						double inter = 0;
						if(tot > 0)
							inter = static_cast<double>(elapsed)/ static_cast<double>(tot);
						new_pos = (m_LocationHistory[i].Position*inter) + (m_LocationHistory[i-1].Position*(1.0-inter));
						new_rot = Quaternion::Slerp2(inter,m_LocationHistory[i-1].Rotation, m_LocationHistory[i].Rotation);
#ifdef _DEBUG_LTC_
						sprintf(debug_text,"interpolation %d: Time before: %d inter:%f",i,(m_LocationHistory[i-1].Time - time),inter);
#endif
						break;
					}
				}
			}

			if (LocationComponentPtr location = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>())
			{
				if (m_ClientLocationMode == FORCE_ATTACHED_TO_PARENT_AND_SEND_RELATIVE || m_ClientLocationMode == UNCHANGED)
				{
					if (m_UpdatePosition)
						location->SetPosition(new_pos);
					if (m_UpdateRotation)
						location->SetRotation(new_rot);
				}
				else if (m_ClientLocationMode == FORCE_ATTACHED_TO_PARENT_AND_SEND_WORLD)
				{
					if (m_UpdatePosition)
						location->SetWorldPosition(new_pos);
					if (m_UpdateRotation)
						location->SetWorldRotation(new_rot);
				}
			}

			GetSceneObject()->PostEvent(PhysicsVelocityEventPtr(new PhysicsVelocityEvent(m_Velocity, m_AngularVelocity)));

#ifdef _DEBUG_LTC_
			GASS_PRINT(std::string(debug_text))
#endif

		}
	}

	void RakNetLocationTransferComponent::OnDeserialize(NetworkDeserializeRequestPtr message)
	{
		if(message->GetPackage()->Id == TRANSFORMATION_DATA)
		{
			NetworkPackagePtr package = message->GetPackage();
			TransformationPackagePtr trans_package = GASS_DYNAMIC_PTR_CAST<TransformationPackage>(package);

			m_Velocity = trans_package->Velocity;
			m_AngularVelocity = trans_package->AngularVelocity;

			for(size_t i = m_LocationHistory.size()-1; i > 0 ;i--)
			{
				m_LocationHistory[i] = m_LocationHistory[i-1];
			}
			m_LocationHistory[0].Position = trans_package->Position;
			m_LocationHistory[0].Rotation = trans_package->Rotation;
			m_LocationHistory[0].Time = message->GetTimeStamp();
		}
	}
}

