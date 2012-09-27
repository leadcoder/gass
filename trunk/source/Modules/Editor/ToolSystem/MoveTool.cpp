#include "MoveTool.h"
#include "MouseToolController.h"
#include "../Components/GizmoComponent.h"
#include "../EditorManager.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/ComponentSystem/GASSIComponent.h"
#include "Core/ComponentSystem/GASSBaseComponentContainerTemplateManager.h"
#include "Sim/Scene/GASSScene.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/Scene/GASSSceneObject.h"
#include "Sim/Components/Graphics/GASSILocationComponent.h"
#include "Sim/Scene/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Scene/GASSPhysicsSceneObjectMessages.h"


namespace GASS
{

	MoveTool::MoveTool(MouseToolController* controller): m_MouseIsDown(false),
		m_Controller(controller),
		m_MoveUpdateCount(0),
		m_UseGizmo(true),
		m_Active(false),
		m_SnapToMouse(false)
	{
		EditorManager::GetPtr()->GetMessageManager()->RegisterForMessage(REG_TMESS(MoveTool::OnSceneObjectSelected,ObjectSelectedMessage,0));
	}

	MoveTool::~MoveTool()
	{

	}

	void MoveTool::MoveTo(const CursorInfo &info)
	{
		int from_id = (int) this;

		SceneObjectPtr selected(m_SelectedObject,boost::detail::sp_nothrow_tag());
		if(m_MouseIsDown && selected && CheckIfEditable(selected))
		{
			SceneObjectPtr gizmo(m_CurrentGizmo,boost::detail::sp_nothrow_tag());
			if(gizmo)
			{
				GizmoComponentPtr gc = gizmo->GetFirstComponentByClass<GizmoComponent>();
				Vec3 new_position = gc->GetPosition(info.m_RayStart,info.m_RayDir);

				
				
				if(m_MoveUpdateCount == 0)
				{
					//calc offset
					LocationComponentPtr comp = gizmo->GetFirstComponentByClass<GASS::ILocationComponent>();
					if(comp)
					{
						m_Offset = comp->GetWorldPosition();
						if(gc->GetSpaceMode() == "World")
						{
							m_Offset.x = m_Controller->SnapPosition(m_Offset.x);
							m_Offset.y = m_Controller->SnapPosition(m_Offset.y);
							m_Offset.z = m_Controller->SnapPosition(m_Offset.z);
						}

						m_Offset = new_position - m_Offset;
					}
				}
				new_position = new_position - m_Offset;
				
				GASS::MessagePtr pos_msg(new GASS::WorldPositionMessage(new_position,from_id));
				gizmo->PostMessage(pos_msg);
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
						LocationComponentPtr comp = selected->GetFirstComponentByClass<GASS::ILocationComponent>();
						if(comp)
						{
							m_Offset = comp->GetWorldPosition();
							m_Offset = info.m_3DPos - m_Offset;

							SceneObjectPtr gizmo(m_CurrentGizmo,boost::detail::sp_nothrow_tag());
							if(gizmo)
							{
								GizmoComponentPtr gc = gizmo->GetFirstComponentByClass<GizmoComponent>();
								if(gc->GetSpaceMode() == "World")
								{
									m_Offset.x = m_Controller->SnapPosition(m_Offset.x);
									m_Offset.y = m_Controller->SnapPosition(m_Offset.y);
									m_Offset.z = m_Controller->SnapPosition(m_Offset.z);
								}
							}
						}
						//m_Offset.Set(0,0,0);
					}
					else
					{
						//move seleced object

						if(m_SnapToMouse)
							m_Offset.Set(0,0,0);

						Vec3 new_position = info.m_3DPos - m_Offset;
						

						new_position.x = EditorManager::Get().GetMouseToolController()->SnapPosition(new_position.x);
						new_position.y = EditorManager::Get().GetMouseToolController()->SnapPosition(new_position.y);
						new_position.z = EditorManager::Get().GetMouseToolController()->SnapPosition(new_position.z);


						GASS::MessagePtr pos_msg(new GASS::WorldPositionMessage(new_position,from_id));
						selected->PostMessage(pos_msg);

					}
				}

			}
			const double time = SimEngine::Get().GetTime();
			static double last_time = 0;
			const double send_freq = 20; 
			if(time - last_time > 1.0/send_freq)
			{
				last_time = time;
				std::vector<std::string> attribs;
				attribs.push_back("Position");
				attribs.push_back("Latitude");
				attribs.push_back("Longitude");
				attribs.push_back("Projected");
				GASS::MessagePtr attrib_change_msg(new ObjectAttributeChangedMessage(selected,attribs, from_id, 1.0/send_freq));
				EditorManager::GetPtr()->GetMessageManager()->PostMessage(attrib_change_msg);
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
			GizmoComponentPtr gc = obj_under_cursor->GetFirstComponentByClass<GizmoComponent>();
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
			GizmoComponentPtr gc = g_obj->GetFirstComponentByClass<GizmoComponent>();
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
			if(gizmo && m_Controller->GetEnableGizmo())
				SendMessageRec(gizmo,col_msg);
		}

		if(slection_mode) //selection mode
		{
			SceneObjectPtr obj_under_cursor (info.m_ObjectUnderCursor,boost::detail::sp_nothrow_tag());
			if(obj_under_cursor)
			{
				if(CheckIfEditable(obj_under_cursor))
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

		int from_id = (int) this;
		GASS::MessagePtr change_msg(new SceneChangedMessage(from_id));
		EditorManager::GetPtr()->GetMessageManager()->SendImmediate(change_msg);
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
			ScenePtr scene = m_Controller->GetScene();

			std::string gizmo_name = "GizmoMoveObject_YUp";
		
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

	void MoveTool::Start() 
	{
		SetGizmoVisiblity(m_Controller->GetEnableGizmo());
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
				LocationComponentPtr lc = message->GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
				if(lc) //only support gizmo for objects with location component
				{
					SetGizmoVisiblity(m_Controller->GetEnableGizmo());
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

