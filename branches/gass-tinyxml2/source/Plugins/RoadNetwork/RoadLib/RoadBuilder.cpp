#include "RoadBuilder.h"
#include "RoadEdge.h"
#include "RoadNode.h"
#include "RoadNetwork.h"
namespace GASS
{

	RoadBuilder::EdgeConnection::EdgeConnection(RoadNode* node, RoadEdge* edge): Node(node),Edge(edge)
	{
		_Init();
	}
	RoadBuilder::EdgeConnection::~EdgeConnection()
	{

	}

	void RoadBuilder::EdgeConnection::ClipPaths(EdgeLine clip_left, EdgeLine clip_right)
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

	RoadBuilder::EdgeLine RoadBuilder::EdgeConnection::_LineOffset(Float offset, const EdgeLine &line)
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

	void RoadBuilder::EdgeConnection::_Init()
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

	std::vector<Vec3> RoadBuilder::EdgeConnection::GetLeftPath(size_t i) const
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

	void RoadBuilder::EdgeConnection::SetLeftPath(const std::vector<Vec3> &path, size_t i)
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

	std::vector<Vec3> RoadBuilder::EdgeConnection::GetRightPath(size_t i) const
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

	void RoadBuilder::EdgeConnection::SetRightPath(const std::vector<Vec3> &path, size_t i)
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

	std::vector<Vec3> RoadBuilder::EdgeConnection::_Clip(std::vector<Vec3> path,EdgeLine line)
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



	void RoadBuilder::Extend(std::vector<Vec3> &waypoints, Float dist)
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

		void RoadBuilder::CreateLanes(const RoadNetwork &network)
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
}
