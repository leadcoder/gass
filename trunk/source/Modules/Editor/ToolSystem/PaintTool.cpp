#include "PaintTool.h"
#include "MouseToolController.h"
#include "../EditorManager.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/IMessage.h"
#include "Core/ComponentSystem/IComponent.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Components/Graphics/ILocationComponent.h"
#include "Core/ComponentSystem/BaseComponentContainerTemplateManager.h"
#include "Sim/Scenario/Scene/SceneObjectManager.h"
#include "Sim/Components/Graphics/Geometry/ITerrainComponent.h"
#include "Sim/Scenario/Scene/Messages/GraphicsSceneObjectMessages.h"
#include "Sim/Scenario/Scene/Messages/PhysicsSceneObjectMessages.h"


namespace GASS
{

	PaintTool::PaintTool(MouseToolController* controller): m_MouseIsDown(false),
		m_Controller(controller)
	{
		EditorManager::GetPtr()->GetMessageManager()->RegisterForMessage(REG_TMESS(PaintTool::OnSceneObjectSelected,ObjectSelectedMessage,0));
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
			if(selected)
			{
				TerrainComponentPtr terrain = selected->GetFirstComponentByClass<ITerrainComponent>();
				if(terrain)
				{
					selected->GetParentSceneObject()->PostMessage(MessagePtr(new TerrainHeightModifyMessage(TerrainHeightModifyMessage::MT_DEFORM,info.m_3DPos,116, 90,1.0)));
				}
			}

			GASS::MessagePtr paint_msg(new PaintMessage(info.m_3DPos,selected,from_id));
			EditorManager::GetPtr()->GetMessageManager()->SendImmediate(paint_msg);
			/*int from_id = (int) this;
			boost::shared_ptr<GASS::Message> rot_msg(new GASS::Message(GASS::ScenarioScene::OBJECT_MESSAGE_ROTATION,from_id));
			rot_msg->SetData("Rotation",Quaternion(new_rot));
			m_SelectedObject->GetMessageManager()->SendImmediate(rot_msg);*/
		}
		SceneObjectPtr gizmo = GetMasterGizmo();
		if(gizmo)
		{
			GASS::MessagePtr pos_msg(new GASS::WorldPositionMessage(info.m_3DPos,from_id));
			gizmo->PostMessage(pos_msg);
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

	void PaintTool::Stop()
	{
		SetGizmoVisiblity(false);
		m_Active = false;
	}

	void PaintTool::Start() 
	{
		SetGizmoVisiblity(true);
		m_Active = true;
	}

	SceneObjectPtr PaintTool::GetMasterGizmo()
	{
		SceneObjectPtr gizmo(m_MasterGizmoObject,boost::detail::sp_nothrow_tag());
		if(!gizmo &&  m_Controller->GetScene())
		{
			ScenarioScenePtr scene = m_Controller->GetScene();
			std::string gizmo_name = "PaintGizmo";
			GASS::SceneObjectPtr scene_object = m_Controller->GetScene()->GetObjectManager()->LoadFromTemplate(gizmo_name);
			m_MasterGizmoObject = scene_object;
			gizmo = scene_object;

			//Send selection message to inform gizmo about current object
			if(gizmo)
			{
				SceneObjectPtr current (m_SelectedObject,boost::detail::sp_nothrow_tag());
				if(current)
				{
					MessagePtr selection_msg(new ObjectSelectedMessage(current,(int) this));
					EditorManager::GetPtr()->GetMessageManager()->PostMessage(selection_msg);
				}
			}
		}
		return gizmo;
	}

	void PaintTool::SetGizmoVisiblity(bool value)
	{
		SceneObjectPtr gizmo = GetMasterGizmo();
		if(gizmo)
		{
			int from_id = (int) this;
			MessagePtr col_msg(new GASS::CollisionSettingsMessage(value,from_id));
			SendMessageRec(gizmo,col_msg);
			MessagePtr vis_msg(new GASS::VisibilityMessage(value,from_id));
			SendMessageRec(gizmo,vis_msg);
		}
	}

	void PaintTool::OnSceneObjectSelected(ObjectSelectedMessagePtr message)
	{
		if(m_Active)
		{
			//hide gizmo
			if(message->GetSceneObject())
			{
				LocationComponentPtr lc = message->GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
				if(lc) //only support gizmo for objects with location component
				{
					SetGizmoVisiblity(true);
				}
				else
				{
					SetGizmoVisiblity(false);
				}
			}
			else
			{
				SetGizmoVisiblity(false);
			}
		}
		m_SelectedObject = message->GetSceneObject();
	}


	void PaintTool::SendMessageRec(SceneObjectPtr obj,MessagePtr msg)
	{
		obj->PostMessage(msg);
		GASS::IComponentContainer::ComponentContainerIterator iter = obj->GetChildren();
		while(iter.hasMoreElements())
		{
			SceneObjectPtr child = boost::shared_static_cast<SceneObject>(iter.getNext());
			SendMessageRec(child,msg);
		}
	}
}
