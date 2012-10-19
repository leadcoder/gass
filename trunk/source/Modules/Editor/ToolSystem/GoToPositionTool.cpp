#include "GoToPositionTool.h"
#include "MouseToolController.h"
#include "Modules/Editor/EditorSystem.h"
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
#include "Plugins/Game/GameMessages.h"


namespace GASS
{

	GoToPositionTool::GoToPositionTool(MouseToolController* controller): m_MouseIsDown(false),
		m_Controller(controller)
	{
		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(GoToPositionTool::OnSceneObjectSelected,ObjectSelectionChangedMessage,0));
	}

	GoToPositionTool::~GoToPositionTool()
	{

	}

	void GoToPositionTool::MoveTo(const CursorInfo &info)
	{

	}

	void GoToPositionTool::MouseDown(const CursorInfo &info)
	{
		m_MouseIsDown = true;
		SceneObjectPtr selected(m_SelectedObject,boost::detail::sp_nothrow_tag());
		SceneObjectPtr object_under_c(info.m_ObjectUnderCursor,boost::detail::sp_nothrow_tag());
		
		if(selected && object_under_c)
		{
			//Send message
			int from_id = (int) this;
			MessagePtr goto_msg(new GotoPositionMessage(info.m_3DPos,from_id));
			selected->PostMessage(goto_msg);
		}
	}


	void GoToPositionTool::MouseUp(const CursorInfo &info)
	{
		m_MouseIsDown = false;
	}


	void GoToPositionTool::OnSceneObjectSelected(ObjectSelectionChangedMessagePtr message)
	{
		m_SelectedObject = message->GetSceneObject();
	}
	
}

