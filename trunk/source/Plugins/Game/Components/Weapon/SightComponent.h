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

#include "Sim/Components/BaseSceneComponent.h"
#include "Sim/Scenario/Scene/Messages/GraphicsSceneObjectMessages.h"
#include "Sim/Scenario/Scene/Messages/PhysicsSceneObjectMessages.h"
#include "Sim/Scenario/Scene/Messages/CoreSceneObjectMessages.h"
#include "Sim/Scenario/Scene/SceneObjectLink.h"
#include "Sim/Systems/Input/ControlSetting.h"
#include "Sim/Common.h"
#include "Sim/Scheduling/ITaskListener.h"
#include "Plugins/Game/GameMessages.h"
#include "Utils/PIDControl.h"

namespace GASS
{

	class SceneObject;
	typedef boost::shared_ptr<SceneObject> SceneObjectPtr;
	typedef boost::weak_ptr<SceneObject> SceneObjectWeakPtr;

	class SightComponent :  public Reflection<SightComponent,BaseSceneComponent>, public ITaskListener
	{
	public:
		SightComponent();
		virtual ~SightComponent();
		static void RegisterReflection();
		virtual void OnCreate();
	private:
		void UpdateAimTransformation(double delta_time);
		
		void SetMaxPitchVelocity(float value) {m_MaxPitchVelocity = value;}
		float GetMaxPitchVelocity() const {return m_MaxPitchVelocity;}

		void SetMaxYawVelocity(float value) {m_MaxYawVelocity = value;}
		float GetMaxYawVelocity() const {return m_MaxYawVelocity;}
		
		//void SetSteerForce(float value) {m_SteerForce = value;}
		//float GetSteerForce() const {return m_SteerForce;}

		void SetYawMaxMinAngle(const Vec2 &value) {m_YawMaxAngle = value.x;m_YawMinAngle= value.y;}
		Vec2 GetYawMaxMinAngle() const {return Vec2(m_YawMaxAngle,m_YawMinAngle);}
		void SetPitchMaxMinAngle(const Vec2 &value) {m_PitchMaxAngle = value.x;m_PitchMinAngle= value.y;}
		Vec2 GetPitchMaxMinAngle() const {return Vec2(m_PitchMaxAngle,m_PitchMinAngle);}
		SceneObjectLink  GetAutoAimObject() const {return m_AutoAimObject;}
		void SetAutoAimObject(const SceneObjectLink  &value) {m_AutoAimObject = value;}
		
		
		//Vec3 ProjectVectorOnPlane(const Vec3 plane_normal,const Vec3 &v);
		//Float GetPitchAngle(const Vec3 v1,const Vec3 v2);
		//Float GetAngleOnPlane(const Vec3 &plane_normal,const Vec3 &v1,const Vec3 &v2);
		
		void OnLoad(LoadGameComponentsMessagePtr message);
		void OnInput(ControllerMessagePtr message);
		void OnBaseTransformation(TransformationNotifyMessagePtr message);
		void OnBarrelTransformation(BarrelTransformationMessagePtr message);
		void OnUnload(UnloadComponentsMessagePtr message);
		void OnAimAtPosition(AimAtPositionMessagePtr message);
		//void OnPhysicsMessage(VelocityNotifyMessagePtr message);
		TaskGroup GetTaskGroup() const;
		void Update(double delta_time);
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
		std::string m_TargetName;
		//Float m_AngularVelocity;

		Float m_YawValue;
		Float m_PitchValue;
		Float m_TargetDistance;

		Float m_YawInput;
		Float m_PitchInput;

		Float m_CurrentYawAngle;
		Float m_CurrentPitchAngle;
		
		SceneObjectLink m_AutoAimObject;

		ADD_ATTRIBUTE(int,AutoAimPriority);
		ADD_ATTRIBUTE(std::string,SendDesiredPointController);
		ADD_ATTRIBUTE(std::string,ResetToBarrelController);
		ADD_ATTRIBUTE(std::string,ToggleZoomController);
		ADD_ATTRIBUTE(std::string,ActivateController);
		ADD_ATTRIBUTE(std::string,YawController);
		ADD_ATTRIBUTE(std::string,PitchController);
		ADD_ATTRIBUTE(std::string,TargetDistanceController);
		ADD_ATTRIBUTE(std::vector<float>,ZoomValues);
		ADD_ATTRIBUTE(bool,Debug);
		ADD_ATTRIBUTE(bool,ResetToBarrelWhileInactive);
		
		
	};
	typedef boost::shared_ptr<SightComponent> SightComponentPtr;
}
#endif
