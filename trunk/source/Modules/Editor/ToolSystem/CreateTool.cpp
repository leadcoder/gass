#include <boost/bind.hpp>
#include "CreateTool.h"
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

	void CreateTool::MouseMoved(const MouseData &data, const SceneCursorInfo &info)
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

	void CreateTool::MouseDown(const MouseData &data, const SceneCursorInfo &info)
	{
		m_MouseIsDown = true;
		SceneObjectPtr obj_under_cursor (info.m_ObjectUnderCursor,NO_THROW);

		SceneObjectPtr parent_obj(m_ParentObject,NO_THROW);
		if(obj_under_cursor && parent_obj)
		{
			GASS::SceneObjectPtr scene_object = m_Controller->GetEditorSceneManager()->GetScene()->LoadObjectFromTemplate(m_ObjectName,parent_obj);
			if(scene_object)
			{
				int from_id = (int) this;
				GASS::MessagePtr pos_msg(new GASS::WorldPositionMessage(info.m_3DPos,from_id));
				scene_object->SendImmediate(pos_msg);
			}
			else
			{
				//failed to create object
				GASS_EXCEPT(Exception::ERR_INTERNAL_ERROR,"Failed to create object","MouseToolController::Init()");
			}
		}
	}

	void CreateTool::MouseUp(const MouseData &data, const SceneCursorInfo &info)
	{
		m_MouseIsDown = false;
	}

}
