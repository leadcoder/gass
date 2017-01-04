#include "GoToPositionTool.h"
#include "MouseToolController.h"
#include "Modules/Editor/EditorSceneManager.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSPlatformMessages.h"


namespace GASS
{

	GoToPositionTool::GoToPositionTool(MouseToolController* controller): m_MouseIsDown(false),
		m_Controller(controller)
	{
		controller->GetEditorSceneManager()->GetScene()->RegisterForMessage(REG_TMESS(GoToPositionTool::OnSelectionChanged,EditorSelectionChangedEvent,0));
	}

	GoToPositionTool::~GoToPositionTool()
	{

	}

	void GoToPositionTool::MouseMoved(const MouseData &/*data*/, const SceneCursorInfo &/*info*/)
	{
	}

	void GoToPositionTool::MouseDown(const MouseData &/*data*/, const SceneCursorInfo &info)
	{
		m_MouseIsDown = true;
		SceneObjectPtr object_under_cursor = info.m_ObjectUnderCursor.lock();

		if(object_under_cursor)
		{
			for(size_t i = 0; i< m_Selection.size(); i++)
			{
				SceneObjectPtr selected = m_Selection[i].lock();

				//Send message
				int from_id = GASS_PTR_TO_INT(this);
				//MessagePtr goto_msg(new GotoPositionRequest(info.m_3DPos,from_id));

				Vec3 dir(1,0,1);
				dir.Normalize();
				FaceDirectionRequestPtr fd_msg(new FaceDirectionRequest(dir,from_id));
				selected->PostRequest(PathfindToPositionMessagePtr(new PathfindToPositionMessage(info.m_3DPos,from_id)));
				selected->PostRequest(fd_msg);
			}
		}
	}

	void GoToPositionTool::MouseUp(const MouseData &/*data*/, const SceneCursorInfo &/*info*/)
	{
		m_MouseIsDown = false;
	}

	void GoToPositionTool::OnSelectionChanged(EditorSelectionChangedEventPtr message)
	{
		m_Selection = message->m_Selection;
	}
}

