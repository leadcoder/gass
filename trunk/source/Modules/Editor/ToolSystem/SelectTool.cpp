#include "SelectTool.h"
#include "MouseToolController.h"
#include "Modules/Editor/EditorSystem.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/ComponentSystem/GASSIComponent.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/Interface/GASSILocationComponent.h"

#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSPhysicsSceneObjectMessages.h"


namespace GASS
{

	SelectTool::SelectTool(MouseToolController* controller): m_MouseIsDown(false),
		m_Controller(controller)
	{

	}

	SelectTool::~SelectTool()
	{

	}

	void SelectTool::MouseMoved(const CursorInfo &info)
	{

	}

	void SelectTool::MouseDown(const CursorInfo &info)
	{
		m_MouseIsDown = true;

		SceneObjectPtr obj_under_cursor(info.m_ObjectUnderCursor,boost::detail::sp_nothrow_tag());
		if(obj_under_cursor)// && !m_Controller->GetEditorSystem()->IsObjectStatic(obj_under_cursor))
		{
			m_Controller->GetEditorSystem()->SelectSceneObject(obj_under_cursor);
			if(m_Controller->GetEditorSystem()->IsObjectLocked(obj_under_cursor))
			{
				m_SelectedObject = obj_under_cursor;
				MessagePtr col_msg(new GASS::CollisionSettingsMessage(false,(int) this));
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

