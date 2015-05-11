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
		m_Active(false),
		m_SnapToMouse(true)
	{
		m_Controller->GetEditorSceneManager()->GetScene()->RegisterForMessage(REG_TMESS(MoveTool::OnSceneObjectSelected,ObjectSelectionChangedEvent,0));
	}

	MoveTool::~MoveTool()
	{

	}

	void MoveTool::MouseMoved(const MouseData &data, const SceneCursorInfo &info)
	{
		int from_id = PTR_TO_INT(this);

		SceneObjectPtr selected(m_SelectedObject,NO_THROW);
		if(m_MouseIsDown && selected && CheckIfEditable(selected))
		{
			SceneObjectPtr gizmo(m_CurrentGizmo,NO_THROW);
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
						if(gc->GetMode() == GM_WORLD)
						{
							m_Offset.x = m_Controller->SnapPosition(m_Offset.x);
							m_Offset.y = m_Controller->SnapPosition(m_Offset.y);
							m_Offset.z = m_Controller->SnapPosition(m_Offset.z);
						}

						m_Offset = new_position - m_Offset;
					}
				}
				new_position = new_position - m_Offset;
				gizmo->PostRequest(WorldPositionRequestPtr(new WorldPositionRequest(new_position,from_id)));
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

							SceneObjectPtr gizmo(m_CurrentGizmo,NO_THROW);
							if(gizmo)
							{
								GizmoComponentPtr gc = gizmo->GetFirstComponentByClass<GizmoComponent>();
								if(gc->GetMode() == GM_WORLD)
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
						//move selected object

						if(m_SnapToMouse)
							m_Offset.Set(0,0,0);

						Vec3 new_position = info.m_3DPos - m_Offset;


						new_position.x = m_Controller->GetEditorSceneManager()->GetMouseToolController()->SnapPosition(new_position.x);
						new_position.y = m_Controller->GetEditorSceneManager()->GetMouseToolController()->SnapPosition(new_position.y);
						new_position.z = m_Controller->GetEditorSceneManager()->GetMouseToolController()->SnapPosition(new_position.z);

						selected->PostRequest(WorldPositionRequestPtr(new WorldPositionRequest(new_position,from_id)));
					}
				}

			}
			const double time = SimEngine::Get().GetRunTimeController()->GetTime();
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

			}

			m_MoveUpdateCount++;
		}
	}


	void MoveTool::MouseDown(const MouseData &data, const SceneCursorInfo &info)
	{
		m_MouseIsDown = true;
		m_MouseDownPos.Set(data.XAbsNorm,data.YAbsNorm);
		m_GroundSnapMove = false;
		SceneObjectPtr obj_under_cursor(info.m_ObjectUnderCursor,NO_THROW);

		if(obj_under_cursor  && CheckIfEditable(obj_under_cursor ))
		{
			GizmoComponentPtr gc = obj_under_cursor->GetFirstComponentByClass<GizmoComponent>();
			if(gc)
			{
				m_CurrentGizmo = obj_under_cursor;
				if(gc)
					gc->SetActive(true);

				//create copy if shift is pressed
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
				m_GroundSnapMove = true;
				int from_id = PTR_TO_INT(this);

				//Disable object collision

				//create copy if shift is pressed
				SceneObjectPtr selected = SceneObjectPtr(m_SelectedObject,NO_THROW);
				if(m_Controller->IsShiftDown() && selected && selected->GetParentSceneObject())
				{
					SceneObjectPtr new_obj = selected->CreateCopy();
					selected->GetParentSceneObject()->AddChildSceneObject(new_obj,true);
					m_Controller->GetEditorSceneManager()->SelectSceneObject(new_obj);
					selected = new_obj;
				}
				CollisionSettingsRequestPtr col_request(new CollisionSettingsRequest(false,from_id));
				SendMessageRec(selected, col_request);
				SceneObjectPtr gizmo = GetOrCreateGizmo();
				if(gizmo)
					SendMessageRec(gizmo,col_request);
			}
			m_MoveUpdateCount = 0;
		}
	}

	void MoveTool::SendMessageRec(SceneObjectPtr obj, SceneObjectRequestMessagePtr msg)
	{
		obj->PostRequest(msg);
		GASS::ComponentContainer::ComponentContainerIterator iter = obj->GetChildren();
		while(iter.hasMoreElements())
		{
			SceneObjectPtr child = STATIC_PTR_CAST<SceneObject>(iter.getNext());
			SendMessageRec(child,msg);
		}
	}

	void MoveTool::MouseUp(const MouseData &data, const SceneCursorInfo &info)
	{
		m_MouseIsDown = false;
		bool slection_mode = false;



		if(fabs(data.XAbsNorm - m_MouseDownPos.x) + abs(data.YAbsNorm - m_MouseDownPos.y) < 0.05)
		{
			slection_mode = true;
		}

		SceneObjectPtr g_obj(m_CurrentGizmo,NO_THROW);
		if(g_obj)
		{
			GizmoComponentPtr gc = g_obj->GetFirstComponentByClass<GizmoComponent>();
			if(gc)
			{
				gc->SetActive(false);
			}
		}

		m_CurrentGizmo.reset();

		SceneObjectPtr selected(m_SelectedObject,NO_THROW);

		if(selected && CheckIfEditable(selected))
		{
			int from_id = PTR_TO_INT(this);
			CollisionSettingsRequestPtr col_msg(new CollisionSettingsRequest(true,from_id));
			SendMessageRec(selected,col_msg);

			SceneObjectPtr gizmo = GetOrCreateGizmo();
			if(gizmo && m_Controller->GetEnableGizmo())
				SendMessageRec(gizmo,col_msg);
		}

		if(slection_mode) //selection mode
		{
			SceneObjectPtr obj_under_cursor (info.m_ObjectUnderCursor,NO_THROW);
			if(obj_under_cursor)
			{
				if(CheckIfEditable(obj_under_cursor))
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
		int from_id = PTR_TO_INT(this);
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
		SceneObjectPtr gizmo(m_MasterGizmoObject,NO_THROW);
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
				SceneObjectPtr current (m_SelectedObject,NO_THROW);
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
		SetGizmoVisiblity(m_Controller->GetEnableGizmo());
		m_Active = true;
	}

	void MoveTool::SetGizmoVisiblity(bool value)
	{
		SceneObjectPtr gizmo = GetOrCreateGizmo();
		if(gizmo)
		{
			int from_id = PTR_TO_INT(this);
			CollisionSettingsRequestPtr col_msg(new CollisionSettingsRequest(value,from_id));
			SendMessageRec(gizmo,col_msg);
			LocationVisibilityRequestPtr vis_msg(new LocationVisibilityRequest(value,from_id));
			SendMessageRec(gizmo,vis_msg);
		}
	}

	void MoveTool::OnSceneObjectSelected(ObjectSelectionChangedEventPtr message)
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

