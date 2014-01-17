
#ifndef AI_ROAD_NETWORK_COMPONENT
#define AI_ROAD_NETWORK_COMPONENT

#include "Sim/GASS.h"
#include "Core/Utils/GASSFilePath.h"
#include "Plugins/Game/PlatformType.h"
#include "Plugins/Base/CoreMessages.h"
#include "Plugins/AI/AIRoadLaneComponent.h"
#include <tbb/spin_mutex.h>
#include <set>
#include <tbb/atomic.h>
#include "Plugins/AI/micropather.h"

namespace GASS
{
	class AIRoadLaneSectionComponent;
	typedef SPTR<AIRoadLaneSectionComponent> AIRoadLaneSectionComponentPtr;

	class AIRoadNetwork :  public Reflection<AIRoadNetwork,BaseSceneComponent> 
	{
	public:
		AIRoadNetwork(void);
		~AIRoadNetwork(void);
		static void RegisterReflection();
		virtual void OnInitialize();
	private:
	};
	typedef SPTR<AIRoadNetwork> AIRoadNetworkPtr;

	class RoadNode
	{
		public:

		RoadNode()
		{

		}
		virtual ~RoadNode()
		{

		}
		Vec3 Position;
		std::vector<RoadNode*> Edges;
	};

	class RoadNavigation  : public micropather::Graph
	{
	private:
		RoadNavigation( const RoadNavigation& );
		void operator=( const RoadNavigation& );

		micropather::MicroPather* pather;
	public:
		RoadNavigation()  
		{
			pather = new micropather::MicroPather( this, 20 );	// Use a very small memory block to stress the pather
		}

		virtual ~RoadNavigation() {
			delete pather;
		}

		unsigned Checksum() { return pather->Checksum(); }

		int GetPath( RoadNode *from_node, RoadNode* to_node, std::vector<RoadNode*>  &path ) 
		{
			int result = 0;
			float totalCost;
			std::vector<void*> void_path;
			result = pather->Solve( RoadNodeToVoid( from_node), RoadNodeToVoid( to_node), &void_path, &totalCost );

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

		virtual float LeastCostEstimate( void* nodeStart, void* nodeEnd ) 
		{
			RoadNode* start = VoidToRoadNode( nodeStart);
			RoadNode* end = VoidToRoadNode( nodeEnd);

			/* Compute the minimum path cost using distance measurement. It is possible
			to compute the exact minimum path using the fact that you can move only 
			on a straight line or on a diagonal, and this will yield a better result.
			*/
			return (float) (start->Position - end->Position).Length();
		}

		virtual void AdjacentCost( void* node, std::vector< micropather::StateCost > *neighbors ) 
		{
			RoadNode* start = VoidToRoadNode(node);

			for(size_t i = 0; i < start->Edges.size(); i++)
			{
				RoadNode* end = start->Edges[i];
				float cost = (float) (start->Position-end->Position).Length();
				micropather::StateCost nodeCost = { RoadNodeToVoid(start->Edges[i]), cost};
				neighbors->push_back( nodeCost );
			}
		}

		virtual void PrintStateInfo( void* node ) 
		{
			//int x, y;
			//NodeToXY( node, &x, &y );
			//printf( "(%d,%d)", x, y );
		}
	};
}
#endif