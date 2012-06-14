#include "RotateTool.h"
#include "../Components/GizmoComponent.h"

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
#include "Sim/GASSSimEngine.h"



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
				GizmoComponentPtr gc = gizmo->GetFirstComponentByClass<GizmoComponent>();
				Float rotation_rad_step = (info.m_Delta.x + info.m_Delta.y);
				Quaternion new_rot = gc->GetRotation(rotation_rad_step);
				int from_id = (int) this;
				GASS::MessagePtr rot_msg(new GASS::WorldRotationMessage(new_rot,from_id));
				//gizmo->PostMessage(rot_msg);
				selected->PostMessage(rot_msg);

				//SendMessageRec(selected,GASS::MessagePtr(new GASS::UpdateEulerAnglesMessage(from_id)));

				const double time = SimEngine::Get().GetTime();
				static double last_time = 0;
				const double send_freq = 20; 
				if(time - last_time > 1.0/send_freq)
				{
					last_time = time;
					std::vector<std::string> attribs;
					attribs.push_back("Rotation");
					attribs.push_back("Quaternion");
					GASS::MessagePtr attrib_change_msg(new ObjectAttributeChangedMessage(selected,attribs, from_id, 1.0/send_freq));
					EditorManager::GetPtr()->GetMessageManager()->SendImmediate(attrib_change_msg);
				}

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
			GizmoComponentPtr gc = obj_under_cursor->GetFirstComponentByClass<GizmoComponent>();
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
						GizmoComponentPtr gc = obj_under_cursor->GetFirstComponentByClass<GizmoComponent>();
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

		int from_id = (int) this;
		GASS::MessagePtr change_msg(new SceneChangedMessage(from_id));
		EditorManager::GetPtr()->GetMessageManager()->SendImmediate(change_msg);
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
			ScenePtr scene = m_Controller->GetScene();
			std::string gizmo_name = "GizmoRotateObject_YUp";
			
			GASS::SceneObjectPtr scene_object = m_Controller->GetScene()->LoadObjectFromTemplate(gizmo_name,m_Controller->GetScene()->GetRootSceneObject());
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


