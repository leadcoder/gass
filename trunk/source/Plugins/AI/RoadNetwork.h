
#ifndef ROAD_NETWORK
#define ROAD_NETWORK

#include "Core/Utils/GASSFilePath.h"
#include "Core/Math/GASSMath.h"
#include "Core/Math/GASSVector.h"
#include "micropather.h"
#include <set>
class TiXmlElement;

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
	
	class RoadNetwork 	
	{
	public:
		RoadNetwork(void);
		~RoadNetwork(void);
		void Clear();
		std::vector<Vec3> Search(const Vec3 &from_point,const Vec3 &to_point);
		void Load(const std::string &filename);
		void Save(const std::string &filename);
		void SaveXML(TiXmlElement * elem);
		void LoadXML(TiXmlElement * elem);
		std::vector<RoadNode*> m_Nodes;
		std::vector<RoadEdge*> m_Edges;
	private:
		RoadNode* InsertNodeOnEdge(const Vec3& point,RoadEdge* edge);
		RoadEdge* GetCloesestEdge(const Vec3 &point);
		RoadNode* GetCloesestNode(const Vec3 &point);
		void RemoveNode(RoadNode* node);
	};

	
}
#endif