#include "VerticalMoveTool.h"
#include "MouseToolController.h"
#include "Modules/Editor/EditorSystem.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/ComponentSystem/GASSIComponent.h"
#include "Sim/Scene/GASSScene.h"
#include "Sim/Scene/GASSSceneObject.h"
#include "Sim/Components/Graphics/GASSILocationComponent.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/Systems/GASSSimSystemManager.h"
#include "Sim/Scene/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Scene/GASSPhysicsSceneObjectMessages.h"


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
				ScenePtr scene = selected->GetScene();
				Vec3 up(0,1,0);
				Vec3 new_position = comp->GetPosition();

				//move seleced object
				up = up*(-info.m_Delta.y);
				new_position = new_position + up;

				int from_id = (int) this;
				MessagePtr pos_msg(new GASS::PositionMessage(new_position,from_id));
				selected->SendImmediate(pos_msg);

				GASS::MessagePtr change_msg(new SceneChangedMessage(from_id));
				SimEngine::Get().GetSimSystemManager()->SendImmediate(change_msg);
			}
		}
	}

	void VerticalMoveTool::MouseDown(const CursorInfo &info)
	{
		m_MouseIsDown = true;

		SceneObjectPtr obj_under_cursor(info.m_ObjectUnderCursor,boost::detail::sp_nothrow_tag());
		if(obj_under_cursor && !m_Controller->GetEditorSystem()->IsObjectStatic(obj_under_cursor))
		{
			m_Controller->GetEditorSystem()->SelectSceneObject(obj_under_cursor);

			if(!m_Controller->GetEditorSystem()->IsObjectLocked(obj_under_cursor))
			{
				int from_id = (int) this;
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

