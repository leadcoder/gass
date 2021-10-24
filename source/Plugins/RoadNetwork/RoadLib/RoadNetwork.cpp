#include <limits>

#include "RoadNetwork.h"
#include "RoadNode.h"
#include "RoadEdge.h"
#include "RoadNavigation.h"
#include "RoadBuilder.h"
#include "Core/Math/GASSMath.h"
#include "Core/Math/GASSPath.h"
#include "Core/Math/GASSLineSegment2D.h"
#include "Core/Serialize/tinyxml2.h"
#include <sstream>

namespace GASS
{
	RoadNetwork::RoadNetwork(void)
	{

	}

	RoadNetwork::~RoadNetwork(void)
	{
		Clear();
	}

	void RoadNetwork::Clear()
	{
		for(size_t i = 0; i < m_Edges.size(); i++)
		{
			delete m_Edges[i];
		}
		m_Edges.clear();

		for(size_t i = 0; i < m_Nodes.size();i++)
		{
			delete m_Nodes[i];
		}
		m_Nodes.clear();
	}

	void RoadNetwork::GenerateLanes()
	{
		RoadBuilder builder;
		builder.CreateLanes(*this);
	}


	bool RoadNetwork::GetClosestRoadPoint(const Vec3 &point, Vec3 &closest_point) const
	{
		bool success = false;
		if(RoadEdge* edge = GetCloesestEdge(point))
		{
			int seg_index;
			success = Path::GetClosestPointOnPath(point, edge->Waypoints, seg_index, closest_point);
		}
		return success;
	}

	std::vector<Vec3> RoadNetwork::Search(const Vec3 &from_point,const Vec3 &to_point) const
	{
		std::vector<Vec3> path;
		path.push_back(from_point);
		RoadEdge* from_edge = GetCloesestEdge(from_point);
		RoadEdge* to_edge = GetCloesestEdge(to_point);

		if(from_edge && from_edge == to_edge) //no need to path find!
		{
			Vec3 start_point,end_point;
			int start_seg_index,end_seg_index;
			Path::GetClosestPointOnPath(from_point,from_edge->Waypoints, start_seg_index, start_point);
			Path::GetClosestPointOnPath(to_point, from_edge->Waypoints, end_seg_index, end_point);

			path.push_back(start_point);


			std::vector<Vec3> edge_path;

			if(start_seg_index < end_seg_index)
			{
				for(int i = start_seg_index+1; i <= end_seg_index; i++)
				{
					edge_path.push_back(from_edge->Waypoints[i]);
				}
			}
			else if(start_seg_index > end_seg_index)
			{
				for(int i = start_seg_index; i > end_seg_index; i--)
				{
					edge_path.push_back(from_edge->Waypoints[i]);
				}
			}
			else //same segment
			{

			}



			for(size_t j = 0; j < edge_path.size(); j++)
			{
				path.push_back(edge_path[j]);
			}
			path.push_back(end_point);
			//add edge offset
			path = Path::GenerateOffset(path,from_edge->LaneWidth*0.5);

		}
		else if(from_edge && to_edge)
		{
			RoadNode* start_node = InsertNodeOnEdge(from_point,from_edge);
			RoadNode* end_node = InsertNodeOnEdge(to_point,to_edge);
			//do search!
			RoadNavigation nav;
			std::vector<RoadNode*> node_path;
			/*int ret = */nav.GetPath(start_node, end_node, node_path);

			//std::vector<RoadNode*> edge_path;
			if(node_path.size() > 0)
			{
				for(size_t i = 0; i < node_path.size()-1; i++)
				{
					RoadNode* n1 = node_path[i];
					RoadNode* n2 = node_path[i+1];
					//Get edge!
					RoadEdge* edge = nullptr;
					bool invert_dir = false;
					for(size_t j = 0; j < n1->Edges.size(); j++)
					{
						if(n1->Edges[j]->EndNode == n2 )
						{
							edge = n1->Edges[j];
							break;
						}

						if(n1->Edges[j]->StartNode == n2 )
						{
							invert_dir = true;
							edge = n1->Edges[j];
							break;
						}
					}
					if(edge)
					{
						std::vector<Vec3> edge_path;
						edge_path = edge->Waypoints;
						if(invert_dir)
						{
							//if(edge->LLWaypoints.size() >  0)
							//	edge_path = edge->LLWaypoints[0];
							std::reverse(edge_path.begin(),edge_path.end());
						}
						else
						{
							//if(edge->RLWaypoints.size() >  0)
							//	edge_path = edge->RLWaypoints[0];
						}

						//add edge offset
						edge_path = Path::GenerateOffset(edge_path, edge->LaneWidth*0.5);

						if(edge_path.size() > 1 && path.size() > 1)
						{
							Vec3 p1 = path[path.size()-2];
							Vec3 p2 = path[path.size()-1];


							//extend p2
							Vec3 dir = p2 - p1;
							dir.Normalize();
							p2 = p2 + dir*20;

							Vec3 p3 = edge_path[0];
							Vec3 p4 = edge_path[1];

							//extend p4
							dir = p4 - p3;
							dir.Normalize();
							p3 = p3 - dir*20;

							Vec2 isect;
							if (LineSegment2Dd::GetIntersection(LineSegment2Dd(Vec2(p1.x, p1.z), Vec2(p2.x, p2.z)), 
								LineSegment2Dd(Vec2(p3.x, p3.z), Vec2(p4.x, p4.z)), 
								isect))
							{
								Vec3 new_p(isect.x,p3.y,isect.y);
								path[path.size()-1]  = new_p;
								edge_path[0] = new_p;
							}
						}

						for(size_t j = 0; j < edge_path.size(); j++)
						{
							path.push_back(edge_path[j]);
						}
					}
				}
			}

			//remove temp nodes
			_RemoveNode(start_node);
			_RemoveNode(end_node);
			from_edge->Enabled = true;
			to_edge->Enabled = true;
		}
		path.push_back(to_point);
		//trim
		std::vector<Vec3> final_path;

		final_path.push_back(path[0]);
		for(size_t i = 0; i < path.size(); i++)
		{
			if (i > 0)
			{
				Float segment_len = (path[i - 1] - path[i]).Length();
				Float tolerance = 0.01;
#ifdef TEST_HIGH_TOLERANCE_LAST_WP
				//higher tolerance for last waypoint to avoid offroad behavior at end points
				if (i == path.size() - 1)
				{
					tolerance = 1.0;
				}
#endif				
				if (segment_len > tolerance)
					final_path.push_back(path[i]);
			}
		}
		//final_path = Math::GenerateOffset(final_path,2);
		return final_path;
	}

	void RoadNetwork::_RemoveNode(RoadNode* node) const
	{
		std::vector<RoadEdge*> edges = node->Edges;
		for(size_t i = 0; i < edges.size(); i++)
		{
			delete edges[i];
		}
		delete node;
	}


	void RoadNetwork::RemoveEdge(RoadEdge* edge)
	{
		std::vector<RoadEdge*>::iterator iter =  m_Edges.begin();
		while(iter != m_Edges.end())
		{
			if(*iter == edge)
				iter = m_Edges.erase(iter);
			else
				++iter;
		}
	}

	void RoadNetwork::RemoveNode(RoadNode* node)
	{
		std::vector<RoadNode*>::iterator iter =  m_Nodes.begin();
		while(iter != m_Nodes.end())
		{
			if(*iter == node)
				iter = m_Nodes.erase(iter);
			else
				++iter;
		}
	}

	void RoadNetwork::_ConvertNodeToWaypoint(RoadNode* node)
	{
		std::vector<Vec3>  wps1 = node->Edges[0]->Waypoints;
		std::vector<Vec3>  wps2 = node->Edges[1]->Waypoints;
		RoadNode* start_node = nullptr;
		RoadNode* end_node = nullptr;
		if(node->Edges[0]->StartNode == node)
		{
			std::reverse(wps1.begin(),wps1.end());
			start_node = node->Edges[0]->EndNode;
		}
		else
			start_node = node->Edges[0]->StartNode;

		std::vector<Vec3>  final_wps = wps1;

		if(node->Edges[1]->StartNode != node)
		{
			std::reverse(wps2.begin(),wps2.end());
			end_node = node->Edges[1]->StartNode;
		}
		else
			end_node = node->Edges[1]->EndNode;

		for(size_t i= 1; i < wps2.size() ; i++)
			final_wps.push_back(wps2[i]);

		RoadEdge* edge = new RoadEdge;
		edge->Waypoints = final_wps;
		edge->Distance = Path::GetPathLength(edge->Waypoints);
		edge->StartNode = start_node;
		edge->EndNode = end_node;
		edge->LaneWidth = node->Edges[0]->LaneWidth;
		edge->LeftLanes = node->Edges[0]->LeftLanes;
		edge->RightLanes = node->Edges[0]->RightLanes;
		start_node->Edges.push_back(edge);
		end_node->Edges.push_back(edge);
		m_Edges.push_back(edge);
		//remove node
		RemoveEdge(node->Edges[0]);
		RemoveEdge(node->Edges[1]);
		RoadEdge* edge0 = node->Edges[0];
		RoadEdge* edge1 = node->Edges[1];
		delete edge0;
		delete edge1;
		RemoveNode(node);
		delete node;
	}

	void RoadNetwork::ConvertNodesToWaypoint()
	{
		std::vector<RoadNode*> nodes = m_Nodes;
		for(size_t i = 0; i < nodes.size();i++)
		{
			if(nodes[i]->Edges.size() == 2 &&
				nodes[i]->Edges[0]->LaneWidth == nodes[i]->Edges[1]->LaneWidth &&
				nodes[i]->Edges[0]->LeftLanes == nodes[i]->Edges[1]->LeftLanes &&
				nodes[i]->Edges[0]->RightLanes == nodes[i]->Edges[1]->RightLanes)
				_ConvertNodeToWaypoint(nodes[i]);
		}
	}

	RoadNode* RoadNetwork::InsertNodeOnEdge(const Vec3& point,RoadEdge* edge) const
	{
		int seg_index;
		Vec3 target_point;

		Path::GetClosestPointOnPath(point, edge->Waypoints, seg_index, target_point);

		RoadEdge* edge1 = new RoadEdge();
		RoadEdge* edge2 = new RoadEdge();

		RoadNode* new_node = new RoadNode();
		new_node->Position = target_point;
		edge1->Waypoints.push_back(target_point);
		edge2->Waypoints.push_back(target_point);

		for(int i = 0;  i < seg_index+1; i++)
		{
			edge1->Waypoints.push_back(edge->Waypoints[seg_index - i]);
		}

		for(int i = seg_index+1;  i < static_cast<int>(edge->Waypoints.size()); i++)
		{
			edge2->Waypoints.push_back(edge->Waypoints[i]);
		}

		edge1->Distance = Path::GetPathLength(edge1->Waypoints);
		edge1->StartNode = new_node;
		edge1->EndNode = edge->StartNode;
		edge1->LaneWidth = edge->LaneWidth;
		edge1->LeftLanes = edge->LeftLanes;
		edge1->RightLanes = edge->RightLanes;
		new_node->Edges.push_back(edge1);
		edge->StartNode->Edges.push_back(edge1);

		edge2->Distance = Path::GetPathLength(edge2->Waypoints);
		edge2->StartNode = new_node;
		edge2->EndNode = edge->EndNode;
		edge2->LaneWidth = edge->LaneWidth;
		edge2->LeftLanes = edge->LeftLanes;
		edge2->RightLanes = edge->RightLanes;

		new_node->Edges.push_back(edge2);
		edge->EndNode->Edges.push_back(edge2);
		edge->Enabled = false;
		return new_node;
	}

	RoadEdge* RoadNetwork::GetCloesestEdge(const Vec3 &point) const
	{
		int seg_index;
		RoadEdge* best_edge = nullptr;
		Vec3 target_point;
		//Vec3 best_target_point;
		Float min_dist  = FLT_MAX;//std::numeric_limits<Float>::max();
		for(size_t i = 0; i < m_Edges.size();i++)
		{
			if (Path::GetClosestPointOnPath(point, m_Edges[i]->Waypoints, seg_index, target_point))
			{
				Float dist = (target_point - point).Length();
				if(dist < min_dist )
				{
					min_dist = dist;
					//best_target_point = target_point;
					best_edge = m_Edges[i];
				}
			}
		}
		return best_edge;
	}

	RoadNode* RoadNetwork::GetCloesestNode(const Vec3 &point) const
	{
		RoadNode* best_node = nullptr;
		Float min_dist  = FLT_MAX;//std::numeric_limits<Float>::max();
		for(size_t i = 0; i < m_Nodes.size();i++)
		{
			Float dist = (m_Nodes[i]->Position - point).Length();
			if(dist < min_dist )
			{
				min_dist = dist;
				best_node = m_Nodes[i];
			}
		}
		return best_node;
	}

	void RoadNetwork::SaveXML(tinyxml2::XMLElement * elem)
	{
		tinyxml2::XMLDocument *rootXMLDoc = elem->GetDocument();
		tinyxml2::XMLElement * net_elem = rootXMLDoc->NewElement("RoadNetwork");
		elem->LinkEndChild( net_elem);

		tinyxml2::XMLElement * nodes_elem = rootXMLDoc->NewElement("Nodes");
		net_elem->LinkEndChild( nodes_elem);
		for(size_t i = 0; i < m_Nodes.size();i++)
		{
			int id = GASS_PTR_TO_INT(m_Nodes[i]);
			tinyxml2::XMLElement *node_prop_elem = rootXMLDoc->NewElement( "Node");
			nodes_elem->LinkEndChild( node_prop_elem);
			node_prop_elem->SetAttribute("ID", id);
			std::string pos = m_Nodes[i]->Position.ToString();
			node_prop_elem->SetAttribute("Position",pos.c_str());
		}
		tinyxml2::XMLElement *edges_elem = rootXMLDoc->NewElement( "Edges");
		net_elem->LinkEndChild( edges_elem);
		for(size_t i = 0; i < m_Edges.size();i++)
		{
			tinyxml2::XMLElement *edge_prop_elem = rootXMLDoc->NewElement( "Edge");
			edges_elem->LinkEndChild( edge_prop_elem);
			edge_prop_elem ->SetAttribute("Dir",(int) m_Edges[i]->Dir);
			edge_prop_elem ->SetAttribute("StartNode",GASS_PTR_TO_INT(m_Edges[i]->StartNode));
			edge_prop_elem ->SetAttribute("EndNode",GASS_PTR_TO_INT(m_Edges[i]->EndNode));
			edge_prop_elem ->SetAttribute("Distance",m_Edges[i]->Distance);
			edge_prop_elem ->SetAttribute("LaneWidth",m_Edges[i]->LaneWidth);
			tinyxml2::XMLElement *wps_elem = rootXMLDoc->NewElement( "Waypoints");
			edge_prop_elem->LinkEndChild( wps_elem);
			for(size_t j = 0; j < m_Edges[i]->Waypoints.size(); j++)
			{
				tinyxml2::XMLElement *wp_prop_elem = rootXMLDoc->NewElement( "WP");
				wps_elem->LinkEndChild( wp_prop_elem);
				std::string pos = m_Edges[i]->Waypoints[j].ToString();
				wp_prop_elem->SetAttribute("Position",pos.c_str());
			}
		}
	}

	void RoadNetwork::LoadXML(tinyxml2::XMLElement * elem)
	{
		std::map<int,RoadNode*> mapping;
		tinyxml2::XMLElement *net_elem = elem->FirstChildElement("RoadNetwork");
		if(net_elem)
		{
			tinyxml2::XMLElement *nodes_elem = net_elem->FirstChildElement("Nodes");
			if(nodes_elem)
			{
				tinyxml2::XMLElement *node_elem = nodes_elem->FirstChildElement("Node");
				while(node_elem)
				{
					int id;
					RoadNode* node = new RoadNode();
					node_elem->QueryIntAttribute("ID",&id);
					std::string pos = node_elem->Attribute("Position");
					node_elem = node_elem->NextSiblingElement();
					std::stringstream ss(pos);
					ss >> node->Position;
					m_Nodes.push_back(node);
					mapping[id] = node;
				}
			}
			tinyxml2::XMLElement *edges_elem = net_elem->FirstChildElement("Edges");
			if(edges_elem)
			{
				tinyxml2::XMLElement *edge_elem = edges_elem->FirstChildElement("Edge");
				while(edge_elem)
				{
					RoadEdge* edge = new RoadEdge();
					int sn_id,en_id;
					edge_elem->QueryIntAttribute("StartNode",&sn_id);
					edge_elem->QueryIntAttribute("EndNode",&en_id);
					edge_elem->QueryDoubleAttribute("LaneWidth",&edge->LaneWidth);
					edge_elem->QueryDoubleAttribute("Distance",&edge->Distance);

					edge->StartNode = mapping[sn_id];
					edge->EndNode = mapping[en_id];
					edge->StartNode->Edges.push_back(edge);
					edge->EndNode->Edges.push_back(edge);

					tinyxml2::XMLElement *wps_elem = edge_elem->FirstChildElement("Waypoints");
					wps_elem = wps_elem->FirstChildElement("WP");
					while(wps_elem)
					{
						std::string pos_str = wps_elem->Attribute("Position");
						Vec3 pos;
						std::stringstream ss(pos_str);
						ss >> pos;
						edge->Waypoints.push_back(pos);
						wps_elem = wps_elem->NextSiblingElement();
					}
					m_Edges.push_back(edge);
					edge_elem = edge_elem->NextSiblingElement();
				}
			}
			GenerateLanes();
		}
	}
}
