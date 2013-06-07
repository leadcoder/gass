
#ifndef AI_ROAD_LANE_COMPONENT
#define AI_ROAD_LANE_COMPONENT

#include "Sim/GASS.h"
#include "Core/Utils/GASSFilePath.h"
#include "Plugins/Game/PlatformType.h"
#include "Plugins/Base/CoreMessages.h"
#include "tbb/spin_mutex.h"
#include <set>
#include "tbb/atomic.h"

namespace GASS
{

	class LaneObject
	{
	public:
		LaneObject()
		{}
		~LaneObject(){}
		tbb::atomic<double> m_Speed;
		tbb::atomic<double> m_Distance;
		tbb::atomic<double> m_DistanceToPath;
	};


	enum LaneDirection
	{
		LD_UPSTREAM,
		LD_DOWNSTREAM,
		LD_BIDIR
	};

	START_ENUM_BINDER(LaneDirection,LaneDirectionBinder)
		BIND(LD_UPSTREAM)
		BIND(LD_DOWNSTREAM)
		BIND(LD_BIDIR)
	END_ENUM_BINDER(LaneDirection,LaneDirectionBinder)


	class AIRoadLaneComponent;
	typedef SPTR<AIRoadLaneComponent> AIRoadLaneComponentPtr;

	class AIRoadLaneComponent :  public Reflection<AIRoadLaneComponent,BaseSceneComponent> 
	{
	public:
		AIRoadLaneComponent(void);
		~AIRoadLaneComponent(void);
		static void RegisterReflection();
		virtual void OnInitialize();
		void SetWidth(Float dist);
		Float GetWidth() const;
		void UpdateLane();
		//bool IsDownStream() const {return m_DownStream;}
		void SetWaypoints(const std::vector<Vec3> &lane_wps) {m_Waypoints = lane_wps;}
		std::vector<Vec3> GetWaypoints() const {return m_Waypoints;}
		std::vector<Vec3> * GetWaypointsPtr() {return &m_Waypoints;}
		std::vector<AIRoadLaneComponentPtr>* GetNextLanesPtr() {return &m_NextLanes;}
		ADD_ATTRIBUTE(LaneDirectionBinder,Direction);
		ADD_ATTRIBUTE(int ,LaneID);
		void RegisterLaneObject(LaneObject* object);
		void UnregisterLaneObject(LaneObject* object);

		std::vector<LaneObject*>* GetLaneObjectsPtr()
		{
			return &m_LaneObjects;
		}
		LaneObject* GetClosest(LaneObject* source);
		//bool IsRoadFree(double in_distance);
		//bool IsStartOfRoadFree(double in_distance);
		bool FirstFreeLocation(Vec3 &pos, Quaternion &rot, Float &distance, Float vehicle_separation);
	private:
		std::vector<Vec3> GenerateOffset(std::vector<Vec3> wps, Float offset);
		//ADD_ATTRIBUTE(SceneObjectID,NextLane);
		//ADD_ATTRIBUTE(SceneObjectID,PrevLane);
		ADD_ATTRIBUTE(SceneObjectRef ,WaypointsObject);
	
		void OnWaypointsChanged(UpdateWaypointListMessagePtr message);
		Float m_Width;
		bool m_Initialized;
		std::vector<Vec3> m_Waypoints;
		std::vector<AIRoadLaneComponentPtr> m_NextLanes;
		tbb::spin_mutex m_LaneMutex;
		std::vector<LaneObject*> m_LaneObjects;
	};

	
}
#endif