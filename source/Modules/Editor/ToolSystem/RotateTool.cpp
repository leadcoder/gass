#include "RotateTool.h"
#include "../Components/GizmoComponent.h"
#include "MouseToolController.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/Interface/GASSILocationComponent.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSPhysicsSceneObjectMessages.h"
#include "Sim/GASSSimEngine.h"



namespace GASS
{
	RotateTool::RotateTool(MouseToolController* controller): m_MouseIsDown(false),
		m_Controller(controller),
		m_Active(false)
	{
		controller->GetEditorSceneManager()->GetScene()->RegisterForMessage(REG_TMESS(RotateTool::OnSelectionChanged, EditorSelectionChangedEvent,0));
	}

	RotateTool::~RotateTool()
	{

	}

	void RotateTool::MouseDown(const MouseData &data, const SceneCursorInfo &info)
	{
		m_MouseIsDown = true;
		m_MouseDownPos.Set(data.XAbsNorm,data.YAbsNorm);
		m_RotateY = false;
		SceneObjectPtr obj_under_cursor = info.m_ObjectUnderCursor.lock();
		if(obj_under_cursor && CheckIfEditable(obj_under_cursor))
		{
			GizmoComponentPtr gc = obj_under_cursor->GetFirstComponentByClass<GizmoComponent>();
			if(gc)
			{
				m_CurrentGizmo = obj_under_cursor;

				/*SceneObjectPtr selected = m_SelectedObject.lock();
				if(m_Controller->IsShiftDown() && selected && selected->GetParentSceneObject())
				{
					SceneObjectPtr new_obj = selected->CreateCopy();
					selected->GetParentSceneObject()->AddChildSceneObject(new_obj,true);
					m_Controller->GetEditorSceneManager()->SelectSceneObject(new_obj);
				}*/

			}
			
		/*	else if(obj_under_cursor == m_SelectedObject.lock())
			{
				m_RotateY = true;
				int from_id = GASS_PTR_TO_INT(this);

				SceneObjectPtr selected = m_SelectedObject.lock();
				if(m_Controller->IsShiftDown() && selected && selected->GetParentSceneObject())
				{
					SceneObjectPtr new_obj = selected->CreateCopy();
					selected->GetParentSceneObject()->AddChildSceneObject(new_obj,true);
					m_Controller->GetEditorSceneManager()->SelectSceneObject(new_obj);
					selected = new_obj;
				}

				CollisionSettingsRequestPtr col_msg(new CollisionSettingsRequest(false,from_id));
				SendMessageRec(selected,col_msg);
				SceneObjectPtr gizmo = GetMasterGizmo();
				if(gizmo)
					SendMessageRec(gizmo,col_msg);
			}*/
		}
	}

	void RotateTool::MouseMoved(const MouseData &data, const SceneCursorInfo &/*info*/)
	{
		if(m_MouseIsDown && m_Selected.size() > 0)
		{
			int from_id = GASS_PTR_TO_INT(this);
			SceneObjectPtr gizmo = m_CurrentGizmo.lock();
			if(gizmo)
			{
				GizmoComponentPtr gc = gizmo->GetFirstComponentByClass<GizmoComponent>();
				Float rotation_rad_step = data.XRel*0.2;
				rotation_rad_step = rotation_rad_step;
				Quaternion new_rot = gc->GetRotation(rotation_rad_step);
				
				gizmo->PostRequest(WorldRotationRequestPtr(new WorldRotationRequest(new_rot, from_id)));

				//SendMessageRec(selected,GASS::MessagePtr(new GASS::UpdateEulerAnglesRequest(from_id)));

				/*const double time = SimEngine::Get().GetRunTimeController()->GetTime();
				static double last_time = 0;
				const double send_freq = 20;
				if(time - last_time > 1.0/send_freq)
				{
					last_time = time;
					std::vector<std::string> attribs;
					attribs.push_back("Rotation");
					attribs.push_back("Quaternion");
					GASS::SceneMessagePtr attrib_change_msg(new ObjectAttributeChangedEvent(selected,attribs, from_id, 1.0/send_freq));
					m_Controller->GetEditorSceneManager()->GetScene()->SendImmediate(attrib_change_msg);
				}*/
			}
			else
			{
				Float rotation_rad_step = data.XRel*0.2;
				rotation_rad_step = rotation_rad_step;
				for (size_t i = 0; i < m_Selected.size(); i++)
				{
					SceneObjectPtr selected = m_Selected[i].lock();
					if (selected)
					{
						Quaternion rot = selected->GetFirstComponentByClass<ILocationComponent>()->GetRotation() * Quaternion::CreateFromEulerYXZ(GASS::Vec3(0, rotation_rad_step, 0));
						selected->SendImmediateRequest(WorldRotationRequestPtr(new WorldRotationRequest(rot, from_id)));
					}
				}
			}
		}
	}

	void RotateTool::MouseUp(const MouseData &data, const SceneCursorInfo &info)
	{
		m_MouseIsDown = false;
		bool selection_mode = false;
		if(fabs(data.XAbsNorm - m_MouseDownPos.x) + fabs(data.YAbsNorm - m_MouseDownPos.y) < 0.05)
		{
			selection_mode = true;
		}
		m_CurrentGizmo.reset();

		/*SceneObjectPtr selected = m_SelectedObject.lock();
		if(selected && CheckIfEditable(selected))
		{
			int from_id = GASS_PTR_TO_INT(this);
			CollisionSettingsRequestPtr col_msg(new CollisionSettingsRequest(true,from_id));
			//selected->SendImmediate(col_msg);
			SendMessageRec(selected,col_msg);

			SceneObjectPtr gizmo = GetMasterGizmo();
			if(gizmo)
				SendMessageRec(gizmo,col_msg);
		}*/

		if (selection_mode) //selection mode
		{
			SceneObjectPtr obj_under_cursor = info.m_ObjectUnderCursor.lock();
			if (obj_under_cursor)
			{
				if (CheckIfEditable(obj_under_cursor))
				{
					GizmoComponentPtr gc = obj_under_cursor->GetFirstComponentByClass<GizmoComponent>();
					//Send selection message
					if (!gc) //don't select gizmo objects
					{
						m_Controller->SelectHelper(obj_under_cursor);
					}
				}
			}
		}

		int from_id = GASS_PTR_TO_INT(this);
		GASS::SystemMessagePtr change_msg(new SceneChangedEvent(from_id));
		SimEngine::Get().GetSimSystemManager()->SendImmediate(change_msg);
	}

	void RotateTool::SendMessageRec(SceneObjectPtr obj,SceneObjectRequestMessagePtr msg)
	{
		obj->PostRequest(msg);
		GASS::ComponentContainer::ComponentContainerIterator iter = obj->GetChildren();
		while(iter.hasMoreElements())
		{
			SceneObjectPtr child = GASS_STATIC_PTR_CAST<SceneObject>(iter.getNext());
			SendMessageRec(child,msg);
		}
	}

	bool RotateTool::CheckIfEditable(SceneObjectPtr obj) const
	{
		return (!m_Controller->GetEditorSceneManager()->IsObjectStatic(obj) && !m_Controller->GetEditorSceneManager()->IsObjectLocked(obj) && m_Controller->GetEditorSceneManager()->IsObjectVisible(obj));
	}

	void RotateTool::Stop()
	{
		SetGizmoVisiblity(false);
		m_Active = false;
	}

	SceneObjectPtr RotateTool::GetMasterGizmo()
	{
		SceneObjectPtr gizmo = m_MasterGizmoObject.lock();
		if(!gizmo &&  m_Controller->GetEditorSceneManager()->GetScene())
		{
			ScenePtr scene = m_Controller->GetEditorSceneManager()->GetScene();
			std::string gizmo_name = "GizmoRotateObject";

			GASS::SceneObjectPtr scene_object = m_Controller->GetEditorSceneManager()->GetScene()->LoadObjectFromTemplate(gizmo_name,m_Controller->GetEditorSceneManager()->GetScene()->GetRootSceneObject());
			m_MasterGizmoObject = scene_object;
			gizmo = scene_object;
			//Send selection message to inform gizmo about current object
			/*if(gizmo)
			{
				SceneObjectPtr current = m_SelectedObject.lock();
				if(current)
				{
					m_Controller->GetEditorSceneManager()->SelectSceneObject(current);
				}
			}*/
		}
		return gizmo;
	}

	void RotateTool::Start()
	{
		if (m_Controller->GetEnableGizmo())
			SetGizmoVisiblity(true);
		else
			SetGizmoVisiblity(false);
		m_Active = true;
	}

	void RotateTool::OnSelectionChanged(EditorSelectionChangedEventPtr message)
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

		if (m_Active)
		{
			//hide gizmo
			if (m_Selected.size() > 0)
			{
				if (m_Controller->GetEnableGizmo())
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

	void RotateTool::SetGizmoVisiblity(bool value)
	{
		SceneObjectPtr gizmo = GetMasterGizmo();
		if(gizmo)
		{
			int from_id = GASS_PTR_TO_INT(this);
			CollisionSettingsRequestPtr col_msg(new CollisionSettingsRequest(value,from_id));
			SendMessageRec(gizmo,col_msg);
			LocationVisibilityRequestPtr vis_msg(new LocationVisibilityRequest(value,from_id));
			SendMessageRec(gizmo,vis_msg);
		}
	}
}


