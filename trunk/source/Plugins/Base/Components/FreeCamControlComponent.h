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

#pragma once


#include "Sim/GASSCommon.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/Interface/GASSIControlSettingsSystem.h"
#include "Sim/Messages/GASSGraphicsSystemMessages.h"
#include "Plugins/Base/CoreMessages.h"

namespace GASS
{
	class ControlSetting;
	class ILocationComponent;
	class ICameraComponent;
	class Scene;
	typedef SPTR<Scene> ScenePtr;
	typedef WPTR<Scene> SceneWeakPtr;

	enum MotionMode
	{
		MM_AIRCRAFT,
		MM_RTS,
	};

	START_ENUM_BINDER(MotionMode,MotionModeBinder)
		BIND(MM_AIRCRAFT)
		BIND(MM_RTS)
	END_ENUM_BINDER(MotionMode,MotionModeBinder)

	/**
	Basic motion model that can be used to move a camera. 
	By listen to a ControlSettings the motion model takes input and
	then calculate a new postion and rotation and then send a  
	transformation request messages. Its the location components job to listen to 
	this kind of messages and actually implement the transformation change
	*/

	class FreeCamControlComponent  :  public Reflection<FreeCamControlComponent, BaseSceneComponent>
	{
	public:
		FreeCamControlComponent();
		virtual ~FreeCamControlComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual void OnDelete();
		virtual void SceneManagerTick(double delta_time);
	protected:
		void OnCameraChanged(CameraChangedEventPtr message);
		void OnInput(ControllSettingsMessagePtr message);
		void PositionChange(MessagePtr message);
		void RotationChange(MessagePtr message);
		void SetMode(const MotionModeBinder &value) {m_Mode = value;}
		MotionModeBinder GetMode()const  {return m_Mode;}
		void SetDebug(bool debug) {m_Debug= debug;}
		bool GetDebug()const  {return m_Debug;}
		void SetRunSpeed(Float speed){ m_RunSpeed = speed;}
		Float GetRunSpeed()const { return m_RunSpeed;}
		void SetWalkSpeed(Float speed){ m_WalkSpeed = speed;}
		Float GetWalkSpeed()const { return m_WalkSpeed;}
		void SetTurnSpeed(Float speed){ m_TurnSpeed = speed;}
		Float GetTurnSpeed()const { return m_TurnSpeed;}
		void StepPhysics(double delta);
		
		MotionModeBinder m_Mode;
		std::string m_ControlSettingName;
		std::string m_AltControlSettingName;
		Float m_FovChangeSpeed;
		Float m_RunSpeed;
		Float m_TurnSpeed;
		Float m_WalkSpeed;
		Vec3 m_Pos;
		Vec3 m_Rot;
		bool m_Active;
		bool m_EnableRotInput;
		bool m_SpeedBoostInput;
		bool m_Debug;
		Float m_ThrottleInput;
		Float m_StrafeInput;
		Float m_PitchInput;
		Float m_HeadingInput;
		Float m_UpDownInput;
		Float m_CurrentFov;
		Float m_MaxFov;
		Float m_MinFov;
	};
}

