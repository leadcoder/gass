
#ifndef ROAD_NETWORK
#define ROAD_NETWORK

#include "Core/Utils/GASSFilePath.h"
#include "Core/Math/GASSMath.h"
#include "Core/Math/GASSVector.h"
#include "micropather.h"
#include <set>

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
		void Load(const std::string &filename);
		void Save(const std::string &filename);
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
		void _RemoveNode(RoadNode* node) const;
		void _ConvertNodeToWaypoint(RoadNode* node);
	};
}
#endif