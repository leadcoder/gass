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
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/Messages/GASSGraphicsSystemMessages.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/Interface/GASSIControlSettingsSystem.h"

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
		~TopCamControlComponent() override;
		static void RegisterReflection();
		void OnInitialize() override;
		void OnDelete() override;
		void SceneManagerTick(double delta_time) override;
	protected:
		void OnCameraChanged(CameraChangedEventPtr message);
		void OnInput(ControllSettingsMessagePtr message);
		void OnCameraParameter(CameraParameterRequestPtr message);
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
		float m_ZoomSpeed{5};
		float m_FixedHeight{0};
		Vec3 m_Pos;
		Vec3 m_Rot;
		bool m_Active{false};
		bool m_EnablePanInput{false};
		float m_ScrollBoostInput{0};
		float m_ScrollUpInput{0};
		float m_ScrollDownInput{0};
		float m_ZoomInput{0};
		float m_CurrentWindowSize{45};
		float m_MaxWindowSize{520};
		float m_MinWindowSize{10};
	};
}

