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
	typedef SPTR<GameSceneManager> GameSceneManagerPtr;
	
	class EnterVehicleMessage : public SceneObjectRequestMessage
	{
	public:
		EnterVehicleMessage(SenderID sender_id = -1, double delay= 0) : 
		  SceneObjectRequestMessage(sender_id , delay)
		  {

		  }
	private:
	};
	typedef SPTR<EnterVehicleMessage> EnterVehicleMessagePtr;


	class ExitVehicleMessage : public SceneObjectRequestMessage
	{
	public:
		ExitVehicleMessage(SenderID sender_id = -1, double delay= 0) : 
		  SceneObjectRequestMessage(sender_id , delay)
		  {

		  }
	private:
	};
	typedef SPTR<ExitVehicleMessage> ExitVehicleMessagePtr;

	class GotoPositionMessage : public SceneObjectRequestMessage
	{
	public:
		GotoPositionMessage(const Vec3 &pos, SenderID sender_id = -1, double delay= 0) : 
		  SceneObjectRequestMessage(sender_id , delay),m_Position(pos)
		  {

		  }
		  Vec3 GetPosition() const {return m_Position;}
	private:
		Vec3 m_Position;
	};
	typedef SPTR<GotoPositionMessage> GotoPositionMessagePtr;

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
	typedef SPTR<FaceDirectionRequest> FaceDirectionRequestPtr;



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
	typedef SPTR<PathfindToPositionMessage> PathfindToPositionMessagePtr;


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
	typedef SPTR<DesiredSpeedMessage> DesiredSpeedMessagePtr;

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
	typedef SPTR<AimAtPositionMessage> AimAtPositionMessagePtr;

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
	typedef SPTR<ActivateAutoAimMessage> ActivateAutoAimMessagePtr;


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
	typedef SPTR<BarrelTransformationMessage> BarrelTransformationMessagePtr;


	class ReloadMessage : public SceneObjectRequestMessage
	{
	public:
		ReloadMessage(SenderID sender_id = -1, double delay= 0) : 
		  SceneObjectRequestMessage(sender_id , delay)
		  {

		  }
	private:
	};
	typedef SPTR<ReloadMessage> ReloadMessagePtr;


	class FireMessage : public SceneObjectRequestMessage
	{
	public:
		FireMessage(SenderID sender_id = -1, double delay= 0) : 
		  SceneObjectRequestMessage(sender_id , delay)
		  {

		  }
	private:
	};
	typedef SPTR<FireMessage> FireMessagePtr;


	class ReadyToFireMessage : public SceneObjectEventMessage
	{
	public:
		ReadyToFireMessage(SenderID sender_id = -1, double delay= 0) : 
		  SceneObjectEventMessage(sender_id , delay)
		  {

		  }
	private:
	};
	typedef SPTR<ReadyToFireMessage> ReadyToFireMessagePtr;


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
	typedef SPTR<HitMessage> HitMessagePtr;


	class OutOfArmorMessage : public SceneObjectEventMessage
	{
	public:
		OutOfArmorMessage(SenderID sender_id = -1, double delay= 0) : 
		  SceneObjectEventMessage(sender_id , delay)
		  {

		  }
		  
	private:
		
		
	};
	typedef SPTR<OutOfArmorMessage> OutOfArmorMessagePtr;

	/**
		Send message to client to enter object
	*/
	class ClientRemoteMessage : public BaseMessage
	{
	public:
		ClientRemoteMessage(const std::string &client, const std::string message, const std::string data, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay) , m_Client(client),m_Message(message){}
		std::string GetClient() const {return m_Client;}
		std::string GetMessage() const {return m_Message;}
		std::string GetData() const {return m_Data;}
	private:
		std::string m_Client;
		std::string m_Message;
		std::string m_Data;
	};
	typedef SPTR<ClientRemoteMessage> ClientRemoteMessagePtr;


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
	typedef SPTR<LODMessage> LODMessagePtr;


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
	typedef SPTR<VehicleEngineStatusMessage> VehicleEngineStatusMessagePtr;

	class SensorMessage : public SceneObjectEventMessage
	{
	public:
		SensorMessage(const DetectionVector &detection_data, SenderID sender_id = -1, double delay= 0) : SceneObjectEventMessage(sender_id , delay) , 
			m_DetectionData(detection_data){}
		DetectionVector GetDetectionVector() const {return m_DetectionData;}
	private:
		DetectionVector m_DetectionData;
	};
	typedef SPTR<SensorMessage> SensorMessagePtr;


	class SensorGotTargetMessage : public SceneObjectEventMessage
	{
	public:
		SensorGotTargetMessage (const DetectionData &detection_data, SenderID sender_id = -1, double delay= 0) : SceneObjectEventMessage(sender_id , delay) , 
			m_DetectionData(detection_data){}
		DetectionData GetDetectionData() const {return m_DetectionData;}
	private:
		DetectionData m_DetectionData;
	};
	typedef SPTR<SensorGotTargetMessage> SensorGotTargetMessagePtr;


	class SensorLostTargetMessage : public SceneObjectEventMessage
	{
	public:
		SensorLostTargetMessage(const DetectionData &detection_data, SenderID sender_id = -1, double delay= 0) : SceneObjectEventMessage(sender_id , delay) , 
			m_DetectionData(detection_data){}
		DetectionData GetDetectionData() const {return m_DetectionData;}
	private:
		DetectionData m_DetectionData;
	};
	typedef SPTR<SensorLostTargetMessage> SensorLostTargetMessagePtr;


	class CharacterAnimationRequest : public BaseMessage
	{
	public:
		CharacterAnimationRequest(const std::string &animation_name, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay),m_AnimationName(animation_name)
		  {

		  }
		  std::string GetAnimationName() const {return m_AnimationName;}
	private:
		std::string m_AnimationName;
	};
	typedef SPTR<CharacterAnimationRequest> CharacterAnimationRequestPtr;

	class CharacterInputRequest : public BaseMessage
	{
	public:
		CharacterInputRequest(const std::string &input_name,float value, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay),m_InputName(input_name),
			  m_Value(value)
		  {

		  }
		  std::string GetInputName() const {return m_InputName;}
		  float GetValue() const {return m_Value;}
	private:
		std::string m_InputName;
		float m_Value;
	};
	typedef SPTR<CharacterInputRequest> CharacterInputRequestPtr;

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
	typedef SPTR<InputRelayEvent> InputRelayEventPtr;


}
#endif
