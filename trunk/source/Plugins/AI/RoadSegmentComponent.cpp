#include "RoadSegmentComponent.h"
#include "AISceneManager.h"
#include "Plugins/Base/CoreMessages.h"
#include "Plugins/Game/GameMessages.h"



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
		m_StartNode = node;
		if(m_StartNode.IsValid())
			m_StartNode->RegisterForMessage(REG_TMESS(RoadSegmentComponent::OnTransformationChanged,TransformationNotifyMessage,0));
	}

	SceneObjectRef RoadSegmentComponent::GetStartNode() const
	{
		return m_StartNode;
	}

	void RoadSegmentComponent::SetEndNode(SceneObjectRef node)
	{
		m_EndNode = node;
		if(m_EndNode.IsValid())
			m_EndNode->RegisterForMessage(REG_TMESS(RoadSegmentComponent::OnTransformationChanged,TransformationNotifyMessage,0));
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
		UpdateMesh();
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
			MessagePtr mesh_message(new ManualMeshDataMessage(mesh_data));
			GetSceneObject()->PostMessage(mesh_message);
		}
	}
}
