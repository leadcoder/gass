
#ifndef ROAD_EDGE
#define ROAD_EDGE

#include "Core/Utils/GASSFilePath.h"
#include "Core/Math/GASSMath.h"
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

		RoadEdge() : Enabled(true),
			StartNode(NULL),
			EndNode(NULL),
			Distance(-1),
			Dir(BIDIR),
			LaneWidth(1),
			LeftLanes(1),
			RightLanes(1)
		{

		}
		virtual ~RoadEdge();
		
		std::vector<Vec3> Waypoints;
		Float Distance;
		bool Enabled;
		RoadDirection Dir;
		RoadNode* StartNode;
		RoadNode* EndNode;
		Float LaneWidth;
		int LeftLanes;
		int RightLanes;

		//debug
		std::vector< std::vector<Vec3> > LLWaypoints;
		std::vector< std::vector<Vec3> > RLWaypoints;
	};
}

#endif