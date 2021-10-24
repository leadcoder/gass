#include "RoadNode.h"
#include "RoadEdge.h"

namespace GASS
{
	RoadNode::RoadNode() : Position(0,0,0)
	{

	}

	RoadNode::~RoadNode()
	{
		/*std::vector<RoadEdge*>::iterator iter = Edges.begin();
		while(iter != Edges.end(); i++)
		{
		delete *iter;
		iter = Edges.begin();
		}*/
		//reset  edge pointers
		std::vector<RoadEdge*>::iterator iter = Edges.begin();
		while(iter != Edges.end())
		{
			if((*iter)->StartNode == this)
				(*iter)->StartNode = nullptr;
			if((*iter)->EndNode == this)
				(*iter)->EndNode = nullptr;
			++iter;
		}

	}

	void RoadNode::RemoveEdge(RoadEdge* edge)
	{
		std::vector<RoadEdge*>::iterator iter = Edges.begin();
		while(iter != Edges.end())
		{
			if((*iter)== edge)
			{
				iter = Edges.erase(iter);
			}
			else
				++iter;
		}
	}	
}
