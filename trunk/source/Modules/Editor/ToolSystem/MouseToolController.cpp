

#include <boost/bind.hpp>
#include "MouseToolController.h"
#include "Modules/Editor/EditorSystem.h"
#include "IMouseTool.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/ComponentSystem/GASSIComponent.h"
#include "Core/Utils/GASSException.h"
#include "Sim/Scene/GASSScene.h"
#include "Sim/Scene/GASSScene.h"
#include "Sim/Scene/GASSSceneObject.h"

#include "Sim/Systems/Input/GASSIInputSystem.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/Systems/Input/GASSIControlSettingsSystem.h"
#include "Sim/Systems/GASSSimSystemManager.h"
#include "Sim/Components/Graphics/GASSICameraComponent.h"
#include "Sim/Components/GASSBaseSceneComponent.h"
#include "Sim/Components/Graphics/GASSMeshData.h"
#include "Sim/Scene/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Components/Graphics/Geometry/GASSIGeometryComponent.h"
#include "Sim/Components/Graphics/GASSILocationComponent.h"
#include "Sim/Systems/Graphics/GASSIGraphicsSystem.h"


namespace GASS
{
	MouseToolController::MouseToolController(EditorSystem* system): m_ActiveTool(NULL),
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
		m_LastScreenPos(0,0),
		m_ControlSettingName("EditorInputSettings"),
		m_EditorSystem(system)
	{

	}

	MouseToolController::~MouseToolController(void)
	{

	}

	void MouseToolController::Init()
	{
		m_EditorSystem->GetSimSystemManager()->RegisterForMessage(REG_TMESS(MouseToolController::OnFocusChanged,WindowFocusChangedMessage,0));
		m_EditorSystem->GetSimSystemManager()->RegisterForMessage(REG_TMESS(MouseToolController::OnInput,ControllSettingsMessage,0));
		InputSystemPtr input_system = SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystem<IInputSystem>();
		input_system->AddMouseListener(this);
	}

	void MouseToolController::OnFocusChanged(WindowFocusChangedMessagePtr message)
	{
		std::string window = message->GetWindow();
		if(window == "RenderWindow")
		{
			m_Active = message->GetFocus();
		}
	}

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
					name == TID_TERRAIN_DEFORM ||
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
				m_EditorSystem->GetSimSystemManager()->PostMessage(MessagePtr(new ToolChangedMessage(tool_name)));
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
					MessagePtr tool_msg(new ToolChangedMessage(m_Tools[new_tool]->GetName(),id));
					m_EditorSystem->GetSimSystemManager()->PostMessage(tool_msg);
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
					MessagePtr tool_msg(new ToolChangedMessage(m_Tools[new_tool]->GetName(),id));
					m_EditorSystem->GetSimSystemManager()->PostMessage(tool_msg);

					return;
				}
			}
		}
	}


	void MouseToolController::MoveTo(const CursorInfo &info)
	{
		if(m_ActiveTool)
			m_ActiveTool->MoveTo(info);
	}

	void MouseToolController::MouseDown(const CursorInfo &info)
	{
		if(m_ActiveTool)
			m_ActiveTool->MouseDown(info);
	}

	void MouseToolController::MouseUp(const CursorInfo &info)
	{
		if(m_ActiveTool)
			m_ActiveTool->MouseUp(info);
	}



	GASS::CollisionResult MouseToolController::CameraRaycast(CameraComponentPtr cam, const Vec2 &viewport_pos, Float raycast_distance, int col_bits)
	{
		GASS::CollisionResult result;
		result.Coll = false;
		GASS::BaseSceneComponentPtr bsc = boost::shared_dynamic_cast<GASS::BaseSceneComponent>(cam);
		if(cam && bsc)
		{

			//ScenePtr scene = cam_obj->GetScene();
			Vec3 ray_start;
			Vec3 ray_direction;
			cam->GetCameraToViewportRay(viewport_pos.x, viewport_pos.y,ray_start,ray_direction);
			ray_direction = ray_direction*raycast_distance;
			GASS::CollisionSystemPtr col_sys = GASS::SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystem<GASS::ICollisionSystem>();

			if(!col_sys)
				GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Failed to get CollisionSystem", "MouseToolController::CameraRaycast");

			GASS::CollisionRequest request;
			request.LineStart = ray_start;
			request.LineEnd = ray_start + ray_direction;
			request.Type = COL_LINE;
			request.Scene = bsc->GetSceneObject()->GetScene();
			request.ReturnFirstCollisionPoint = false;
			request.CollisionBits = col_bits;
			col_sys->Force(request,result);
		}
		return result;
	}

	CursorInfo MouseToolController::GetCursorInfo(const Vec2 &cursor_pos, Float raycast_distance)
	{
		CursorInfo info;
		info.m_ScreenPos = cursor_pos;
		info.m_Delta.x = info.m_ScreenPos.x - m_LastScreenPos.x;
		info.m_Delta.y = info.m_ScreenPos.y - m_LastScreenPos.y;
		CameraComponentPtr cam = m_EditorSystem->GetActiveCamera();

		//get ray direction
		if(cam)
			cam->GetCameraToViewportRay(cursor_pos.x, cursor_pos.y,info.m_RayStart,info.m_RayDir);

		int flags = GEOMETRY_FLAG_GIZMO;
		GASS::CollisionResult gizmo_result = CameraRaycast(cam, cursor_pos, raycast_distance, flags);

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
			flags = GEOMETRY_FLAG_UNKOWN | GEOMETRY_FLAG_GROUND | GEOMETRY_FLAG_STATIC_OBJECT | GEOMETRY_FLAG_DYNAMIC_OBJECT;
		
			GASS::CollisionResult mesh_result  = CameraRaycast(cam, cursor_pos, raycast_distance, flags);
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
		m_EditorSystem->GetSimSystemManager()->PostMessage(MessagePtr(new GridMessage(m_GridSize,m_GridSpacing)));
	}

	void MouseToolController::SetGridSize(Float value)
	{
		m_GridSize = value;
		m_EditorSystem->GetSimSystemManager()->PostMessage(MessagePtr(new GridMessage(m_GridSize,m_GridSpacing)));
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
		ScenePtr scene = m_EditorSystem->GetScene();;
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
		m_Delta = delta;
		//debug message

		/*std::stringstream ss;
		ss << " Cursor pos:" << m_LastScreenPos << "\n";
		const std::string message = ss.str();
		SimEngine::Get().GetSimSystemManager()->PostMessage(MessagePtr( new DebugPrintMessage(message)));*/

		CursorInfo info = GetCursorInfo(m_LastScreenPos,m_RayPickDistance);
		SceneObjectPtr obj_under_cursor(info.m_ObjectUnderCursor,boost::detail::sp_nothrow_tag());
		if(obj_under_cursor)
		{
			std::stringstream ss;
			ss << " Cursor pos:" << info.m_3DPos << " 2d:" << info.m_ScreenPos << "\n";
			const std::string message = "Object under cursor:" + obj_under_cursor->GetName() + ss.str();
			SimEngine::Get().GetSimSystemManager()->PostMessage(MessagePtr( new DebugPrintMessage(message)));
		}
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
		Vec2 mouse_pos(data.XAbsNorm,data.YAbsNorm);
		CursorInfo info = GetCursorInfo(mouse_pos,m_RayPickDistance);
		MoveTo(info);
		//saved for delta calculation, remove this?
		m_LastScreenPos = mouse_pos;
		int mess_id = (int) this;
		MessagePtr cursor_msg(new CursorMoved3DMessage(mouse_pos,info.m_3DPos, SceneObjectPtr(info.m_ObjectUnderCursor,boost::detail::sp_nothrow_tag()),mess_id));
		m_EditorSystem->GetSimSystemManager()->PostMessage(cursor_msg);
		return true;
	}

	bool MouseToolController::MousePressed(const MouseData &data, MouseButtonId button )
	{
		if(m_Active)
		{
			if(button == MBID_LEFT)
			{
				Vec2 mouse_pos(data.XAbsNorm,data.YAbsNorm);
				CursorInfo info = GetCursorInfo(mouse_pos,m_RayPickDistance);
				MouseDown(info);
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
				CursorInfo info = GetCursorInfo(mouse_pos,m_RayPickDistance);
				MouseUp(info);
				return true;
		}

		else if(button == MBID_RIGHT)
		{
			Vec2 mouse_pos(data.XAbsNorm,data.YAbsNorm);	
			CursorInfo info = GetCursorInfo(mouse_pos,m_RayPickDistance);
			
			//check if cursor at rest
			Vec2 delta;
			delta.x	= m_MBRScreenPos.x - mouse_pos.x;
			delta.y	= m_MBRScreenPos.y - mouse_pos.y;
			if(abs(delta.x) + abs(delta.y) < 0.001)
			{
				SceneObjectPtr selected = m_EditorSystem->GetSelectedObject();
				if(SceneObjectPtr(info.m_ObjectUnderCursor,boost::detail::sp_nothrow_tag()) == selected)
				{
					//show Immediate!
					//Disable OIS input to avoid background selection
					GASS::MessagePtr message(new RequestShowObjectMenuMessage(selected,Vec2(data.XAbs,data.YAbs)));
					m_EditorSystem->GetSimSystemManager()->SendImmediate(message);
				}
			}
			return true;
		}
		return false;
	}

	void MouseToolController::CreateSceneObject(const std::string name, const Vec2 &mouse_pos)
	{
		CursorInfo cursorInfo = GetCursorInfo(mouse_pos, 1000000);
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
		ScenePtr cur_scene = m_EditorSystem->GetScene();
		SceneObjectPtr site = m_EditorSystem->GetObjectSite();
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

