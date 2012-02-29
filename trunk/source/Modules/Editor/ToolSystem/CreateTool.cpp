#include <boost/bind.hpp>
#include "CreateTool.h"
#include "MouseToolController.h"
#include "../EditorManager.h"
#include "Core/MessageSystem/MessageManager.h"

#include "Core/ComponentSystem/IComponent.h"
#include "Sim/Scenario/Scenario.h"
#include "Sim/Scenario/Scene/SceneObjectManager.h"
#include "Sim/Components/Graphics/ILocationComponent.h"
#include "Sim/Scenario/Scene/Messages/GraphicsSceneObjectMessages.h"



namespace GASS
{

	CreateTool::CreateTool(MouseToolController* controller): m_MouseIsDown(false),
		m_Controller(controller),
		m_FirstMoveUpdate(true)
	{
		EditorManager::GetPtr()->GetMessageManager()->RegisterForMessage(REG_TMESS(CreateTool::OnToolChanged,ToolChangedMessage,0));	
	}

	CreateTool::~CreateTool()
	{

	}

	void CreateTool::MoveTo(const CursorInfo &info)
	{

	}

	void CreateTool::OnToolChanged(ToolChangedMessagePtr message)
	{
		std::string new_tool = message->GetTool();
		if(new_tool == "CreateTool")
		{
			m_ObjectName = message->GetCreateObjectName();
			m_ParentObject = message->GetCreateParentObject();
		}

	}

	void CreateTool::MouseDown(const CursorInfo &info)
	{
		m_MouseIsDown = true;
		SceneObjectPtr obj_under_cursor (info.m_ObjectUnderCursor,boost::detail::sp_nothrow_tag());

		SceneObjectPtr parent_obj(m_ParentObject,boost::detail::sp_nothrow_tag());
		if(obj_under_cursor)
		{

			GASS::SceneObjectPtr scene_object = m_Controller->GetScenario()->GetObjectManager()->LoadFromTemplate(m_ObjectName,parent_obj);
			if(scene_object)
			{
				int from_id = (int) this;
				GASS::MessagePtr pos_msg(new GASS::WorldPositionMessage(info.m_3DPos,from_id));
				scene_object->SendImmediate(pos_msg);
			}
			else
			{
				//failed to create object
				Log::Error("Failed to create object");
			}
		}
	}

	void CreateTool::MouseUp(const CursorInfo &info)
	{
		m_MouseIsDown = false;
	}

}
