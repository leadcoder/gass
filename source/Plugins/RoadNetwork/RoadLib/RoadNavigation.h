
#ifndef ROAD_NAVIGATION
#define ROAD_NAVIGATION

#include "Core/Utils/GASSFilePath.h"

namespace GASS
{
	class RoadNode;
	class RoadEdge;

	class RoadNavigation : public micropather::Graph
	{
	private:
		RoadNavigation( const RoadNavigation& );
		void operator=( const RoadNavigation& );
		micropather::MicroPather* m_Pather;
	public:
		RoadNavigation()  
		{
			m_Pather = new micropather::MicroPather( this, 20 );	// Use a very small memory block to stress the pather
		}

		~RoadNavigation() override {
			delete m_Pather;
		}

		unsigned int Checksum() { return static_cast<unsigned int>( m_Pather->Checksum()); }

		int GetPath( RoadNode *from_node, RoadNode* to_node, std::vector<RoadNode*>  &path ) 
		{
			int result = 0;
			float total_cost;
			std::vector<void*> void_path;
			result = m_Pather->Solve( RoadNodeToVoid( from_node), RoadNodeToVoid( to_node), &void_path, &total_cost );

			for(size_t i = 0; i < void_path.size(); i++)
			{
				path.push_back((RoadNode*)void_path[i]);
			}

			if ( result == micropather::MicroPather::SOLVED ) 
			{
				//playerX = nx;
				//playerY = ny;
			}
			printf( "Pather returned %d\n", result );
			return result;
		}

		RoadNode * VoidToRoadNode( void* node) 
		{
			return (RoadNode*)node;
		}

		void* RoadNodeToVoid( RoadNode* inter)
		{
			return (void*) inter;
		}

		float LeastCostEstimate( void* nodeStart, void* nodeEnd ) override 
		{
			RoadNode* start = VoidToRoadNode( nodeStart);
			RoadNode* end = VoidToRoadNode( nodeEnd);

			/* Compute the minimum path cost using distance measurement. It is possible
			to compute the exact minimum path using the fact that you can move only 
			on a straight line or on a diagonal, and this will yield a better result.
			*/
			return (float) (start->Position - end->Position).Length();
		}

		void AdjacentCost( void* node, std::vector< micropather::StateCost > *neighbors ) override 
		{
			RoadNode* road_node = VoidToRoadNode(node);

			for(size_t i = 0; i < road_node->Edges.size(); i++)
			{
				RoadEdge* edge = road_node->Edges[i];
				if(edge->Enabled && (edge->Dir == BIDIR || 
					(edge->Dir == DOWNSTREAM && road_node == road_node->Edges[i]->StartNode) ||
					(edge->Dir == UPSTREAM && road_node == road_node->Edges[i]->EndNode)))
				{
					auto cost = (float) edge->Distance;
					RoadNode* target_node;
					if(road_node->Edges[i]->StartNode == road_node)
						target_node = road_node->Edges[i]->EndNode;
					else
						target_node = road_node->Edges[i]->StartNode;

					micropather::StateCost node_cost = { RoadNodeToVoid(target_node), cost};
					neighbors->push_back(node_cost);
				}
			}
		}

		void PrintStateInfo( void* /*node*/ ) override 
		{
			//int x, y;
			//NodeToXY( node, &x, &y );
			//printf( "(%d,%d)", x, y );
		}
	};
}
#endif