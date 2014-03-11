#include "SelectTool.h"
#include "MouseToolController.h"
#include "Modules/Editor/EditorSystem.h"
#include "Modules/Editor/EditorSceneManager.h"
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

	void SelectTool::MouseMoved(const MouseData &data, const SceneCursorInfo &info)
	{

	}

	void SelectTool::MouseDown(const MouseData &data, const SceneCursorInfo &info)
	{
		m_MouseIsDown = true;
		SceneObjectPtr obj_under_cursor(info.m_ObjectUnderCursor,NO_THROW);
		if(obj_under_cursor && !m_Controller->GetEditorSceneManager()->IsObjectStatic(obj_under_cursor))
		{
			m_Controller->GetEditorSceneManager()->SelectSceneObject(obj_under_cursor);
			if(m_Controller->GetEditorSceneManager()->IsObjectLocked(obj_under_cursor))
			{
				m_SelectedObject = obj_under_cursor;
				obj_under_cursor->PostRequest(CollisionSettingsMessagePtr(new CollisionSettingsMessage(false,(int) this)));
			}
		}
	}

	void SelectTool::MouseUp(const MouseData &data, const SceneCursorInfo &info)
	{
		m_MouseIsDown = false;
		SceneObjectPtr selected(m_SelectedObject,NO_THROW);
		if(selected)
		{
			int from_id = (int) this;
			selected->PostRequest(CollisionSettingsMessagePtr(new CollisionSettingsMessage(true,from_id)));
		}
		m_SelectedObject.reset();
	}
}

