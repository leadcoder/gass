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

		controller->GetEditorSceneManager()->GetScene()->RegisterForMessage(REG_TMESS(EditPositionTool::OnSceneObjectSelected,ObjectSelectionChangedEvent,0));
	}

	EditPositionTool::~EditPositionTool()
	{

	}

	void EditPositionTool::MouseMoved(const MouseData &data, const SceneCursorInfo &info)
	{

	}

	void EditPositionTool::MouseDown(const MouseData &data, const SceneCursorInfo &info)
	{
		m_MouseIsDown = true;
		SceneObjectPtr selected(m_SelectedObject,NO_THROW);
		SceneObjectPtr object_under_c(info.m_ObjectUnderCursor,NO_THROW);

		if(selected && object_under_c)
		{
			//Send message
			int from_id = PTR_TO_INT(this);

			selected->PostRequest(EditPositionMessagePtr(new EditPositionMessage(info.m_3DPos,from_id)));
		}
	}


	void EditPositionTool::MouseUp(const MouseData &data, const SceneCursorInfo &info)
	{
		m_MouseIsDown = false;
	}

	void EditPositionTool::OnSceneObjectSelected(ObjectSelectionChangedEventPtr message)
	{
		m_SelectedObject = message->GetSceneObject();
	}

}

