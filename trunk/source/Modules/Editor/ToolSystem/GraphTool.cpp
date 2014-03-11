
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
				/*Float min_dist  = 1;
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
				*/
				GraphEdgeComponentPtr edge =  _GetClosestEdge(parent_obj,info.m_3DPos , 1.0); 
				if(edge)
				{
					
					GraphNodeComponentPtr start_node = DYNAMIC_PTR_CAST<IGraphNodeComponent>(edge->GetStartNode());
					GraphNodeComponentPtr end_node = DYNAMIC_PTR_CAST<IGraphNodeComponent>(edge->GetEndNode());


					SceneObjectPtr current_obj = m_Controller->GetEditorSceneManager()->GetScene()->LoadObjectFromTemplate(m_NodeObjectName,parent_obj);
					GASSAssert(current_obj,"Failed to create scene object in GraphTool::MouseDown");

					int from_id = (int) this;
					current_obj->SendImmediateRequest(WorldPositionMessagePtr(new WorldPositionMessage(info.m_3DPos,from_id)));
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
					SceneObjectPtr prev_obj(m_PrevObject,NO_THROW);

					//Check if object under cursor has IGraphNodeComponent to support reconnection
					GraphNodeComponentPtr current_node = obj_under_cursor->GetFirstComponentByClass<IGraphNodeComponent>();
					SceneObjectPtr current_obj;

					if(!current_node && prev_obj != obj_under_cursor)
					{
						current_obj = m_Controller->GetEditorSceneManager()->GetScene()->LoadObjectFromTemplate(m_NodeObjectName,parent_obj);
						GASSAssert(current_obj,"Failed to create scene object in GraphTool::MouseDown");

						int from_id = (int) this;
						current_obj->SendImmediateRequest(WorldPositionMessagePtr(new WorldPositionMessage(info.m_3DPos,from_id)));
						current_node = current_obj->GetFirstComponentByClass<IGraphNodeComponent>();
						GASSAssert(current_node,"Failed to find IGraphNodeComponent in GraphTool::MouseDown");
						//auto insert?
						_TryInsert(current_obj, info.m_3DPos, parent_obj);
					}
					else
					{
						current_obj = obj_under_cursor;
					}
					
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
						GraphComponentPtr graph = parent_obj->GetFirstComponentByClass<IGraphComponent>();
						GASSAssert(graph,"Failed to find IGraphComponent in GraphTool::MouseDown");
						graph->RebuildGraph();
					}
					m_PrevObject = current_obj;
				}
			}
		}
	}

	void GraphTool::MouseUp(const MouseData &data, const SceneCursorInfo &info)
	{
		m_MouseIsDown = false;
	}

	void GraphTool::_TryInsert(SceneObjectPtr new_obj, const Vec3 &obj_pos, SceneObjectPtr parent_obj) const
	{
		GraphEdgeComponentPtr edge = _GetClosestEdge(parent_obj, obj_pos,1.0);
		if(edge)
		{
			GraphNodeComponentPtr start_node = DYNAMIC_PTR_CAST<IGraphNodeComponent>(edge->GetStartNode());
			GraphNodeComponentPtr end_node = DYNAMIC_PTR_CAST<IGraphNodeComponent>(edge->GetEndNode());
		
			GraphNodeComponentPtr current_node = new_obj->GetFirstComponentByClass<IGraphNodeComponent>();
			GASSAssert(current_node,"Failed to find IGraphNodeComponent in GraphTool::_Insert");

			edge->SetEndNode(current_node);
			current_node->AddEdge(edge);
			end_node->RemoveEdge(edge);

			GASS::SceneObjectPtr edge_object = m_Controller->GetEditorSceneManager()->GetScene()->LoadObjectFromTemplate(m_EdgeObjectName,parent_obj);
			GASSAssert(edge_object,"Failed to LoadObjectFromTemplate in GraphTool::_Insert");
			GraphEdgeComponentPtr new_edge = edge_object->GetFirstComponentByClass<IGraphEdgeComponent>();
			GASSAssert(new_edge,"Failed to find IGraphEdgeComponent in GraphTool::_Insert");

			new_edge->SetStartNode(current_node);
			new_edge->SetEndNode(end_node);
			end_node->AddEdge(new_edge);
			current_node->AddEdge(new_edge);
		}
	}


	GraphEdgeComponentPtr GraphTool::_GetClosestEdge(SceneObjectPtr graph_obj, const Vec3 &pos, Float treshhold_dist) const 
	{
		GraphEdgeComponentPtr edge;
		int index = -1;
		Float min_dist = treshhold_dist;
		//get closest edge and check if it's within threshold!
		IComponentContainer::ComponentVector comps;
		graph_obj->GetComponentsByClass<IGraphEdgeComponent>(comps,true);
		for(size_t i =  0; i < comps.size(); i++)
		{
			GraphEdgeComponentPtr edge = DYNAMIC_PTR_CAST<IGraphEdgeComponent>(comps[i]);
			BaseSceneComponentPtr start_node = DYNAMIC_PTR_CAST<BaseSceneComponent>(edge->GetStartNode());
			BaseSceneComponentPtr end_node = DYNAMIC_PTR_CAST<BaseSceneComponent>(edge->GetEndNode());
			if(start_node && end_node)
			{
				Vec3 start_pos  = start_node->GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->GetPosition();
				Vec3 end_pos  = end_node->GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->GetPosition();
				Vec3 cpos = Math::ClosestPointOnLine(start_pos, end_pos, pos);
				Float dist = (cpos - pos).Length();
				if(dist < min_dist) 
				{
					min_dist = dist;
					index = i;
				}
			}
		}

		if(index > -1)
		{
			edge = DYNAMIC_PTR_CAST<IGraphEdgeComponent>(comps[index]);
		}
		return edge;
	}

}
