#include "RotateTool.h"
#include "../Components/GizmoComponent.h"
#include "MouseToolController.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
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

			const double rotation_rad_step = data.XRel*0.2;

			SceneObjectPtr gizmo = m_CurrentGizmo.lock();
			if(gizmo)
			{
				GizmoComponentPtr gc = gizmo->GetFirstComponentByClass<GizmoComponent>();
				Quaternion new_rot = gc->GetRotation(rotation_rad_step);
				gizmo->GetFirstComponentByClass<ILocationComponent>()->SetWorldRotation(new_rot);
			}
			else
			{
				for (size_t i = 0; i < m_Selected.size(); i++)
				{
					SceneObjectPtr selected = m_Selected[i].lock();
					if (selected)
					{
						LocationComponentPtr location = selected->GetFirstComponentByClass<ILocationComponent>();
						Quaternion new_rot = location->GetWorldRotation() * Quaternion::CreateFromEulerYXZ(GASS::Vec3(0, rotation_rad_step, 0));
						location->SetWorldRotation(new_rot);
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

		if (!selection_mode)
		{
			int from_id = GASS_PTR_TO_INT(this);
			GASS::SystemMessagePtr change_msg(new SceneChangedEvent(from_id));
			SimEngine::Get().GetSimSystemManager()->SendImmediate(change_msg);
		}
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


	SceneObjectPtr _CreateRotateAxisGizmo(const std::string &name, const EulerRotation &rotation, const ColorRGBA &color)
	{
		SceneObjectPtr axis_gizmo = SceneObjectPtr(new SceneObject());

		axis_gizmo->SetName(name);
		axis_gizmo->SetSerialize(false);
		ComponentPtr location_comp = ComponentFactory::Get().Create("LocationComponent");
		location_comp->SetPropertyValue("Rotation", rotation);
		axis_gizmo->AddComponent(location_comp);

		ComponentPtr editor_comp = ComponentFactory::Get().Create("EditorComponent");
		editor_comp->SetPropertyValue("AllowDragAndDrop", false);
		editor_comp->SetPropertyValue("AllowRemove", false);
		editor_comp->SetPropertyValue("ShowInTree", false);
		editor_comp->SetPropertyValue("ShowBBWhenSelected", false);
		editor_comp->SetPropertyValue("ChangeMaterialWhenSelected", false);
		axis_gizmo->AddComponent(editor_comp);

		ComponentPtr gizmo_comp = ComponentFactory::Get().Create("GizmoComponent");
		gizmo_comp->SetPropertyValue("Type", GizmoTypeBinder(GT_AXIS));
		gizmo_comp->SetPropertyValue("Color", color);
		gizmo_comp->SetPropertyValue("Size", 2.0f);
		axis_gizmo->AddComponent(gizmo_comp);

		ComponentPtr mesh_comp = ComponentFactory::Get().Create("ManualMeshComponent");
		mesh_comp->SetPropertyValue("GeometryFlags", GeometryFlagsBinder(GEOMETRY_FLAG_GIZMO));
		axis_gizmo->AddComponent(mesh_comp);
		return axis_gizmo;
	}

	SceneObjectPtr _CreateRotateGizmo()
	{
		SceneObjectPtr gizmo = SceneObjectPtr(new SceneObject());
		gizmo->SetName("GizmoRotateGizmo");
		gizmo->SetID("ROTATE_GIZMO");
		gizmo->SetSerialize(false);

		ComponentPtr editor_comp = ComponentFactory::Get().Create("EditorComponent");
		editor_comp->SetPropertyValue("AllowDragAndDrop", false);
		editor_comp->SetPropertyValue("AllowRemove", false);
		editor_comp->SetPropertyValue("ShowInTree", false);
		editor_comp->SetPropertyValue("ShowBBWhenSelected", false);
		editor_comp->SetPropertyValue("ChangeMaterialWhenSelected", false);
		gizmo->AddComponent(editor_comp);

		//Create Axis
		gizmo->AddChildSceneObject(_CreateRotateAxisGizmo("GizmoObjectXAxis", EulerRotation(0, 0, 0), ColorRGBA(0, 1, 0, 1)), false);
		gizmo->AddChildSceneObject(_CreateRotateAxisGizmo("GizmoObjectYAxis", EulerRotation(0, 0, 90), ColorRGBA(1, 0, 0, 1)), false);
		gizmo->AddChildSceneObject(_CreateRotateAxisGizmo("GizmoObjectZAxis", EulerRotation(-90, 0, 0), ColorRGBA(0, 0, 1, 1)), false);

		return gizmo;
	}


	SceneObjectPtr RotateTool::GetMasterGizmo()
	{
		SceneObjectPtr gizmo = m_MasterGizmoObject.lock();
		if(!gizmo &&  m_Controller->GetEditorSceneManager()->GetScene())
		{
			gizmo = _CreateRotateGizmo();
			
			//Add gizmo to scene
			m_Controller->GetEditorSceneManager()->GetScene()->GetRootSceneObject()->AddChildSceneObject(gizmo, true);

			m_MasterGizmoObject = gizmo;
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


