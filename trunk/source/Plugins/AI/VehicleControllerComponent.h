
#ifndef AI_VEHICLE_CONTROLLER_COMPONENT
#define AI_VEHICLE_CONTROLLER_COMPONENT

#include "Sim/GASS.h"
#include "Core/Utils/GASSFilePath.h"
#include "Plugins/Game/PlatformType.h"
#include "Plugins/Base/CoreMessages.h"
#include "VehicleBehaviorComponent.h"

#include <set>

namespace GASS
{
	//FDECL(VehicleBehaviorComponent);
	FDECL(VehicleControllerComponent);
	IFDECL(NavigationComponent);
	
	class VehicleControllerComponent :  public Reflection<VehicleControllerComponent,BaseSceneComponent> 
	{
		
	public:
		VehicleControllerComponent(void);
		~VehicleControllerComponent(void);
		static void RegisterReflection();
		virtual void OnInitialize();
		SceneObjectPtr GetVehicle() const {return SceneObjectPtr(m_Vehicle,boost::detail::sp_nothrow_tag());}
		void SetBehaviorList(std::vector<VehicleBehaviorComponentPtr> behaviors);
		void OnUpdate(double);
		Float GetTargetSpeed() const {return m_TargetSpeed;}
		VehicleControllerComponentPtr GetLeader() const {return VehicleControllerComponentPtr(m_Leader);}
		void SetLeader(VehicleControllerComponentPtr leader) {m_Leader= leader;}
		void SetGroupID(int id) {m_GroupID = id;}
		int GetGroupID() const {return m_GroupID;}
		void SetCurrentDistance(Float value){m_CurrentPathDist = value;}
		Float GetCurrentDistance() const {return m_CurrentPathDist;}
	private:
		Vec3 GetVehiclePos() const{return m_VehiclePos;}
		bool GetFormationPosition(VehicleControllerComponentPtr slave, Vec3 &target_pos, Float &path_distance);
		void OnVehicleVelocity(VelocityNotifyMessagePtr message);
		void OnVehicleTransformation(TransformationNotifyMessagePtr message);
		void OnScenarioEvent(ScenarioStateRequestPtr message);
		void OnTransformation(TransformationNotifyMessagePtr message);
		
		void _UpdateFormationPathfollow(double time);
		void _UpdateLeaderPathfollow(double time);
		NavigationComponentPtr _GetNavigation() const {return NavigationComponentPtr(m_Navigation,NO_THROW);}
		void _Apply(VehicleBehaviorComponentPtr comp, bool first_behavior);
		
		void SetVehicleTemplate(const std::string &template_name);
		std::string GetVehicleTemplate() const;
		
		bool m_Initialized;
		SceneObjectWeakPtr m_Vehicle;
		std::string  m_VehicleTemplate;
		Vec3 m_StartPos;
		Quaternion m_StartRot;
		std::vector<Vec3> m_Path;
		std::vector<Vec3> m_FullPath;
		
		bool m_TargetReached;
		Float m_TargetRadius;
		Float m_TargetSpeed;
		ScenarioState m_ScenarioState;
		Float m_CurrentPathDist;
		Float m_PreviousDist;
		std::vector<VehicleBehaviorComponentPtr> m_BehaviorWaypoints;
		bool m_HasTargetDist;
		int m_GroupID;
		VehicleControllerComponentWeakPtr m_Leader;
		FormationType m_CurrentFormation;
		NavigationComponentWeakPtr m_Navigation;
		Vec3 m_VehiclePos;
		Quaternion m_VehicleRot;
		Float m_VehicleSpeed;
		Float m_DelayAtWaypoint;
	};
}
#endif