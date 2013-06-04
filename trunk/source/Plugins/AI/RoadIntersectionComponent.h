
#ifndef AI_ROAD_INTER_COMPONENT
#define AI_ROAD_INTER_COMPONENT

#include "Core/Utils/GASSFilePath.h"
#include "Sim/GASS.h"
#include "Plugins/Game/PlatformType.h"
#include "Plugins/Base/CoreMessages.h"
#include "Plugins/AI/AIMessages.h"
#include <set>

namespace GASS
{

	

	class RoadSegmentComponent;
	typedef SPTR<RoadSegmentComponent>  RoadSegmentComponentPtr;
 	
	class TrafficLight
	{
	public:
		TrafficLight(): m_Stop(true){}
		~TrafficLight(){}
		std::vector<RoadSegmentComponentPtr> m_Roads;
		bool m_Stop;
	};

	enum TurnDir
	{
		TURN_NONE,
		TURN_LEFT,
		TURN_RIGHT
	};

	class RoadIntersectionComponent :  public Reflection<RoadIntersectionComponent,BaseSceneComponent> 
	{
	public:
		RoadIntersectionComponent(void);
		~RoadIntersectionComponent(void);
		static void RegisterReflection();
		virtual void OnInitialize();
		void AddRoad(RoadSegmentComponentPtr road);
		void RemoveRoad(RoadSegmentComponentPtr road);
		RoadSegmentComponentPtr GetRandomRoad(RoadSegmentComponentPtr road);
		void AddLight(const TrafficLight &light);
		bool GetTrafficLight(RoadSegmentComponentPtr road, TrafficLight &light);
		TurnDir CheckTurn(RoadSegmentComponentPtr in_road,RoadSegmentComponentPtr out_road);
	private:
		void OnToggleTrafficLight(ToggleTrafficLightMessagePtr message);
		Vec3  GetRoadDir(RoadSegmentComponentPtr road);
		bool m_Initialized;
		std::vector<RoadSegmentComponentPtr> m_Connections;
		std::vector<TrafficLight> m_Lights;
		unsigned int m_CurrentGreen;
		bool m_AllowLeftTurn;
		bool m_DisableTrafficLight;
	};
	typedef SPTR<RoadIntersectionComponent> RoadIntersectionComponentPtr;
}
#endif