
#ifndef AI_ROAD_SEG_COMPONENT
#define AI_ROAD_SEG_COMPONENT

#include "Sim/GASS.h"
#include "Core/Utils/GASSFilePath.h"
#include "Plugins/Game/PlatformType.h"
#include "Plugins/Base/CoreMessages.h"
#include <tbb/spin_mutex.h>
#include <set>
#include <tbb/atomic.h>

namespace GASS
{


	class LaneVehicle
	{
	public:
		LaneVehicle()
		{}
		~LaneVehicle(){}
		tbb::atomic<double> m_Speed;
		tbb::atomic<double> m_Distance;
		tbb::atomic<double> m_DistanceToPath;
	};

	class RoadIntersectionComponent;
	typedef SPTR<RoadIntersectionComponent> RoadIntersectionComponentPtr;

	class RoadSegmentComponent :  public Reflection<RoadSegmentComponent,BaseSceneComponent> 
	{
		
	public:
		RoadSegmentComponent(void);
		~RoadSegmentComponent(void);
		static void RegisterReflection();
		virtual void OnInitialize();
		double GetDistanceToIntersection(const Vec3 &source_pos);
		RoadIntersectionComponentPtr GetNextIntersection(RoadIntersectionComponentPtr current);
		bool StartInIntersection(RoadIntersectionComponentPtr from);
		std::vector<Vec3> GetWaypointList(bool invert_dir);
		void SetStartNode(SceneObjectRef node);
		SceneObjectRef GetStartNode() const;
		void SetEndNode(SceneObjectRef node);
		SceneObjectRef GetEndNode() const;
		std::vector<Vec3> GenerateOffset(std::vector<Vec3> wps, Float offset);
		std::vector<Vec3> GetLane(int lane, bool upstream);
		void UpdateMesh();
		void RegisterVehicle(LaneVehicle* vehicle, bool up_stream);
		void UnregisterVehicle(LaneVehicle* vehicle, bool up_stream);
		std::vector<LaneVehicle*> GetLaneVehicles(bool up_stream) const 
		{
			if(up_stream) return m_UpStreamLaneVehicles;
			else m_DownStreamLaneVehicles;
		}
		LaneVehicle* GetClosest(bool up_stream, LaneVehicle* source);
		bool IsRoadFree(bool up_stream, double in_distance);
		bool IsStartOfRoadFree(bool up_stream, double in_distance);
		bool FirstFreeLocation(bool up_stream, Vec3 &pos, Quaternion &rot, Float &distance, Float vehicle_separation);
	private:
		void UpdateLanes();
		std::vector<Vec3> GenerateLane(std::vector<Vec3> wps, bool upstream);
		void OnTransformationChanged(TransformationChangedEventPtr message);

		SceneObjectRef m_StartNode;
		SceneObjectRef m_EndNode;
		bool m_Initialized;
		

		typedef std::vector<Vec3> Lane;
		std::vector<Lane> m_UpStreamLanes;
		std::vector<Lane> m_DownStreamLanes;

		std::vector<LaneVehicle*> m_DownStreamLaneVehicles;
		std::vector<LaneVehicle*> m_UpStreamLaneVehicles;
	
		//next instersection in right line
		//RoadIntersectionComponentPtr m_RightIntersection;
		//RoadIntersectionComponentPtr m_LeftIntersection;
		tbb::spin_mutex m_RoadMutex;
	};

	typedef SPTR<RoadSegmentComponent> RoadSegmentComponentPtr;
}
#endif