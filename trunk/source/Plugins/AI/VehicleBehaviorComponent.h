
#ifndef AI_VEHICLE_BEHAVIOR_COMPONENT
#define AI_VEHICLE_BEHAVIOR_COMPONENT

#include "Sim/GASS.h"
#include "Core/Utils/GASSFilePath.h"
#include "Plugins/Game/PlatformType.h"
#include "Plugins/Base/CoreMessages.h"
#include <set>

namespace GASS
{
	enum FormationType
	{
		FT_UNCHANGED,
		FT_LINE,
		FT_WALL,
	};

	START_ENUM_BINDER(FormationType,FormationTypeBinder)
		BIND(FT_UNCHANGED)
		BIND(FT_LINE)
		BIND(FT_WALL)
	END_ENUM_BINDER(FormationType,FormationTypeBinder)


	class VehicleBehaviorComponent :  public Reflection<VehicleBehaviorComponent,BaseSceneComponent> 
	{
	public:
		VehicleBehaviorComponent(void);
		~VehicleBehaviorComponent(void);
		static void RegisterReflection();
		virtual void OnInitialize();
		ADD_PROPERTY(FormationTypeBinder,Formation)
		ADD_PROPERTY(Float,Speed)
	private:
		bool m_Initialized;
	
	};

	typedef SPTR<VehicleBehaviorComponent> VehicleBehaviorComponentPtr;
}
#endif