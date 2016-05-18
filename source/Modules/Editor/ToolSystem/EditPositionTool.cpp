#include "EditPositionTool.h"
#include "MouseToolController.h"
#include "Modules/Editor/EditorSystem.h"
#include "Modules/Editor/EditorSceneManager.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/ComponentSystem/GASSComponent.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/Interface/GASSILocationComponent.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSPhysicsSceneObjectMessages.h"
#include "Sim/Messages/GASSPlatformMessages.h"


namespace GASS
{

	EditPositionTool::EditPositionTool(MouseToolController* controller): m_MouseIsDown(false),
		m_Controller(controller)
	{

		controller->GetEditorSceneManager()->GetScene()->RegisterForMessage(REG_TMESS(EditPositionTool::OnSelectionChanged, EditorSelectionChangedEvent, 0));
	}

	EditPositionTool::~EditPositionTool()
	{

	}

	void EditPositionTool::MouseMoved(const MouseData &/*data*/, const SceneCursorInfo &/*info*/)
	{
	}

	void EditPositionTool::MouseDown(const MouseData &/*data*/, const SceneCursorInfo &info)
	{
		m_MouseIsDown = true;
		SceneObjectPtr object_under_cursor = info.m_ObjectUnderCursor.lock();
		int from_id = GASS_PTR_TO_INT(this);
		if(object_under_cursor)
		{
			for (size_t i = 0; i< m_Selection.size(); i++)
			{
				SceneObjectPtr obj = m_Selection[i].lock();
				if(obj)
					obj->PostRequest(EditPositionMessagePtr(new EditPositionMessage(info.m_3DPos, from_id)));
			}
		}
	}

	void EditPositionTool::MouseUp(const MouseData &/*data*/, const SceneCursorInfo &/*info*/)
	{
		m_MouseIsDown = false;
	}

	void EditPositionTool::OnSelectionChanged(EditorSelectionChangedEventPtr message)
	{
		m_Selection = message->m_Selection;
	}
}

