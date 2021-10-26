
#ifndef ROAD_NETWORK
#define ROAD_NETWORK

#include "Core/Math/GASSVector.h"
#include "micropather.h"

namespace tinyxml2
{
	class XMLElement;
}

namespace GASS
{
	class RoadNode;
	class RoadEdge;
	class RoadNetwork 	
	{
	public:
		RoadNetwork(void);
		~RoadNetwork(void);
		void Clear();
		void GenerateLanes();
		void ConvertNodesToWaypoint();
		std::vector<Vec3> Search(const Vec3 &from_point,const Vec3 &to_point) const;
		bool GetClosestRoadPoint(const Vec3 &point, Vec3 &closest_point) const;
		//void Load(const std::string &filename);
		//void Save(const std::string &filename);
		void SaveXML(tinyxml2::XMLElement * elem);
		void LoadXML(tinyxml2::XMLElement * elem);

		void RemoveNode(RoadNode* node);
		void RemoveEdge(RoadEdge* node);
		std::vector<RoadNode*> m_Nodes;
		std::vector<RoadEdge*> m_Edges;
	private:
		RoadNode* InsertNodeOnEdge(const Vec3& point,RoadEdge* edge) const;
		RoadEdge* GetCloesestEdge(const Vec3 &point) const;
		RoadNode* GetCloesestNode(const Vec3 &point) const;
		void RemoveNode(RoadNode* node) const;
		void ConvertNodeToWaypoint(RoadNode* node);
	};
}
#endif
