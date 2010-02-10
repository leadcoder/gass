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

namespace GASS
{
	enum GameSceneObjectMessages
	{
		OBJECT_RM_ENTER_VEHICLE = 1000,
		OBJECT_RM_EXIT_VEHICLE = 1001,
		OBJECT_RM_GOTO_POSITION = 1002,
		OBJECT_NM_PLAYER_INPUT = 1003,
		OBJECT_RM_RELOAD = 1004,
		OBJECT_RM_FIRE = 1005,
		OBJECT_NM_READY_TO_FIRE = 1006,
		OBJECT_NM_HIT = 1007,
		OBJECT_RM_OUT_OF_ARMOR = 1008,
	};


	enum GameScenarioSceneMessages
	{
		SCENARIO_RM_ENTER_VEHICLE = 1000,
	};


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

	class PlayerInputMessage : public BaseMessage
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
	typedef boost::shared_ptr<PlayerInputMessage> PlayerInputMessagePtr;


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
		HitMessage(float damage, const Vec3 &hit_direction, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay) , m_Damage(damage), m_HitDirection (hit_direction)
		  {

		  }
		  float GetDamage() const {return m_Damage;}
		  Vec3 GetHitDirection() const {return m_HitDirection;}
	private:
		float m_Damage;
		Vec3 m_HitDirection;
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

}
#endif
