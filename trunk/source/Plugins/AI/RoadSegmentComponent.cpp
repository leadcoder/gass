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

		if(m_StartNode.IsValid() && m_EndNode.IsValid())
		{
			UpdateLanes();
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

		if(m_StartNode.IsValid() && m_EndNode.IsValid())
		{
			UpdateLanes();
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


	bool RoadSegmentComponent::FirstFreeLocation(bool up_stream, Vec3 &pos,Quaternion &rot, Float &distance, Float vehicle_separation)
	{

		tbb::spin_mutex::scoped_lock lock(m_RoadMutex);
		//if(m_UpStreamLanes.size() == 0 && m_DownStreamLanes.size() == 0)		
		//	UpdateLanes();
		std::vector<LaneVehicle*> *vehicles;
		if(up_stream) 
			vehicles = &m_UpStreamLaneVehicles;
		else 
			vehicles =  &m_DownStreamLaneVehicles;

		std::vector<Vec3> wps = GetLane(0, up_stream);
		double d = 0;
		double tot_dist = 0;
		for(size_t i = 1 ; i < wps.size() ; i++)
		{
			Vec3 dir = (wps[i] - wps[i-1]);
			d = dir.Length();
			
			dir.Normalize();
			Float dist = vehicle_separation;
			while(dist < d)
			{
				bool free = true;
				for(size_t j = 0 ; j < vehicles->size() ; j++)
				{
					if(((tot_dist + dist) - vehicles->at(j)->m_Distance) < vehicle_separation)
					{
						free = false;
						break;
					}
				}
				if(free)
				{
					pos = wps[i-1] + (dir*dist);
					
					Mat4 rot_mat;
					rot_mat.Identity();
					Vec3 up(0,1,0);
					rot_mat.SetUpVector(up);
					rot_mat.SetViewDirVector(-dir);
					Vec3 right = -Math::Cross(-dir,up);
					rot_mat.SetRightVector(right);
					rot.FromRotationMatrix(rot_mat);

					distance = tot_dist + dist;
					return true;
				}
				dist += vehicle_separation;
			}
			tot_dist += d; 
		}
		return false;
	}


	void RoadSegmentComponent::UpdateMesh()
	{
		if(m_StartNode.IsValid() && m_EndNode.IsValid())
		{

			Vec3 start_pos = m_StartNode->GetFirstComponentByClass<ILocationComponent>()->GetWorldPosition();
			Vec3 end_pos = m_EndNode->GetFirstComponentByClass<ILocationComponent>()->GetWorldPosition();


			RoadIntersectionComponentPtr start_ric = m_StartNode->GetFirstComponentByClass<RoadIntersectionComponent>();
			RoadIntersectionComponentPtr end_ric = m_EndNode->GetFirstComponentByClass<RoadIntersectionComponent>();
			
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

			TrafficLight light;
			if(start_ric->GetTrafficLight(DYNAMIC_PTR_CAST<RoadSegmentComponent>(shared_from_this()),light))
			{
				if(light.m_Stop)
					vertex.Color.Set(1,0,0,1);
				else
					vertex.Color.Set(0,1,0,1);
				Vec3 pos = m_UpStreamLanes[0][m_UpStreamLanes.size()-1];
				vertex.Pos = pos;
				mesh_data->VertexVector.push_back(vertex);
				vertex.Pos = pos + Vec3(0,2,0);
				mesh_data->VertexVector.push_back(vertex);
			}

			if(end_ric->GetTrafficLight(DYNAMIC_PTR_CAST<RoadSegmentComponent>(shared_from_this()),light))
			{
				if(light.m_Stop)
					vertex.Color.Set(1,0,0,1);
				else
					vertex.Color.Set(0,1,0,1);
				Vec3 pos = m_DownStreamLanes[0][m_DownStreamLanes.size()-1];
				vertex.Pos = pos;
				mesh_data->VertexVector.push_back(vertex);
				vertex.Pos = pos + Vec3(0,2,0);
				mesh_data->VertexVector.push_back(vertex);
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

	void RoadSegmentComponent::RegisterVehicle(LaneVehicle* vehicle, bool up_stream)
	{
		tbb::spin_mutex::scoped_lock lock(m_RoadMutex);
		if(up_stream)
			m_UpStreamLaneVehicles.push_back(vehicle);
		else
			m_DownStreamLaneVehicles.push_back(vehicle);
	}

	void RoadSegmentComponent::UnregisterVehicle(LaneVehicle* vehicle, bool up_stream)
	{
		tbb::spin_mutex::scoped_lock lock(m_RoadMutex);
		if(up_stream)
		{
			std::vector<LaneVehicle*>::iterator iter = m_UpStreamLaneVehicles.begin();
			while(m_UpStreamLaneVehicles.end() != iter)
			{
				if(*iter == vehicle)
					iter = m_UpStreamLaneVehicles.erase(iter);
				else iter++;
			}
		}
		else
		{
			std::vector<LaneVehicle*>::iterator iter = m_DownStreamLaneVehicles.begin();
			while(m_DownStreamLaneVehicles.end() != iter)
			{
				if(*iter == vehicle)
					iter = m_DownStreamLaneVehicles.erase(iter);
				else iter++;
			}
		}
	}


	bool RoadSegmentComponent::IsRoadFree(bool up_stream, double in_distance)
	{
		std::vector<Vec3> wps = GetLane(0, up_stream);
		double d = 0;
		for(size_t i = 1 ; i < wps.size() ; i++)
		{
			d += (wps[i] - wps[i-1]).FastLength();
		}

		tbb::spin_mutex::scoped_lock lock(m_RoadMutex);
		std::vector<LaneVehicle*> *vehicles;
		if(up_stream) 
			vehicles = &m_UpStreamLaneVehicles;
		else 
			vehicles =  &m_DownStreamLaneVehicles;
		
		for(size_t i = 0 ; i < vehicles->size() ; i++)
		{
			if((d - vehicles->at(i)->m_Distance) < in_distance)
			{
				return false;
			}
		}
		return true;
	}


	bool RoadSegmentComponent::IsStartOfRoadFree(bool up_stream, double in_distance)
	{
		tbb::spin_mutex::scoped_lock lock(m_RoadMutex);
		std::vector<LaneVehicle*> *vehicles;
		if(up_stream) 
			vehicles = &m_UpStreamLaneVehicles;
		else 
			vehicles =  &m_DownStreamLaneVehicles;
		
		for(size_t i = 0 ; i < vehicles->size() ; i++)
		{
			if(vehicles->at(i)->m_Distance < in_distance && vehicles->at(i)->m_DistanceToPath < 10)
			{
				return false;
			}
		}
		return true;
	}


	LaneVehicle* RoadSegmentComponent::GetClosest(bool up_stream, LaneVehicle* source)
	{
		tbb::spin_mutex::scoped_lock lock(m_RoadMutex);
		std::vector<LaneVehicle*> *vehicles;
		if(up_stream) vehicles = &m_UpStreamLaneVehicles;
		else vehicles =  &m_DownStreamLaneVehicles;

		LaneVehicle* closest = NULL;
		double min_dist = 0;
		for(size_t i = 0 ; i < vehicles->size() ; i++)
		{
			if(vehicles->at(i) != source)
			{
				if(source->m_Distance < vehicles->at(i)->m_Distance)
				{
					double dist = vehicles->at(i)->m_Distance - source->m_Distance;
					if(closest == NULL || dist < min_dist)
					{
						closest = vehicles->at(i);
						min_dist = dist; 
					}
				}
			}
		}
		return closest;
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
