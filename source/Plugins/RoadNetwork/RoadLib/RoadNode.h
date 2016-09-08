
#ifndef ROAD_NODE
#define ROAD_NODE

#include "Core/Math/GASSVector.h"

namespace GASS
{
	class RoadEdge;
	class RoadNode
	{
		public:
		RoadNode();
		virtual ~RoadNode();
		void RemoveEdge(RoadEdge* edge);
		Vec3 Position;
		std::vector<RoadEdge*> Edges;
	};
}
#endif