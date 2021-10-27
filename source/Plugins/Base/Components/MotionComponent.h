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
#include "Sim/GASSComponent.h"
#include "Core/Utils/GASSEnumBinder.h"
#include "Core/MessageSystem/GASSIMessage.h"

#include "Sim/Interface/GASSIControlSettingsSystem.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Interface/GASSIPlatformComponent.h"
#include "Sim/Interface/GASSITerrainComponent.h"
#include "Sim/Messages/GASSGraphicsSystemMessages.h"
#include "Sim/Messages/GASSPlatformMessages.h"
#include "Sim/Messages/GASSInputMessages.h"
#include "Sim/GASSPlatformType.h"
#include "Plugins/Base/CoreMessages.h"

namespace GASS
{
	class ControlSetting;
	class ILocationComponent;
	class ICameraComponent;
	class Scene;
	using ScenePtr = std::shared_ptr<Scene>;
	using SceneWeakPtr = std::weak_ptr<Scene>;

	/**
	Basic motion model that can be used to move a camera. 
	By listen to a ControlSettings the motion model takes input and
	then calculate a new position and rotation and then send a  
	transformation request messages. Its the location components job to listen to 
	this kind of messages and actually implement the transformation change
	*/

	class MotionComponent : public Reflection<MotionComponent, Component>, public IPlatformComponent
	{
	public:
		MotionComponent();
		~MotionComponent() override;
		static void RegisterReflection();
		void OnInitialize() override;
		void SceneManagerTick(double delta_time) override;
		PlatformType GetType() const override { return m_PlatformType.GetValue(); }
		Vec3 GetSize() const override { return m_PlatformSize; }
		Float GetMaxSpeed() const override { return m_MaxSpeed; }
	protected:
		//void OnCameraChanged(CameraChangedEventPtr message);
		void OnInput(InputRelayEventPtr message);
		void OnTransMessage(TransformationChangedEventPtr message);
		void OnGotoPosition(GotoPositionRequestPtr message);
		void OnGeometryChanged(GeometryChangedEventPtr message);
		void StepPhysics(double delta);
		bool m_GroundClamp{true};
		bool m_Debug{false};
		Float m_Acceleration{10};
		Float m_MaxTurnSpeed{1};
		void SetMaxSpeed(Float speed){ m_MaxSpeed = speed; }
		Float m_MaxSpeed{20};
		//bool m_Active;
		Float m_ThrottleInput{0};
		Float m_SteerInput{0};
		Float m_BreakInput{0};
		Float m_CurrentSpeed{0};
		Vec3 m_CurrentPos;
		bool m_HasHeight{false};
		Float m_DesiredHeight{0};
		Quaternion m_CurrentRot;
		HeightmapTerrainComponentPtr m_Heightmap;
		Vec3 m_PlatformSize;
		PlatformTypeReflection m_PlatformType;
	};
}

