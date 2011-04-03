#include "RotateTool.h"
#include "../Components/GizmoComponent.h"

#include "MouseToolController.h"
#include "../EditorManager.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/IMessage.h"
#include "Core/ComponentSystem/IComponent.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Scenario/Scene/SceneObjectManager.h"
#include "Sim/Components/Graphics/ILocationComponent.h"


namespace GASS
{

	RotateTool::RotateTool(MouseToolController* controller): m_MouseIsDown(false),
		m_Controller(controller),
		m_Active(false)
	{
		EditorManager::GetPtr()->GetMessageManager()->RegisterForMessage(REG_TMESS(RotateTool::OnSceneObjectSelected,ObjectSelectedMessage,0));
	}

	RotateTool::~RotateTool()
	{

	}


	void RotateTool::MoveTo(const CursorInfo &info)
	{
		SceneObjectPtr selected(m_SelectedObject,boost::detail::sp_nothrow_tag());
		if(m_MouseIsDown && selected && CheckIfEditable(selected))
		{
			SceneObjectPtr gizmo(m_CurrentGizmo,boost::detail::sp_nothrow_tag());
			if(gizmo)
			{
				GizmoComponentPtr gc = gizmo->GetFirstComponent<GizmoComponent>();
				Float rotation_rad_step = (info.m_Delta.x + info.m_Delta.y)*0.1;
				Quaternion new_rot = gc->GetRotation(rotation_rad_step);
				int from_id = (int) this;
				GASS::MessagePtr rot_msg(new GASS::WorldRotationMessage(new_rot,from_id));
				//gizmo->PostMessage(rot_msg);
				selected->PostMessage(rot_msg);

				//SendMessageRec(selected,GASS::MessagePtr(new GASS::UpdateEulerAnglesMessage(from_id)));

				GASS::MessagePtr change_msg(new ScenarioChangedMessage(from_id));
				EditorManager::GetPtr()->GetMessageManager()->SendImmediate(change_msg);
			}
			else if(m_RotateY)
			{
				/*GASS::LocationComponentPtr comp = selected->GetFirstComponent<GASS::ILocationComponent>();
				Vec3 new_rot = comp->GetEulerRotation();

				float rot_angle = EditorManager::Get().GetMouseToolController()->SetSnapAngle(info.m_Delta.x*10);


				//rotate selected object
				if(selected->GetSceneObjectManager()->GetScenarioScene()->GetSceneUp() == Vec3(0,1,0))
				new_rot.x += rot_angle;
				else if(selected->GetSceneObjectManager()->GetScenarioScene()->GetSceneUp() == Vec3(0,0,1))
				new_rot.z += rot_angle;

				comp->SetEulerRotation(new_rot);
				int from_id = (int) this;
				GASS::MessagePtr change_msg(new ScenarioChangedMessage(from_id));
				EditorManager::GetPtr()->GetMessageManager()->SendImmediate(change_msg);*/
			}
		}
	}

	bool RotateTool::CheckIfEditable(SceneObjectPtr obj)
	{
		return (!m_Controller->IsObjectStatic(obj) && !m_Controller->IsObjectLocked(obj) && m_Controller->IsObjectVisible(obj));
	}

	void RotateTool::MouseDown(const CursorInfo &info)
	{
		m_MouseIsDown = true;
		m_MouseDownPos = info.m_ScreenPos;
		m_RotateY = false;
		SceneObjectPtr obj_under_cursor(info.m_ObjectUnderCursor,boost::detail::sp_nothrow_tag());
		if(obj_under_cursor && CheckIfEditable(obj_under_cursor))
		{
			GizmoComponentPtr gc = obj_under_cursor->GetFirstComponent<GizmoComponent>();
			if(gc)
			{
				m_CurrentGizmo = obj_under_cursor;
			}
			else if(obj_under_cursor == SceneObjectPtr(m_SelectedObject,boost::detail::sp_nothrow_tag()))
			{
				m_RotateY = true;
				int from_id = (int) this;
				MessagePtr col_msg(new GASS::CollisionSettingsMessage(false,from_id));
				SendMessageRec(obj_under_cursor,col_msg);
				SceneObjectPtr gizmo = GetMasterGizmo();
				if(gizmo)
					SendMessageRec(gizmo,col_msg);

			}
			//m_FirstMoveUpdate = true;
		}
	}


	void RotateTool::MouseUp(const CursorInfo &info)
	{
		m_MouseIsDown = false;
		bool slection_mode = false;

		if(info.m_ScreenPos == m_MouseDownPos)
		{
			slection_mode = true;
		}
		m_CurrentGizmo.reset();


		SceneObjectPtr selected(m_SelectedObject,boost::detail::sp_nothrow_tag());
		if(selected && CheckIfEditable(selected))
		{
			int from_id = (int) this;
			MessagePtr col_msg(new GASS::CollisionSettingsMessage(true,from_id));
			//selected->SendImmediate(col_msg);
			SendMessageRec(selected,col_msg);

			SceneObjectPtr gizmo = GetMasterGizmo();
			if(gizmo)
				SendMessageRec(gizmo,col_msg);
		}

		if(slection_mode) //selection mode
		{
			SceneObjectPtr obj_under_cursor (info.m_ObjectUnderCursor,boost::detail::sp_nothrow_tag());
			if(obj_under_cursor && CheckIfEditable(obj_under_cursor))
			{
				if(!m_Controller->IsObjectStatic(obj_under_cursor))
				{
					if(!m_Controller->IsObjectLocked(obj_under_cursor))
					{
						GizmoComponentPtr gc = obj_under_cursor->GetFirstComponent<GizmoComponent>();
						//Send selection message
						if(!gc) //don't select gizmo objects
						{
							int from_id = (int) this;
							MessagePtr selection_msg(new ObjectSelectedMessage(obj_under_cursor,from_id));
							EditorManager::GetPtr()->GetMessageManager()->PostMessage(selection_msg);
						}
					}
				}
			}
		}
	}



	void RotateTool::SendMessageRec(SceneObjectPtr obj,MessagePtr msg)
	{
		obj->PostMessage(msg);
		GASS::IComponentContainer::ComponentContainerIterator iter = obj->GetChildren();
		while(iter.hasMoreElements())
		{
			SceneObjectPtr child = boost::shared_static_cast<SceneObject>(iter.getNext());
			SendMessageRec(child,msg);
		}
	}

	void RotateTool::Stop()
	{
		SetGizmoVisiblity(false);
		m_Active = false;
	}

	SceneObjectPtr RotateTool::GetMasterGizmo()
	{
		SceneObjectPtr gizmo(m_MasterGizmoObject,boost::detail::sp_nothrow_tag());
		if(!gizmo &&  m_Controller->GetScene())
		{
			ScenarioScenePtr scene = m_Controller->GetScene();
			std::string gizmo_name = "GizmoRotateObject_YUp";
			if(abs(scene->GetSceneUp().z) > 0)
				gizmo_name = "GizmoRotateObject_ZUp";

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

	void RotateTool::Start() 
	{
		SetGizmoVisiblity(true);
		m_Active = true;
	}


	void RotateTool::OnSceneObjectSelected(ObjectSelectedMessagePtr message)
	{
		if(m_Active)
		{
			//hide gizmo
			if(message->GetSceneObject())
			{
				LocationComponentPtr lc = message->GetSceneObject()->GetFirstComponent<ILocationComponent>();
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


	void RotateTool::SetGizmoVisiblity(bool value)
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
}


