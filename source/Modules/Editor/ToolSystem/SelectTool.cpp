#include "SelectTool.h"
#include "MouseToolController.h"
#include "Modules/Editor/EditorSystem.h"
#include "Modules/Editor/EditorSceneManager.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/ComponentSystem/GASSComponent.h"
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

	void SelectTool::MouseMoved(const MouseData &/*data*/, const SceneCursorInfo &/*info*/)
	{
		
	}

	void SelectTool::MouseDown(const MouseData &/*data*/, const SceneCursorInfo &info)
	{
		m_MouseIsDown = true;
		SceneObjectPtr obj_under_cursor = info.m_ObjectUnderCursor.lock();
		if(obj_under_cursor && !m_Controller->GetEditorSceneManager()->IsObjectStatic(obj_under_cursor))
		{
			m_Controller->GetEditorSceneManager()->SelectSceneObject(obj_under_cursor);
			if(m_Controller->GetEditorSceneManager()->IsObjectLocked(obj_under_cursor))
			{
				m_SelectedObject = obj_under_cursor;
				obj_under_cursor->PostRequest(CollisionSettingsRequestPtr(new CollisionSettingsRequest(false,GASS_PTR_TO_INT(this) )));
			}
		}
	}

	void SelectTool::MouseUp(const MouseData &/*data*/, const SceneCursorInfo &/*info*/)
	{
		m_MouseIsDown = false;
		SceneObjectPtr selected = m_SelectedObject.lock();
		if(selected)
		{
			int from_id = GASS_PTR_TO_INT(this);
			selected->PostRequest(CollisionSettingsRequestPtr(new CollisionSettingsRequest(true,from_id)));
		}
		m_SelectedObject.reset();
	}
}

