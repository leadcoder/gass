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

#ifndef GAME_MESSAGES_H
#define GAME_MESSAGES_H

#include "Sim/GASSCommon.h"
#include "Core/MessageSystem/GASSBaseMessage.h"
#include "Plugins/Game/PlatformType.h"
#include "Plugins/Game/DetectionData.h"
#include "Core/Math/GASSVector.h"
#include "Sim/Interface/GASSIControlSettingsSystem.h"
#include "Sim/Messages/GASSCoreSceneObjectMessages.h"

namespace GASS
{
	class GameSceneManager;
	typedef GASS_SHARED_PTR<GameSceneManager> GameSceneManagerPtr;
	
	class EnterVehicleRequest : public SceneObjectRequestMessage
	{
	public:
		EnterVehicleRequest(SenderID sender_id = -1, double delay= 0) : 
		  SceneObjectRequestMessage(sender_id , delay)
		  {

		  }
	private:
	};
	typedef GASS_SHARED_PTR<EnterVehicleRequest> EnterVehicleRequestPtr;


	class ExitVehicleRequest : public SceneObjectRequestMessage
	{
	public:
		ExitVehicleRequest(SenderID sender_id = -1, double delay= 0) : 
		  SceneObjectRequestMessage(sender_id , delay)
		  {

		  }
	private:
	};
	typedef GASS_SHARED_PTR<ExitVehicleRequest> ExitVehicleRequestPtr;

	class GotoPositionRequest : public SceneObjectRequestMessage
	{
	public:
		GotoPositionRequest(const Vec3 &pos, SenderID sender_id = -1, double delay= 0) : 
		  SceneObjectRequestMessage(sender_id , delay),m_Position(pos)
		  {

		  }
		  Vec3 GetPosition() const {return m_Position;}
	private:
		Vec3 m_Position;
	};
	typedef GASS_SHARED_PTR<GotoPositionRequest> GotoPositionRequestPtr;

	class FaceDirectionRequest : public SceneObjectRequestMessage
	{
	public:
		FaceDirectionRequest(const Vec3 &direction, SenderID sender_id = -1, double delay= 0) : 
		  SceneObjectRequestMessage(sender_id , delay),m_Dir(direction)
		  {

		  }
		  Vec3 GetDirection() const {return m_Dir;}
	private:
		Vec3 m_Dir;
	};
	typedef GASS_SHARED_PTR<FaceDirectionRequest> FaceDirectionRequestPtr;



	class PathfindToPositionMessage : public SceneObjectRequestMessage
	{
	public:
		PathfindToPositionMessage(const Vec3 &pos, SenderID sender_id = -1, double delay= 0) : 
		  SceneObjectRequestMessage(sender_id , delay),m_Position(pos)
		  {

		  }
		  Vec3 GetPosition() const {return m_Position;}
	private:
		Vec3 m_Position;
	};
	typedef GASS_SHARED_PTR<PathfindToPositionMessage> PathfindToPositionMessagePtr;


	class DesiredSpeedMessage : public SceneObjectRequestMessage
	{
	public:
		DesiredSpeedMessage(float speed, SenderID sender_id = -1, double delay= 0) : 
		  SceneObjectRequestMessage(sender_id , delay),m_Speed(speed)
		  {

		  }
		  float GetSpeed() const {return m_Speed;}
	private:
		float m_Speed;
	};
	typedef GASS_SHARED_PTR<DesiredSpeedMessage> DesiredSpeedMessagePtr;

	class AimAtPositionMessage : public SceneObjectRequestMessage
	{
	public:
		AimAtPositionMessage(const Vec3 &pos, int priority = 0, SenderID sender_id = -1, double delay= 0) : 
		  SceneObjectRequestMessage(sender_id , delay),m_Position(pos), m_Priority(priority)
		  {

		  }
		  Vec3 GetPosition() const {return m_Position;}
		  int GetPriority() const {return m_Priority;}
	private:
		Vec3 m_Position;
		int m_Priority;
	};
	typedef GASS_SHARED_PTR<AimAtPositionMessage> AimAtPositionMessagePtr;

	class ActivateAutoAimMessage : public SceneObjectRequestMessage
	{
	public:
		ActivateAutoAimMessage(bool value, int priority = 0, SenderID sender_id = -1, double delay= 0) : 
		  SceneObjectRequestMessage(sender_id , delay),m_Active(value), m_Priority(priority)
		  {

		  }
		  bool GetActive() const {return m_Active;}
		  int GetPriority() const {return m_Priority;}
	private:
		bool m_Active;
		int m_Priority;
	};
	typedef GASS_SHARED_PTR<ActivateAutoAimMessage> ActivateAutoAimMessagePtr;


	class BarrelTransformationMessage : public SceneObjectEventMessage
	{
	public:
		BarrelTransformationMessage(const Mat4 &value, SenderID sender_id = -1, double delay= 0) : 
		  SceneObjectEventMessage(sender_id , delay),m_Trans(value)
		  {

		  }
		  
		  Mat4 GetTransformation() const {return m_Trans;}
	private:
		Mat4 m_Trans;
		
	};
	typedef GASS_SHARED_PTR<BarrelTransformationMessage> BarrelTransformationMessagePtr;


	class ReloadMessage : public SceneObjectRequestMessage
	{
	public:
		ReloadMessage(SenderID sender_id = -1, double delay= 0) : 
		  SceneObjectRequestMessage(sender_id , delay)
		  {

		  }
	private:
	};
	typedef GASS_SHARED_PTR<ReloadMessage> ReloadMessagePtr;


	class FireMessage : public SceneObjectRequestMessage
	{
	public:
		FireMessage(SenderID sender_id = -1, double delay= 0) : 
		  SceneObjectRequestMessage(sender_id , delay)
		  {

		  }
	private:
	};
	typedef GASS_SHARED_PTR<FireMessage> FireMessagePtr;


	class ReadyToFireMessage : public SceneObjectEventMessage
	{
	public:
		ReadyToFireMessage(SenderID sender_id = -1, double delay= 0) : 
		  SceneObjectEventMessage(sender_id , delay)
		  {

		  }
	private:
	};
	typedef GASS_SHARED_PTR<ReadyToFireMessage> ReadyToFireMessagePtr;


	class HitMessage : public SceneObjectEventMessage
	{
	public:
		HitMessage(float damage, const Vec3 &hit_position, const Vec3 &hit_direction, SenderID sender_id = -1, double delay= 0) : 
		  SceneObjectEventMessage(sender_id , delay) , m_Damage(damage), m_HitDirection (hit_direction),m_HitPosition(hit_position)
		  {

		  }
		  float GetDamage() const {return m_Damage;}
		  Vec3 GetHitDirection() const {return m_HitDirection;}
		  Vec3 GetHitPosition() const {return m_HitPosition;}
	private:
		float m_Damage;
		Vec3 m_HitDirection;
		Vec3 m_HitPosition;
	};
	typedef GASS_SHARED_PTR<HitMessage> HitMessagePtr;


	class OutOfArmorMessage : public SceneObjectEventMessage
	{
	public:
		OutOfArmorMessage(SenderID sender_id = -1, double delay= 0) : 
		  SceneObjectEventMessage(sender_id , delay)
		  {

		  }
		  
	private:
		
		
	};
	typedef GASS_SHARED_PTR<OutOfArmorMessage> OutOfArmorMessagePtr;

	/**
		Send message to client to enter object
	*/
	class ClientRemoteMessage : public SceneObjectRequestMessage
	{
	public:
		ClientRemoteMessage(const std::string &client, const std::string message, const std::string data, SenderID sender_id = -1, double delay= 0) :
		  SceneObjectRequestMessage(sender_id , delay) , m_Client(client),m_Message(message){}
		std::string GetClient() const {return m_Client;}
		std::string GetMessage() const {return m_Message;}
		std::string GetData() const {return m_Data;}
	private:
		std::string m_Client;
		std::string m_Message;
		std::string m_Data;
	};
	typedef GASS_SHARED_PTR<ClientRemoteMessage> ClientRemoteMessagePtr;


	/**
		Message sent by the LOD component to inform other components lod level for the object
	*/
	class LODMessage : public SceneObjectEventMessage
	{
		
	public:
		enum LODLevel
		{
			LOD_HIGH,
			LOD_MEDIUM,
			LOD_LOW
		};

		LODMessage(LODLevel level, SenderID sender_id = -1, double delay= 0) :
		  SceneObjectEventMessage(sender_id , delay) , m_Level(level){}
		LODLevel GetLevel() const {return m_Level;}
	private:
		LODLevel m_Level;
	};
	typedef GASS_SHARED_PTR<LODMessage> LODMessagePtr;


	class VehicleEngineStatusMessage : public SceneObjectEventMessage
	{
	public:
		VehicleEngineStatusMessage(float rpm, float speed,int gear, SenderID sender_id = -1, double delay= 0) : SceneObjectEventMessage(sender_id , delay) , 
			m_RPM(rpm),
			m_Speed(speed),
			m_Gear(gear){}
		float GetRPM() const {return m_RPM;}
		float GetSpeed() const {return m_Speed;}
		int GetGear() const {return m_Gear;}
	private:
		float m_RPM;
		float m_Speed;
		int m_Gear;
	};
	typedef GASS_SHARED_PTR<VehicleEngineStatusMessage> VehicleEngineStatusMessagePtr;

	class SensorMessage : public SceneObjectEventMessage
	{
	public:
		SensorMessage(const DetectionVector &detection_data, SenderID sender_id = -1, double delay= 0) : SceneObjectEventMessage(sender_id , delay) , 
			m_DetectionData(detection_data){}
		//const DetectionVector& GetDetectionVector() const {return m_DetectionData;}
		DetectionVector GetDetectionVector() const {return m_DetectionData;}
	private:
		DetectionVector m_DetectionData;
	};
	typedef GASS_SHARED_PTR<SensorMessage> SensorMessagePtr;


	class SensorGotTargetMessage : public SceneObjectEventMessage
	{
	public:
		SensorGotTargetMessage (const DetectionData &detection_data, SenderID sender_id = -1, double delay= 0) : SceneObjectEventMessage(sender_id , delay) , 
			m_DetectionData(detection_data){}
		DetectionData GetDetectionData() const {return m_DetectionData;}
	private:
		DetectionData m_DetectionData;
	};
	typedef GASS_SHARED_PTR<SensorGotTargetMessage> SensorGotTargetMessagePtr;


	class SensorLostTargetMessage : public SceneObjectEventMessage
	{
	public:
		SensorLostTargetMessage(const DetectionData &detection_data, SenderID sender_id = -1, double delay= 0) : SceneObjectEventMessage(sender_id , delay) , 
			m_DetectionData(detection_data){}
		DetectionData GetDetectionData() const {return m_DetectionData;}
	private:
		DetectionData m_DetectionData;
	};
	typedef GASS_SHARED_PTR<SensorLostTargetMessage> SensorLostTargetMessagePtr;


	class VehicleRadarEvent : public SceneObjectEventMessage
	{
	public:
		VehicleRadarEvent(bool has_isect,const Vec3 &isect_pos, Float dist, SceneObjectPtr isect_obj, SenderID sender_id = -1, double delay= 0) : SceneObjectEventMessage(sender_id , delay) , 
			m_HasIsect(has_isect),
			m_IsectPos(isect_pos),
			m_IsectObject(isect_obj),
			m_IsectDist(dist)
		{}

		bool m_HasIsect;
		Vec3 m_IsectPos;
		SceneObjectPtr m_IsectObject;
		Float m_IsectDist;
	private:
	};
	typedef GASS_SHARED_PTR<VehicleRadarEvent> VehicleRadarEventPtr;



	class CharacterAnimationRequest : public SceneObjectRequestMessage
	{
	public:
		CharacterAnimationRequest(const std::string &animation_name, SenderID sender_id = -1, double delay= 0) : 
		  SceneObjectRequestMessage(sender_id , delay),m_AnimationName(animation_name)
		  {

		  }
		  std::string GetAnimationName() const {return m_AnimationName;}
	private:
		std::string m_AnimationName;
	};
	typedef GASS_SHARED_PTR<CharacterAnimationRequest> CharacterAnimationRequestPtr;

	class CharacterBehaviorRequest : public SceneObjectRequestMessage
	{
	public:
		CharacterBehaviorRequest(const std::string &b_class, const std::string &state ,SenderID sender_id = -1, double delay= 0) : 
		  SceneObjectRequestMessage(sender_id , delay),m_BehaviorClass(b_class),m_BehaviorState(state)
		  {

		  }
		  std::string GetClass() const {return m_BehaviorClass;}
		  std::string GetState() const {return m_BehaviorState;}
	private:
		std::string m_BehaviorClass;
		std::string m_BehaviorState;
	};
	typedef GASS_SHARED_PTR<CharacterBehaviorRequest> CharacterBehaviorRequestPtr;

	class CharacterEventRequest : public SceneObjectRequestMessage
	{
	public:
		CharacterEventRequest(const std::string &event, SenderID sender_id = -1, double delay= 0) : 
		  SceneObjectRequestMessage(sender_id , delay),m_Event(event)
		  {

		  }
		  std::string GetEvent() const {return m_Event;}
	private:
		std::string m_Event;
	};
	typedef GASS_SHARED_PTR<CharacterEventRequest> CharacterEventRequestPtr;


	class CharacterInputRequest : public SceneObjectRequestMessage
	{
	public:
		CharacterInputRequest(const std::string &input_name,float value, SenderID sender_id = -1, double delay= 0) : 
		  SceneObjectRequestMessage(sender_id , delay),m_InputName(input_name),
			  m_Value(value)
		  {

		  }
		  std::string GetInputName() const {return m_InputName;}
		  float GetValue() const {return m_Value;}
	private:
		std::string m_InputName;
		float m_Value;
	};
	typedef GASS_SHARED_PTR<CharacterInputRequest> CharacterInputRequestPtr;

	class InputRelayEvent : public SceneObjectEventMessage
	{
	public:
		InputRelayEvent(const std::string &settings, const std::string &controller, float value, ControllerType ct, SenderID sender_id = -1, double delay= 0) : 
		  SceneObjectEventMessage(sender_id , delay), m_Controller(controller), 
			  m_Value(value),
			  m_ControllerType(ct),
			  m_Settings(settings)
		  {

		  }
		  std::string GetSettings()const {return m_Settings;}
		  std::string GetController()const {return m_Controller;}
		  float GetValue() const {return m_Value;} 
		  ControllerType GetControllerType() const {return m_ControllerType;}
	private:
		std::string m_Settings;
		std::string m_Controller;
		float m_Value;
		ControllerType m_ControllerType;
	};
	typedef GASS_SHARED_PTR<InputRelayEvent> InputRelayEventPtr;


}
#endif
