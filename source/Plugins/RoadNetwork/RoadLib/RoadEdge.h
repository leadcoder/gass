
#ifndef ROAD_EDGE
#define ROAD_EDGE

#include "Core/Math/GASSVector.h"

namespace GASS
{
	enum RoadDirection
	{
		UPSTREAM,
		DOWNSTREAM,
		BIDIR,
	};

	class RoadNode;

	class RoadEdge
	{
	public:

		RoadEdge() 
			
		{

		}
		virtual ~RoadEdge();
		
		std::vector<Vec3> Waypoints;
		Float Distance{-1};
		bool Enabled{true};
		RoadDirection Dir{BIDIR};
		RoadNode* StartNode{nullptr};
		RoadNode* EndNode{nullptr};
		Float LaneWidth{1};
		int LeftLanes{1};
		int RightLanes{1};

		//debug
		std::vector< std::vector<Vec3> > LLWaypoints;
		std::vector< std::vector<Vec3> > RLWaypoints;
	};
}

#endif