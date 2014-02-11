#include <limits>
#include "RoadNetwork.h"
#include "RoadNavigation.h"
#include <algorithm>
#include <tinyxml.h>
#include <sstream>

namespace GASS
{

	RoadEdge::~RoadEdge()
	{
		//remove this from start and end node
		if(StartNode)
			StartNode->RemoveEdge(this);
		if(EndNode)
			EndNode->RemoveEdge(this);
	}


	/* Helper class to generate lanes to road network*/
	class LaneBuilder
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
			EdgeConnection(RoadNode* node, RoadEdge* edge): Node(node),Edge(edge)
			{
				_Init();
			}
			~EdgeConnection(){}

			void ClipPaths(EdgeLine clip_left, EdgeLine clip_right)
			{
				for(size_t i = 0; i < Edge->LLWaypoints.size() ; i++)
				{
					SetLeftPath(_Clip(GetLeftPath(i), clip_left),i);
				}

				for(size_t i = 0; i < Edge->RLWaypoints.size() ; i++)
				{
					SetRightPath(_Clip(GetRightPath(i), clip_right),i);
				}
			}
			double GetAngle() const {return Angle;}
			EdgeLine GetLeftClip() const {return LeftClip;}
			EdgeLine GetRightClip() const {return RightClip;}
		private:
			RoadEdge* Edge;
			RoadNode* Node;
			double Angle;
			EdgeLine Center;
			EdgeLine LeftClip;
			EdgeLine RightClip;

			EdgeLine _LineOffset(Float offset, const EdgeLine &line)
			{
				Vec3 dir = line.p2 - line.p1;
				Float temp = dir.z;
				dir.z = dir.x;
				dir.x = -temp;
				dir.y = 0;
				dir.Normalize();
				EdgeLine offset_line;
				offset_line.p1 = line.p1 -  dir * offset;
				offset_line.p2 = line.p2 -  dir * offset;
				return offset_line;
			}

			void _Init()
			{
				std::vector<Vec3> wps = Edge->Waypoints;
				if(Node != Edge->StartNode)
				{
					Center.p1 = wps[wps.size()-1];
					Center.p2 = wps[wps.size()-2];
				}
				else
				{
					Center.p1 = wps[0];
					Center.p2 = wps[1];
				}
				Vec3 dir = Center.p2 - Center.p1;
				dir.y = 0;
				dir.Normalize();
				dir = dir * Edge->LaneWidth*4;
				Center.p1 = Center.p1-dir;
				//Edge = network.m_Nodes[i]->Edges[j]; 
				Angle = atan2(dir.z,dir.x);
				LeftClip = _LineOffset(Edge->LeftLanes*Edge->LaneWidth, Center);
				//Node = network.m_Nodes[i];
				RightClip = _LineOffset(-Edge->RightLanes*Edge->LaneWidth, Center);
			}

			std::vector<Vec3> GetLeftPath(size_t i) const
			{
				if(Edge->StartNode == Node)
				{
					std::vector<Vec3> ret = Edge->LLWaypoints[i];
					std::reverse(ret.begin(),ret.end());
					return ret;
				}
				else
				{
					return Edge->RLWaypoints[i];
				}
			}

			void SetLeftPath(const std::vector<Vec3> &path, size_t i)
			{
				if(Edge->StartNode == Node)
				{
					Edge->LLWaypoints[i] = path;
					std::reverse(Edge->LLWaypoints[i].begin(),Edge->LLWaypoints[i].end());
				}
				else
				{
					Edge->RLWaypoints[i] = path;
				}
			}

			std::vector<Vec3> GetRightPath(size_t i) const
			{
				if(Edge->StartNode == Node)
				{
					std::vector<Vec3> ret = Edge->RLWaypoints[i];
					std::reverse(ret.begin(),ret.end());
					return ret;
				}
				else
				{
					return Edge->LLWaypoints[i];
				}
			}

			void SetRightPath(const std::vector<Vec3> &path, size_t i)
			{
				if(Edge->StartNode == Node)
				{
					Edge->RLWaypoints[i] = path;
					std::reverse(Edge->RLWaypoints[i].begin(),Edge->RLWaypoints[i].end());
				}
				else
				{
					Edge->LLWaypoints[i] = path;
				}
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

		static bool EdgeSort(const EdgeConnection &edge1, const EdgeConnection &edge2)
		{
			return edge1.GetAngle() < edge2.GetAngle();
		}

		void Extend(std::vector<Vec3> &waypoints, Float dist)
		{
			Vec3 dir = waypoints[0] - waypoints[1];
			dir.y = 0;
			dir.Normalize();
			waypoints[0] = waypoints[0] + dir*dist;

			dir = waypoints[waypoints.size()-1] - waypoints[waypoints.size()-2];
			dir.y = 0;
			dir.Normalize();
			waypoints[waypoints.size()-1] = waypoints[waypoints.size()-1] + dir*dist;
		}

		void CreateLanes(const RoadNetwork &network)
		{
			for(size_t i = 0; i < network.m_Edges.size() ; i++)
			{
				RoadEdge* e = network.m_Edges[i];
				e->LLWaypoints.clear();
				for(size_t j = 0; j < e->LeftLanes; j++)
				{
					e->LLWaypoints.push_back(Math::GenerateOffset(e->Waypoints,(e->LaneWidth*(j+1))));
					//extend end points to support better clipping
					//Extend(e->LLWaypoints[j], e->LaneWidth*e->LeftLanes*4);
				}

				e->RLWaypoints.clear();
				for(size_t j = 0; j < e->RightLanes; j++)
				{
					e->RLWaypoints.push_back(Math::GenerateOffset(e->Waypoints,-(e->LaneWidth*(j+1))));
					//Extend(e->RLWaypoints[j],e->LaneWidth*e->RightLanes*4);
				}
			}

			/*for(size_t i = 0; i < network.m_Nodes.size();i++)
			{
				std::vector<EdgeConnection> sorted_edges;
				for(size_t j = 0; j < network.m_Nodes[i]->Edges.size();j++)
				{
					EdgeConnection ec(network.m_Nodes[i],network.m_Nodes[i]->Edges[j]);
					sorted_edges.push_back(ec);
				}

				std::sort(sorted_edges.begin(), sorted_edges.end(), EdgeSort);

				for(size_t j = 0; j < sorted_edges.size();j++)
				{
					//bool clip_start = false;
					size_t next_j = (j+1)%sorted_edges.size();
					size_t prev_j = (j-1);
					if(j == 0)
						prev_j = sorted_edges.size()-1;
					sorted_edges[j].ClipPaths(sorted_edges[next_j].GetLeftClip(), sorted_edges[prev_j].GetRightClip());
				}
			}*/
		}
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

	void RoadNetwork::GenerateLanes()
	{
		LaneBuilder builder;
		builder.CreateLanes(*this);
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
			path = Math::GenerateOffset(path,from_edge->LaneWidth*0.5);
			
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
						edge_path = Math::GenerateOffset(edge_path,edge->LaneWidth*0.5);

						if(edge_path.size() > 1 && path.size() > 1)
						{
							Vec3 p1 = path[path.size()-2];
							Vec3 p2 = path[path.size()-1];


							//extend p2 
							Vec3 dir = p2 - p1;
							dir.Normalize();
							p2 = p1 + dir*20;

							Vec3 p3 = edge_path[0];
							Vec3 p4 = edge_path[1];

							//extend p4 
							dir = p4 - p3;
							dir.Normalize();
							p4 = p3 + dir*20;

							Vec2 isect;
							if(Math::GetLineIntersection(Vec2(p1.x,p1.z),Vec2(p2.x,p2.z), Vec2(p3.x ,p3.z), Vec2(p4.x, p4.z), isect))
							{
								Vec3 new_p(isect.x,p1.y,isect.y);							
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
		for(size_t i = 0; i < path.size(); i++)
		{
			if(i > 0 && (path[i-1] - path[i]).Length() > 0.01) 
				final_path.push_back(path[i]);
		}
		//final_path = Math::GenerateOffset(final_path,2);
		return final_path;
	}

	void RoadNetwork::_RemoveNode(RoadNode* node)
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
				iter++;
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
				iter++;
		}
	}

	void RoadNetwork::_ConvertNodeToWaypoint(RoadNode* node)
	{
		std::vector<Vec3>  wps1 = node->Edges[0]->Waypoints;
		std::vector<Vec3>  wps2 = node->Edges[1]->Waypoints;
		RoadNode* start_node = NULL;
		RoadNode* end_node = NULL;
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
		edge->Distance =  Math::GetPathLength(edge->Waypoints);
		edge->StartNode = start_node;
		edge->EndNode = end_node;
		start_node->Edges.push_back(edge);
		end_node->Edges.push_back(edge);
		m_Edges.push_back(edge);
		//remove old node
		
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
		edge1->LaneWidth = edge->LaneWidth;
		edge1->LeftLanes = edge->LeftLanes;
		edge1->RightLanes = edge->RightLanes;
		new_node->Edges.push_back(edge1);
		edge->StartNode->Edges.push_back(edge1);

		edge2->Distance =  Math::GetPathLength(edge2->Waypoints);
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
			edge_prop_elem ->SetAttribute("LaneWidth",m_Edges[i]->LaneWidth);
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
					edge_elem->QueryDoubleAttribute("LaneWidth",&edge->LaneWidth);
					edge_elem->QueryDoubleAttribute("Distance",&edge->Distance);
					
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
			GenerateLanes();
		}
	}
}
