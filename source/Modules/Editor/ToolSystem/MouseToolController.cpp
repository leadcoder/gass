


#include "MouseToolController.h"

#include <memory>
#include "Modules/Editor/EditorSceneManager.h"
#include "Modules/Editor/EditorSystem.h"
#include "IMouseTool.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/Math/GASSMath.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSGeometryFlags.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/GASSGraphicsMesh.h"
#include "Sim/GASSResourceManager.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/Interface/GASSIInputSystem.h"
#include "Sim/Interface/GASSIControlSettingsSystem.h"
#include "Sim/Interface/GASSICameraComponent.h"
#include "Sim/Interface/GASSILocationComponent.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"

#include "Modules/Editor/ToolSystem/MoveTool.h"
#include "Modules/Editor/ToolSystem/SelectTool.h"
#include "Modules/Editor/ToolSystem/PaintTool.h"
#include "Modules/Editor/ToolSystem/RotateTool.h"
#include "Modules/Editor/ToolSystem/CreateTool.h"
#include "Modules/Editor/ToolSystem/MeasurementTool.h"
#include "Modules/Editor/ToolSystem/TerrainDeformTool.h"
#include "Modules/Editor/ToolSystem/GoToPositionTool.h"
#include "Modules/Editor/ToolSystem/EditPositionTool.h"
#include "Modules/Editor/ToolSystem/GraphTool.h"
#include "Modules/Editor/ToolSystem/BoxTool.h"

namespace GASS
{
	MouseToolController::MouseToolController(EditorSceneManager* sm): m_ActiveTool(nullptr),
		m_Active(false),
		m_GridSpacing(1),
		m_GridSize(10),
		m_SnapMovment(1),
		m_SnapAngle(15),
		m_EnableMovmentSnap(false),
		m_EnableAngleSnap(false),
		m_RayPickDistance(6371000.0),
		m_EnableGizmo(true),
		m_UseTerrainNormalOnDrop(false),
		m_ControlSettingName("EditorInputSettings"),
		m_EditorSceneManager(sm),
		m_ShiftDown(false),
		m_CtrlDown(false)
	{

	}

	MouseToolController::~MouseToolController(void)
	{
		for(size_t i = 0; i < m_Tools.size(); i++)
		{
			delete m_Tools[i];
		}
		InputSystemPtr input_system = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<IInputSystem>();
		input_system->RemoveMouseListener(this);
		input_system->RemoveKeyListener(this);
	}

	void MouseToolController::Init()
	{
		SetEditMode(GM_LOCAL);
		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(MouseToolController::OnInput,ControllSettingsMessage,0));
		InputSystemPtr input_system = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<IInputSystem>();
		input_system->AddMouseListener(this);
		input_system->AddKeyListener(this);

		//add default tools (based on config?)
		IMouseTool* tool = new MoveTool(this);
		AddTool(tool);
		tool = new GASS::SelectTool(this);
		AddTool(tool);
		tool = new RotateTool(this);
		AddTool(tool);
		tool = new CreateTool(this);
		AddTool(tool);
		tool = new MeasurementTool(this);
		AddTool(tool);
		tool = new GoToPositionTool(this);
		AddTool(tool);
		tool = new EditPositionTool(this);
		AddTool(tool);
		tool = new TerrainDeformTool(this);
		AddTool(tool);
		tool = new PaintTool(this);
		AddTool(tool);
		tool = new GraphTool(this);
		AddTool(tool);
		
		tool = new BoxTool(this);
		AddTool(tool);

		SelectTool(TID_SELECT);
		SetActive(true);
	}

	IMouseTool* MouseToolController::GetTool(const std::string &tool_name)
	{
		for(size_t i = 0; i < m_Tools.size(); i++)
		{
			if(tool_name == m_Tools[i]->GetName())
			{
				return m_Tools[i];
			}
		}
		return nullptr;
	}

	void MouseToolController::OnInput(ControllSettingsMessagePtr message)
	{
		//Check settings
		if(m_ControlSettingName != message->GetSettings())
			return;
		/*if(m_Active)
		{
			std::string name = message->GetController();
			float value = message->GetValue();

			if(name == "ChangeTool")
			{
				//std::cout << value << "\n";
				if(value > 0.5)
					NextTool();
				else if(value < -0.5)
					PrevTool();
			}
			//temp solution, implement this in custom tool selection class
			else if(name == TID_MOVE ||
					name == TID_VERTICAL_MOVE ||
					name == TID_ROTATE ||
					name == TID_PAINT ||
					name == TID_TERRAIN ||
					name == TID_GOTO_POS ||
					name == TID_EDIT_POS
					)
			{
				SelectTool(name);
			}
		}*/
	}

	void MouseToolController::AddTool(IMouseTool* tool)
	{
		m_Tools.push_back(tool);
	}

	bool MouseToolController::SelectTool(const std::string &tool_name)
	{
		for(size_t i = 0; i < m_Tools.size(); i++)
		{
			if(tool_name == m_Tools[i]->GetName())
			{
				if(m_ActiveTool)
					m_ActiveTool->Stop();
				m_ActiveTool = m_Tools[i];
				m_ActiveTool->Start();
				SimEngine::Get().GetSimSystemManager()->PostMessage(SystemMessagePtr(new ToolChangedEvent(tool_name)));
				return true;
			}
		}
		return false;
	}

	void MouseToolController::NextTool()
	{
		if(m_ActiveTool)
		{
			for(size_t i = 0; i < m_Tools.size(); i++)
			{
				if(m_ActiveTool == m_Tools[i])
				{
					size_t new_tool = i+1;
					if(new_tool > m_Tools.size()-1)
						new_tool = 0;
					int id = GASS_PTR_TO_INT(this);
					SystemMessagePtr tool_msg(new ToolChangedEvent(m_Tools[new_tool]->GetName(),id));
					SimEngine::Get().GetSimSystemManager()->PostMessage(tool_msg);
					return;
				}
			}
		}
	}

	void MouseToolController::PrevTool()
	{
		if(m_ActiveTool)
		{
			for(size_t i = 0; i < m_Tools.size(); i++)
			{
				if(m_ActiveTool == m_Tools[i])
				{
				    size_t new_tool = i;
				    if(new_tool == 0)
						new_tool = static_cast<int>(m_Tools.size()-1);
                    else
                        new_tool = i-1;



					int id = GASS_PTR_TO_INT(this);
					SystemMessagePtr tool_msg(new ToolChangedEvent(m_Tools[new_tool]->GetName(),id));
					SimEngine::Get().GetSimSystemManager()->PostMessage(tool_msg);

					return;
				}
			}
		}
	}

	GASS::CollisionResult MouseToolController::CameraRaycast(CameraComponentPtr cam, const Vec2 &viewport_pos, Float raycast_distance, GeometryFlags col_bits) const
	{
		CollisionResult result;
		result.Coll = false;
		BaseSceneComponentPtr bsc = GASS_DYNAMIC_PTR_CAST<BaseSceneComponent>(cam);
		if(cam && bsc)
		{
			CollisionSceneManagerPtr col_sm = bsc->GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<ICollisionSceneManager>();
			Ray ray;
			cam->GetCameraToViewportRay(static_cast<float>(viewport_pos.x), static_cast<float>(viewport_pos.y), ray);
			Vec3 ray_direction = ray.m_Dir*raycast_distance;
			col_sm->Raycast(ray.m_Origin, ray_direction,col_bits,result);
		}
		return result;
	}

	SceneCursorInfo MouseToolController::GetSceneCursorInfo(const Vec2 &cursor_pos, Float raycast_distance) const
	{
		SceneCursorInfo info;
		CameraComponentPtr cam = m_EditorSceneManager->GetActiveCamera();

		//get ray direction
		if(cam)
			cam->GetCameraToViewportRay(static_cast<float>(cursor_pos.x), static_cast<float>(cursor_pos.y), info.m_Ray);

		GASS::CollisionResult gizmo_result = CameraRaycast(cam, cursor_pos, raycast_distance, GEOMETRY_FLAG_GIZMO);

		if(gizmo_result.Coll)
		{
			SceneObjectPtr col_obj = gizmo_result.CollSceneObject.lock();
			if(col_obj)
			{
				info.m_3DPos = gizmo_result.CollPosition;
				info.m_Normal = gizmo_result.CollNormal;
				info.m_ObjectUnderCursor = gizmo_result.CollSceneObject;
			}
		}
		else
		{
			GASS::CollisionResult col_result  = CameraRaycast(cam, cursor_pos, raycast_distance, static_cast<GeometryFlags>( static_cast<int>(GEOMETRY_FLAG_SCENE_OBJECTS) | static_cast<int>(GEOMETRY_FLAG_EDITOR)));
			if (col_result.Coll)
			{
				SceneObjectPtr col_obj = col_result.CollSceneObject.lock();
				if(col_obj)
				{
					info.m_3DPos = col_result.CollPosition;
					info.m_Normal = col_result.CollNormal;
					info.m_ObjectUnderCursor = col_result.CollSceneObject;
				}
			}
		}
		return info;
	}

	void MouseToolController::SetGridSpacing(Float value)
	{
		m_GridSpacing = value;
		SimEngine::Get().GetSimSystemManager()->PostMessage(SystemMessagePtr(new ChangeGridRequest(m_GridSize,m_GridSpacing)));
	}

	void MouseToolController::SetGridSize(Float value)
	{
		m_GridSize = value;
		SimEngine::Get().GetSimSystemManager()->PostMessage(SystemMessagePtr(new ChangeGridRequest(m_GridSize,m_GridSpacing)));
	}

	void MouseToolController::SetSnapMovment(Float value)
	{
		m_SnapMovment= value;
	}

	void MouseToolController::SetSnapAngle(Float value)
	{
		m_SnapAngle = value;
	}

	Float MouseToolController::SnapPosition(Float value) const
	{
		if(m_EnableMovmentSnap)
		{
			Float new_value = value/m_SnapMovment;
			new_value = int(new_value);
			return new_value*m_SnapMovment;
		}
		return value;
	}

	Float MouseToolController::SnapAngle(Float value) const
	{
		if(m_EnableAngleSnap)
		{
			Float rad_angle = Math::Deg2Rad(m_SnapAngle);
			Float new_value = value/rad_angle;
			new_value = static_cast<int>(new_value);
			return new_value*rad_angle;
		}
		else
			return value;
	}

	SceneObjectPtr MouseToolController::GetPointerObject()
	{
		ScenePtr scene = m_EditorSceneManager->GetScene();;
		SceneObjectPtr pointer = m_PointerObject.lock();
		if(!pointer &&  scene)
		{
			GASS::SceneObjectPtr scene_object = scene->LoadObjectFromTemplate("PointerObject",scene->GetRootSceneObject());
			m_PointerObject = scene_object;
			pointer = scene_object;

			GraphicsMeshPtr mesh_data(new GraphicsMesh());
			GraphicsSubMeshPtr sub_mesh_data(new GraphicsSubMesh());
			mesh_data->SubMeshVector.push_back(sub_mesh_data);

			Vec3 pos = Vec3(0,0,-1);
			sub_mesh_data->PositionVector.push_back(pos);
			pos = Vec3(0,0,1);
			sub_mesh_data->PositionVector.push_back(pos);
			sub_mesh_data->IndexVector.push_back(0);
			sub_mesh_data->IndexVector.push_back(1);

			pos = Vec3(-1,0,0);
			sub_mesh_data->PositionVector.push_back(pos);
			pos = Vec3(1,0,0);
			sub_mesh_data->PositionVector.push_back(pos);
			sub_mesh_data->IndexVector.push_back(2);
			sub_mesh_data->IndexVector.push_back(3);

			pos = Vec3(0,-1,0);
			sub_mesh_data->PositionVector.push_back(pos);
			pos = Vec3(0,1,0);
			sub_mesh_data->PositionVector.push_back(pos);
			sub_mesh_data->IndexVector.push_back(4);
			sub_mesh_data->IndexVector.push_back(5);

			sub_mesh_data->MaterialName = "WhiteNoLighting";
			sub_mesh_data->Type = LINE_LIST;

			scene_object->PostRequest(std::make_shared<ManualMeshDataRequest>(mesh_data));
		}
		return pointer;
	}

	void MouseToolController::Update(double delta)
	{
		if(m_ActiveTool)
			m_ActiveTool->Update(delta);
		
		//debug message
		/*
		GASS_PRINT("Cursor pos:" << m_LastScreenPos);
	
		/*SceneObjectPtr obj_under_cursor(info.m_ObjectUnderCursor,NO_THROW);
		if(obj_under_cursor)
		{
			GASS_PRINT("Object under cursor:" << obj_under_cursor->GetName() << "Cursor pos:" << info.m_3DPos << " 2d:" << info.m_ScreenPos)
		}*/
	}

	void MouseToolController::SetEnableGizmo(int value)
	{
		if(value > 0)
			m_EnableGizmo = true;
		else
			m_EnableGizmo = false;

		if(m_ActiveTool)
		{
			m_ActiveTool->Stop();
			m_ActiveTool->Start();
		}
	}

	bool MouseToolController::MouseMoved(const MouseData &data)
	{
		SceneCursorInfo info = GetSceneCursorInfo(Vec2(data.XAbsNorm,data.YAbsNorm), m_RayPickDistance);
		//inform tools
		if(m_ActiveTool)
			m_ActiveTool->MouseMoved(data,info);

		int mess_id = GASS_PTR_TO_INT(this);
		SceneMessagePtr cursor_msg(new CursorMovedOverSceneEvent(Vec2(data.XAbsNorm, data.YAbsNorm), info.m_3DPos, info.m_ObjectUnderCursor.lock(), mess_id));
		if(m_EditorSceneManager->GetScene())
			m_EditorSceneManager->GetScene()->PostMessage(cursor_msg);
		return true;
	}

	bool MouseToolController::MousePressed(const MouseData &data, MouseButtonId button )
	{
		if(m_Active)
		{
			if(button == MBID_LEFT)
			{
				Vec2 mouse_pos(data.XAbsNorm,data.YAbsNorm);
				SceneCursorInfo info = GetSceneCursorInfo(mouse_pos,m_RayPickDistance);
				if(m_ActiveTool)
					m_ActiveTool->MouseDown(data,info);
				return true;
			}
			else if(button == MBID_RIGHT)
			{
				m_MBRScreenPos = Vec2(data.XAbsNorm,data.YAbsNorm);
			}
		}
		return false;
	}

	bool MouseToolController::MouseReleased(const MouseData &data, MouseButtonId button )
	{
		if(button == MBID_LEFT)
		{
				Vec2 mouse_pos(data.XAbsNorm,data.YAbsNorm);
				SceneCursorInfo info = GetSceneCursorInfo(mouse_pos,m_RayPickDistance);
				if(m_ActiveTool)
					m_ActiveTool->MouseUp(data,info);
				return true;
		}

		else if(button == MBID_RIGHT)
		{
			if (m_ActiveTool && m_ActiveTool->GetName() == TID_CREATE)
			{
				SelectTool(TID_MOVE);
			}
			else
			{
				const Vec2 mouse_pos(data.XAbsNorm, data.YAbsNorm);
				const SceneCursorInfo info = GetSceneCursorInfo(mouse_pos, m_RayPickDistance);

				//check if cursor at rest
				const Vec2 delta(m_MBRScreenPos.x - mouse_pos.x, m_MBRScreenPos.y - mouse_pos.y);
				if (fabs(delta.x) + abs(delta.y) < 0.001)
				{
					//Check that object under cursor is in selection
					if (m_EditorSceneManager->IsSelected(info.m_ObjectUnderCursor.lock()))
					{
						//show Immediate!
						//Disable OIS input to avoid background selection
						GASS::SceneMessagePtr message(new ShowSceneObjectMenuRequest(Vec2(data.XAbs, data.YAbs)));
						m_EditorSceneManager->GetScene()->SendImmediate(message);
					}
				}
			}
			return true;
		}
		return false;
	}


	bool MouseToolController::KeyPressed( int key, unsigned int /*text*/)
	{
		if(key == KEY_LSHIFT)
				m_ShiftDown = true;

		if(key == KEY_LCONTROL)
				m_CtrlDown = true;

		if(key == KEY_G && m_CtrlDown)
		{
			if(m_EditMode == GM_LOCAL)
				SetEditMode(GM_WORLD);
			else
				SetEditMode(GM_LOCAL);

		}

		if(key == KEY_F5 && m_CtrlDown)
		{
			GASS::SimEngine::Get().GetResourceManager()->ReloadAll();
		}

		if(key == KEY_F6 && m_CtrlDown)
		{
			GASS::SimEngine::Get().ReloadTemplates();
		}

		if (key == KEY_E && m_CtrlDown)
		{
			const bool toggle_value = !GASS::SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<EditorSystem>()->GetShowGUI();
			GASS::SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<EditorSystem>()->SetShowGUI(toggle_value);
		}

		return true;
	}

	bool MouseToolController::KeyReleased( int key, unsigned int /*text*/)
	{
		if(key == KEY_LSHIFT)
			m_ShiftDown = false;
		if(key == KEY_LCONTROL)
				m_CtrlDown = false;
		return true;
	}


	bool MouseToolController::GetMouseWorldPosAndRot(const Vec2 &mouse_pos, GASS::Vec3 &world_pos, GASS::Quaternion &world_rot) const
	{
		SceneCursorInfo cursor_info = GetSceneCursorInfo(mouse_pos, 1000000);
		SceneObjectPtr object_under_cursor = cursor_info.m_ObjectUnderCursor.lock();
		if (!object_under_cursor)
			return false;

		world_pos = cursor_info.m_3DPos;
		world_pos.x = SnapPosition(world_pos.x);
		world_pos.y = SnapPosition(world_pos.y);
		world_pos.z = SnapPosition(world_pos.z);
		const CollisionSceneManagerPtr csm = object_under_cursor->GetScene()->GetFirstSceneManagerByClass<ICollisionSceneManager>(true);
		if (csm)
		{
			csm->GetOrientation(world_pos, world_rot);
		}
		return true;
	}

	void MouseToolController::CreateSceneObject(const std::string template_name, const Vec2 &mouse_pos)
	{
		Vec3 drop_pos(0,0,0);
		Quaternion drop_rot(GASS::Quaternion::IDENTITY);
		if (GetMouseWorldPosAndRot(mouse_pos, drop_pos, drop_rot))
		{
			CreateObjectFromTemplateAtPosition(template_name, drop_pos, drop_rot);
		}
		else
		{
			CreateObjectFromTemplateAtPosition(template_name, drop_pos, drop_rot);
		}
	}

	void MouseToolController::SetEditMode(GizmoEditMode value)
	{
		m_EditMode = value;
		SimEngine::Get().GetSimSystemManager()->PostMessage(SystemMessagePtr(new EditModeChangedEvent(m_EditMode)));
	}

	void MouseToolController::SelectHelper(SceneObjectPtr obj) const
	{
		if (IsCtrlDown())
		{
			if(GetEditorSceneManager()->IsSelected(obj))
				GetEditorSceneManager()->UnselectSceneObject(obj);
			else
				GetEditorSceneManager()->SelectSceneObject(obj);
		}
		else
		{
			GetEditorSceneManager()->UnselectAllSceneObjects();
			GetEditorSceneManager()->SelectSceneObject(obj);
		}
	}

	void MouseToolController::CreateObjectFromTemplateAtPosition(const std::string &obj_name, const GASS::Vec3 &pos, const GASS::Quaternion &rot)
	{
		ScenePtr cur_scene = m_EditorSceneManager->GetScene();
		SceneObjectPtr site = m_EditorSceneManager->GetObjectSite();
		if(site && cur_scene)
		{
			SceneObjectPtr so = SimEngine::Get().CreateObjectFromTemplate(obj_name);
			if(so)
			{
				site->AddChildSceneObject(so, true);
				if (LocationComponentPtr location = so->GetFirstComponentByClass<ILocationComponent>())
				{
					location->SetWorldPosition(pos);
					location->SetWorldRotation(rot);
				}
			}
			else
			{
				//failed to create object
			}
		}
	}

}

