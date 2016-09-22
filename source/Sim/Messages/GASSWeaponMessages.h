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
	};
	typedef GASS_SHARED_PTR<ReloadMessage> ReloadMessagePtr;


	class FireMessage : public SceneObjectRequestMessage
	{
	public:
		FireMessage(SenderID sender_id = -1, double delay= 0) : 
		  SceneObjectRequestMessage(sender_id , delay)
		  {

		  }
	};
	typedef GASS_SHARED_PTR<FireMessage> FireMessagePtr;


	class ReadyToFireMessage : public SceneObjectEventMessage
	{
	public:
		ReadyToFireMessage(SenderID sender_id = -1, double delay= 0) : 
		  SceneObjectEventMessage(sender_id , delay)
		  {

		  }
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
	};
	typedef GASS_SHARED_PTR<OutOfArmorMessage> OutOfArmorMessagePtr;
}