

#include <boost/bind.hpp>
#include "MouseToolController.h"
#include "Modules/Editor/EditorSystem.h"
#include "Modules/Editor/EditorSceneManager.h"
#include "IMouseTool.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/ComponentSystem/GASSIComponent.h"
#include "Core/Utils/GASSException.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSGeometryFlags.h"
#include "Sim/Interface/GASSIInputSystem.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/Interface/GASSIControlSettingsSystem.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/Interface/GASSICameraComponent.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/GASSMeshData.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Interface/GASSIGeometryComponent.h"
#include "Sim/Interface/GASSILocationComponent.h"
#include "Sim/Interface/GASSIGraphicsSystem.h"

#include "Modules/Editor/ToolSystem/MoveTool.h"
#include "Modules/Editor/ToolSystem/SelectTool.h"
#include "Modules/Editor/ToolSystem/PaintTool.h"
#include "Modules/Editor/ToolSystem/VerticalMoveTool.h"
#include "Modules/Editor/ToolSystem/RotateTool.h"
#include "Modules/Editor/ToolSystem/CreateTool.h"
#include "Modules/Editor/ToolSystem/MeasurementTool.h"
#include "Modules/Editor/ToolSystem/TerrainDeformTool.h"
#include "Modules/Editor/ToolSystem/GoToPositionTool.h"
#include "Modules/Editor/ToolSystem/EditPositionTool.h"



namespace GASS
{
	MouseToolController::MouseToolController(EditorSceneManager* sm): m_ActiveTool(NULL),
		m_Active(false),
		m_GridSpacing(1),
		m_GridSize(10),
		m_SnapMovment(1),
		m_SnapAngle(15),
		m_EnableMovmentSnap(false),
		m_EnableAngleSnap(false),
		m_RayPickDistance(3000),
		m_EnableGizmo(true),
		m_UseTerrainNormalOnDrop(false),
		m_ControlSettingName("EditorInputSettings"),
		m_EditorSceneManager(sm)
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

	}

	void MouseToolController::Init()
	{
		//SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(MouseToolController::OnFocusChanged,WindowFocusChangedMessage,0));
		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(MouseToolController::OnInput,ControllSettingsMessage,0));
		InputSystemPtr input_system = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<IInputSystem>();
		input_system->AddMouseListener(this);

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
		SelectTool(TID_SELECT);
		SetActive(true);
	}

	IMouseTool* MouseToolController::GetTool(const std::string &tool_name)
	{
		for(int i = 0; i < m_Tools.size(); i++)
		{
			if(tool_name == m_Tools[i]->GetName())
			{
				return m_Tools[i];
			}
		}
		return NULL;
	}

	/*void MouseToolController::OnFocusChanged(WindowFocusChangedMessagePtr message)
	{
		std::string window = message->GetWindow();
		if(window == "RenderWindow")
		{
			m_Active = message->GetFocus();
		}
	}*/

	void MouseToolController::OnInput(ControllSettingsMessagePtr message)
	{
		//Check settings
		if(m_ControlSettingName != message->GetSettings())
			return;
		if(m_Active)
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
		}
	}

	void MouseToolController::AddTool(IMouseTool* tool)
	{
		m_Tools.push_back(tool);
	}

	bool MouseToolController::SelectTool(const std::string &tool_name)
	{
		for(int i = 0; i < m_Tools.size(); i++)
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
			for(int i = 0; i < m_Tools.size(); i++)
			{
				if(m_ActiveTool == m_Tools[i])
				{
					int new_tool = i+1;
					if(new_tool > m_Tools.size()-1)
						new_tool = 0;
					int id = (int) this;
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
			for(int i = 0; i < m_Tools.size(); i++)
			{
				if(m_ActiveTool == m_Tools[i])
				{
					int new_tool = i-1;
					if(new_tool < 0)
						new_tool = static_cast<int>(m_Tools.size()-1);

					int id = (int) this;
					SystemMessagePtr tool_msg(new ToolChangedEvent(m_Tools[new_tool]->GetName(),id));
					SimEngine::Get().GetSimSystemManager()->PostMessage(tool_msg);

					return;
				}
			}
		}
	}

	/*void MouseToolController::MouseMoved(const MouseData &data, const SceneCursorInfo &info)
	{
		if(m_ActiveTool)
			m_ActiveTool->MouseMoved(info);
	}

	void MouseToolController::MouseDown(const MouseData &data, const SceneCursorInfo &info)
	{
		if(m_ActiveTool)
			m_ActiveTool->MouseDown(info);
	}

	void MouseToolController::MouseUp(const MouseData &data, const SceneCursorInfo &info)
	{
		if(m_ActiveTool)
			m_ActiveTool->MouseUp(info);
	}*/



	GASS::CollisionResult MouseToolController::CameraRaycast(CameraComponentPtr cam, const Vec2 &viewport_pos, Float raycast_distance, GeometryFlags col_bits)
	{
		CollisionResult result;
		result.Coll = false;
		BaseSceneComponentPtr bsc = boost::shared_dynamic_cast<BaseSceneComponent>(cam);
		if(cam && bsc)
		{
			CollisionSceneManagerPtr col_sm = bsc->GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<ICollisionSceneManager>();
			Vec3 ray_start;
			Vec3 ray_direction;
			cam->GetCameraToViewportRay(viewport_pos.x, viewport_pos.y,ray_start,ray_direction);
			ray_direction = ray_direction*raycast_distance;
			GASS::CollisionRequest request;
			request.LineStart = ray_start;
			request.LineEnd = ray_start + ray_direction;
			request.Type = COL_LINE;
			request.ReturnFirstCollisionPoint = false;
			request.CollisionBits = col_bits;
			col_sm->Force(request,result);
		}
		return result;
	}

	SceneCursorInfo MouseToolController::GetSceneCursorInfo(const Vec2 &cursor_pos, Float raycast_distance)
	{
		SceneCursorInfo info;
		CameraComponentPtr cam = m_EditorSceneManager->GetActiveCamera();

		//get ray direction
		if(cam)
			cam->GetCameraToViewportRay(cursor_pos.x, cursor_pos.y,info.m_RayStart,info.m_RayDir);

		GASS::CollisionResult gizmo_result = CameraRaycast(cam, cursor_pos, raycast_distance, GEOMETRY_FLAG_GIZMO);

		int from_id = int(this);

		if(gizmo_result.Coll)
		{
			SceneObjectPtr col_obj(gizmo_result.CollSceneObject,boost::detail::sp_nothrow_tag());
			if(col_obj)
			{
				info.m_3DPos = gizmo_result.CollPosition;
				info.m_ObjectUnderCursor = gizmo_result.CollSceneObject;
				//std::cout << result.CollPosition << std::endl;
			}
		}
		else
		{
			GASS::CollisionResult mesh_result  = CameraRaycast(cam, cursor_pos, raycast_distance, GEOMETRY_FLAG_SCENE_OBJECTS);
			if(mesh_result.Coll)
			{
				SceneObjectPtr col_obj(mesh_result.CollSceneObject,boost::detail::sp_nothrow_tag());
				if(col_obj)
				{
					info.m_3DPos = mesh_result.CollPosition;
					info.m_ObjectUnderCursor = mesh_result.CollSceneObject;
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

	Float MouseToolController::SnapPosition(Float value)
	{
		if(m_EnableMovmentSnap)
		{
			Float new_value = value/m_SnapMovment;
			new_value = int(new_value);
			return new_value*m_SnapMovment;
		}
		return value;
	}

	Float MouseToolController::SnapAngle(Float value)
	{
		if(m_EnableAngleSnap)
		{
			float rad_angle = Math::Deg2Rad(m_SnapAngle);
			Float new_value = value/rad_angle;
			new_value = int(new_value);
			return new_value*rad_angle;
		}
		else 
			return value;
	}

	/*void MouseToolController::OnSnapSettingsMessage(SnapSettingsMessagePtr message)
	{
		m_SnapMovment = message->GetMovementSnap();
		m_SnapAngle = message->GetRotationSnap();
	}*/

	/*void MouseToolController::OnSnapModeMessage(SnapModeMessagePtr message)
	{
		m_EnableMovmentSnap = message->MovementSnapEnabled();
		m_EnableAngleSnap = message->RotationSnapEnabled();
	}*/

	SceneObjectPtr MouseToolController::GetPointerObject()
	{
		ScenePtr scene = m_EditorSceneManager->GetScene();;
		SceneObjectPtr pointer(m_PointerObject,boost::detail::sp_nothrow_tag());
		if(!pointer &&  scene)
		{
			GASS::SceneObjectPtr scene_object = scene->LoadObjectFromTemplate("PointerObject",scene->GetRootSceneObject());
			m_PointerObject = scene_object;
			pointer = scene_object;

			ManualMeshDataPtr mesh_data(new ManualMeshData);

			MeshVertex vertex;
			float box_volume = 1;

			vertex.TexCoord.Set(0,0);
			vertex.Color  = Vec4(1,1,1,1);

			vertex.Pos = Vec3(0,0,-1);
			mesh_data->VertexVector.push_back(vertex);
			vertex.Pos = Vec3(0,0,1);
			mesh_data->VertexVector.push_back(vertex);
			mesh_data->IndexVector.push_back(0);
			mesh_data->IndexVector.push_back(1);

			vertex.Pos = Vec3(-1,0,0);
			mesh_data->VertexVector.push_back(vertex);
			vertex.Pos = Vec3(1,0,0);
			mesh_data->VertexVector.push_back(vertex);
			mesh_data->IndexVector.push_back(2);
			mesh_data->IndexVector.push_back(3);

			vertex.Pos = Vec3(0,-1,0);
			mesh_data->VertexVector.push_back(vertex);
			vertex.Pos = Vec3(0,1,0);
			mesh_data->VertexVector.push_back(vertex);
			mesh_data->IndexVector.push_back(4);
			mesh_data->IndexVector.push_back(5);

			mesh_data->Material = "GizmoArrowMat";
			mesh_data->Type = LINE_LIST;


			MessagePtr mesh_message(new ManualMeshDataMessage(mesh_data));
			scene_object->PostMessage(mesh_message);
		}
		return pointer;
	}

	void MouseToolController::Update(double delta)
	{
		//m_Delta = delta;
		//debug message

		/*std::stringstream ss;
		ss << " Cursor pos:" << m_LastScreenPos << "\n";
		const std::string message = ss.str();
		SimEngine::Get().GetSimSystemManager()->PostMessage(MessagePtr( new DebugPrintRequest(message)));*/

		

		/*for(int i = 0; i < m_Tools.size(); i++)
		{
		}*/
		if(m_ActiveTool)
			m_ActiveTool->Update(delta);
		//MouseMoved(info);
		/*SceneObjectPtr obj_under_cursor(info.m_ObjectUnderCursor,boost::detail::sp_nothrow_tag());
		if(obj_under_cursor)
		{
			std::stringstream ss;
			ss << " Cursor pos:" << info.m_3DPos << " 2d:" << info.m_ScreenPos << "\n";
			const std::string message = "Object under cursor:" + obj_under_cursor->GetName() + ss.str();
			//SimEngine::Get().GetSimSystemManager()->PostMessage(MessagePtr( new DebugPrintRequest(message)));
		}*/
	}

	void MouseToolController::SetEnableGizmo(int value) 
	{
		m_EnableGizmo=value;
		if(m_ActiveTool)
		{
			m_ActiveTool->Stop();
			m_ActiveTool->Start();
		}
	}

	bool MouseToolController::MouseMoved(const MouseData &data)
	{
		SceneCursorInfo info = GetSceneCursorInfo(Vec2(data.XAbsNorm,data.YAbsNorm),m_RayPickDistance);
		//inform tools
		if(m_ActiveTool)
			m_ActiveTool->MouseMoved(data,info);
		
		int mess_id = (int) this;
		SceneMessagePtr cursor_msg(new CursorMovedOverSceneEvent(Vec2(data.XAbsNorm,data.YAbsNorm),info.m_3DPos, SceneObjectPtr(info.m_ObjectUnderCursor,boost::detail::sp_nothrow_tag()),mess_id));
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
			Vec2 mouse_pos(data.XAbsNorm,data.YAbsNorm);	
			SceneCursorInfo info = GetSceneCursorInfo(mouse_pos,m_RayPickDistance);
			
			//check if cursor at rest
			Vec2 delta;
			delta.x	= m_MBRScreenPos.x - mouse_pos.x;
			delta.y	= m_MBRScreenPos.y - mouse_pos.y;
			if(abs(delta.x) + abs(delta.y) < 0.001)
			{
				SceneObjectPtr selected = m_EditorSceneManager->GetSelectedObject();
				if(SceneObjectPtr(info.m_ObjectUnderCursor,boost::detail::sp_nothrow_tag()) == selected)
				{
					//show Immediate!
					//Disable OIS input to avoid background selection
					GASS::SceneMessagePtr message(new ShowObjectMenuRequest(selected,Vec2(data.XAbs,data.YAbs)));
					m_EditorSceneManager->GetScene()->SendImmediate(message);
				}
			}
			return true;
		}
		return false;
	}

	void MouseToolController::CreateSceneObject(const std::string name, const Vec2 &mouse_pos)
	{
		SceneCursorInfo cursorInfo = GetSceneCursorInfo(mouse_pos, 1000000);
		GASS::Vec3 drop_pos = cursorInfo.m_3DPos;
		drop_pos.x = SnapPosition(drop_pos.x);
		drop_pos.y = SnapPosition(drop_pos.y);
		drop_pos.z = SnapPosition(drop_pos.z);
		//create rotation?
		GASS::Quaternion rot;
		CreateObjectFromTemplateAtPosition(name,drop_pos,rot);
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

				int from_id = (int) this;

				MessagePtr pos_msg(new GASS::WorldPositionMessage(pos,from_id));
				so->SendImmediate(pos_msg);

				MessagePtr rot_msg(new GASS::WorldRotationMessage(rot,from_id));
				so->SendImmediate(rot_msg);
			}
			else
			{
				//failed to create object
			}
		}
	}
	
}				

