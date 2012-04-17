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
#include "Sim/Components/GASSBaseSceneComponent.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Plugins/Base/CoreMessages.h"

namespace GASS
{
	class ControlSetting;
	class ILocationComponent;
	class ICameraComponent;
	class Scenario;
	typedef boost::shared_ptr<Scenario> ScenarioPtr;
	typedef boost::weak_ptr<Scenario> ScenarioWeakPtr;

		/**
			Basic motion modell that can be used to move a camera. 
			By listen to a ControlSetting the motion model takes input and
			then calculate a new postion and rotation and send a request message
			with this new data. Its the location components jobb to listen to 
			this kind of messages and actually implement the transformations
		*/

	class FreeCamControlComponent  :  public Reflection<FreeCamControlComponent, BaseSceneComponent>
	{
	public:
		FreeCamControlComponent();
		virtual ~FreeCamControlComponent();
		static void RegisterReflection();
		virtual void OnCreate();
		virtual void SceneManagerTick(double delta_time);
	protected:
		
		void OnChangeCamera(MessagePtr message);
		void OnInput(MessagePtr message);
		void OnLoad(LoadCoreComponentsMessagePtr message);
		void OnUnload(MessagePtr message);

		void PositionChange(MessagePtr message);
		void RotationChange(MessagePtr message);
		void SetMode(const std::string &value) {m_Mode= value;}
		std::string GetMode()const  {return m_Mode;}

		void SetDebug(bool debug) {m_Debug= debug;}
		bool GetDebug()const  {return m_Debug;}
		
		void SetRunSpeed(Float speed){ m_RunSpeed = speed;}
		Float GetRunSpeed()const { return m_RunSpeed;}
		void SetWalkSpeed(Float speed){ m_WalkSpeed = speed;}
		Float GetWalkSpeed()const { return m_WalkSpeed;}
		void SetTurnSpeed(Float speed){ m_TurnSpeed = speed;}
		Float GetTurnSpeed()const { return m_TurnSpeed;}

		void StepPhysics(double delta);
		
		std::string m_Mode;
		ControlSetting* m_ControlSetting;
		ControlSetting* m_AltControlSetting;
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

