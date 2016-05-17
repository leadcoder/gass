#include "MoveTool.h"
#include "MouseToolController.h"
#include "../Components/GizmoComponent.h"
#include "Modules/Editor/EditorSystem.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/ComponentSystem/GASSComponent.h"
#include "Core/ComponentSystem/GASSComponentContainerTemplateManager.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/Interface/GASSILocationComponent.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSPhysicsSceneObjectMessages.h"


namespace GASS
{

	MoveTool::MoveTool(MouseToolController* controller): m_MouseIsDown(false),
		m_Controller(controller),
		m_MoveUpdateCount(0),
		m_UseGizmo(true),
		m_Active(false)
	{
		m_Controller->GetEditorSceneManager()->GetScene()->RegisterForMessage(REG_TMESS(MoveTool::OnSelectionChanged,EditorSelectionChangedEvent,0));
	}

	MoveTool::~MoveTool()
	{

	}

	void MoveTool::MouseMoved(const MouseData &/*data*/, const SceneCursorInfo &info)
	{
		int from_id = GASS_PTR_TO_INT(this);
		if(m_MouseIsDown &&  m_Selected.size() > 0)
		{
			SceneObjectPtr gizmo = m_CurrentGizmo.lock();
			if(gizmo)
			{
				GizmoComponentPtr gc = gizmo->GetFirstComponentByClass<GizmoComponent>();
				LocationComponentPtr gizmo_lc = gizmo->GetFirstComponentByClass<GASS::ILocationComponent>();
				Vec3 new_position = gc->GetPosition(info.m_Ray);
				if (m_MoveUpdateCount > 0)
				{
					Vec3 delta_move = new_position - m_PreviousPos;
					gizmo->PostRequest(WorldPositionRequestPtr(new WorldPositionRequest(gizmo_lc->GetWorldPosition() + delta_move, from_id)));
				}
				m_PreviousPos = new_position;
			}
			else if(m_GroundSnapMove)
			{
				
				//wait 3 frames to be sure that collision is disabled, we want to snap to object bellow and not just terrain height
				if(m_MoveUpdateCount > 3)
				{
						Vec3 delta_move = info.m_3DPos - m_PreviousPos;
						for (size_t i = 0; i < m_Selected.size(); i++)
						{
							SceneObjectPtr selected = m_Selected[i].lock();
							if (selected)
							{
								Vec3 current_pos = selected->GetFirstComponentByClass<ILocationComponent>()->GetWorldPosition();
								selected->PostRequest(WorldPositionRequestPtr(new WorldPositionRequest(current_pos + delta_move, from_id)));
							}
						}
				}
				m_PreviousPos = info.m_3DPos;
			}
			

			/*const double time = SimEngine::Get().GetRunTimeController()->GetTime();
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
				GASS::SceneMessagePtr attrib_change_msg(new ObjectAttributeChangedEvent(selected,attribs, from_id, 1.0/send_freq));
				m_Controller->GetEditorSceneManager()->GetScene()->PostMessage(attrib_change_msg);
			}*/
			m_MoveUpdateCount++;
		}
	}

	void MoveTool::MouseDown(const MouseData &data, const SceneCursorInfo &info)
	{
		m_MouseIsDown = true;
		m_MouseDownPos.Set(data.XAbsNorm,data.YAbsNorm);
		m_GroundSnapMove = false;

		SceneObjectPtr obj_under_cursor = info.m_ObjectUnderCursor.lock();
		
		if(obj_under_cursor && CheckIfEditable(obj_under_cursor))
		{
			GizmoComponentPtr gc = obj_under_cursor->GetFirstComponentByClass<GizmoComponent>();
			if(gc)
			{
				m_CurrentGizmo = obj_under_cursor;
				if(gc)
					gc->SetActive(true);

				//create copy if shift is pressed
				if(m_Controller->IsShiftDown())
				{
					//unselect all
					m_Controller->GetEditorSceneManager()->UnselectAllSceneObjects();
					for(size_t i = 0; i < m_Selected.size(); i++)
					{
						SceneObjectPtr selected = m_Selected[i].lock();
						if(selected && selected->GetParentSceneObject())
						{
							SceneObjectPtr new_obj = selected->CreateCopy();
							selected->GetParentSceneObject()->AddChildSceneObject(new_obj,true);
							m_Controller->GetEditorSceneManager()->SelectSceneObject(new_obj);
						}
					}
				}
			}
			else
			{
				bool selected_clicked = false;
				for (size_t i = 0; i < m_Selected.size(); i++)
				{
					SceneObjectPtr selected = m_Selected[i].lock();
					if (obj_under_cursor == selected)
						selected_clicked = true;
				}
				if (selected_clicked)
				{
					int from_id = GASS_PTR_TO_INT(this);
					m_GroundSnapMove = true;
					//unselect all
					if (m_Controller->IsShiftDown())
					{
						m_Controller->GetEditorSceneManager()->UnselectAllSceneObjects();
						for (size_t i = 0; i < m_Selected.size(); i++)
						{
							SceneObjectPtr selected = m_Selected[i].lock();
							if (selected && selected->GetParentSceneObject())
							{
								SceneObjectPtr new_obj = selected->CreateCopy();
								selected->GetParentSceneObject()->AddChildSceneObject(new_obj, true);
								m_Controller->GetEditorSceneManager()->SelectSceneObject(new_obj);
								SendMessageRec(new_obj, CollisionSettingsRequestPtr(new CollisionSettingsRequest(false, from_id)));
							}
						}
					}
					else
					{
						for (size_t i = 0; i < m_Selected.size(); i++)
						{
							SceneObjectPtr selected = m_Selected[i].lock();
							if (selected)
							{
								SendMessageRec(selected, CollisionSettingsRequestPtr(new CollisionSettingsRequest(false, from_id)));
							}
						}
					}
					SceneObjectPtr gizmo = GetOrCreateGizmo();
					if(gizmo)
						SendMessageRec(gizmo, CollisionSettingsRequestPtr(new CollisionSettingsRequest(false, from_id)));
				}
			}
		}
		m_MoveUpdateCount = 0;
	}

	void MoveTool::SendMessageRec(SceneObjectPtr obj, SceneObjectRequestMessagePtr msg)
	{
		obj->PostRequest(msg);
		GASS::ComponentContainer::ComponentContainerIterator iter = obj->GetChildren();
		while(iter.hasMoreElements())
		{
			SceneObjectPtr child = GASS_STATIC_PTR_CAST<SceneObject>(iter.getNext());
			SendMessageRec(child,msg);
		}
	}

	void MoveTool::MouseUp(const MouseData &data, const SceneCursorInfo &info)
	{
		m_MouseIsDown = false;
		bool selection_mode = false;

		if(fabs(data.XAbsNorm - m_MouseDownPos.x) + abs(data.YAbsNorm - m_MouseDownPos.y) < 0.05)
		{
			selection_mode = true;
		}

		SceneObjectPtr g_obj = m_CurrentGizmo.lock();
		if(g_obj)
		{
			GizmoComponentPtr gc = g_obj->GetFirstComponentByClass<GizmoComponent>();
			if(gc)
			{
				gc->SetActive(false);
			}
		}
	
		m_CurrentGizmo.reset();
		for(size_t i = 0; i < m_Selected.size(); i++)
		{
			SceneObjectPtr selected = m_Selected[i].lock();
			if(selected && CheckIfEditable(selected))
			{
				SendMessageRec(selected, CollisionSettingsRequestPtr(new CollisionSettingsRequest(true, GASS_PTR_TO_INT(this))));
			}
		}

		SceneObjectPtr gizmo = GetOrCreateGizmo();
		if(gizmo && m_Controller->GetEnableGizmo())
			SendMessageRec(gizmo, CollisionSettingsRequestPtr(new CollisionSettingsRequest(true, GASS_PTR_TO_INT(this))));

		if(selection_mode) //selection mode
		{
			SceneObjectPtr obj_under_cursor = info.m_ObjectUnderCursor.lock();
			if(obj_under_cursor)
			{
				if(CheckIfEditable(obj_under_cursor))
				{
					GizmoComponentPtr gc = obj_under_cursor->GetFirstComponentByClass<GizmoComponent>();
					//Send selection message
					if(!gc) //don't select gizmo objects
					{
						if (!m_Controller->IsCtrlDown())
							m_Controller->GetEditorSceneManager()->UnselectAllSceneObjects();
						m_Controller->GetEditorSceneManager()->SelectSceneObject(obj_under_cursor);
					}
				}
			}
		}
		
		int from_id = GASS_PTR_TO_INT(this);
		GASS::SystemMessagePtr change_msg(new SceneChangedEvent(from_id));
		SimEngine::Get().GetSimSystemManager()->SendImmediate(change_msg);
	}

	bool MoveTool::CheckIfEditable(SceneObjectPtr obj)
	{
		return (!m_Controller->GetEditorSceneManager()->IsObjectStatic(obj) && !m_Controller->GetEditorSceneManager()->IsObjectLocked(obj) && m_Controller->GetEditorSceneManager()->IsObjectVisible(obj));
	}

	void MoveTool::Stop()
	{
		SetGizmoVisiblity(false);
		m_Active = false;
	}

	SceneObjectPtr MoveTool::GetOrCreateGizmo()
	{
		SceneObjectPtr gizmo = m_MasterGizmoObject.lock();
		if(!gizmo &&  m_Controller->GetEditorSceneManager()->GetScene())
		{
			ScenePtr scene = m_Controller->GetEditorSceneManager()->GetScene();
			std::string gizmo_name = "GizmoMoveObject";
			GASS::SceneObjectPtr scene_object = m_Controller->GetEditorSceneManager()->GetScene()->LoadObjectFromTemplate(gizmo_name,m_Controller->GetEditorSceneManager()->GetScene()->GetRootSceneObject());
			m_MasterGizmoObject = scene_object;
			gizmo = scene_object;
			//Send selection message to inform gizmo about current object
			/*if(gizmo)
			{
				SceneObjectPtr current = m_SelectedObject.lock();
				if(current)
				{
					//gizmo->PostRequest();
					//m_Controller->GetEditorSceneManager()->SelectSceneObject(current);
				}
			}*/
		}
		return gizmo;
	}

	void MoveTool::Start()
	{
		if(m_Controller->GetEnableGizmo())
			SetGizmoVisiblity(true);
		else
			SetGizmoVisiblity(false);
		m_Active = true;
	}

	void MoveTool::SetGizmoVisiblity(bool value)
	{
		SceneObjectPtr gizmo = GetOrCreateGizmo();
		if(gizmo)
		{
			int from_id = GASS_PTR_TO_INT(this);
			CollisionSettingsRequestPtr col_msg(new CollisionSettingsRequest(value,from_id));
			SendMessageRec(gizmo,col_msg);
			LocationVisibilityRequestPtr vis_msg(new LocationVisibilityRequest(value,from_id));
			SendMessageRec(gizmo,vis_msg);
		}
	}

	void MoveTool::OnSelectionChanged(EditorSelectionChangedEventPtr message)
	{
		m_Selected.clear();
		for (size_t i = 0; i< message->m_Selection.size(); i++)
		{
			SceneObjectPtr obj = message->m_Selection[i].lock();
			if (obj && CheckIfEditable(obj) && obj->GetFirstComponentByClass<ILocationComponent>())
			{
				m_Selected.push_back(message->m_Selection[i]);
			}
		}

		if(m_Active)
		{
			//hide gizmo
			if(m_Selected.size() > 0)
			{
					if(m_Controller->GetEnableGizmo())
						SetGizmoVisiblity(true);
					else
						SetGizmoVisiblity(false);
			}
			else
			{
				SetGizmoVisiblity(false);
			}
		}
	}
}