
#ifndef AI_VEHICLE_GROUP_CONTROLLER_COMPONENT
#define AI_VEHICLE_GROUP_CONTROLLER_COMPONENT

#include "Sim/GASS.h"
#include "Sim/Interface/GASSITemplateSourceComponent.h"
#include "Core/Utils/GASSFilePath.h"
#include "Plugins/Game/PlatformType.h"
#include "Plugins/Base/CoreMessages.h"
#include <set>

namespace GASS
{
	FDECL(VehicleControllerComponent)
	FDECL(VehicleBehaviorComponent)
	class VehicleGroupControllerComponent :  public Reflection<VehicleGroupControllerComponent,BaseSceneComponent> , public ITemplateSourceComponent
	{
		
	public:
		VehicleGroupControllerComponent(void);
		~VehicleGroupControllerComponent(void);
		static void RegisterReflection();
		virtual void OnInitialize();
		std::vector<std::string>  GetTemplates() const {return m_Templates;}
		void SceneManagerTick(double delta_time);
	private:
		void OnTransformation(TransformationNotifyMessagePtr message);
		void OnScenarioEvent(ScenarioStateRequestPtr message);

		ADD_PROPERTY(SceneObjectRef,WaypointList)
		void SetTemplates(const std::vector<std::string> &template_vec) {m_Templates = template_vec;}
		Vec3 GetOffset() const;
		void OnPlay();
		void OnUpdate(double delta_time);
		
		VehicleControllerComponentPtr GetLeader() const;
		bool m_Initialized;
		Vec3 m_Pos;
		Quaternion m_Rot;
		std::vector<std::string> m_Templates;
		std::vector<VehicleBehaviorComponentPtr> m_BehaviorWaypoints;
		VehicleControllerComponentWeakPtr  m_Leader;
		bool m_Update;
	};

	typedef SPTR<VehicleGroupControllerComponent> VehicleGroupControllerComponentPtr;
}
#endif