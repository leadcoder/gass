
#include "CreateTool.h"
#include "MouseToolController.h"
#include "Modules/Editor/EditorSystem.h"
#include "Modules/Editor/EditorSceneManager.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/Utils/GASSException.h"
#include "Core/ComponentSystem/GASSComponent.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/Interface/GASSILocationComponent.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"



namespace GASS
{

	CreateTool::CreateTool(MouseToolController* controller): m_MouseIsDown(false),
		m_Controller(controller),
		m_FirstMoveUpdate(true)
	{
		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(CreateTool::OnToolChanged,ToolChangedEvent,0));
	}

	CreateTool::~CreateTool()
	{

	}

	void CreateTool::MouseMoved(const MouseData &/*data*/, const SceneCursorInfo &/*info*/)
	{
	}

	void CreateTool::OnToolChanged(ToolChangedEventPtr message)
	{
		std::string new_tool = message->GetTool();
		if(new_tool == TID_CREATE)
		{
			//m_ObjectName = message->GetCreateObjectName();
			//m_ParentObject = message->GetCreateParentObject();
		}

	}

	void CreateTool::MouseDown(const MouseData &/*data*/, const SceneCursorInfo &info)
	{
		m_MouseIsDown = true;
		SceneObjectPtr obj_under_cursor =info.m_ObjectUnderCursor.lock();

		SceneObjectPtr parent_obj = m_ParentObject.lock();
		if(obj_under_cursor && parent_obj)
		{
			SceneObjectPtr scene_object = SimEngine::Get().CreateObjectFromTemplate(m_ObjectName);
			//GASS::SceneObjectPtr scene_object = m_Controller->GetEditorSceneManager()->GetScene()->LoadObjectFromTemplate(m_ObjectName,parent_obj);
			if(scene_object)
			{
				parent_obj->AddChildSceneObject(scene_object,true);
				int from_id = GASS_PTR_TO_INT(this);
				scene_object->SendImmediateRequest(WorldPositionRequestPtr(new WorldPositionRequest(info.m_3DPos,from_id)));
			}
			else
			{
				//failed to create object
				GASS_EXCEPT(Exception::ERR_INTERNAL_ERROR,"Failed to create object","MouseToolController::Init()");
			}
		}
	}

	void CreateTool::MouseUp(const MouseData &/*data*/, const SceneCursorInfo &/*info*/)
	{
		m_MouseIsDown = false;
	}

}
