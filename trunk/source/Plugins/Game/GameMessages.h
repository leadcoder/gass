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
#include "Core/MessageSystem/GASSBaseMessage.h"
#include "Plugins/Game/PlatformType.h"
#include "Plugins/Game/DetectionData.h"
#include "Core/Math/GASSVector.h"


namespace GASS
{
	class GameSceneManager;
	typedef boost::shared_ptr<GameSceneManager> GameSceneManagerPtr;
	

	class EnterVehicleMessage : public BaseMessage
	{
	public:
		EnterVehicleMessage(SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay)
		  {

		  }
	private:
	};
	typedef boost::shared_ptr<EnterVehicleMessage> EnterVehicleMessagePtr;


	class ExitVehicleMessage : public BaseMessage
	{
	public:
		ExitVehicleMessage(SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay)
		  {

		  }
	private:
	};
	typedef boost::shared_ptr<ExitVehicleMessage> ExitVehicleMessagePtr;

	class GotoPositionMessage : public BaseMessage
	{
	public:
		GotoPositionMessage(const Vec3 &pos, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay),m_Position(pos)
		  {

		  }
		  Vec3 GetPosition() const {return m_Position;}
	private:
		Vec3 m_Position;
	};
	typedef boost::shared_ptr<GotoPositionMessage> GotoPositionMessagePtr;


	class DesiredSpeedMessage : public BaseMessage
	{
	public:
		DesiredSpeedMessage(float speed, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay),m_Speed(speed)
		  {

		  }
		  float GetSpeed() const {return m_Speed;}
	private:
		float m_Speed;
	};
	typedef boost::shared_ptr<DesiredSpeedMessage> DesiredSpeedMessagePtr;


	

	class AimAtPositionMessage : public BaseMessage
	{
	public:
		AimAtPositionMessage(const Vec3 &pos, int priority = 0, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay),m_Position(pos), m_Priority(priority)
		  {

		  }
		  Vec3 GetPosition() const {return m_Position;}
		  int GetPriority() const {return m_Priority;}
	private:
		Vec3 m_Position;
		int m_Priority;
	};
	typedef boost::shared_ptr<AimAtPositionMessage> AimAtPositionMessagePtr;

	class ActivateAutoAimMessage : public BaseMessage
	{
	public:
		ActivateAutoAimMessage(bool value, int priority = 0, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay),m_Active(value), m_Priority(priority)
		  {

		  }
		  bool GetActive() const {return m_Active;}
		  int GetPriority() const {return m_Priority;}
	private:
		bool m_Active;
		int m_Priority;
	};
	typedef boost::shared_ptr<ActivateAutoAimMessage> ActivateAutoAimMessagePtr;


	class BarrelTransformationMessage : public BaseMessage
	{
	public:
		BarrelTransformationMessage(const Mat4 &value, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay),m_Trans(value)
		  {

		  }
		  
		  Mat4 GetTransformation() const {return m_Trans;}
	private:
		Mat4 m_Trans;
		
	};
	typedef boost::shared_ptr<BarrelTransformationMessage> BarrelTransformationMessagePtr;

	/*class PlayerInputMessage : public BaseMessage
	{
	public:
		PlayerInputMessage(const std::string &controller,float value, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay),m_Controller(controller), m_Value(value)
		  {

		  }
		  std::string GetController() const {return m_Controller;}
		  float GetValue() const {return m_Value;}
	private:
		std::string m_Controller;
		float m_Value;
	};
	typedef boost::shared_ptr<PlayerInputMessage> PlayerInputMessagePtr;*/


	class ReloadMessage : public BaseMessage
	{
	public:
		ReloadMessage(SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay)
		  {

		  }
	private:
	};
	typedef boost::shared_ptr<ReloadMessage> ReloadMessagePtr;


	class FireMessage : public BaseMessage
	{
	public:
		FireMessage(SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay)
		  {

		  }
	private:
	};
	typedef boost::shared_ptr<FireMessage> FireMessagePtr;


	class ReadyToFireMessage : public BaseMessage
	{
	public:
		ReadyToFireMessage(SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay)
		  {

		  }
	private:
	};
	typedef boost::shared_ptr<ReadyToFireMessage> ReadyToFireMessagePtr;


	class HitMessage : public BaseMessage
	{
	public:
		HitMessage(float damage, const Vec3 &hit_position, const Vec3 &hit_direction, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay) , m_Damage(damage), m_HitDirection (hit_direction),m_HitPosition(hit_position)
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
	typedef boost::shared_ptr<HitMessage> HitMessagePtr;


	class OutOfArmorMessage : public BaseMessage
	{
	public:
		OutOfArmorMessage(SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay)
		  {

		  }
		  
	private:
		
		
	};
	typedef boost::shared_ptr<OutOfArmorMessage> OutOfArmorMessagePtr;

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
	typedef boost::shared_ptr<ClientRemoteMessage> ClientRemoteMessagePtr;


	/**
		Message sent by the LOD component to inform other components lod level for the object
	*/
	class LODMessage : public BaseMessage
	{
		
	public:
		enum LODLevel
		{
			LOD_HIGH,
			LOD_MEDIUM,
			LOD_LOW
		};

		LODMessage(LODLevel level, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay) , m_Level(level){}
		LODLevel GetLevel() const {return m_Level;}
	private:
		LODLevel m_Level;
	};
	typedef boost::shared_ptr<LODMessage> LODMessagePtr;


	class VehicleEngineStatusMessage : public BaseMessage
	{
	public:
		VehicleEngineStatusMessage(float rpm, float speed,int gear, SenderID sender_id = -1, double delay= 0) : BaseMessage(sender_id , delay) , 
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
	typedef boost::shared_ptr<VehicleEngineStatusMessage> VehicleEngineStatusMessagePtr;


	

	class SensorMessage : public BaseMessage
	{
	public:
		SensorMessage(const DetectionVector &detection_data, SenderID sender_id = -1, double delay= 0) : BaseMessage(sender_id , delay) , 
			m_DetectionData(detection_data){}
		DetectionVector GetDetectionVector() const {return m_DetectionData;}
	private:
		DetectionVector m_DetectionData;
	};
	typedef boost::shared_ptr<SensorMessage> SensorMessagePtr;


	class SensorGotTargetMessage : public BaseMessage
	{
	public:
		SensorGotTargetMessage (const DetectionData &detection_data, SenderID sender_id = -1, double delay= 0) : BaseMessage(sender_id , delay) , 
			m_DetectionData(detection_data){}
		DetectionData GetDetectionData() const {return m_DetectionData;}
	private:
		DetectionData m_DetectionData;
	};
	typedef boost::shared_ptr<SensorGotTargetMessage> SensorGotTargetMessagePtr;


	class SensorLostTargetMessage : public BaseMessage
	{
	public:
		SensorLostTargetMessage(const DetectionData &detection_data, SenderID sender_id = -1, double delay= 0) : BaseMessage(sender_id , delay) , 
			m_DetectionData(detection_data){}
		DetectionData GetDetectionData() const {return m_DetectionData;}
	private:
		DetectionData m_DetectionData;
	};
	typedef boost::shared_ptr<SensorLostTargetMessage> SensorLostTargetMessagePtr;
	
}
#endif
