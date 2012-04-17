#include "VerticalMoveTool.h"
#include "MouseToolController.h"
#include "../EditorManager.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/ComponentSystem/GASSIComponent.h"
#include "Sim/Scenario/GASSScenario.h"
#include "Sim/Scenario/Scene/GASSSceneObject.h"
#include "Sim/Components/Graphics/GASSILocationComponent.h"
#include "Sim/Scenario/Scene/GASSSceneObjectManager.h"
#include "Sim/Scenario/Scene/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Scenario/Scene/Messages/GASSPhysicsSceneObjectMessages.h"


namespace GASS
{

	VerticalMoveTool::VerticalMoveTool(MouseToolController* controller):m_MouseIsDown(false),
		m_Controller(controller)
	{

	}

	VerticalMoveTool::~VerticalMoveTool()
	{

	}

	void VerticalMoveTool::MoveTo(const CursorInfo &info)
	{
		SceneObjectPtr selected(m_SelectedObject,boost::detail::sp_nothrow_tag());
		if(m_MouseIsDown && selected)
		{
			LocationComponentPtr comp = selected->GetFirstComponentByClass<GASS::ILocationComponent>();
			if(comp)
			{
				ScenarioPtr scenario = selected->GetSceneObjectManager()->GetScenario();
				Vec3 up(0,1,0);
				Vec3 new_position = comp->GetPosition();

				//move seleced object
				up = up*(-info.m_Delta.y);
				new_position = new_position + up;

				int from_id = (int) this;
				MessagePtr pos_msg(new GASS::PositionMessage(new_position,from_id));
				selected->SendImmediate(pos_msg);

				GASS::MessagePtr change_msg(new ScenarioChangedMessage(from_id));
				EditorManager::GetPtr()->GetMessageManager()->SendImmediate(change_msg);
			}
		}
	}

	void VerticalMoveTool::MouseDown(const CursorInfo &info)
	{
		m_MouseIsDown = true;

		SceneObjectPtr obj_under_cursor(info.m_ObjectUnderCursor,boost::detail::sp_nothrow_tag());
		if(obj_under_cursor && !m_Controller->IsObjectStatic(obj_under_cursor))
		{
			//fetch object under mouse

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
				obj_under_cursor->SendImmediate(col_msg);

			}
		}
	}

	void VerticalMoveTool::MouseUp(const CursorInfo &info)
	{
		m_MouseIsDown = false;
		SceneObjectPtr selected(m_SelectedObject,boost::detail::sp_nothrow_tag());
		if(selected)
		{
			int from_id = (int) this;
			MessagePtr col_msg(new GASS::CollisionSettingsMessage(true,from_id));
			selected->SendImmediate(col_msg);

		}
		m_SelectedObject.reset();;
	}
}

