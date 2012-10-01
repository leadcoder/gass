#include "EditPositionTool.h"
#include "MouseToolController.h"
#include "../EditorManager.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/ComponentSystem/GASSIComponent.h"
#include "Sim/Scene/GASSScene.h"
#include "Sim/Scene/GASSSceneObject.h"
#include "Sim/Components/Graphics/GASSILocationComponent.h"

#include "Sim/Scene/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Scene/GASSPhysicsSceneObjectMessages.h"
#include "Plugins/Game/GameMessages.h"


namespace GASS
{

	EditPositionTool::EditPositionTool(MouseToolController* controller): m_MouseIsDown(false),
		m_Controller(controller)
	{
		EditorManager::GetPtr()->GetMessageManager()->RegisterForMessage(REG_TMESS(EditPositionTool::OnSceneObjectSelected,ObjectSelectionChangedMessage,0));
	}

	EditPositionTool::~EditPositionTool()
	{

	}

	void EditPositionTool::MoveTo(const CursorInfo &info)
	{

	}

	void EditPositionTool::MouseDown(const CursorInfo &info)
	{
		m_MouseIsDown = true;
		SceneObjectPtr selected(m_SelectedObject,boost::detail::sp_nothrow_tag());
		SceneObjectPtr object_under_c(info.m_ObjectUnderCursor,boost::detail::sp_nothrow_tag());
		
		if(selected && object_under_c)
		{
			//Send message
			int from_id = (int) this;
			MessagePtr goto_msg(new EditPositionMessage(info.m_3DPos,from_id));
			selected->PostMessage(goto_msg);
		}
	}


	void EditPositionTool::MouseUp(const CursorInfo &info)
	{
		m_MouseIsDown = false;
	}


	void EditPositionTool::OnSceneObjectSelected(ObjectSelectionChangedMessagePtr message)
	{
		m_SelectedObject = message->GetSceneObject();
	}
	
}

