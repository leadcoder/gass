
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
#include "Plugins/Game/GameMessages.h"
#include "Sim/Interface/GASSIGraphComponent.h"
class TiXmlElement;

namespace GASS
{
	class AIRoadLaneSectionComponent;
	class RoadNode;
	class RoadEdge;
	

	typedef SPTR<AIRoadLaneSectionComponent> AIRoadLaneSectionComponentPtr;

	class AIRoadNetwork :  public Reflection<AIRoadNetwork,BaseSceneComponent> , public IGraphComponent
	{
	public:
		AIRoadNetwork(void);
		~AIRoadNetwork(void);
		void OnInitialize();
		static void RegisterReflection();
		void OnPathfindToLocation(PathfindToPositionMessagePtr message);
		std::vector<Vec3> Search(const Vec3 &from_point,const Vec3 &to_point);

		bool GetEdit() const;
		void SetEdit(bool value);

		//IGprahComponent interface
		ADD_PROPERTY(std::string,NodeTemplate)
		ADD_PROPERTY(std::string,EdgeTemplate)
		void RebuildGraph();
	private:
		void GenerateGraph();
		bool GetBuild() const;
		void SetBuild(bool value);
		void Rebuild();
		//void AddLane(AIRoadLaneComponentPtr lane, RoadEdge* prev_edge);
		RoadNode* InsertNodeOnEdge(const Vec3& point,RoadEdge* edge);
		RoadEdge* GetCloesestEdge(const Vec3 &point);
		RoadNode* GetCloesestNode(const Vec3 &point);
		void RemoveNode(RoadNode* node);
		void BuildNetwork();
		void Load(const std::string &filename);
		void Save(const std::string &filename);
		void SaveXML(TiXmlElement * elem);
		void LoadXML(TiXmlElement * elem);
		std::vector<RoadNode*> m_Nodes;
		std::vector<RoadEdge*> m_Edges;
		bool m_Edit;
	};
	typedef SPTR<AIRoadNetwork> AIRoadNetworkPtr;

	enum RoadDirection
	{
		UPSTREAM,
		DOWNSTREAM,
		BIDIR,
	};

	class RoadEdge
	{
	public:

		RoadEdge() : Enabled(true),
			StartNode(NULL),
			EndNode(NULL),
			Distance(-1),
			Dir(BIDIR)
		{

		}
		virtual ~RoadEdge()
		{

		}
		std::vector<Vec3> Waypoints;
		Float Distance;
		bool Enabled;
		RoadDirection Dir;
		RoadNode* StartNode;
		RoadNode* EndNode;
	};

	class RoadNode
	{
		public:

		RoadNode() : Position(0,0,0)
		{

		}
		virtual ~RoadNode()
		{

		}
		Vec3 Position;
		std::vector<RoadEdge*> Edges;
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
			RoadNode* road_node = VoidToRoadNode(node);

			for(size_t i = 0; i < road_node->Edges.size(); i++)
			{
				RoadEdge* edge = road_node->Edges[i];
				if(edge->Enabled && (edge->Dir == BIDIR || 
					(edge->Dir == DOWNSTREAM && road_node == road_node->Edges[i]->StartNode) ||
					(edge->Dir == UPSTREAM && road_node == road_node->Edges[i]->EndNode)))
				{
					float cost = (float) edge->Distance;
					RoadNode* target_node;
					if(road_node->Edges[i]->StartNode == road_node)
						target_node = road_node->Edges[i]->EndNode;
					else
						target_node = road_node->Edges[i]->StartNode;

					micropather::StateCost nodeCost = { RoadNodeToVoid(target_node), cost};
					neighbors->push_back(nodeCost);
				}
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