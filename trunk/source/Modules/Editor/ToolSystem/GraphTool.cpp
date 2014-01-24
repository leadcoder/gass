
#include "GraphTool.h"
#include "MouseToolController.h"
#include "Modules/Editor/EditorSystem.h"
#include "Modules/Editor/EditorSceneManager.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/Utils/GASSException.h"
#include "Core/ComponentSystem/GASSIComponent.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSimEngine.h"
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
		m_FirstMoveUpdate(true)
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
		if(obj_under_cursor && parent_obj)
		{
			{
				GASS::SceneObjectPtr scene_object = m_Controller->GetEditorSceneManager()->GetScene()->LoadObjectFromTemplate(m_NodeObjectName,parent_obj);
				if(scene_object)
				{
					int from_id = (int) this;
					GASS::MessagePtr pos_msg(new GASS::WorldPositionMessage(info.m_3DPos,from_id));
					scene_object->SendImmediate(pos_msg);

					GraphNodeComponentPtr current_node = scene_object->GetFirstComponentByClass<IGraphNodeComponent>();
					GASSAssert(current_node,"Failed to find IGraphNodeComponent in GraphTool::MouseDown");
					
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
					}
					m_PrevObject = scene_object;

					GraphComponentPtr graph = parent_obj->GetFirstComponentByClass<IGraphComponent>();
					GASSAssert(graph,"Failed to find IGraphComponent in GraphTool::MouseDown");
					graph->RebuildGraph();
				}
				else
				{
					//failed to create object
					GASS_EXCEPT(Exception::ERR_INTERNAL_ERROR,"Failed to create object","MouseToolController::Init()");
				}
			}
		}
	}

	void GraphTool::MouseUp(const MouseData &data, const SceneCursorInfo &info)
	{
		m_MouseIsDown = false;
	}

}
