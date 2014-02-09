#include <limits>
#include "RoadNetwork.h"
#include "RoadNavigation.h"
#include <algorithm>
#include <tinyxml.h>
#include <sstream>

namespace GASS
{
	/* Helper class to generate intersections from road network*/
	class IntersectionBuilder
	{
	public:
			/* Helper class to represent a line */
			struct EdgeLine
			{
				Vec3 p1;
				Vec3 p2;
			};

			class EdgeConnection
			{
			public:
				EdgeConnection(){}
				~EdgeConnection(){}
				RoadEdge* Edge;
				RoadNode* Node;

				double Angle;
				EdgeLine Center;
				EdgeLine c1;
				EdgeLine c2;

				std::vector<Vec3> GetLeftPath() const
				{
					if(Edge->StartNode == Node)
					{
						std::vector<Vec3> ret = Edge->Lane1;
						std::reverse(ret.begin(),ret.end());
						return ret;
					}
					else
					{
						return Edge->Lane2;
					}
				}

				void SetLeftPath(const std::vector<Vec3> &path)
				{
					if(Edge->StartNode == Node)
					{
						Edge->Lane1 = path;
						std::reverse(Edge->Lane1.begin(),Edge->Lane1.end());
					}
					else
					{
						Edge->Lane2 = path;
					}
				}

				std::vector<Vec3> GetRightPath() const
				{
					if(Edge->StartNode == Node)
					{
						std::vector<Vec3> ret = Edge->Lane2;
						std::reverse(ret.begin(),ret.end());
						return ret;
					}
					else
					{
						return Edge->Lane1;
					}
				}

				void SetRightPath(const std::vector<Vec3> &path)
				{
					if(Edge->StartNode == Node)
					{
						Edge->Lane2 = path;
						std::reverse(Edge->Lane2.begin(),Edge->Lane2.end());
					}
					else
					{
						Edge->Lane1 = path;
					}
				}

				void ClipPaths(EdgeLine clip_left, EdgeLine clip_right)
				{
					SetLeftPath(_Clip(GetLeftPath(), clip_left));
					SetRightPath(_Clip(GetRightPath(), clip_right));
				}

				std::vector<Vec3> _Clip(std::vector<Vec3> path,EdgeLine line)
				{
					std::vector<Vec3> out;
					Vec2 p1(line.p1.x,line.p1.z);
					Vec2 p2(line.p2.x,line.p2.z);
					out.push_back(path[0]);
					for(size_t i = 1; i  < path.size(); i++)
					{
						Vec2 p3(path[i-1].x,path[i-1].z);
						Vec2 p4(path[i].x,path[i].z);
						Vec2 isect;
						if(Math::GetLineIntersection(p1, p2, p3, p4, isect))
						{
							out.push_back(Vec3(isect.x,path[i].y,isect.y));	
							break;
						}
						else
							out.push_back(path[i]);
					}
					return out;
				}
			};
		};

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

		std::vector<Vec3> RoadNetwork::Search(const Vec3 &from_point,const Vec3 &to_point)
		{
			std::vector<Vec3> path;
			path.push_back(from_point);
			RoadEdge* from_edge = GetCloesestEdge(from_point);
			RoadEdge* to_edge = GetCloesestEdge(to_point);

			if(from_edge && from_edge == to_edge) //no need to path find!
			{
				Vec3 start_point,end_point;
				int start_seg_index,end_seg_index;
				Math::GetClosestPointOnPath(from_point,from_edge->Waypoints, start_seg_index, start_point);
				Math::GetClosestPointOnPath(to_point,from_edge->Waypoints, end_seg_index, end_point);

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

				//add edge offset
				//edge_path = Math::GenerateOffset(edge_path,2);
				for(size_t j = 0; j < edge_path.size(); j++)
				{
					path.push_back(edge_path[j]);
				}

				path.push_back(end_point);
			}
			else if(from_edge && to_edge)
			{
				RoadNode* start_node = InsertNodeOnEdge(from_point,from_edge);
				RoadNode* end_node = InsertNodeOnEdge(to_point,to_edge);
				//do search!
				RoadNavigation nav;
				std::vector<RoadNode*> node_path;
				int ret = nav.GetPath(start_node, end_node, node_path);

				//std::vector<RoadNode*> edge_path;
				if(node_path.size() > 0)
				{
					for(size_t i = 0; i < node_path.size()-1; i++)
					{
						RoadNode* n1 = node_path[i];
						RoadNode* n2 = node_path[i+1];
						//Get edge!
						RoadEdge* edge = NULL;
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
								std::reverse(edge_path.begin(),edge_path.end());
							}

							//add edge offset
							//edge_path = Math::GenerateOffset(edge_path,2);

							for(size_t j = 0; j < edge_path.size(); j++)
							{
								path.push_back(edge_path[j]);
							}
						}
					}
				}

				//remove temp nodes
				RemoveNode(start_node);
				RemoveNode(end_node);
				from_edge->Enabled = true;
				to_edge->Enabled = true;
			}
			path.push_back(to_point);
			//trim 

			std::vector<Vec3> final_path;
			for(size_t i = 0; i < path.size(); i++)
			{
				if(i > 0 && (path[i-1] -path[i]).Length() > 0.01) 
					final_path.push_back(path[i]);
			}
			final_path = Math::GenerateOffset(final_path,2);
			return final_path ;
		}

		void RoadNetwork::RemoveNode(RoadNode* node)
		{
			for(size_t i = 0; i < node->Edges.size(); i++)
			{
				std::vector<RoadEdge*>::iterator iter = node->Edges[i]->EndNode->Edges.begin();
				while(iter != node->Edges[i]->EndNode->Edges.end())
				{
					if((*iter)->StartNode == node)
					{
						iter = node->Edges[i]->EndNode->Edges.erase(iter);
					}
					else
						iter++;
				}
				delete node->Edges[i];
			}
			delete node;
		}

		RoadNode* RoadNetwork::InsertNodeOnEdge(const Vec3& point,RoadEdge* edge)
		{
			int seg_index;
			Vec3 target_point;

			Math::GetClosestPointOnPath(point,edge->Waypoints, seg_index, target_point);

			RoadEdge* edge1 = new RoadEdge();
			RoadEdge* edge2 = new RoadEdge();

			RoadNode* new_node = new RoadNode();
			new_node->Position = target_point;
			edge1->Waypoints.push_back(target_point);
			edge2->Waypoints.push_back(target_point);

			for(size_t i = 0;  i < seg_index+1; i++)
			{
				edge1->Waypoints.push_back(edge->Waypoints[seg_index - i]);
			}

			for(size_t i = seg_index+1;  i < edge->Waypoints.size(); i++)
			{
				edge2->Waypoints.push_back(edge->Waypoints[i]);
			}

			edge1->Distance =  Math::GetPathLength(edge1->Waypoints);
			edge1->StartNode = new_node;
			edge1->EndNode = edge->StartNode;
			new_node->Edges.push_back(edge1);
			edge->StartNode->Edges.push_back(edge1);

			edge2->Distance =  Math::GetPathLength(edge2->Waypoints);
			edge2->StartNode = new_node;
			edge2->EndNode = edge->EndNode;
			new_node->Edges.push_back(edge2);
			edge->EndNode->Edges.push_back(edge2);
			edge->Enabled = false;
			return new_node;
		}

		RoadEdge* RoadNetwork::GetCloesestEdge(const Vec3 &point)
		{
			int seg_index;
			RoadEdge* best_edge = NULL;
			Vec3 target_point;
			Vec3 best_target_point;
			Float min_dist  = FLT_MAX;//std::numeric_limits<Float>::max();
			for(size_t i = 0; i < m_Edges.size();i++)
			{
				if(Math::GetClosestPointOnPath(point,m_Edges[i]->Waypoints, seg_index, target_point))
				{
					Float dist = (target_point - point).Length();
					if(dist < min_dist )
					{
						min_dist = dist;
						best_target_point = target_point;
						best_edge = m_Edges[i];
					}
				}
			}
			return best_edge;
		}

		RoadNode* RoadNetwork::GetCloesestNode(const Vec3 &point)
		{
			RoadNode* best_node = NULL;
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

		void RoadNetwork::SaveXML(TiXmlElement * elem)
		{
			TiXmlElement * net_elem = new TiXmlElement("RoadNetwork");  
			elem->LinkEndChild( net_elem);

			TiXmlElement * nodes_elem = new TiXmlElement("Nodes");  
			net_elem->LinkEndChild( nodes_elem);  
			for(size_t i = 0; i < m_Nodes.size();i++)
			{
				int id = (int) (m_Nodes[i]);
				TiXmlElement *node_prop_elem = new TiXmlElement( "Node");
				nodes_elem->LinkEndChild( node_prop_elem);  
				node_prop_elem->SetAttribute("ID", id);
				std::string pos = m_Nodes[i]->Position.ToString(" ");
				node_prop_elem->SetAttribute("Position",pos.c_str());
			}
			TiXmlElement *edges_elem = new TiXmlElement( "Edges");
			net_elem->LinkEndChild( edges_elem);  
			for(size_t i = 0; i < m_Edges.size();i++)
			{
				TiXmlElement *edge_prop_elem = new TiXmlElement( "Edge");
				edges_elem->LinkEndChild( edge_prop_elem);  
				edge_prop_elem ->SetAttribute("Dir",(int) m_Edges[i]->Dir);
				edge_prop_elem ->SetAttribute("StartNode",(int) m_Edges[i]->StartNode);
				edge_prop_elem ->SetAttribute("EndNode",(int) m_Edges[i]->EndNode);
				edge_prop_elem ->SetAttribute("Distance",m_Edges[i]->Distance);
				TiXmlElement *wps_elem = new TiXmlElement( "Waypoints");
				edge_prop_elem->LinkEndChild( wps_elem);  
				for(size_t j = 0; j < m_Edges[i]->Waypoints.size(); j++)
				{
					TiXmlElement *wp_prop_elem = new TiXmlElement( "WP");
					wps_elem->LinkEndChild( wp_prop_elem);  
					std::string pos = m_Edges[i]->Waypoints[j].ToString(" ");
					wp_prop_elem->SetAttribute("Position",pos.c_str());
				}
			}
		}

		void RoadNetwork::LoadXML(TiXmlElement * elem)
		{
			std::map<int,RoadNode*> mapping;
			TiXmlElement *net_elem = elem->FirstChildElement("RoadNetwork");
			if(net_elem)
			{
				TiXmlElement *nodes_elem = net_elem->FirstChildElement("Nodes");
				if(nodes_elem)
				{
					TiXmlElement *node_elem = nodes_elem->FirstChildElement("Node");
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
				TiXmlElement *edges_elem = net_elem->FirstChildElement("Edges");
				if(edges_elem)
				{
					TiXmlElement *edge_elem = edges_elem->FirstChildElement("Edge");
					while(edge_elem)
					{
						RoadEdge* edge = new RoadEdge();
						int sn_id,en_id;
						edge_elem->QueryIntAttribute("StartNode",&sn_id);
						edge_elem->QueryIntAttribute("EndNode",&en_id);
						edge->StartNode = mapping[sn_id];
						edge->EndNode = mapping[en_id];
						edge->StartNode->Edges.push_back(edge);
						edge->EndNode->Edges.push_back(edge);

						TiXmlElement *wps_elem = edge_elem->FirstChildElement("Waypoints");
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
			}
		}
	}
