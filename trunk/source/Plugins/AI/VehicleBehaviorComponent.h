
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

	enum SpeedType
	{
		ST_UNCHANGED,
		ST_SLOW,
		ST_REGULAR,
		ST_FAST,
	};

	START_ENUM_BINDER(SpeedType,SpeedTypeBinder)
		BIND(ST_UNCHANGED)
		BIND(ST_SLOW)
		BIND(ST_REGULAR)
		BIND(ST_FAST)
	END_ENUM_BINDER(SpeedType,SpeedTypeBinder)

	class VehicleBehaviorComponent :  public Reflection<VehicleBehaviorComponent,BaseSceneComponent> 
	{
	public:
		VehicleBehaviorComponent(void);
		~VehicleBehaviorComponent(void);
		static void RegisterReflection();
		virtual void OnInitialize();
		void Reset();
		Float GetSpeed() const;
		Float GetWaypointRadius() const;
		void SetWaypointRadius(Float radius);
		Float GenerateRandomDelay() const;
		void SetComplete(bool value);
		bool GetComplete() const;
		bool IsSyncronized() const;
		ADD_PROPERTY(FormationTypeBinder,Formation)
		ADD_PROPERTY(SpeedTypeBinder,SpeedMode)
		ADD_PROPERTY(Float,RegularSpeedValue)
		ADD_PROPERTY(Vec2,Delay)
		ADD_PROPERTY(SceneObjectRef,Synchronize);
		ADD_PROPERTY(std::vector<SceneObjectRef>,Triggers);
		std::vector<SceneObjectPtr>  _GetSyncEnumeration() const;
		std::vector<SceneObjectPtr>  _GetTriggerEnumeration() const;
	private:
		bool _CheckTriggers() const;
		bool _CheckWaypoints() const;

		bool m_Initialized;
		Float m_Radius;
		bool m_Complete;
	
	};

	typedef SPTR<VehicleBehaviorComponent> VehicleBehaviorComponentPtr;
}
#endif