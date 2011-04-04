#include "MoveTool.h"
#include "MouseToolController.h"
#include "../Components/GizmoComponent.h"
#include "../EditorManager.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/IMessage.h"
#include "Core/ComponentSystem/IComponent.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Components/Graphics/ILocationComponent.h"
#include "Core/ComponentSystem/BaseComponentContainerTemplateManager.h"
#include "Sim/Scenario/Scene/SceneObjectManager.h"

namespace GASS
{

	MoveTool::MoveTool(MouseToolController* controller): m_MouseIsDown(false),
		m_Controller(controller),
		m_MoveUpdateCount(0),
		m_UseGizmo(true),
		m_Active(false)
	{
		EditorManager::GetPtr()->GetMessageManager()->RegisterForMessage(REG_TMESS(MoveTool::OnSceneObjectSelected,ObjectSelectedMessage,0));
	}

	MoveTool::~MoveTool()
	{

	}

	void MoveTool::MoveTo(const CursorInfo &info)
	{
		SceneObjectPtr selected(m_SelectedObject,boost::detail::sp_nothrow_tag());
		if(m_MouseIsDown && selected && CheckIfEditable(selected))
		{
			SceneObjectPtr gizmo(m_CurrentGizmo,boost::detail::sp_nothrow_tag());
			if(gizmo)
			{
				GizmoComponentPtr gc = gizmo->GetFirstComponent<GizmoComponent>();
				Vec3 new_position = gc->GetPosition(info.m_RayStart,info.m_RayDir);

				if(m_MoveUpdateCount == 0)
				{
					//calc offset
					LocationComponentPtr comp = gizmo->GetFirstComponent<GASS::ILocationComponent>();
					m_Offset = comp->GetWorldPosition();
					m_Offset = new_position - m_Offset;
				}
				new_position = new_position - m_Offset;
				int from_id = (int) this;

				GASS::MessagePtr pos_msg(new GASS::WorldPositionMessage(new_position,from_id));
				gizmo->PostMessage(pos_msg);

				//SceneObjectPtr master_gizmo = GetMasterGizmo();
				//if(master_gizmo)
				//	SendMessageRec(master_gizmo,pos_msg);

				GASS::MessagePtr change_msg(new ScenarioChangedMessage(from_id));
				EditorManager::GetPtr()->GetMessageManager()->SendImmediate(change_msg);
			}
			else if(m_GroundSnapMove)
			{
				//wait 3 frames to be sure that mesh is hidden
				//m_MoveUpdateCount 

				if(m_MoveUpdateCount > 3)
				{

					if(m_MoveUpdateCount == 4)
					{
						//calc offset
						LocationComponentPtr comp = selected->GetFirstComponent<GASS::ILocationComponent>();
						m_Offset = comp->GetWorldPosition();
						m_Offset = info.m_3DPos - m_Offset;
						//m_Offset.Set(0,0,0);
					}
					else
					{
						//move seleced object
						Vec3 new_position = info.m_3DPos - m_Offset;
						int from_id = (int) this;

						new_position.x = EditorManager::Get().GetMouseToolController()->SnapPosition(new_position.x);
						new_position.y = EditorManager::Get().GetMouseToolController()->SnapPosition(new_position.y);
						new_position.z = EditorManager::Get().GetMouseToolController()->SnapPosition(new_position.z);


						GASS::MessagePtr pos_msg(new GASS::WorldPositionMessage(new_position,from_id));
						selected->PostMessage(pos_msg);


						GASS::MessagePtr change_msg(new ScenarioChangedMessage(from_id));
						EditorManager::GetPtr()->GetMessageManager()->SendImmediate(change_msg);
					}
				}

			}
			m_MoveUpdateCount++;
		}
	}


	void MoveTool::MouseDown(const CursorInfo &info)
	{
		m_MouseIsDown = true;
		m_MouseDownPos = info.m_ScreenPos;
		m_GroundSnapMove = false;
		SceneObjectPtr obj_under_cursor(info.m_ObjectUnderCursor,boost::detail::sp_nothrow_tag());

		if(obj_under_cursor  && CheckIfEditable(obj_under_cursor ))
		{
			GizmoComponentPtr gc = obj_under_cursor->GetFirstComponent<GizmoComponent>();
			if(gc)
			{
				m_CurrentGizmo = obj_under_cursor;
				if(gc)
					gc->SetActive(true);
			}
			else if(obj_under_cursor == SceneObjectPtr(m_SelectedObject,boost::detail::sp_nothrow_tag()))
			{
				m_GroundSnapMove = true;
				int from_id = (int) this;
				MessagePtr col_msg(new GASS::CollisionSettingsMessage(false,from_id));
				SendMessageRec(obj_under_cursor,col_msg);
				SceneObjectPtr gizmo = GetMasterGizmo();
				if(gizmo)
					SendMessageRec(gizmo,col_msg);

			}
			m_MoveUpdateCount = 0;
		}
	}

	void MoveTool::SendMessageRec(SceneObjectPtr obj,MessagePtr msg)
	{
		obj->PostMessage(msg);
		GASS::IComponentContainer::ComponentContainerIterator iter = obj->GetChildren();
		while(iter.hasMoreElements())
		{
			SceneObjectPtr child = boost::shared_static_cast<SceneObject>(iter.getNext());
			SendMessageRec(child,msg);
		}
	}

	void MoveTool::MouseUp(const CursorInfo &info)
	{
		m_MouseIsDown = false;
		bool slection_mode = false;

		if(info.m_ScreenPos == m_MouseDownPos)
		{
			slection_mode = true;
		}

		SceneObjectPtr g_obj(m_CurrentGizmo,boost::detail::sp_nothrow_tag());
		if(g_obj)
		{
			GizmoComponentPtr gc = g_obj->GetFirstComponent<GizmoComponent>();
			if(gc)
			{
				gc->SetActive(false);
			}
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
			if(obj_under_cursor)
			{
				if(CheckIfEditable(obj_under_cursor))
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


	bool MoveTool::CheckIfEditable(SceneObjectPtr obj)
	{
		return (!m_Controller->IsObjectStatic(obj) && !m_Controller->IsObjectLocked(obj) && m_Controller->IsObjectVisible(obj));
	}

	void MoveTool::Stop()
	{
		SetGizmoVisiblity(false);
		m_Active = false;
	}

	SceneObjectPtr MoveTool::GetMasterGizmo()
	{
		SceneObjectPtr gizmo(m_MasterGizmoObject,boost::detail::sp_nothrow_tag());
		if(!gizmo &&  m_Controller->GetScene())
		{
			ScenarioScenePtr scene = m_Controller->GetScene();
			std::string gizmo_name = "GizmoMoveObject_YUp";
			if(abs(scene->GetSceneUp().z) > 0)
				gizmo_name = "GizmoMoveObject_ZUp";

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

	void MoveTool::Start() 
	{
		SetGizmoVisiblity(true);
		m_Active = true;
	}

	void MoveTool::SetGizmoVisiblity(bool value)
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

	void MoveTool::OnSceneObjectSelected(ObjectSelectedMessagePtr message)
	{

		//Move gizmo to new object


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

}

