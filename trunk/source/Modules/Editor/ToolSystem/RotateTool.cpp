#include "RotateTool.h"
#include "../Components/GizmoComponent.h"

#include "MouseToolController.h"
#include "Modules/Editor/EditorSystem.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/ComponentSystem/GASSComponent.h"
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
		//SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(RotateTool::OnSceneObjectSelected,ObjectSelectionChangedEvent,0));
		controller->GetEditorSceneManager()->GetScene()->RegisterForMessage(REG_TMESS(RotateTool::OnSceneObjectSelected,ObjectSelectionChangedEvent,0));
	}

	RotateTool::~RotateTool()
	{

	}


	void RotateTool::MouseMoved(const MouseData &data, const SceneCursorInfo &info)
	{
		SceneObjectPtr selected(m_SelectedObject,NO_THROW);
		if(m_MouseIsDown && selected && CheckIfEditable(selected))
		{
			SceneObjectPtr gizmo(m_CurrentGizmo,NO_THROW);
			if(gizmo)
			{
				GizmoComponentPtr gc = gizmo->GetFirstComponentByClass<GizmoComponent>();
				Float rotation_rad_step = data.XRel*0.2;
				rotation_rad_step = rotation_rad_step;
				Quaternion new_rot = gc->GetRotation(rotation_rad_step);
				int from_id = (int) this;
				
				selected->PostRequest(WorldRotationRequestPtr(new WorldRotationRequest(new_rot,from_id)));

				//SendMessageRec(selected,GASS::MessagePtr(new GASS::UpdateEulerAnglesRequest(from_id)));

				const double time = SimEngine::Get().GetTime();
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
				}

			}
		}
	}

	bool RotateTool::CheckIfEditable(SceneObjectPtr obj)
	{
		return (!m_Controller->GetEditorSceneManager()->IsObjectStatic(obj) && !m_Controller->GetEditorSceneManager()->IsObjectLocked(obj) && m_Controller->GetEditorSceneManager()->IsObjectVisible(obj));
	}

	void RotateTool::MouseDown(const MouseData &data, const SceneCursorInfo &info)
	{
		m_MouseIsDown = true;
		m_MouseDownPos.Set(data.XAbsNorm,data.YAbsNorm);
		m_RotateY = false;
		SceneObjectPtr obj_under_cursor(info.m_ObjectUnderCursor,NO_THROW);
		if(obj_under_cursor && CheckIfEditable(obj_under_cursor))
		{
			GizmoComponentPtr gc = obj_under_cursor->GetFirstComponentByClass<GizmoComponent>();
			if(gc)
			{
				m_CurrentGizmo = obj_under_cursor;

				SceneObjectPtr selected = SceneObjectPtr(m_SelectedObject,NO_THROW);
				if(m_Controller->IsShiftDown() && selected && selected->GetParentSceneObject())
				{
					SceneObjectPtr new_obj = selected->CreateCopy();
					selected->GetParentSceneObject()->AddChildSceneObject(new_obj,true);
					m_Controller->GetEditorSceneManager()->SelectSceneObject(new_obj);
				}

			}
			else if(obj_under_cursor == SceneObjectPtr(m_SelectedObject,NO_THROW))
			{
				m_RotateY = true;
				int from_id = (int) this;

				SceneObjectPtr selected = SceneObjectPtr(m_SelectedObject,NO_THROW);
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
			}
		}
	}


	void RotateTool::MouseUp(const MouseData &data, const SceneCursorInfo &info)
	{
		m_MouseIsDown = false;
		bool slection_mode = false;

		if(Vec2(data.XAbsNorm,data.YAbsNorm) == m_MouseDownPos)
		{
			slection_mode = true;
		}
		m_CurrentGizmo.reset();


		SceneObjectPtr selected(m_SelectedObject,NO_THROW);
		if(selected && CheckIfEditable(selected))
		{
			int from_id = (int) this;
			CollisionSettingsRequestPtr col_msg(new CollisionSettingsRequest(true,from_id));
			//selected->SendImmediate(col_msg);
			SendMessageRec(selected,col_msg);

			SceneObjectPtr gizmo = GetMasterGizmo();
			if(gizmo)
				SendMessageRec(gizmo,col_msg);
		}

		if(slection_mode) //selection mode
		{
			SceneObjectPtr obj_under_cursor (info.m_ObjectUnderCursor,NO_THROW);
			if(obj_under_cursor && CheckIfEditable(obj_under_cursor))
			{
				if(!m_Controller->GetEditorSceneManager()->IsObjectStatic(obj_under_cursor))
				{
					if(!m_Controller->GetEditorSceneManager()->IsObjectLocked(obj_under_cursor))
					{
						GizmoComponentPtr gc = obj_under_cursor->GetFirstComponentByClass<GizmoComponent>();
						//Send selection message
						if(!gc) //don't select gizmo objects
						{
							m_Controller->GetEditorSceneManager()->SelectSceneObject(obj_under_cursor);
						}
					}
				}
			}
		}

		int from_id = (int) this;
		GASS::SystemMessagePtr change_msg(new SceneChangedEvent(from_id));
		SimEngine::Get().GetSimSystemManager()->SendImmediate(change_msg);
	}



	void RotateTool::SendMessageRec(SceneObjectPtr obj,SceneObjectRequestMessagePtr msg)
	{
		obj->PostRequest(msg);
		GASS::ComponentContainer::ComponentContainerIterator iter = obj->GetChildren();
		while(iter.hasMoreElements())
		{
			SceneObjectPtr child = STATIC_PTR_CAST<SceneObject>(iter.getNext());
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
		SceneObjectPtr gizmo(m_MasterGizmoObject,NO_THROW);
		if(!gizmo &&  m_Controller->GetEditorSceneManager()->GetScene())
		{
			ScenePtr scene = m_Controller->GetEditorSceneManager()->GetScene();
			std::string gizmo_name = "GizmoRotateObject";
			
			GASS::SceneObjectPtr scene_object = m_Controller->GetEditorSceneManager()->GetScene()->LoadObjectFromTemplate(gizmo_name,m_Controller->GetEditorSceneManager()->GetScene()->GetRootSceneObject());
			m_MasterGizmoObject = scene_object;
			gizmo = scene_object;
			//Send selection message to inform gizmo about current object

			if(gizmo)
			{
				SceneObjectPtr current (m_SelectedObject,NO_THROW);
				if(current)
				{
					m_Controller->GetEditorSceneManager()->SelectSceneObject(current);
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


	void RotateTool::OnSceneObjectSelected(ObjectSelectionChangedEventPtr message)
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
			CollisionSettingsRequestPtr col_msg(new CollisionSettingsRequest(value,from_id));
			SendMessageRec(gizmo,col_msg);
			VisibilityRequestPtr vis_msg(new VisibilityRequest(value,from_id));
			SendMessageRec(gizmo,vis_msg);
		}
	}
}


