#include "RoadSegmentComponent.h"
#include "AISceneManager.h"
#include "Plugins/Base/CoreMessages.h"
#include "Plugins/Game/GameMessages.h"
#include "RoadIntersectionComponent.h"


namespace GASS
{
	RoadSegmentComponent::RoadSegmentComponent(void) :
		m_Initialized(false)
	{
		
	}	

	RoadSegmentComponent::~RoadSegmentComponent(void)
	{

	}

	void RoadSegmentComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("RoadSegmentComponent",new Creator<RoadSegmentComponent, IComponent>);
		//REG_ATTRIBUTE(Vec2,RandomVelocity,RoadSegmentComponent)
		RegisterProperty<SceneObjectRef>("StartNode", &RoadSegmentComponent::GetStartNode, &RoadSegmentComponent::SetStartNode);
		RegisterProperty<SceneObjectRef>("EndNode", &RoadSegmentComponent::GetEndNode, &RoadSegmentComponent::SetEndNode);
	}

	void RoadSegmentComponent::SetStartNode(SceneObjectRef node)
	{
		if(m_StartNode.IsValid())
		{
			if(m_StartNode.GetRefObject() != node.GetRefObject())
			{
				m_StartNode->UnregisterForMessage(UNREG_TMESS(RoadSegmentComponent::OnTransformationChanged,TransformationNotifyMessage));
				RoadIntersectionComponentPtr old_intersection = m_StartNode->GetFirstComponentByClass<RoadIntersectionComponent>();

				RoadSegmentComponentPtr this_ptr = DYNAMIC_PTR_CAST<RoadSegmentComponent>(shared_from_this());
				old_intersection->RemoveRoad(this_ptr);
			}
			else
				return;
		}
		
		m_StartNode = node;
		if(m_StartNode.IsValid())
		{
			RoadIntersectionComponentPtr new_intersection = node->GetFirstComponentByClass<RoadIntersectionComponent>();
			RoadSegmentComponentPtr this_ptr = DYNAMIC_PTR_CAST<RoadSegmentComponent>(shared_from_this());
			new_intersection->AddRoad(this_ptr);
			m_StartNode->RegisterForMessage(REG_TMESS(RoadSegmentComponent::OnTransformationChanged,TransformationNotifyMessage,0));
		}
	}

	SceneObjectRef RoadSegmentComponent::GetStartNode() const
	{
		return m_StartNode;
	}

	void RoadSegmentComponent::SetEndNode(SceneObjectRef node)
	{
		if(m_EndNode.IsValid())
		{
			if(m_EndNode.GetRefObject() != node.GetRefObject())
			{
				m_EndNode->UnregisterForMessage(UNREG_TMESS(RoadSegmentComponent::OnTransformationChanged,TransformationNotifyMessage));
				RoadIntersectionComponentPtr old_intersection = m_EndNode->GetFirstComponentByClass<RoadIntersectionComponent>();

				RoadSegmentComponentPtr this_ptr = DYNAMIC_PTR_CAST<RoadSegmentComponent>(shared_from_this());
				old_intersection->RemoveRoad(this_ptr);
			}
			else
				return;
		}
		
		m_EndNode = node;
		if(m_EndNode.IsValid())
		{
			RoadIntersectionComponentPtr new_intersection = node->GetFirstComponentByClass<RoadIntersectionComponent>();
			RoadSegmentComponentPtr this_ptr = DYNAMIC_PTR_CAST<RoadSegmentComponent>(shared_from_this());
			new_intersection->AddRoad(this_ptr);
			m_EndNode->RegisterForMessage(REG_TMESS(RoadSegmentComponent::OnTransformationChanged,TransformationNotifyMessage,0));
		}
	}

	SceneObjectRef RoadSegmentComponent::GetEndNode() const
	{
		return m_EndNode;
	}

	void RoadSegmentComponent::OnInitialize()
	{
		m_Initialized = true;
	}

	void RoadSegmentComponent::OnTransformationChanged(TransformationNotifyMessagePtr message)
	{
		UpdateLanes();
		UpdateMesh();
	}


	void RoadSegmentComponent::UpdateLanes()
	{
		m_DownStreamLanes.clear();
		m_UpStreamLanes.clear();
		std::vector<Vec3> wps = GetWaypointList(false);
		if(wps.size() > 1)
		{

			Float lane_offset = 4;
			size_t num_us_lanes = 2;
			size_t num_ds_lanes = 2;

			//move first and last waypoint back to match intersection size
			Vec3 dir = wps[1] - wps[0];
			dir.Normalize();
			wps[0] = wps[0] + dir*lane_offset*max(num_ds_lanes,num_us_lanes);

			dir = wps[wps.size()-2] - wps[wps.size()-1];
			dir.Normalize();
			wps[wps.size()-1] = wps[wps.size()-1] + dir*lane_offset*2;
			
			for(size_t i = 0; i < num_us_lanes; i++)
			{
				std::vector<Vec3> lane = GenerateOffset(wps, -lane_offset*(i+1));
				std::reverse(lane.begin(),lane.end());
				m_UpStreamLanes.push_back(lane);
			}

			for(size_t i = 0; i < num_ds_lanes; i++)
			{
				std::vector<Vec3> lane = GenerateOffset(wps, lane_offset*(i+1));
				m_DownStreamLanes.push_back(lane);
			}
		}
	}

	void RoadSegmentComponent::UpdateMesh()
	{
		if(m_StartNode.IsValid() && m_EndNode.IsValid())
		{
			Vec3 start_pos = m_StartNode->GetFirstComponentByClass<ILocationComponent>()->GetWorldPosition();
			Vec3 end_pos = m_EndNode->GetFirstComponentByClass<ILocationComponent>()->GetWorldPosition();
			
			ManualMeshDataPtr mesh_data(new  ManualMeshData());
			mesh_data->Type = LINE_LIST;
			mesh_data->Material = "WhiteTransparentNoLighting";

			MeshVertex vertex;
			vertex.TexCoord.Set(0,0);
			vertex.Color.Set(0.2,0.2,1,1);
			vertex.Normal = Vec3(0,1,0);
			vertex.Pos = start_pos;
			mesh_data->VertexVector.push_back(vertex);
			vertex.Pos = end_pos;
			mesh_data->VertexVector.push_back(vertex);

			for(size_t i = 0; i < m_DownStreamLanes.size(); i++)
			{
				for(size_t j = 1; j < m_DownStreamLanes[i].size(); j++)
				{
					vertex.Pos = m_DownStreamLanes[i][j-1];
					mesh_data->VertexVector.push_back(vertex);
					vertex.Pos = m_DownStreamLanes[i][j];
					mesh_data->VertexVector.push_back(vertex);
				}
			}

			for(size_t i = 0; i < m_UpStreamLanes.size(); i++)
			{
				for(size_t j = 1; j < m_UpStreamLanes[i].size(); j++)
				{
					vertex.Pos = m_UpStreamLanes[i][j-1];
					mesh_data->VertexVector.push_back(vertex);
					vertex.Pos = m_UpStreamLanes[i][j];
					mesh_data->VertexVector.push_back(vertex);
				}
			}

			MessagePtr mesh_message(new ManualMeshDataMessage(mesh_data));
			GetSceneObject()->PostMessage(mesh_message);
		}
	}

	RoadIntersectionComponentPtr RoadSegmentComponent::GetNextIntersection(RoadIntersectionComponentPtr current)
	{
		RoadIntersectionComponentPtr intersection = m_StartNode->GetFirstComponentByClass<RoadIntersectionComponent>();
		if(current == intersection)
		{
			return m_EndNode->GetFirstComponentByClass<RoadIntersectionComponent>();
		}
		else
			return m_StartNode->GetFirstComponentByClass<RoadIntersectionComponent>();
	}


	bool RoadSegmentComponent::StartInIntersection(RoadIntersectionComponentPtr from)
	{
		RoadIntersectionComponentPtr intersection = m_StartNode->GetFirstComponentByClass<RoadIntersectionComponent>();
		if(from == intersection)
		{
			return true;
		}
		return false;
	}

	std::vector<Vec3> RoadSegmentComponent::GetWaypointList(bool upstream)
	{
		std::vector<Vec3> path;
		Vec3 start_pos = m_StartNode->GetFirstComponentByClass<ILocationComponent>()->GetWorldPosition();
		Vec3 end_pos = m_EndNode->GetFirstComponentByClass<ILocationComponent>()->GetWorldPosition();
		if(upstream)
		{
			path.push_back(end_pos);
			path.push_back(start_pos);
		}
		else
		{
			path.push_back(start_pos);
			path.push_back(end_pos);
		}
		return path;
	}

	std::vector<Vec3> RoadSegmentComponent::GetLane(int lane, bool upstream)
	{
		if(upstream)
			return m_UpStreamLanes[lane];
		else
			return m_DownStreamLanes[lane];
	}

	std::vector<Vec3> RoadSegmentComponent::GenerateOffset(std::vector<Vec3> wps, Float offset)
	{
		std::vector<Vec3> lane;
		for(size_t i = 0; i < wps.size(); i++)
		{
			Vec3 side; //= wps[i+1] - wps[i];

			if( i== 0)
			{
				side = (wps[1] - wps[0]); 
				side.y = 0;
			}
			else if(i < wps.size() - 1)
			{
				Vec3 d1 = (wps[i]-wps[i-1]);
				Vec3 d2 = (wps[i+1]-wps[i]);
				d1.Normalize();
				d2.Normalize();
				side = d1 + d2;
			}
			else
			{
				side = wps[i]-wps[i-1];
			}
			side.Normalize();
			side = Math::Cross(side,Vec3(0,1,0)); 
			side.Normalize();
			lane.push_back(wps[i] + side*offset);
		}
		return lane;
	}
}
