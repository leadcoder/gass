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
#include "Core/Math/GASSVector.h"
#include "Sim/Messages/GASSCoreSceneObjectMessages.h"

namespace GASS
{
	class EnterVehicleRequest : public SceneObjectRequestMessage
	{
	public:
		EnterVehicleRequest(SenderID sender_id = -1, double delay= 0) : 
		  SceneObjectRequestMessage(sender_id , delay)
		  {

		  }
	};
	typedef GASS_SHARED_PTR<EnterVehicleRequest> EnterVehicleRequestPtr;

	class ExitVehicleRequest : public SceneObjectRequestMessage
	{
	public:
		ExitVehicleRequest(SenderID sender_id = -1, double delay= 0) : 
		  SceneObjectRequestMessage(sender_id , delay)
		  {

		  }
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
}

