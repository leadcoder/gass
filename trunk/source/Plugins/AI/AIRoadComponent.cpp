#include "AIRoadComponent.h"
#include "AISceneManager.h"
#include "Plugins/Base/CoreMessages.h"
#include "Plugins/Game/GameMessages.h"
#include "RoadIntersectionComponent.h"


namespace GASS
{
	AIRoadComponent::AIRoadComponent(void) :
		m_Initialized(false)
	{
		
	}	

	AIRoadComponent::~AIRoadComponent(void)
	{

	}

	void AIRoadComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("AIRoadComponent",new Creator<AIRoadComponent, IComponent>);
		RegisterProperty<SceneObjectRef>("PrevNode", &AIRoadComponent::GetStartNode, &AIRoadComponent::SetStartNode);
		RegisterProperty<SceneObjectRef>("NextNode", &AIRoadComponent::GetEndNode, &AIRoadComponent::SetEndNode);
		RegisterProperty<SceneObjectRef>("WaypointsObject", &AIRoadComponent::GetWaypointsObject, &AIRoadComponent::SetWaypointsObject);
		RegisterProperty<SceneObjectRef>("LaneSectionsObject", &AIRoadComponent::GetLaneSectionsObject, &AIRoadComponent::SetLaneSectionsObject);
	}

	void AIRoadComponent::SetStartNode(SceneObjectRef node)
	{
		if(m_StartNode.IsValid())
		{
			if(m_StartNode.GetRefObject() != node.GetRefObject())
			{
				m_StartNode->UnregisterForMessage(UNREG_TMESS(AIRoadComponent::OnTransformationChanged,TransformationNotifyMessage));
				//RoadIntersectionComponentPtr old_intersection = m_StartNode->GetFirstComponentByClass<RoadIntersectionComponent>();
				//AIRoadComponentPtr this_ptr = DYNAMIC_PTR_CAST<AIRoadComponent>(shared_from_this());
				//old_intersection->RemoveRoad(this_ptr);
			}
			else
				return;
		}
		
		m_StartNode = node;
		if(m_StartNode.IsValid())
		{
			//RoadIntersectionComponentPtr new_intersection = node->GetFirstComponentByClass<RoadIntersectionComponent>();
			//AIRoadComponentPtr this_ptr = DYNAMIC_PTR_CAST<AIRoadComponent>(shared_from_this());
			//new_intersection->AddRoad(this_ptr);
			m_StartNode->RegisterForMessage(REG_TMESS(AIRoadComponent::OnTransformationChanged,TransformationNotifyMessage,0));
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
				m_EndNode->UnregisterForMessage(UNREG_TMESS(AIRoadComponent::OnTransformationChanged,TransformationNotifyMessage));
				//RoadIntersectionComponentPtr old_intersection = m_EndNode->GetFirstComponentByClass<RoadIntersectionComponent>();

				//AIRoadComponentPtr this_ptr = DYNAMIC_PTR_CAST<AIRoadComponent>(shared_from_this());
				//old_intersection->RemoveRoad(this_ptr);
			}
			else
				return;
		}
		
		m_EndNode = node;
		if(m_EndNode.IsValid())
		{
			//RoadIntersectionComponentPtr new_intersection = node->GetFirstComponentByClass<RoadIntersectionComponent>();
			//AIRoadComponentPtr this_ptr = DYNAMIC_PTR_CAST<AIRoadComponent>(shared_from_this());
			//new_intersection->AddRoad(this_ptr);
			m_EndNode->RegisterForMessage(REG_TMESS(AIRoadComponent::OnTransformationChanged,TransformationNotifyMessage,0));
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

	void AIRoadComponent::OnWaypointsChanged(UpdateWaypointListMessagePtr message)
	{
		//Update all lines
	}

	void AIRoadComponent::OnTransformationChanged(TransformationNotifyMessagePtr message)
	{
		
	}

	void AIRoadComponent::UpdateMesh()
	{

		//render lanes!
			
		ManualMeshDataPtr mesh_data(new  ManualMeshData());
		mesh_data->Type = LINE_LIST;
		mesh_data->Material = "WhiteTransparentNoLighting";

		MeshVertex vertex;
		vertex.TexCoord.Set(0,0);
		vertex.Color.Set(0.2,0.2,1,1);
		vertex.Normal = Vec3(0,1,0);
	
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
	
}
