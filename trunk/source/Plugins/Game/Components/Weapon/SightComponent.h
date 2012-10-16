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

#ifndef SIGHT_COMPONENT_H
#define SIGHT_COMPONENT_H

#include "Sim/Components/GASSBaseSceneComponent.h"
#include "Sim/Scene/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Scene/GASSPhysicsSceneObjectMessages.h"
#include "Sim/Scene/GASSCoreSceneObjectMessages.h"
#include "Sim/Scene/GASSSceneObjectLink.h"
#include "Sim/Systems/Input/GASSIControlSettingsSystem.h"
#include "Sim/GASSCommon.h"
#include "Sim/Scheduling/GASSITaskListener.h"
#include "Plugins/Game/GameMessages.h"
#include "Utils/PIDControl.h"

namespace GASS
{

	class SceneObject;
	typedef boost::shared_ptr<SceneObject> SceneObjectPtr;
	typedef boost::weak_ptr<SceneObject> SceneObjectWeakPtr;

	class SightComponent :  public Reflection<SightComponent,BaseSceneComponent>
	{
	public:
		SightComponent();
		virtual ~SightComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual void OnDelete();
	private:
		void OnInput(InputControllerMessagePtr message);
		void OnBaseTransformation(TransformationNotifyMessagePtr message);
		void OnBarrelTransformation(BarrelTransformationMessagePtr message);
		void OnAimAtPosition(AimAtPositionMessagePtr message);

		void UpdateAimTransformation(double delta_time);
		void SetMaxPitchVelocity(float value) {m_MaxPitchVelocity = value;}
		float GetMaxPitchVelocity() const {return m_MaxPitchVelocity;}
		void SetMaxYawVelocity(float value) {m_MaxYawVelocity = value;}
		float GetMaxYawVelocity() const {return m_MaxYawVelocity;}
		void SetYawMaxMinAngle(const Vec2 &value) {m_YawMaxAngle = value.x;m_YawMinAngle= value.y;}
		Vec2 GetYawMaxMinAngle() const {return Vec2(m_YawMaxAngle,m_YawMinAngle);}
		void SetPitchMaxMinAngle(const Vec2 &value) {m_PitchMaxAngle = value.x;m_PitchMinAngle= value.y;}
		Vec2 GetPitchMaxMinAngle() const {return Vec2(m_PitchMaxAngle,m_PitchMinAngle);}
		SceneObjectLink  GetAutoAimObject() const {return m_AutoAimObject;}
		void SetAutoAimObject(const SceneObjectLink  &value) {m_AutoAimObject = value;}
		void SceneManagerTick(double delta_time);
		void UpdateTargetDistance();
		
		float m_MaxYawVelocity;
		float m_MaxPitchVelocity;
		float m_YawMinAngle;
		float m_YawMaxAngle;
		float m_PitchMinAngle;
		float m_PitchMaxAngle;
		int m_CurrentZoom;
		Mat4 m_BaseTransformation;
		Mat4 m_BarrelTransformation;
		Mat4 m_StartRotation;
		Mat4 m_AimRotation;
		bool m_Active;
		bool m_RemoteSim;
		bool m_AimAtPos;
		std::string m_TargetName;
		Float m_YawValue;
		Float m_PitchValue;
		Float m_TargetDistance;
		Float m_YawInput;
		Float m_PitchInput;
		Float m_CurrentYawAngle;
		Float m_CurrentPitchAngle;
		SceneObjectWeakPtr m_TargetObject;
		SceneObjectLink m_AutoAimObject;

		ADD_ATTRIBUTE(int,AutoAimPriority);
		ADD_ATTRIBUTE(std::string,SendDesiredPointController);
		ADD_ATTRIBUTE(std::string,ResetToBarrelController);
		ADD_ATTRIBUTE(std::string,ToggleZoomController);
		ADD_ATTRIBUTE(std::string,ActivateController);
		ADD_ATTRIBUTE(std::string,YawController);
		ADD_ATTRIBUTE(std::string,PitchController);
		ADD_ATTRIBUTE(std::string,TargetObjectTemplate);
		ADD_ATTRIBUTE(std::string,TargetDistanceController);
		ADD_ATTRIBUTE(std::vector<float>,ZoomValues);
		ADD_ATTRIBUTE(bool,Debug);
		ADD_ATTRIBUTE(bool,ResetToBarrelWhileInactive);
		ADD_ATTRIBUTE(int,TurnInputExp);
	};
	typedef boost::shared_ptr<SightComponent> SightComponentPtr;
}
#endif
