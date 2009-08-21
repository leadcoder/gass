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


#include "Sim/Common.h"
#include "Sim/Components/BaseSceneComponent.h"
#include "Core/MessageSystem/Message.h"

namespace GASS
{
	class ControlSetting;
	class ILocationComponent;
	class ICameraComponent;
	class ScenarioScene;

		/**
			Basic motion modell that can be used to move a camera. 
			By listen to a ControlSetting the motion model takes input and
			then calculate a new postion and rotation and send a request message
			with this new data. Its the location components jobb to listen to 
			this kind of messages and actually implement the transformations
		*/

	class GASSExport FreeCamControlComponent  :  public Reflection<FreeCamControlComponent, BaseSceneComponent>
	{
	public:
		FreeCamControlComponent();
		virtual ~FreeCamControlComponent();
		static void RegisterReflection();
		void OnCreate();
		void Update(double delta_time);
	protected:
		void OnChangeCamera(MessagePtr message);
		void OnInput(MessagePtr message);
		void OnInit(MessagePtr message);
		void OnUnload(MessagePtr message);

		void PositionChange(MessagePtr message);
		void RotationChange(MessagePtr message);
		bool TopCameraActive(){if(m_TopCam == 1) return true; else return false;}
		void SetCollision(bool value) {m_CollisionTest = value;}
		void SetFly(bool value) {if(!value) SetCollision(true); m_Fly = value;}
		void ZoomOutFull(float value);
		void SetRunSpeed(float speed){ m_RunSpeed = speed;}
		float GetRunSpeed()const { return m_RunSpeed;}
		void SetWalkSpeed(float speed){ m_WalkSpeed = speed;}
		float GetWalkSpeed()const { return m_WalkSpeed;}
		void SetTurnSpeed(float speed){ m_TurnSpeed = speed;}
		float GetTurnSpeed()const { return m_TurnSpeed;}

		void UpdateFPCam(double delta);
		//void UpdateTopCam(float delta);
		Vec3 m_EllipsoidRadius;
		bool m_DrawEllipsoid;
		bool m_CollisionTest;
		bool m_FreeMouse;
		bool m_Fly;
		int m_TopCam;
		ControlSetting* m_ControlSetting;
		float m_FovChangeSpeed;
		float m_RunSpeed;
		float m_TurnSpeed;
		float m_WalkSpeed;
		float m_MaxFov;
		float m_MinFov;
		float m_Zoom;
		bool m_LockAspect;
		Vec3 m_Pos;
		Vec3 m_Rot;

		bool m_Active;
		bool m_EnableRotInput;
		bool m_SpeedBoostInput;
		float m_ThrottleInput;
		float m_StrafeInput;
		float m_PitchInput;
		float m_HeadingInput;
		ScenarioScene* m_Scene;
	};
}

