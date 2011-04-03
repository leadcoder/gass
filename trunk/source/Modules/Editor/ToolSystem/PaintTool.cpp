#include "PaintTool.h"
#include "MouseToolController.h"
#include "../EditorManager.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/IMessage.h"
#include "Core/ComponentSystem/IComponent.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Components/Graphics/ILocationComponent.h"


namespace GASS
{

	PaintTool::PaintTool(MouseToolController* controller): m_MouseIsDown(false),
		m_Controller(controller)
	{

	}

	PaintTool::~PaintTool()
	{

	}

	void PaintTool::MoveTo(const CursorInfo &info)
	{
		int from_id = (int) this;
		SceneObjectPtr selected(m_SelectedObject,boost::detail::sp_nothrow_tag());
		if(m_MouseIsDown)// && selected)
		{
			GASS::MessagePtr paint_msg(new PaintMessage(info.m_3DPos,selected,from_id));
			EditorManager::GetPtr()->GetMessageManager()->SendImmediate(paint_msg);
			/*int from_id = (int) this;
			boost::shared_ptr<GASS::Message> rot_msg(new GASS::Message(GASS::ScenarioScene::OBJECT_MESSAGE_ROTATION,from_id));
			rot_msg->SetData("Rotation",Quaternion(new_rot));
			m_SelectedObject->GetMessageManager()->SendImmediate(rot_msg);*/
		}
	}

	void PaintTool::MouseDown(const CursorInfo &info)
	{
		m_MouseIsDown = true;
	}

	void PaintTool::MouseUp(const CursorInfo &info)
	{
		m_MouseIsDown = false;

	}

	void PaintTool::Start()
	{

	}

	void PaintTool::Stop()
	{

	}
}
