#include "VerticalMoveTool.h"
#include "MouseToolController.h"
#include "Modules/Editor/EditorSceneManager.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/Interface/GASSILocationComponent.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSPhysicsSceneObjectMessages.h"
#include "Sim/Interface/GASSILocationComponent.h"


namespace GASS
{
	VerticalMoveTool::VerticalMoveTool(MouseToolController* controller):m_MouseIsDown(false),
		m_Controller(controller)
	{

	}

	VerticalMoveTool::~VerticalMoveTool()
	{

	}

	void VerticalMoveTool::MouseMoved(const MouseData &/*data*/, const SceneCursorInfo &/*info*/)
	{
		SceneObjectPtr selected = m_SelectedObject.lock();
		if(m_MouseIsDown && selected)
		{
			LocationComponentPtr comp = selected->GetFirstComponentByClass<GASS::ILocationComponent>();
			if(comp)
			{
				ScenePtr scene = selected->GetScene();
				Vec3 up(0,1,0);
				Vec3 new_position = comp->GetPosition();

				//move selected object
				//up = up*(-info.m_Delta.y);
				new_position = new_position + up;

				int from_id = GASS_PTR_TO_INT(this);

				comp->SetPosition(new_position);

				GASS::SystemMessagePtr change_msg(new SceneChangedEvent(from_id));
				SimEngine::Get().GetSimSystemManager()->SendImmediate(change_msg);
			}
		}
	}

	void VerticalMoveTool::MouseDown(const MouseData &/*data*/, const SceneCursorInfo &info)
	{
		m_MouseIsDown = true;

		SceneObjectPtr obj_under_cursor = info.m_ObjectUnderCursor.lock();
		if(obj_under_cursor && !m_Controller->GetEditorSceneManager()->IsObjectStatic(obj_under_cursor))
		{
			m_Controller->GetEditorSceneManager()->SelectSceneObject(obj_under_cursor);

			if(!m_Controller->GetEditorSceneManager()->IsObjectLocked(obj_under_cursor))
			{
				int from_id = GASS_PTR_TO_INT(this);
				m_SelectedObject = obj_under_cursor;
				obj_under_cursor->SendImmediateRequest(CollisionSettingsRequestPtr(new CollisionSettingsRequest(false,from_id)));
			}
		}
	}

	void VerticalMoveTool::MouseUp(const MouseData &/*data*/, const SceneCursorInfo &/*info*/)
	{
		m_MouseIsDown = false;
		SceneObjectPtr selected = m_SelectedObject.lock();
		if(selected)
		{
			int from_id = GASS_PTR_TO_INT(this);
			selected->SendImmediateRequest(CollisionSettingsRequestPtr(new CollisionSettingsRequest(true,from_id)));

		}
		m_SelectedObject.reset();;
	}
}

