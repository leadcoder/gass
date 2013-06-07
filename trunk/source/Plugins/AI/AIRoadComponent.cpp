#include "AIRoadComponent.h"
#include "AISceneManager.h"
#include "AIRoadLaneSectionComponent.h"
#include "AIRoadLaneComponent.h"
#include "AIRoadIntersectionComponent.h"
#include "Plugins/Base/CoreMessages.h"
#include "Plugins/Game/GameMessages.h"
#include "RoadIntersectionComponent.h"
#include "Sim/Interface/GASSIWaypointListComponent.h"
#include <algorithm>


namespace GASS
{
	AIRoadComponent::AIRoadComponent(void) :
		m_Initialized(false)
	{
		
	}	

	AIRoadComponent::~AIRoadComponent(void)
	{

	}

	std::vector<SceneObjectPtr> ConnectionEnumeration(BaseReflectionObjectPtr obj)
	{
		AIRoadComponentPtr road= DYNAMIC_PTR_CAST<AIRoadComponent>(obj);
		return  road->GetConnectionSelection();
	}

	std::vector<SceneObjectPtr>  AIRoadComponent::GetConnectionSelection() const
	{
		std::vector<SceneObjectPtr> ret;
		SceneObjectPtr so = GetSceneObject();
		if(so)
		{
			IComponentContainer::ComponentVector comps;
			so->GetScene()->GetRootSceneObject()->GetComponentsByClass<AIRoadIntersectionComponent>(comps);
			for(int i = 0 ; i < comps.size();i++)
			{
				if(comps[i]->GetOwner() != so)
				{
					AIRoadIntersectionComponentPtr intersection = DYNAMIC_PTR_CAST<AIRoadIntersectionComponent>(comps[i]);
					if(intersection)
					{
						ret.push_back(intersection->GetSceneObject());
					}
				}
			}
		}
		return ret;
	}


	void AIRoadComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("AIRoadComponent",new Creator<AIRoadComponent, IComponent>);
		GetClassRTTI()->SetMetaData(ObjectMetaDataPtr(new ObjectMetaData("AIRoadComponent", OF_VISIBLE)));
		
		RegisterProperty<SceneObjectRef>("StartNode", &AIRoadComponent::GetStartNode, &AIRoadComponent::SetStartNode,
			SceneObjectEnumerationProxyPropertyMetaDataPtr(new SceneObjectEnumerationProxyPropertyMetaData("Start Node Connection",PF_VISIBLE,ConnectionEnumeration)));
		RegisterProperty<SceneObjectRef>("EndNode", &AIRoadComponent::GetEndNode, &AIRoadComponent::SetEndNode,
			SceneObjectEnumerationProxyPropertyMetaDataPtr(new SceneObjectEnumerationProxyPropertyMetaData("End Node Connection",PF_VISIBLE,ConnectionEnumeration)));

		RegisterProperty<SceneObjectRef>("WaypointsObject", &AIRoadComponent::GetWaypointsObject, &AIRoadComponent::SetWaypointsObject);
		RegisterProperty<SceneObjectRef>("LaneSectionsObject", &AIRoadComponent::GetLaneSectionsObject, &AIRoadComponent::SetLaneSectionsObject);
		RegisterProperty<SceneObjectRef>("LaneDebugObject", &AIRoadComponent::GetLaneDebugObject, &AIRoadComponent::SetLaneDebugObject);
	}

	void AIRoadComponent::SetStartNode(SceneObjectRef node)
	{
		//remove previous connection
		if(m_StartNode.IsValid())
		{
			if(m_StartNode.GetRefObject() != node.GetRefObject())
			{
				//m_StartNode->UnregisterForMessage(UNREG_TMESS(AIRoadComponent::OnTransformationChanged,TransformationNotifyMessage));
				AIRoadIntersectionComponentPtr old_intersection = m_StartNode->GetFirstComponentByClass<AIRoadIntersectionComponent>();
				if(old_intersection)
				{
					AIRoadComponentPtr this_ptr = DYNAMIC_PTR_CAST<AIRoadComponent>(shared_from_this());
					old_intersection->RemoveRoad(this_ptr);
				}
			}
			else
				return;
		}
		
		m_StartNode = node;

		if(m_StartNode.IsValid())
		{
			AIRoadIntersectionComponentPtr new_intersection = node->GetFirstComponentByClass<AIRoadIntersectionComponent>();
			if(new_intersection)
			{
				AIRoadComponentPtr this_ptr = DYNAMIC_PTR_CAST<AIRoadComponent>(shared_from_this());
				new_intersection->AddRoad(this_ptr);
				//m_StartNode->RegisterForMessage(REG_TMESS(AIRoadComponent::OnTransformationChanged,TransformationNotifyMessage,0));
			}
		}
	}

	SceneObjectRef AIRoadComponent::GetStartNode() const
	{
		return m_StartNode;
	}

	void AIRoadComponent::SetEndNode(SceneObjectRef node)
	{
		if(m_EndNode.IsValid())
		{
			if(m_EndNode.GetRefObject() != node.GetRefObject())
			{
				//m_EndNode->UnregisterForMessage(UNREG_TMESS(AIRoadComponent::OnTransformationChanged,TransformationNotifyMessage));
				AIRoadIntersectionComponentPtr old_intersection = m_EndNode->GetFirstComponentByClass<AIRoadIntersectionComponent>();

				if(old_intersection)
				{
					AIRoadComponentPtr this_ptr = DYNAMIC_PTR_CAST<AIRoadComponent>(shared_from_this());
					old_intersection->RemoveRoad(this_ptr);
				}
			}
			else
				return;
		}
		
		m_EndNode = node;
		if(m_EndNode.IsValid())
		{
			AIRoadIntersectionComponentPtr new_intersection = node->GetFirstComponentByClass<AIRoadIntersectionComponent>();
			if(new_intersection)
			{
				AIRoadComponentPtr this_ptr = DYNAMIC_PTR_CAST<AIRoadComponent>(shared_from_this());
				new_intersection->AddRoad(this_ptr);
			}
			//m_EndNode->RegisterForMessage(REG_TMESS(AIRoadComponent::OnTransformationChanged,TransformationNotifyMessage,0));
		}
	}

	SceneObjectRef AIRoadComponent::GetEndNode() const
	{
		return m_EndNode;
	}

	void AIRoadComponent::OnInitialize()
	{
		BaseSceneComponent::InitializeSceneObjectRef();


		if(m_StartNode.IsValid())
		{
			AIRoadIntersectionComponentPtr intersection = m_StartNode->GetFirstComponentByClass<AIRoadIntersectionComponent>();
			if(intersection)
			{
				AIRoadComponentPtr this_ptr = DYNAMIC_PTR_CAST<AIRoadComponent>(shared_from_this());
				intersection->AddRoad(this_ptr);
			}
		}

		if(m_EndNode.IsValid())
		{
			AIRoadIntersectionComponentPtr intersection = m_EndNode->GetFirstComponentByClass<AIRoadIntersectionComponent>();
			if(intersection)
			{
				AIRoadComponentPtr this_ptr = DYNAMIC_PTR_CAST<AIRoadComponent>(shared_from_this());
				intersection->AddRoad(this_ptr);
			}
		}
		
		m_WaypointsObject->RegisterForMessage(REG_TMESS(AIRoadComponent::OnWaypointsChanged,UpdateWaypointListMessage,0));
		m_Initialized = true;
	}

	bool LaneSectionSort(AIRoadLaneSectionComponentPtr lhs, AIRoadLaneSectionComponentPtr rhs)
	{
		return lhs->GetDistance() < rhs->GetDistance();
	}

	void AIRoadComponent::OnWaypointsChanged(UpdateWaypointListMessagePtr message)
	{
		//get all lane sections
		IComponentContainer::ComponentVector comps;
		m_LaneSectionsObject->GetComponentsByClass<AIRoadLaneSectionComponent>(comps);
		
		//get component and sort by distance
		m_LaneSections.clear();
		for(int i = 0 ;  i < comps.size(); i++)
		{
			AIRoadLaneSectionComponentPtr lane_section = DYNAMIC_PTR_CAST<AIRoadLaneSectionComponent>(comps[i]);
			m_LaneSections.push_back(lane_section);
		}
		std::sort(m_LaneSections.begin(), m_LaneSections.end(), LaneSectionSort);

		//update itersection connections
		UpdateLanes();
		//AutoConnectToIntersection();
		UpdateMesh();

		
	}

	void AIRoadComponent::OnTransformationChanged(TransformationNotifyMessagePtr message)
	{
		
	}

	bool AIRoadComponent::StartIn(SceneObjectPtr obj) const
	{
		if(obj == m_StartNode.GetRefObject())
			return true;
		return false;
	}

	Vec3 AIRoadComponent::GetStartPoint() const
	{
		if(!GetWaypointsObject().IsValid())
			return Vec3(0,0,0);
		std::vector<Vec3> road_wps = GetWaypointsObject()->GetFirstComponentByClass<IWaypointListComponent>()->GetWaypoints();
		if(road_wps.size()  > 0)
		{
			return road_wps[0];
		}
		return Vec3(0,0,0);
	}
	
	Vec3 AIRoadComponent::GetEndPoint() const
	{
		if(!GetWaypointsObject().IsValid())
			return Vec3(0,0,0);

		std::vector<Vec3> road_wps = GetWaypointsObject()->GetFirstComponentByClass<IWaypointListComponent>()->GetWaypoints();
		if(road_wps.size()  > 0)
		{
			return road_wps.back();
		}
		return Vec3(0,0,0);
	}


	std::vector<AIRoadLaneComponentPtr> AIRoadComponent::GetStartLanes() const
	{
		std::vector<AIRoadLaneComponentPtr> lanes;
		if(m_LaneSections.size() > 0)
		{
			IComponentContainer::ComponentVector comps;
			m_LaneSections[0]->GetSceneObject()->GetComponentsByClass<AIRoadLaneComponent>(comps);
			for(size_t i =  0; i < comps.size(); i++)
			{
				AIRoadLaneComponentPtr  lane = DYNAMIC_PTR_CAST<AIRoadLaneComponent>(comps[i]);
				lanes.push_back(lane);
			}
		}
		return lanes;
	}

	std::vector<AIRoadLaneComponentPtr> AIRoadComponent::GetEndLanes() const
	{
		std::vector<AIRoadLaneComponentPtr> lanes;
		if(m_LaneSections.size() > 0)
		{
			IComponentContainer::ComponentVector comps;
			m_LaneSections.back()->GetSceneObject()->GetComponentsByClass<AIRoadLaneComponent>(comps);
			for(size_t i =  0; i < comps.size(); i++)
			{
				AIRoadLaneComponentPtr  lane = DYNAMIC_PTR_CAST<AIRoadLaneComponent>(comps[i]);
				lanes.push_back(lane);
			}
		}
		return lanes;
	}

	std::vector<AIRoadLaneComponentPtr> AIRoadComponent::GetStartLanes(LaneDirection lane_dir) const
	{
		std::vector<AIRoadLaneComponentPtr> lanes;
		if(m_LaneSections.size() > 0)
		{
			IComponentContainer::ComponentVector comps;
			m_LaneSections[0]->GetSceneObject()->GetComponentsByClass<AIRoadLaneComponent>(comps);
			for(size_t i =  0; i < comps.size(); i++)
			{
				AIRoadLaneComponentPtr  lane = DYNAMIC_PTR_CAST<AIRoadLaneComponent>(comps[i]);
				if(lane_dir == lane->GetDirection().GetValue())
				{
					lanes.push_back(lane);
				}
			}
		}
		return lanes;
	}

	std::vector<AIRoadLaneComponentPtr> AIRoadComponent::GetEndLanes(LaneDirection lane_dir) const
	{
		std::vector<AIRoadLaneComponentPtr> lanes;
		if(m_LaneSections.size() > 0)
		{
			IComponentContainer::ComponentVector comps;
			m_LaneSections.back()->GetSceneObject()->GetComponentsByClass<AIRoadLaneComponent>(comps);
			for(size_t i =  0; i < comps.size(); i++)
			{
				AIRoadLaneComponentPtr  lane = DYNAMIC_PTR_CAST<AIRoadLaneComponent>(comps[i]);
				if(lane_dir == lane->GetDirection().GetValue())
				{
					lanes.push_back(lane);
				}
			}
		}
		return lanes;
	}

	std::vector<AIRoadLaneComponentPtr> AIRoadComponent::GetIncommingLanes(SceneObjectPtr connection) const
	{
		bool start_node = StartIn(connection);
		if(start_node)
			return GetStartLanes(LD_DOWNSTREAM);
		else
			return GetEndLanes(LD_UPSTREAM);
	}
	
	std::vector<AIRoadLaneComponentPtr> AIRoadComponent::GetOutgoingLanes(SceneObjectPtr connection) const
	{

		bool start_node = StartIn(connection);
		if(start_node)
			return GetStartLanes(LD_UPSTREAM);
		else
			return GetEndLanes(LD_DOWNSTREAM);

	}

	void AIRoadComponent::UpdateLanes()
	{
		//update lane pointers!
		for(size_t i =  0; i < m_LaneSections.size()-1; i++)
		{
			std::vector<AIRoadLaneComponentPtr> lanes = m_LaneSections[i]->GetLanes();
			for(int j = 0 ;  j < lanes.size(); j++)
			{ 
				if(lanes[i]->GetDirection().GetValue() == LD_DOWNSTREAM) 
				{
					std::vector<AIRoadLaneComponentPtr> next_lanes = m_LaneSections[i+1]->GetLanesByID(lanes[i]->GetLaneID());
					std::vector<AIRoadLaneComponentPtr>* connections = lanes[i]->GetNextLanesPtr();
					connections->clear();
					for(int k = 0 ;  k < next_lanes.size(); k++)
					{
						if(next_lanes[i]->GetDirection() == lanes[i]->GetDirection())
						{
							connections->push_back(next_lanes[k]);
						}
					}
				}
			}
		}

		for(size_t i =  1; i < m_LaneSections.size(); i++)
		{
			std::vector<AIRoadLaneComponentPtr> lanes = m_LaneSections[i]->GetLanes();
			for(int j = 0 ;  j < lanes.size(); j++)
			{ 
				if(lanes[i]->GetDirection().GetValue() == LD_UPSTREAM) 
				{
					std::vector<AIRoadLaneComponentPtr> next_lanes = m_LaneSections[i-1]->GetLanesByID(lanes[i]->GetLaneID());
					std::vector<AIRoadLaneComponentPtr>* connections = lanes[i]->GetNextLanesPtr();
					connections->clear();
					for(int k = 0 ;  k < next_lanes.size(); k++)
					{
						if(next_lanes[i]->GetDirection() == lanes[i]->GetDirection())
						{
							connections->push_back(next_lanes[k]);
						}
					}
				}
			}
		}


		Float start = 0;
		Float end = 0;
		Float prev = 0;
		std::vector<Vec3> road_wps = GetWaypointsObject()->GetFirstComponentByClass<IWaypointListComponent>()->GetWaypoints();
	
		if(road_wps.size() > 1)
		{
			for(size_t i =  0; i < m_LaneSections.size(); i++)
			{
				start = prev;
				end = m_LaneSections[i]->GetDistance();
				prev = end;

				std::vector<Vec3> lane_section_wps; 
				if(end > start && end > 0)
					lane_section_wps = Math::ClipPath(start, end,road_wps);
				else 
					lane_section_wps = road_wps;

				IComponentContainer::ComponentVector comps;
				m_LaneSections[i]->GetSceneObject()->GetComponentsByClass<AIRoadLaneComponent>(comps);
				
				for(int j = 0 ;  j < comps.size(); j++)
				{
					AIRoadLaneComponentPtr lane = DYNAMIC_PTR_CAST<AIRoadLaneComponent>(comps[j]);
					
					std::vector<Vec3> lane_wps = Math::GenerateOffset(lane_section_wps, lane->GetWidth());
					if(lane->GetDirection().GetValue() == LD_UPSTREAM) 
					{
						std::reverse(lane_wps.begin(),lane_wps.end());
					}
					lane->SetWaypoints(lane_wps);
				}
			}
		}

		if(m_StartNode.IsValid())
		{
			AIRoadIntersectionComponentPtr intersection = m_StartNode->GetFirstComponentByClass<AIRoadIntersectionComponent>();
			if(intersection)
			{
				intersection->AutoLineConnection();
				intersection->UpdateConnectionLines();
			}
		}

		if(m_EndNode.IsValid())
		{
			AIRoadIntersectionComponentPtr intersection = m_EndNode->GetFirstComponentByClass<AIRoadIntersectionComponent>();
			if(intersection)
			{
				intersection->AutoLineConnection();
				intersection->UpdateConnectionLines();
			}
		}
	}

	void AIRoadComponent::AutoConnectToIntersection()
	{
		Vec3 start_wp = GetStartPoint();
		Vec3 end_wp = GetEndPoint();
		Float connnect_dist = 2;
		Float disconnnect_dist = 6;
			
		IComponentContainer::ComponentVector comps;
		GetSceneObject()->GetParentSceneObject()->GetComponentsByClass<AIRoadIntersectionComponent>(comps,true);
		for(int j = 0 ;  j < comps.size(); j++)
		{
			AIRoadIntersectionComponentPtr intersection = DYNAMIC_PTR_CAST<AIRoadIntersectionComponent>(comps[j]);
			Vec3 inter_pos = intersection->GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->GetWorldPosition();
			if(intersection->GetSceneObject() == GetStartNode().GetRefObject())
			{
				//already connected
				if((inter_pos - start_wp).Length() > disconnnect_dist)
				{
					//disconnect
					SetStartNode(SceneObjectRef());
				}
			}
			else
			{
				if((inter_pos - start_wp).Length() < connnect_dist)
				{
					SetStartNode(SceneObjectRef(intersection->GetSceneObject()));
				}
			}

			if(intersection->GetSceneObject() == GetEndNode().GetRefObject())
			{
				//already connected
				if((inter_pos - end_wp).Length() > disconnnect_dist)
				{
					//disconnect
					SetEndNode(SceneObjectRef());
				}
			}
			else
			{
				if((inter_pos - end_wp).Length() < connnect_dist)
				{
					SetEndNode(SceneObjectRef(intersection->GetSceneObject()));
				}
			}
		}
	}



	void AIRoadComponent::AutoConnectToRoads()
	{
		//Auto connect to cloese roads?
		//get all roads
		

	/*	m_StartConnections.clear();
		m_EndConnections.clear();
		Vec3 start_wp = GetStartPoint();
		Vec3 end_wp = GetEndPoint();

		IComponentContainer::ComponentVector comps;
		GetSceneObject()->GetParentSceneObject()->GetComponentsByClass<AIRoadComponent>(comps,true);
		for(int j = 0 ;  j < comps.size(); j++)
		{
			AIRoadComponentPtr road= DYNAMIC_PTR_CAST<AIRoadComponent>(comps[j]);
			
			if(road != shared_from_this()) 
			{
				// Check edge distance			
				Vec3 connect_start_wp = road->GetStartPoint();
				Vec3 connect_end_wp = road->GetEndPoint();

				Float dist = 10;
				if((connect_start_wp - start_wp).Length() < dist)
				{
					vertex.Pos = connect_start_wp;
					mesh_data->VertexVector.push_back(vertex );
					vertex.Pos = start_wp;
					mesh_data->VertexVector.push_back(vertex );
					//Connect!
					m_StartConnections.push_back(road);
				}

				else if((connect_start_wp - end_wp).Length() < dist)
				{
					vertex.Pos = connect_start_wp;
					mesh_data->VertexVector.push_back(vertex );
					vertex.Pos = end_wp;
					mesh_data->VertexVector.push_back(vertex );
					m_EndConnections.push_back(road);
				}
				else if((connect_end_wp - start_wp).Length() < dist)
				{
					vertex.Pos = connect_end_wp;
					mesh_data->VertexVector.push_back(vertex );
					vertex.Pos = start_wp;
					mesh_data->VertexVector.push_back(vertex );
					m_StartConnections.push_back(road);
				}
				else if((connect_end_wp - end_wp).Length() < dist)
				{
					vertex.Pos = connect_end_wp;
					mesh_data->VertexVector.push_back(vertex );
					vertex.Pos = end_wp;
					mesh_data->VertexVector.push_back(vertex );
					m_EndConnections.push_back(road->GetEndNode());
				}
			}
		}*/
	}

	void AIRoadComponent::UpdateMesh()
	{
	
		ManualMeshDataPtr mesh_data(new ManualMeshData());
		mesh_data->Type = LINE_LIST;
		mesh_data->Material = "WhiteTransparentNoLighting";

		MeshVertex vertex;
		vertex.TexCoord.Set(0,0);
		vertex.Color.Set(0.2,0.2,1,1);
		vertex.Normal = Vec3(0,1,0);

		for(size_t i =  0; i < m_LaneSections.size(); i++)
		{
			IComponentContainer::ComponentVector comps;
			m_LaneSections[i]->GetSceneObject()->GetComponentsByClass<AIRoadLaneComponent>(comps);
			for(int j = 0 ;  j < comps.size(); j++)
			{
				AIRoadLaneComponentPtr  lane = DYNAMIC_PTR_CAST<AIRoadLaneComponent>(comps[j]);
				if(lane->GetDirection().GetValue() == LD_UPSTREAM) 
					vertex.Color.Set(0.2,1,0.2,1);	
				else
					vertex.Color.Set(0.2,0.2,1,1);

				std::vector<Vec3>* lane_wps = lane->GetWaypointsPtr();
				for(size_t k = 1; k < lane_wps->size(); k++)
				{
						vertex.Pos = lane_wps->at(k);
						mesh_data->VertexVector.push_back(vertex );
						vertex.Pos = lane_wps->at(k-1);
						mesh_data->VertexVector.push_back(vertex );
				}
			}
		}


		//Auto connect to cloese roads?
		//get all roads
		

	/*	m_StartConnections.clear();
		m_EndConnections.clear();
		Vec3 start_wp = GetStartPoint();
		Vec3 end_wp = GetEndPoint();

		IComponentContainer::ComponentVector comps;
		GetSceneObject()->GetParentSceneObject()->GetComponentsByClass<AIRoadComponent>(comps,true);
		for(int j = 0 ;  j < comps.size(); j++)
		{
			AIRoadComponentPtr road= DYNAMIC_PTR_CAST<AIRoadComponent>(comps[j]);
			
			if(road != shared_from_this()) 
			{
				// Check edge distance			
				Vec3 connect_start_wp = road->GetStartPoint();
				Vec3 connect_end_wp = road->GetEndPoint();

				Float dist = 10;
				if((connect_start_wp - start_wp).Length() < dist)
				{
					vertex.Pos = connect_start_wp;
					mesh_data->VertexVector.push_back(vertex );
					vertex.Pos = start_wp;
					mesh_data->VertexVector.push_back(vertex );
					//Connect!
					m_StartConnections.push_back(road);
				}

				else if((connect_start_wp - end_wp).Length() < dist)
				{
					vertex.Pos = connect_start_wp;
					mesh_data->VertexVector.push_back(vertex );
					vertex.Pos = end_wp;
					mesh_data->VertexVector.push_back(vertex );
					m_EndConnections.push_back(road);
				}
				else if((connect_end_wp - start_wp).Length() < dist)
				{
					vertex.Pos = connect_end_wp;
					mesh_data->VertexVector.push_back(vertex );
					vertex.Pos = start_wp;
					mesh_data->VertexVector.push_back(vertex );
					m_StartConnections.push_back(road);
				}
				else if((connect_end_wp - end_wp).Length() < dist)
				{
					vertex.Pos = connect_end_wp;
					mesh_data->VertexVector.push_back(vertex );
					vertex.Pos = end_wp;
					mesh_data->VertexVector.push_back(vertex );
					m_EndConnections.push_back(road->GetEndNode());
				}
			}
		}*/

		MessagePtr mesh_message(new ManualMeshDataMessage(mesh_data));
		m_LaneDebugObject->PostMessage(mesh_message);
	}
}
