
#ifndef AI_VEHICLE_CONTROLLER_COMPONENT
#define AI_VEHICLE_CONTROLLER_COMPONENT

#include "Sim/GASS.h"
#include "Core/Utils/GASSFilePath.h"
#include "Plugins/Game/PlatformType.h"
#include "Plugins/Base/CoreMessages.h"

#include <set>

namespace GASS
{
	FDECL(VehicleBehaviorComponent);
	
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
		//void Apply(VehicleBehaviorComponentPtr comp);
		void SetBehaviorList(std::vector<VehicleBehaviorComponentPtr> behaviors);
		void OnUpdate(double);
		bool GetRelativePosition(Float behinde_dist, Vec3 &target_position);
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
		bool m_TargetReached;
		Float m_TargetRadius;
		ScenarioState m_ScenarioState;
		Float m_CurrentPathDist;
		std::vector<VehicleBehaviorComponentPtr> m_BehaviorWaypoints;
	};

	typedef SPTR<VehicleControllerComponent> VehicleControllerComponentPtr;
	typedef WPTR<VehicleControllerComponent> VehicleControllerComponentWeakPtr;
	
}
#endif