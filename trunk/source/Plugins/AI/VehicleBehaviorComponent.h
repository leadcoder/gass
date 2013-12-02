
#ifndef AI_VEHICLE_BEHAVIOR_COMPONENT
#define AI_VEHICLE_BEHAVIOR_COMPONENT

#include "Sim/GASS.h"
#include "Core/Utils/GASSFilePath.h"
#include "Plugins/Game/PlatformType.h"
#include "Plugins/Base/CoreMessages.h"
#include <set>

namespace GASS
{
	class VehicleBehaviorComponent :  public Reflection<VehicleBehaviorComponent,BaseSceneComponent> 
	{
	public:
		VehicleBehaviorComponent(void);
		~VehicleBehaviorComponent(void);
		static void RegisterReflection();
		virtual void OnInitialize();
	private:
		bool m_Initialized;
	};

	typedef SPTR<VehicleBehaviorComponent> VehicleBehaviorComponentPtr;
}
#endif