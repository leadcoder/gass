#include "RoadBuilder.h"
#include "Core/Math/GASSMath.h"
#include "Core/Math/GASSPath.h"
#include "Core/Math/GASSLineSegment2D.h"
#include "RoadEdge.h"
#include "RoadNode.h"
#include "RoadNetwork.h"
namespace GASS
{

	RoadBuilder::EdgeConnection::EdgeConnection(RoadNode* node, RoadEdge* edge): m_Node(node),m_Edge(edge)
	{
		Init();
	}
	RoadBuilder::EdgeConnection::~EdgeConnection()
	{

	}

	void RoadBuilder::EdgeConnection::ClipPaths(EdgeLine clip_left, EdgeLine clip_right)
	{
		for(size_t i = 0; i < m_Edge->LLWaypoints.size() ; i++)
		{
			SetLeftPath(Clip(GetLeftPath(i), clip_left),i);
		}

		for(size_t i = 0; i < m_Edge->RLWaypoints.size() ; i++)
		{
			SetRightPath(Clip(GetRightPath(i), clip_right),i);
		}
	}

	RoadBuilder::EdgeLine RoadBuilder::EdgeConnection::LineOffset(Float offset, const EdgeLine &line)
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

	void RoadBuilder::EdgeConnection::Init()
	{
		std::vector<Vec3> wps = m_Edge->Waypoints;
		if(m_Node != m_Edge->StartNode)
		{
			m_Center.p1 = wps[wps.size()-1];
			m_Center.p2 = wps[wps.size()-2];
		}
		else
		{
			m_Center.p1 = wps[0];
			m_Center.p2 = wps[1];
		}
		Vec3 dir = m_Center.p2 - m_Center.p1;
		dir.y = 0;
		dir.Normalize();
		dir = dir * m_Edge->LaneWidth*4;
		m_Center.p1 = m_Center.p1-dir;
		//Edge = network.m_Nodes[i]->Edges[j];
		m_Angle = atan2(dir.z,dir.x);
		m_LeftClip = LineOffset(m_Edge->LeftLanes*m_Edge->LaneWidth, m_Center);
		//Node = network.m_Nodes[i];
		m_RightClip = LineOffset(-m_Edge->RightLanes*m_Edge->LaneWidth, m_Center);
	}

	std::vector<Vec3> RoadBuilder::EdgeConnection::GetLeftPath(size_t i) const
	{
		if(m_Edge->StartNode == m_Node)
		{
			std::vector<Vec3> ret = m_Edge->LLWaypoints[i];
			std::reverse(ret.begin(),ret.end());
			return ret;
		}
		else
		{
			return m_Edge->RLWaypoints[i];
		}
	}

	void RoadBuilder::EdgeConnection::SetLeftPath(const std::vector<Vec3> &path, size_t i)
	{
		if(m_Edge->StartNode == m_Node)
		{
			m_Edge->LLWaypoints[i] = path;
			std::reverse(m_Edge->LLWaypoints[i].begin(),m_Edge->LLWaypoints[i].end());
		}
		else
		{
			m_Edge->RLWaypoints[i] = path;
		}
	}

	std::vector<Vec3> RoadBuilder::EdgeConnection::GetRightPath(size_t i) const
	{
		if(m_Edge->StartNode == m_Node)
		{
			std::vector<Vec3> ret = m_Edge->RLWaypoints[i];
			std::reverse(ret.begin(),ret.end());
			return ret;
		}
		else
		{
			return m_Edge->LLWaypoints[i];
		}
	}

	void RoadBuilder::EdgeConnection::SetRightPath(const std::vector<Vec3> &path, size_t i)
	{
		if(m_Edge->StartNode == m_Node)
		{
			m_Edge->RLWaypoints[i] = path;
			std::reverse(m_Edge->RLWaypoints[i].begin(),m_Edge->RLWaypoints[i].end());
		}
		else
		{
			m_Edge->LLWaypoints[i] = path;
		}
	}

	std::vector<Vec3> RoadBuilder::EdgeConnection::Clip(std::vector<Vec3> path, EdgeLine line)
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
			if(LineSegment2Dd::GetIntersection(LineSegment2Dd(p1,p2), LineSegment2Dd(p3, p4), isect))
			{
				out.emplace_back(isect.x,path[i].y,isect.y);
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
				for(int j = 0; j < e->LeftLanes; j++)
				{
					e->LLWaypoints.push_back(Path::GenerateOffset(e->Waypoints, (e->LaneWidth*(j + 1))));
					//extend end points to support better clipping
					//Extend(e->LLWaypoints[j], e->LaneWidth*e->LeftLanes*4);
				}

				e->RLWaypoints.clear();
				for(int j = 0; j < e->RightLanes; j++)
				{
					e->RLWaypoints.push_back(Path::GenerateOffset(e->Waypoints, -(e->LaneWidth*(j + 1))));
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
