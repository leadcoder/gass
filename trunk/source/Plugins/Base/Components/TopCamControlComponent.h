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
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/Messages/GASSGraphicsSystemMessages.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/Interface/GASSIControlSettingsSystem.h"
#include "Plugins/Base/CoreMessages.h"

namespace GASS
{
	class ControlSetting;
	class ILocationComponent;
	class ICameraComponent;
	class Scene;

		/**
			Basic motion modell that can be used to move a camera. 
			By listen to a ControlSetting the motion model takes input and
			then calculate a new postion and rotation and send a request message
			with this new data. Its the location components jobb to listen to 
			this kind of messages and actually implement the transformations
		*/

	class TopCamControlComponent : public Reflection<TopCamControlComponent, BaseSceneComponent>
	{
	public:
		TopCamControlComponent();
		virtual ~TopCamControlComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual void OnDelete();
		virtual void SceneManagerTick(double delta_time);
	protected:
		void OnCameraChanged(CameraChangedEventPtr message);
		void OnInput(ControllSettingsMessagePtr message);
		void OnCameraParameter(CameraParameterMessagePtr message);
		float GetMaxWindowSize() const {return m_MaxWindowSize;}
		void SetMaxWindowSize(float value) {m_MaxWindowSize = value;}
		float GetMinWindowSize() const {return m_MinWindowSize;}
		void SetMinWindowSize(float value) {m_MinWindowSize = value;}
		float GetWindowSize() const {return m_CurrentWindowSize;}
		void SetWindowSize(float value) {m_CurrentWindowSize = value;}
		float GetFixedHeight() const {return m_FixedHeight;}
		void SetFixedHeight(float value) {m_FixedHeight = value;}
		void PositionChange(MessagePtr message);
		void RotationChange(MessagePtr message);
		void UpdateTopCam(double delta);
		
		std::string m_ControlSettingName;
		float m_ZoomSpeed;
		float m_FixedHeight;
		Vec3 m_Pos;
		Vec3 m_Rot;
		bool m_Active;
		bool m_EnablePanInput;
		float m_ScrollBoostInput;
		float m_ScrollUpInput;
		float m_ScrollDownInput;
		float m_ZoomInput;
		float m_CurrentWindowSize;
		float m_MaxWindowSize;
		float m_MinWindowSize;
	};
}

