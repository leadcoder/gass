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
#include "Core/MessageSystem/IMessage.h"
#include "Sim/Scheduling/ITaskListener.h"

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

	class TopCamControlComponent  :  public Reflection<TopCamControlComponent, BaseSceneComponent>, ITaskListener
	{
	public:
		TopCamControlComponent();
		virtual ~TopCamControlComponent();
		static void RegisterReflection();
		void OnCreate();
		void Update(double delta_time);
		TaskGroup GetTaskGroup() const;
	protected:
		void OnChangeCamera(MessagePtr message);
		void OnInput(MessagePtr message);
		void OnInit(MessagePtr message);
		void OnUnload(MessagePtr message);

		float GetMaxWindowSize() const {return m_MaxWindowSize;}
		void SetMaxWindowSize(float value) {m_MaxWindowSize = value;}

		float GetMinWindowSize() const {return m_MinWindowSize;}
		void SetMinWindowSize(float value) {m_MinWindowSize = value;}

		float GetFixedHeight() const {return m_FixedHeight;}
		void SetFixedHeight(float value) {m_FixedHeight = value;}
		void PositionChange(MessagePtr message);
		void RotationChange(MessagePtr message);
		
		void UpdateTopCam(double delta);

		ControlSetting* m_ControlSetting;
		float m_ZoomSpeed;
		float m_FixedHeight;

		Vec3 m_Pos;
		Vec3 m_Rot;

		bool m_Active;
		bool m_EnableZoomInput;
		float m_ScrollBoostInput;
		float m_ScrollUpInput;
		float m_ScrollDownInput;
		float m_ZoomInput;
		
	
		float m_CurrentWindowSize;
		float m_MaxWindowSize;
		float m_MinWindowSize;
		

	};
}

