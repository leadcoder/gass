#include "VerticalMoveTool.h"
#include "MouseToolController.h"
#include "Modules/Editor/EditorSystem.h"
#include "Modules/Editor/EditorSceneManager.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/ComponentSystem/GASSIComponent.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/Interface/GASSILocationComponent.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSPhysicsSceneObjectMessages.h"


namespace GASS
{

	VerticalMoveTool::VerticalMoveTool(MouseToolController* controller):m_MouseIsDown(false),
		m_Controller(controller)
	{

	}

	VerticalMoveTool::~VerticalMoveTool()
	{

	}

	void VerticalMoveTool::MouseMoved(const MouseData &data, const SceneCursorInfo &info)
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
				//up = up*(-info.m_Delta.y);
				new_position = new_position + up;

				int from_id = (int) this;
				MessagePtr pos_msg(new GASS::PositionMessage(new_position,from_id));
				selected->SendImmediate(pos_msg);

				GASS::SystemMessagePtr change_msg(new SceneChangedEvent(from_id));
				SimEngine::Get().GetSimSystemManager()->SendImmediate(change_msg);
			}
		}
	}

	void VerticalMoveTool::MouseDown(const MouseData &data, const SceneCursorInfo &info)
	{
		m_MouseIsDown = true;

		SceneObjectPtr obj_under_cursor(info.m_ObjectUnderCursor,boost::detail::sp_nothrow_tag());
		if(obj_under_cursor && !m_Controller->GetEditorSceneManager()->IsObjectStatic(obj_under_cursor))
		{
			m_Controller->GetEditorSceneManager()->SelectSceneObject(obj_under_cursor);

			if(!m_Controller->GetEditorSceneManager()->IsObjectLocked(obj_under_cursor))
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

	void VerticalMoveTool::MouseUp(const MouseData &data, const SceneCursorInfo &info)
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

