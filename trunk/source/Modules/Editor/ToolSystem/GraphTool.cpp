
#include "GraphTool.h"
#include "MouseToolController.h"
#include "Modules/Editor/EditorSystem.h"
#include "Modules/Editor/EditorSceneManager.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/Utils/GASSException.h"
#include "Core/ComponentSystem/GASSIComponent.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/Interface/GASSILocationComponent.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Interface/GASSIGraphNodeComponent.h"
#include "Sim/Interface/GASSIGraphEdgeComponent.h"
#include "Sim/Interface/GASSIGraphComponent.h"




namespace GASS
{

	GraphTool::GraphTool(MouseToolController* controller): m_MouseIsDown(false),
		m_Controller(controller),
		m_FirstMoveUpdate(true),
		m_Mode(GTM_ADD)
	{
		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(GraphTool::OnToolChanged,ToolChangedEvent,0));	
	}

	GraphTool::~GraphTool()
	{

	}

	void GraphTool::MouseMoved(const MouseData &data, const SceneCursorInfo &info)
	{

	}

	void GraphTool::OnToolChanged(ToolChangedEventPtr message)
	{
		std::string new_tool = message->GetTool();
		if(new_tool == TID_CREATE)
		{
			//m_ObjectName = message->GetCreateObjectName();
			//m_ParentObject = message->GetCreateParentObject();
		}

	}

	void GraphTool::MouseDown(const MouseData &data, const SceneCursorInfo &info)
	{
		m_MouseIsDown = true;
		SceneObjectPtr obj_under_cursor (info.m_ObjectUnderCursor,NO_THROW);

		SceneObjectPtr parent_obj(m_ParentObject,NO_THROW);
		if(parent_obj && obj_under_cursor)
		{
			if(m_Mode == GTM_INSERT)
			{
				Float min_dist  = 1;
				int index = -1;
				//get closest edge and check if it's within threshold!
				IComponentContainer::ComponentVector comps;
				parent_obj->GetComponentsByClass<IGraphEdgeComponent>(comps,true);
				for(size_t i =  0; i < comps.size(); i++)
				{
					GraphEdgeComponentPtr edge = DYNAMIC_PTR_CAST<IGraphEdgeComponent>(comps[i]);
					BaseSceneComponentPtr start_node = DYNAMIC_PTR_CAST<BaseSceneComponent>(edge->GetStartNode());
					BaseSceneComponentPtr end_node = DYNAMIC_PTR_CAST<BaseSceneComponent>(edge->GetEndNode());
					if(start_node && end_node)
					{
						Vec3 start_pos  = start_node->GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->GetPosition();
						Vec3 end_pos  = end_node->GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->GetPosition();
						Vec3 cpos = Math::ClosestPointOnLine(start_pos, end_pos, info.m_3DPos);
						Float dist = (cpos - info.m_3DPos).Length();
						if(dist < min_dist) //save edge?
						{
							min_dist = dist;
							index = i;
						}
					}
				}
				if(index > -1)
				{
					GraphEdgeComponentPtr edge = DYNAMIC_PTR_CAST<IGraphEdgeComponent>(comps[index]);
					GraphNodeComponentPtr start_node = DYNAMIC_PTR_CAST<IGraphNodeComponent>(edge->GetStartNode());
					GraphNodeComponentPtr end_node = DYNAMIC_PTR_CAST<IGraphNodeComponent>(edge->GetEndNode());


					SceneObjectPtr current_obj = m_Controller->GetEditorSceneManager()->GetScene()->LoadObjectFromTemplate(m_NodeObjectName,parent_obj);
					GASSAssert(current_obj,"Failed to create scene object in GraphTool::MouseDown");

					int from_id = (int) this;
					GASS::MessagePtr pos_msg(new GASS::WorldPositionMessage(info.m_3DPos,from_id));
					current_obj->SendImmediate(pos_msg);
					GraphNodeComponentPtr current_node = current_obj->GetFirstComponentByClass<IGraphNodeComponent>();
					GASSAssert(current_node,"Failed to find IGraphNodeComponent in GraphTool::MouseDown");

					edge->SetEndNode(current_node);
					current_node->AddEdge(edge);
					end_node->RemoveEdge(edge);

					GASS::SceneObjectPtr edge_object = m_Controller->GetEditorSceneManager()->GetScene()->LoadObjectFromTemplate(m_EdgeObjectName,parent_obj);
					GASSAssert(edge_object,"Failed to LoadObjectFromTemplate in GraphTool::MouseDown");
					GraphEdgeComponentPtr new_edge = edge_object->GetFirstComponentByClass<IGraphEdgeComponent>();
					GASSAssert(new_edge,"Failed to find IGraphEdgeComponent in GraphTool::MouseDown");

					new_edge->SetStartNode(current_node);
					new_edge->SetEndNode(end_node);
					end_node->AddEdge(new_edge);
					current_node->AddEdge(new_edge);

					GraphComponentPtr graph = parent_obj->GetFirstComponentByClass<IGraphComponent>();
					GASSAssert(graph,"Failed to find IGraphComponent in GraphTool::MouseDown");
					graph->RebuildGraph();

				}
			}
			else if(m_Mode == GTM_ADD)
			{
				{
					//Check if object under cursor has IGraphNodeComponent to support reconnection
					GraphNodeComponentPtr current_node = obj_under_cursor->GetFirstComponentByClass<IGraphNodeComponent>();
					SceneObjectPtr current_obj;

					if(!current_node)
					{
						current_obj = m_Controller->GetEditorSceneManager()->GetScene()->LoadObjectFromTemplate(m_NodeObjectName,parent_obj);
						GASSAssert(current_obj,"Failed to create scene object in GraphTool::MouseDown");

						int from_id = (int) this;
						GASS::MessagePtr pos_msg(new GASS::WorldPositionMessage(info.m_3DPos,from_id));
						current_obj->SendImmediate(pos_msg);
						current_node = current_obj->GetFirstComponentByClass<IGraphNodeComponent>();
						GASSAssert(current_node,"Failed to find IGraphNodeComponent in GraphTool::MouseDown");
					}
					else
						current_obj = obj_under_cursor;
					SceneObjectPtr prev_obj(m_PrevObject,NO_THROW);
					if(prev_obj)
					{
						GraphNodeComponentPtr prev_node = prev_obj->GetFirstComponentByClass<IGraphNodeComponent>();
						GASSAssert(prev_node,"Failed to find IGraphNodeComponent in GraphTool::MouseDown");


						GASS::SceneObjectPtr edge_object = m_Controller->GetEditorSceneManager()->GetScene()->LoadObjectFromTemplate(m_EdgeObjectName,parent_obj);
						GASSAssert(edge_object,"Failed to LoadObjectFromTemplate in GraphTool::MouseDown");
						GraphEdgeComponentPtr edge_comp = edge_object->GetFirstComponentByClass<IGraphEdgeComponent>();

						GASSAssert(edge_comp,"Failed to find IGraphEdgeComponent in GraphTool::MouseDown");
						edge_comp->SetStartNode(prev_node);
						edge_comp->SetEndNode(current_node);
						prev_node->AddEdge(edge_comp);
						current_node->AddEdge(edge_comp);
						m_PrevObject = current_obj;

						GraphComponentPtr graph = parent_obj->GetFirstComponentByClass<IGraphComponent>();
						GASSAssert(graph,"Failed to find IGraphComponent in GraphTool::MouseDown");
						graph->RebuildGraph();
					}
				}
			}
		}
	}

	void GraphTool::MouseUp(const MouseData &data, const SceneCursorInfo &info)
	{
		m_MouseIsDown = false;
	}

}
