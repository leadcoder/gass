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
		std::vector<Vec3> road_wps = GetWaypointsObject()->GetFirstComponentByClass<IWaypointListComponent>()->GetWaypoints();
		if(road_wps.size()  > 0)
		{
			return road_wps[0];
		}
		return Vec3(0,0,0);
	}
	
	Vec3 AIRoadComponent::GetEndPoint() const
	{
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

	std::vector<AIRoadLaneComponentPtr> AIRoadComponent::GetStartLanes(bool down_stream) const
	{
		std::vector<AIRoadLaneComponentPtr> lanes;
		if(m_LaneSections.size() > 0)
		{
			IComponentContainer::ComponentVector comps;
			m_LaneSections[0]->GetSceneObject()->GetComponentsByClass<AIRoadLaneComponent>(comps);
			for(size_t i =  0; i < comps.size(); i++)
			{
				AIRoadLaneComponentPtr  lane = DYNAMIC_PTR_CAST<AIRoadLaneComponent>(comps[i]);
				if(down_stream)
				{
					if(lane->IsDownStream())
					{
						lanes.push_back(lane);
					}
				}
				else 
				{
					if(!lane->IsDownStream())
					{
						lanes.push_back(lane);
					}
				}

			}
		}
		return lanes;
	}

	std::vector<AIRoadLaneComponentPtr> AIRoadComponent::GetEndLanes(bool down_stream) const
	{
		std::vector<AIRoadLaneComponentPtr> lanes;
		if(m_LaneSections.size() > 0)
		{
			IComponentContainer::ComponentVector comps;
			m_LaneSections.back()->GetSceneObject()->GetComponentsByClass<AIRoadLaneComponent>(comps);
			for(size_t i =  0; i < comps.size(); i++)
			{
				AIRoadLaneComponentPtr  lane = DYNAMIC_PTR_CAST<AIRoadLaneComponent>(comps[i]);
				if(down_stream)
				{
					if(lane->IsDownStream())
					{
						lanes.push_back(lane);
					}
				}
				else 
				{
					if(!lane->IsDownStream())
					{
						lanes.push_back(lane);
					}
				}

			}
		}
		return lanes;
	}

	std::vector<AIRoadLaneComponentPtr> AIRoadComponent::GetIncommingLanes(SceneObjectPtr connection) const
	{
		bool start_node = StartIn(connection);
		if(start_node)
			return GetStartLanes(true);
		else
			return GetEndLanes(false);
	}
	
	std::vector<AIRoadLaneComponentPtr> AIRoadComponent::GetOutgoingLanes(SceneObjectPtr connection) const
	{

		bool start_node = StartIn(connection);
		if(start_node)
			return GetStartLanes(false);
		else
			return GetEndLanes(true);

	}

	void AIRoadComponent::UpdateLanes()
	{
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
					lane->SetWaypoints(lane_wps);
				}
			}
		}
	}

	void AIRoadComponent::UpdateMesh()
	{
		if(m_StartNode.IsValid())
		{
			AIRoadIntersectionComponentPtr intersection = m_StartNode->GetFirstComponentByClass<AIRoadIntersectionComponent>();
			if(intersection)
			{
				intersection->UpdateConnectionLines();
			}
		}

		if(m_EndNode.IsValid())
		{
			AIRoadIntersectionComponentPtr intersection = m_EndNode->GetFirstComponentByClass<AIRoadIntersectionComponent>();
			if(intersection)
			{
				intersection->UpdateConnectionLines();
			}
		}

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
		MessagePtr mesh_message(new ManualMeshDataMessage(mesh_data));
		m_LaneDebugObject->PostMessage(mesh_message);
	}
}
