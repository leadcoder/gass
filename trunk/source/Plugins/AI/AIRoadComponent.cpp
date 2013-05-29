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
		GetClassRTTI()->SetMetaData(ObjectMetaDataPtr(new ObjectMetaData("AIRoadComponent", OF_VISIBLE)));
		
		
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
				
	}

	void AIRoadComponent::OnTransformationChanged(TransformationNotifyMessagePtr message)
	{
		
	}

	void AIRoadComponent::UpdateMesh()
	{
		
	}
	
}
