#include "MoveTool.h"

#include <memory>
#include "MouseToolController.h"
#include "../Components/GizmoComponent.h"
#include "Modules/Editor/EditorSystem.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/GASSSceneObjectTemplateManager.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSComponentFactory.h"
#include "Sim/Interface/GASSILocationComponent.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSPhysicsSceneObjectMessages.h"


namespace GASS
{
	MoveTool::MoveTool(MouseToolController* controller): m_MouseIsDown(false),
		m_Controller(controller),
		m_MoveUpdateCount(0),
		m_UseGizmo(true),
		m_Active(false),
		m_MouseMoveTime(0)
	{
		m_Controller->GetEditorSceneManager()->GetScene()->RegisterForMessage(REG_TMESS(MoveTool::OnSelectionChanged,EditorSelectionChangedEvent,0));
	}

	MoveTool::~MoveTool()
	{

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
			if(gc) //gizmo under cursor
			{
				m_CurrentGizmo = obj_under_cursor;
				if(gc)
					gc->SetActive(true);

				//create copy if shift is pressed
				if(m_Controller->IsShiftDown())
				{
					EditorSceneManager::SelectionVector new_selection;
					for(size_t i = 0; i < m_Selected.size(); i++)
					{
						SceneObjectPtr selected = m_Selected[i].lock();
						if(selected && selected->GetParentSceneObject())
						{
							SceneObjectPtr new_obj = selected->CreateCopy();
							selected->GetParentSceneObject()->AddChildSceneObject(new_obj,true);
							new_selection.push_back(new_obj);
						}
					}

					if (new_selection.size() > 0)
					{
						m_Controller->GetEditorSceneManager()->SetSelectedObjects(new_selection);
					}
				}
			}
			else // editable object clicked and it's not gizmo based
			{
				//check if object under cursor is selected
				bool selected_clicked = false; 
				for (size_t i = 0; i < m_Selected.size(); i++)
				{
					SceneObjectPtr selected = m_Selected[i].lock();
					if (obj_under_cursor == selected)
						selected_clicked = true;
				}
				//..also check that ctrl not pressed...deselection mode
				if (selected_clicked && !m_Controller->IsCtrlDown()) //selected clicked? if so we are in "GroundSnapMove-mode"
				{
					m_GroundSnapMove = true; //we want to drag object to cursor, or object "behind" cursor
					int from_id = GASS_PTR_TO_INT(this);
					if (m_Controller->IsShiftDown()) //copy operation
					{
						//save selection to be able to restore collision data when mouse is released
						m_SelectionCopy = m_Selected;

						//disable collision, need if we want correct delta movement 
						for (size_t i = 0; i < m_SelectionCopy.size(); i++)
						{
							SceneObjectPtr selected = m_SelectionCopy[i].lock();
							if (selected)
							{
								SendMessageRec(selected, std::make_shared<CollisionSettingsRequest>(false, from_id));
							}
						}

						EditorSceneManager::SelectionVector new_selection;
						for (size_t i = 0; i < m_Selected.size(); i++)
						{
							SceneObjectPtr selected = m_Selected[i].lock();
							if (selected && selected->GetParentSceneObject())
							{
								SceneObjectPtr new_obj = selected->CreateCopy();
								selected->GetParentSceneObject()->AddChildSceneObject(new_obj, true);
								//m_Controller->GetEditorSceneManager()->SelectSceneObject(new_obj);
								new_selection.push_back(new_obj);
								//update selection...
								//m_Selected[i] = new_obj;
							}
						}

						if (new_selection.size() > 0)
						{
							//update internal selection...we don't want to wait for message callback because MouseMove can be called before callback
							m_Selected = new_selection;
							m_Controller->GetEditorSceneManager()->SetSelectedObjects(new_selection);
						}
					}
					
					SceneObjectPtr gizmo = GetOrCreateGizmo();
					if(gizmo)
						SendMessageRec(gizmo, std::make_shared<CollisionSettingsRequest>(false, from_id));
				}
			}
		}
		m_MoveUpdateCount = 0;
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
				const Vec3 new_position = gc->GetPosition(info.m_Ray);
				if (m_MoveUpdateCount > 0)
				{
					const Vec3 delta_move = new_position - m_PreviousPos;
					gizmo_lc->SetWorldPosition(gizmo_lc->GetWorldPosition() + delta_move);
				}
				m_PreviousPos = new_position;
			}
			else if(m_GroundSnapMove)
			{
#ifdef DELTA_MOVE
				if(m_MoveUpdateCount == 0) //we want to move object so disable collision for entire selection
				{
					for (size_t i = 0; i < m_Selected.size(); i++)
					{
						SceneObjectPtr selected = m_Selected[i].lock();
						if (selected)
						{
							SendMessageRec(selected, CollisionSettingsRequestPtr(new CollisionSettingsRequest(false, from_id)));
						}
					}

					//also disable gizmo...collision could be enabled if we have made a copy which will trig selection change event
					SceneObjectPtr gizmo = GetOrCreateGizmo();
					if(gizmo)
						SendMessageRec(gizmo, CollisionSettingsRequestPtr(new CollisionSettingsRequest(false, from_id)));
				}
				else
				{
					if(m_MoveUpdateCount > 2) //we need to wait 2 frames to get correct delta move
					{
						Vec3 delta_move = info.m_3DPos - m_PreviousPos;
						for (size_t i = 0; i < m_Selected.size(); i++)
						{
							SceneObjectPtr selected = m_Selected[i].lock();
							if (selected)
							{
								Vec3 current_pos = selected->GetFirstComponentByClass<ILocationComponent>()->GetWorldPosition();
								selected->SendImmediateRequest(WorldPositionRequestPtr(new WorldPositionRequest(current_pos + delta_move, from_id)));
							}
						}
					}
					m_PreviousPos = info.m_3DPos;
				}
#else
				if(m_MoveUpdateCount == 0) //we want to move object so disable collision for entire selection
				{
					for (size_t i = 0; i < m_Selected.size(); i++)
					{
						SceneObjectPtr selected = m_Selected[i].lock();
						if (selected)
						{
							SendMessageRec(selected, std::make_shared<CollisionSettingsRequest>(false, from_id));
						}
					}
					//also disable gizmo...collision could be enabled if we have made a copy which will trig selection change event
					SceneObjectPtr so_gizmo = GetOrCreateGizmo();
					if(so_gizmo)
						SendMessageRec(so_gizmo, std::make_shared<CollisionSettingsRequest>(false, from_id));
				}
				else
				{
					if(m_MoveUpdateCount == 1)
					{
						//store positions
						m_SelectedLocations.clear();
						for (size_t i = 0; i < m_Selected.size(); i++)
						{
							SceneObjectPtr selected = m_Selected[i].lock();
							if (selected)
							{
								const Vec3 pos = selected->GetFirstComponentByClass<ILocationComponent>()->GetWorldPosition();
								m_SelectedLocations[m_Selected[i]] = pos;
							}
						}
					}
					else if(m_MoveUpdateCount > 1) //we need to wait 2 frames to get correct delta move
					{
						bool has_origo = false;
						Vec3 origo_pos;
						Vec3 offset(0,0,0);
						for (size_t i = 0; i < m_Selected.size(); i++)
						{
							SceneObjectPtr selected = m_Selected[i].lock();
							if (selected)
							{
								if(!has_origo)
								{
									origo_pos =  m_SelectedLocations[m_Selected[i]];
									has_origo = true;
								}
								else
									offset = m_SelectedLocations[m_Selected[i]] - origo_pos;
								selected->GetFirstComponentByClass<ILocationComponent>()->SetWorldPosition(info.m_3DPos + offset);
							}
						}
					}
					m_PreviousPos = info.m_3DPos;
				}
#endif
			}

			const double time = SimEngine::Get().GetTime();
			const double send_freq = 20;
			if(time - m_MouseMoveTime > 1.0 / send_freq)
			{
				m_MouseMoveTime = time;
				std::vector<std::string> attribs;
				attribs.emplace_back("Position");
				attribs.emplace_back("Latitude");
				attribs.emplace_back("Longitude");
				attribs.emplace_back("Projected");
				for (size_t i = 0; i < m_Selected.size(); i++)
				{
					SceneObjectPtr selected = m_Selected[i].lock();
					if (selected)
					{
						GASS::SceneMessagePtr attrib_change_msg(new ObjectAttributeChangedEvent(selected,attribs, from_id));
						m_Controller->GetEditorSceneManager()->GetScene()->PostMessage(attrib_change_msg);
					}
				}
			}
			m_MoveUpdateCount++;
		}
	}

	void MoveTool::MouseUp(const MouseData &data, const SceneCursorInfo &info)
	{
		m_MouseIsDown = false;
		bool selection_mode = false;
		//Check if mouse is static (or just moved  slightly), if so we are in selection mode
		if(fabs(data.XAbsNorm - m_MouseDownPos.x) + fabs(data.YAbsNorm - m_MouseDownPos.y) < 0.05)
		{
			selection_mode = true;
		}
	
		//why?
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
				SendMessageRec(selected, std::make_shared<CollisionSettingsRequest>(true, GASS_PTR_TO_INT(this)));
			}
		}

		for(size_t i = 0; i < m_SelectionCopy.size(); i++)
		{
			SceneObjectPtr selected = m_SelectionCopy[i].lock();
			if(selected && CheckIfEditable(selected))
			{
				SendMessageRec(selected, std::make_shared<CollisionSettingsRequest>(true, GASS_PTR_TO_INT(this)));
			}
		}

		SceneObjectPtr gizmo = GetOrCreateGizmo();
		if(gizmo && m_Controller->GetEnableGizmo())
			SendMessageRec(gizmo, std::make_shared<CollisionSettingsRequest>(true, GASS_PTR_TO_INT(this)));

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

	void MoveTool::SendMessageRec(SceneObjectPtr obj, SceneObjectRequestMessagePtr msg)
	{
		obj->SendImmediateRequest(msg);
		auto iter = obj->GetChildren();
		while(iter.hasMoreElements())
		{
			auto child = iter.getNext();
			SendMessageRec(child,msg);
		}
	}

	bool MoveTool::CheckIfEditable(SceneObjectPtr obj) const
	{
		return (!m_Controller->GetEditorSceneManager()->IsObjectStatic(obj) && 
				!m_Controller->GetEditorSceneManager()->IsObjectLocked(obj) && 
				m_Controller->GetEditorSceneManager()->IsObjectVisible(obj));
	}

	void MoveTool::Stop()
	{
		SetGizmoVisiblity(false);
		m_Active = false;
	}


	SceneObjectPtr _CreateAxisGizmo(const std::string &name, const EulerRotation &rotation, const ColorRGBA &color)
	{
		SceneObjectPtr axis_gizmo = std::make_shared<SceneObject>();

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

	SceneObjectPtr _CreatePlaneGizmo(const std::string &name, const EulerRotation &rotation, const ColorRGBA &color)
	{
		SceneObjectPtr plane_gizmo = std::make_shared<SceneObject>();

		plane_gizmo->SetName(name);
		plane_gizmo->SetSerialize(false);
		ComponentPtr location_comp = ComponentFactory::Get().Create("LocationComponent");
		location_comp->SetPropertyValue("Rotation", rotation);
		plane_gizmo->AddComponent(location_comp);

		ComponentPtr editor_comp = ComponentFactory::Get().Create("EditorComponent");
		editor_comp->SetPropertyValue("AllowDragAndDrop", false);
		editor_comp->SetPropertyValue("AllowRemove", false);
		editor_comp->SetPropertyValue("ShowInTree", false);
		editor_comp->SetPropertyValue("ShowBBWhenSelected", false);
		editor_comp->SetPropertyValue("ChangeMaterialWhenSelected", false);
		plane_gizmo->AddComponent(editor_comp);

		ComponentPtr gizmo_comp = ComponentFactory::Get().Create("GizmoComponent");
		gizmo_comp->SetPropertyValue("Type", GizmoTypeBinder(GT_PLANE));
		gizmo_comp->SetPropertyValue("Color", color);
		gizmo_comp->SetPropertyValue("Size", 0.5f);
		plane_gizmo->AddComponent(gizmo_comp);
		
		ComponentPtr mesh_comp = ComponentFactory::Get().Create("ManualMeshComponent");
		mesh_comp->SetPropertyValue("GeometryFlags", GeometryFlagsBinder(GEOMETRY_FLAG_GIZMO));
		plane_gizmo->AddComponent(mesh_comp);
		return plane_gizmo;
	}

	SceneObjectPtr _CreateMoveGizmo()
	{
		SceneObjectPtr gizmo = std::make_shared<SceneObject>();
		gizmo->SetName("GizmoMoveObject");
		gizmo->SetID("MOVE_GIZMO");
		gizmo->SetSerialize(false);

		ComponentPtr editor_comp = ComponentFactory::Get().Create("EditorComponent");
		editor_comp->SetPropertyValue("AllowDragAndDrop", false);
		editor_comp->SetPropertyValue("AllowRemove", false);
		editor_comp->SetPropertyValue("ShowInTree", false);
		editor_comp->SetPropertyValue("ShowBBWhenSelected", false);
		editor_comp->SetPropertyValue("ChangeMaterialWhenSelected", false);
		gizmo->AddComponent(editor_comp);

		//Create Axis
		gizmo->AddChildSceneObject(_CreateAxisGizmo("GizmoObjectXAxis", EulerRotation(0, 0, 0), ColorRGBA(0, 1, 0, 1)), false);
		gizmo->AddChildSceneObject(_CreateAxisGizmo("GizmoObjectYAxis", EulerRotation(0, 0, 90), ColorRGBA(1, 0, 0, 1)), false);
		gizmo->AddChildSceneObject(_CreateAxisGizmo("GizmoObjectZAxis", EulerRotation(-90, 0, 0), ColorRGBA(0, 0, 1, 1)), false);

		//Create Planes
		gizmo->AddChildSceneObject(_CreatePlaneGizmo("GizmoMovePlaneXZ", EulerRotation(0, 0, 0), ColorRGBA(0, 1, 0, 1)), false);
		gizmo->AddChildSceneObject(_CreatePlaneGizmo("GizmoMovePlaneYZ", EulerRotation(0, 0, 90), ColorRGBA(0, 0, 1, 1)), false);
		gizmo->AddChildSceneObject(_CreatePlaneGizmo("GizmoMovePlaneXY", EulerRotation(0, -90, 0), ColorRGBA(1, 0, 0, 1)), false);

		return gizmo;
	}


	SceneObjectPtr MoveTool::GetOrCreateGizmo()
	{
		SceneObjectPtr gizmo = m_MasterGizmoObject.lock();
		if(!gizmo &&  m_Controller->GetEditorSceneManager()->GetScene())
		{
			gizmo = _CreateMoveGizmo();

			//Add gizmo to scene
			m_Controller->GetEditorSceneManager()->GetScene()->GetRootSceneObject()->AddChildSceneObject(gizmo, true);

			m_MasterGizmoObject = gizmo;
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