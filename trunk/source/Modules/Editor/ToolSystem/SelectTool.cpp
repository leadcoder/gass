#include "SelectTool.h"
#include "MouseToolController.h"
#include "../EditorManager.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/IMessage.h"
#include "Core/ComponentSystem/IComponent.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Components/Graphics/ILocationComponent.h"
#include "Sim/Scenario/Scene/SceneObjectManager.h"
#include "Sim/Scenario/Scene/Messages/GraphicsSceneObjectMessages.h"
#include "Sim/Scenario/Scene/Messages/PhysicsSceneObjectMessages.h"


namespace GASS
{

	SelectTool::SelectTool(MouseToolController* controller): m_MouseIsDown(false),
		m_Controller(controller)
	{

	}

	SelectTool::~SelectTool()
	{

	}

	void SelectTool::MoveTo(const CursorInfo &info)
	{

	}

	void SelectTool::MouseDown(const CursorInfo &info)
	{
		m_MouseIsDown = true;

		SceneObjectPtr obj_under_cursor(info.m_ObjectUnderCursor,boost::detail::sp_nothrow_tag());
		if(obj_under_cursor && !m_Controller->IsObjectStatic(obj_under_cursor))
		{

			//Send selection message
			int from_id = (int) this;
			MessagePtr selection_msg(new ObjectSelectedMessage(obj_under_cursor,from_id));
			EditorManager::GetPtr()->GetMessageManager()->PostMessage(selection_msg);

			if(!m_Controller->IsObjectLocked(obj_under_cursor))
			{
				m_SelectedObject = obj_under_cursor;
				MessagePtr col_msg(new GASS::CollisionSettingsMessage(false,from_id));
				//bool value = false;
				//col_msg->SetData("Enable",value);
				obj_under_cursor->PostMessage(col_msg);
			}
		}
	}

	void SelectTool::MouseUp(const CursorInfo &info)
	{
		m_MouseIsDown = false;
		SceneObjectPtr selected(m_SelectedObject,boost::detail::sp_nothrow_tag());
		if(selected)
		{
			int from_id = (int) this;
			MessagePtr col_msg(new GASS::CollisionSettingsMessage(true,from_id));
			selected->PostMessage(col_msg);

		}
		m_SelectedObject.reset();;
	}


	
}

