
#include "GraphTool.h"
#include "MouseToolController.h"
#include "Modules/Editor/EditorSystem.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/Utils/GASSException.h"
#include "Core/Math/GASSLineSegment.h"
#include "Core/Math/GASSMath.h"
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
		m_Mode(GTM_ADD)
	{
		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(GraphTool::OnToolChanged,ToolChangedEvent,0));
	}

	GraphTool::~GraphTool()
	{

	}

	void GraphTool::MouseMoved(const MouseData &/*data*/, const SceneCursorInfo &/*info*/)
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

	void GraphTool::MouseDown(const MouseData &/*data*/, const SceneCursorInfo &info)
	{
		m_MouseIsDown = true;
		SceneObjectPtr obj_under_cursor = info.m_ObjectUnderCursor.lock();

		SceneObjectPtr parent_obj = m_ParentObject.lock();
		if(parent_obj && obj_under_cursor)
		{
			if(m_Mode == GTM_INSERT)
			{
				/*Float min_dist  = 1;
				int index = -1;
				//get closest edge and check if it's within threshold!
				SceneObject::ComponentVector comps;
				parent_obj->GetComponentsByClass<IGraphEdgeComponent>(comps,true);
				for(size_t i =  0; i < comps.size(); i++)
				{
					GraphEdgeComponentPtr edge = GASS_DYNAMIC_PTR_CAST<IGraphEdgeComponent>(comps[i]);
					BaseSceneComponentPtr start_node = GASS_DYNAMIC_PTR_CAST<BaseSceneComponent>(edge->GetStartNode());
					BaseSceneComponentPtr end_node = GASS_DYNAMIC_PTR_CAST<BaseSceneComponent>(edge->GetEndNode());
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
				GraphEdgeComponentPtr edge =  GetClosestEdge(parent_obj,info.m_3DPos , 1.0);
				if(edge)
				{
					GraphNodeComponentPtr start_node = GASS_DYNAMIC_PTR_CAST<IGraphNodeComponent>(edge->GetStartNode());
					GraphNodeComponentPtr end_node = GASS_DYNAMIC_PTR_CAST<IGraphNodeComponent>(edge->GetEndNode());

					SceneObjectPtr current_obj = SimEngine::Get().CreateObjectFromTemplate(m_NodeObjectName);
					parent_obj->AddChildSceneObject(current_obj,true);
				
					GASSAssert(current_obj,"Failed to create scene object in GraphTool::MouseDown");

					current_obj->GetFirstComponentByClass<ILocationComponent>()->SetWorldPosition(info.m_3DPos);
					
					GraphNodeComponentPtr current_node = current_obj->GetFirstComponentByClass<IGraphNodeComponent>();
					GASSAssert(current_node,"Failed to find IGraphNodeComponent in GraphTool::MouseDown");

					edge->SetEndNode(current_node);
					current_node->AddEdge(edge);
					end_node->RemoveEdge(edge);
					
					SceneObjectPtr edge_object = SimEngine::Get().CreateObjectFromTemplate(m_EdgeObjectName);
					GASSAssert(edge_object,"Failed to CreateObjectFromTemplate in GraphTool::MouseDown");
					parent_obj->AddChildSceneObject(edge_object ,true);
					
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
					SceneObjectPtr prev_obj = m_PrevObject.lock();

					//Check if object under cursor has IGraphNodeComponent to support reconnection
					GraphNodeComponentPtr current_node = obj_under_cursor->GetFirstComponentByClass<IGraphNodeComponent>();
					SceneObjectPtr current_obj;

					if(!current_node && prev_obj != obj_under_cursor)
					{
						//current_obj = m_Controller->GetEditorSceneManager()->GetScene()->LoadObjectFromTemplate(m_NodeObjectName,parent_obj);
						current_obj = SimEngine::Get().CreateObjectFromTemplate(m_NodeObjectName);
						parent_obj->AddChildSceneObject(current_obj,true);

						GASSAssert(current_obj,"Failed to create scene object in GraphTool::MouseDown");
					
						current_obj->GetFirstComponentByClass<ILocationComponent>()->SetWorldPosition(info.m_3DPos);
						current_node = current_obj->GetFirstComponentByClass<IGraphNodeComponent>();
						GASSAssert(current_node,"Failed to find IGraphNodeComponent in GraphTool::MouseDown");
						//auto insert?
						TryInsert(current_obj, info.m_3DPos, parent_obj);
					}
					else
					{
						current_obj = obj_under_cursor;
					}

					if(prev_obj)
					{
						GraphNodeComponentPtr prev_node = prev_obj->GetFirstComponentByClass<IGraphNodeComponent>();
						GASSAssert(prev_node,"Failed to find IGraphNodeComponent in GraphTool::MouseDown");


						//GASS::SceneObjectPtr edge_object = m_Controller->GetEditorSceneManager()->GetScene()->LoadObjectFromTemplate(m_EdgeObjectName,parent_obj);
						SceneObjectPtr edge_object = SimEngine::Get().CreateObjectFromTemplate(m_EdgeObjectName);
						GASSAssert(edge_object,"Failed to CreateObjectFromTemplate in GraphTool::MouseDown");
						parent_obj->AddChildSceneObject(edge_object,true);
						
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

	void GraphTool::MouseUp(const MouseData &/*data*/, const SceneCursorInfo &/*info*/)
	{
		m_MouseIsDown = false;
	}

	void GraphTool::TryInsert(SceneObjectPtr new_obj, const Vec3 &obj_pos, SceneObjectPtr parent_obj) const
	{
		GraphEdgeComponentPtr edge = GetClosestEdge(parent_obj, obj_pos,1.0);
		if(edge)
		{
			GraphNodeComponentPtr start_node = GASS_DYNAMIC_PTR_CAST<IGraphNodeComponent>(edge->GetStartNode());
			GraphNodeComponentPtr end_node = GASS_DYNAMIC_PTR_CAST<IGraphNodeComponent>(edge->GetEndNode());

			GraphNodeComponentPtr current_node = new_obj->GetFirstComponentByClass<IGraphNodeComponent>();
			GASSAssert(current_node,"Failed to find IGraphNodeComponent in GraphTool::_Insert");

			edge->SetEndNode(current_node);
			current_node->AddEdge(edge);
			end_node->RemoveEdge(edge);

			SceneObjectPtr edge_object = SimEngine::Get().CreateObjectFromTemplate(m_EdgeObjectName);
			GASSAssert(edge_object,"Failed to CreateObjectFromTemplate in GraphTool::_Insert");
			parent_obj->AddChildSceneObject(edge_object,true);
			
			GraphEdgeComponentPtr new_edge = edge_object->GetFirstComponentByClass<IGraphEdgeComponent>();
			GASSAssert(new_edge,"Failed to find IGraphEdgeComponent in GraphTool::_Insert");

			new_edge->SetStartNode(current_node);
			new_edge->SetEndNode(end_node);
			end_node->AddEdge(new_edge);
			current_node->AddEdge(new_edge);
		}
	}


	GraphEdgeComponentPtr GraphTool::GetClosestEdge(SceneObjectPtr graph_obj, const Vec3 &pos, Float treshhold_dist) const
	{
		
		int index = -1;
		Float min_dist = treshhold_dist;
		//get closest edge and check if it's within threshold!
		SceneObject::ComponentVector comps;
		graph_obj->GetComponentsByClass<IGraphEdgeComponent>(comps,true);
		for(size_t i =  0; i < comps.size(); i++)
		{
			auto edge = GASS_DYNAMIC_PTR_CAST<IGraphEdgeComponent>(comps[i]);
			auto start_node = GASS_DYNAMIC_PTR_CAST<Component>(edge->GetStartNode());
			auto end_node = GASS_DYNAMIC_PTR_CAST<Component>(edge->GetEndNode());
			if(start_node && end_node)
			{
				Vec3 start_pos  = start_node->GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->GetPosition();
				Vec3 end_pos  = end_node->GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->GetPosition();
				Vec3 cpos = LineSegment(start_pos, end_pos).ClosestPointOnLine(pos);
				Float dist = (cpos - pos).Length();
				if(dist < min_dist)
				{
					min_dist = dist;
					index = static_cast<int>(i);
				}
			}
		}

		GraphEdgeComponentPtr edge;
		if(index > -1)
		{
			edge = GASS_DYNAMIC_PTR_CAST<IGraphEdgeComponent>(comps[index]);
		}
		return edge;
	}
}
