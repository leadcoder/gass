#include "RoadEdge.h"
#include "RoadNode.h"
namespace GASS
{
	RoadEdge::~RoadEdge()
	{
		//remove this edge from start and end node
		if(StartNode)
			StartNode->RemoveEdge(this);
		if(EndNode)
			EndNode->RemoveEdge(this);
	}
}
