
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
	
	class VehicleControllerComponent :  public Reflection<VehicleControllerComponent,BaseSceneComponent> 
	{
		
	public:
		VehicleControllerComponent(void);
		~VehicleControllerComponent(void);
		static void RegisterReflection();
		virtual void OnInitialize();
		SceneObjectPtr GetVehicle() const {return SceneObjectPtr(m_Vehicle,boost::detail::sp_nothrow_tag());}
		void FollowPath(const std::vector<Vec3> path, double target_radius);
		bool GetTargetReached() const {return m_TargetReached;}
		void SetTargetDistance(Float dist);
		Float GetCurrentDistance() const {return m_CurrentPathDist;}
		//void Apply(VehicleBehaviorComponentPtr comp);
		void SetBehaviorList(std::vector<VehicleBehaviorComponentPtr> behaviors);
		void OnUpdate(double);
		bool GetRelativePosition(Float behinde_dist, Vec3 &target_position);
		Float GetTargetSpeed() const {return m_TargetSpeed;}
		bool GetPathDistance(const Vec3 &point, Float &distance);
		void SetOffset(Float offset) {m_PathOffset = offset;}
		void OffsetPath(Float offset);

		VehicleControllerComponentPtr GetLeader() const {return VehicleControllerComponentPtr(m_Leader);}
		void SetLeader(VehicleControllerComponentPtr leader) {m_Leader= leader;}
		void SetGroupID(int id) {m_GroupID = id;}
	private:
		void OnPathfollow(double);
		void Apply(VehicleBehaviorComponentPtr comp);
		void OnScenarioEvent(ScenarioStateRequestPtr message);
		void OnTransformation(TransformationNotifyMessagePtr message);
		void SetVehicleTemplate(const std::string &template_name);
		std::string GetVehicleTemplate() const;
		bool m_Initialized;
		SceneObjectWeakPtr m_Vehicle;
		std::string  m_VehicleTemplate;
		Vec3 m_Pos;
		Quaternion m_Rot;
		std::vector<Vec3> m_Path;
		std::vector<Vec3> m_OrgPath;
		bool m_TargetReached;
		Float m_TargetRadius;
		Float m_TargetSpeed;
		ScenarioState m_ScenarioState;
		Float m_CurrentPathDist;
		std::vector<VehicleBehaviorComponentPtr> m_BehaviorWaypoints;
		bool m_HasTargetDist;
		Float m_TargetDist;
		Float m_PathOffset;
		int m_GroupID;
		VehicleControllerComponentWeakPtr m_Leader;
		FormationType m_CurrentFormation;
	};

	
	
}
#endif