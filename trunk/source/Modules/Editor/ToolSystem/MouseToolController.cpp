

#include <boost/bind.hpp>
#include "MouseToolController.h"
#include "../EditorManager.h"
#include "IMouseTool.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/ComponentSystem/GASSIComponent.h"
#include "Core/Utils/GASSException.h"
#include "Sim/Scene/GASSScene.h"
#include "Sim/Scene/GASSScene.h"
#include "Sim/Scene/GASSSceneObject.h"

#include "Sim/Systems/Input/GASSIInputSystem.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/Systems/Input/GASSControlSettingsManager.h"
#include "Sim/Systems/Input/GASSControlSetting.h"
#include "Sim/Systems/GASSSimSystemManager.h"
#include "Sim/Components/Graphics/GASSICameraComponent.h"
#include "Sim/Components/GASSBaseSceneComponent.h"
#include "Sim/Components/Graphics/GASSMeshData.h"
#include "Sim/Scene/GASSGraphicsSceneObjectMessages.h"



namespace GASS
{
	MouseToolController::MouseToolController(bool scene_objects_selectable): m_SceneObjectsSelectable(scene_objects_selectable),
		m_ActiveTool(NULL),
		m_Active(false),
		m_ColMeshHandle(0),
		m_ColGizmoHandle(0),
		m_GridSpacing(1),
		m_GridSize(10),
		m_SnapMovment(1),
		m_SnapAngle(15),
		m_EnableMovmentSnap(false),
		m_EnableAngleSnap(false),
		m_RayPickDistance(3000),
		m_EnableGizmo(true),
		m_UseTerrainNormalOnDrop(false),
		m_LastScreenPos(0,0)
	{

	}

	MouseToolController::~MouseToolController(void)
	{


	}

	void MouseToolController::Init()
	{
		EditorManager::GetPtr()->GetMessageManager()->RegisterForMessage(REG_TMESS(MouseToolController::OnCursorMoved,CursorMoved2DMessage,0));
		EditorManager::GetPtr()->GetMessageManager()->RegisterForMessage(REG_TMESS(MouseToolController::OnMouseButton,MouseButtonMessage,0));
		EditorManager::GetPtr()->GetMessageManager()->RegisterForMessage(REG_TMESS(MouseToolController::OnToolChanged,ToolChangedMessage,0));
		EditorManager::GetPtr()->GetMessageManager()->RegisterForMessage(REG_TMESS(MouseToolController::OnFocusChanged,WindowFocusChangedMessage,0));
		EditorManager::GetPtr()->GetMessageManager()->RegisterForMessage(REG_TMESS(MouseToolController::OnObjectLock,ObjectLockMessage,0));
		EditorManager::GetPtr()->GetMessageManager()->RegisterForMessage(REG_TMESS(MouseToolController::OnObjectVisible,ObjectVisibleMessage,0));
		EditorManager::GetPtr()->GetMessageManager()->RegisterForMessage(REG_TMESS(MouseToolController::OnSnapSettingsMessage,SnapSettingsMessage,0));
		EditorManager::GetPtr()->GetMessageManager()->RegisterForMessage(REG_TMESS(MouseToolController::OnSnapModeMessage,SnapModeMessage,0));

		//register for input
		m_EditorControlSetting = SimEngine::Get().GetControlSettingsManager()->GetControlSetting("EditorInputSettings");
		if(m_EditorControlSetting)
		{
			m_EditorControlSetting->GetMessageManager()->RegisterForMessage(REG_TMESS(MouseToolController::OnInput,ControllerMessage,0));
		}
		else
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Failed to find EditorInputSettings in ControlSettingsManager","MouseToolController::Init()");

		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(MouseToolController::OnSceneLoaded,SceneLoadedNotifyMessage,0));
		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(MouseToolController::OnNewScene,SceneAboutToLoadNotifyMessage,0));
		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(MouseToolController::OnUnloadScene,SceneUnloadNotifyMessage,0));
	}


	void MouseToolController::OnFocusChanged(WindowFocusChangedMessagePtr message)
	{
		std::string window = message->GetWindow();
		if(window == "RenderWindow")
		{
			m_Active = message->GetFocus();
		}
	}

	void MouseToolController::OnSceneLoaded(SceneLoadedNotifyMessagePtr message)
	{
		ScenePtr scene = message->GetScene();
		IComponentContainer::ComponentContainerIterator iter = scene->GetRootSceneObject()->GetChildren();
		while(iter.hasMoreElements())
		{
			//Lock recursive?
			SceneObjectPtr obj = boost::shared_static_cast<SceneObject>(iter.getNext());
			//EditorManager::GetPtr()->GetMessageManager()->SendImmediate(MessagePtr(new ObjectLockMessage(obj,true)));
			if(!m_SceneObjectsSelectable)
				m_StaticObjects.insert(obj);
		}
		m_Scene = scene;
		//load selection object
		GASS::SceneObjectPtr scene_object = scene->LoadObjectFromTemplate("SelectionObject",scene->GetRootSceneObject());
	}

	void MouseToolController::OnNewScene(GASS::SceneAboutToLoadNotifyMessagePtr message)
	{
		GASS::ScenePtr scene = message->GetScene();
		scene->RegisterForMessage(REG_TMESS(MouseToolController::OnChangeCamera,ChangeCameraMessage,0));

		m_ColMeshHandle = 0;
		m_ColGizmoHandle = 0;

	}

	void MouseToolController::OnUnloadScene(GASS::SceneUnloadNotifyMessagePtr message)
	{
		m_LockedObjects.clear();
		m_StaticObjects.clear();
		m_InvisibleObjects.clear();
		ScenePtr scene = message->GetScene();
		scene->UnregisterForMessage(UNREG_TMESS(MouseToolController::OnChangeCamera,ChangeCameraMessage));

		m_ColMeshHandle = 0;
		m_ColGizmoHandle = 0;
	}

	void MouseToolController::OnChangeCamera(ChangeCameraMessagePtr message)
	{
		SceneObjectPtr cam_obj =  message->GetCamera();
		m_ActiveCameraObject = cam_obj;
		m_ActiveCamera = cam_obj->GetFirstComponentByClass<ICameraComponent>();
	}

	void MouseToolController::OnCursorMoved(CursorMoved2DMessagePtr message)
	{
		//m_CursorInfo.m_ScreenPos = message->GetScreenPosition();
		Vec2 c_pos = message->GetScreenPosition();
		CursorInfo info = GetCursorInfo(c_pos,m_RayPickDistance);
		MoveTo(info);

		m_LastScreenPos = c_pos;

		int mess_id = (int) this;
		MessagePtr cursor_msg(new CursorMoved3DMessage(message->GetScreenPosition(),info.m_3DPos, SceneObjectPtr(info.m_ObjectUnderCursor,boost::detail::sp_nothrow_tag()),mess_id));
		EditorManager::GetPtr()->GetMessageManager()->PostMessage(cursor_msg);

		//if(CheckScenePosition())
		//if(ForceScenePosition())
		//	MoveTo(m_CursorInfo);
	}

	void MouseToolController::OnToolChanged(ToolChangedMessagePtr message)
	{
		std::string new_tool = message->GetTool();
		SelectTool(new_tool);

	}

	void MouseToolController::OnMouseButton(MouseButtonMessagePtr message)
	{
		if(m_Active)
		{
			MouseButtonMessage::MouseButtonId button = message->GetButton();
			bool down = message->IsDown();
			if(button == MouseButtonMessage::MBID_LEFT)
			{
				CursorInfo info = GetCursorInfo(message->GetScreenPosition(),m_RayPickDistance);
				if(down)
					MouseDown(info);
				else 
					MouseUp(info);
			}
		}
	}

	void MouseToolController::OnInput(ControllerMessagePtr message)
	{
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
			else if(name == "MoveTool")
			{
				int id = (int) this;
				MessagePtr tool_msg(new ToolChangedMessage("MoveTool",id));
				EditorManager::GetPtr()->GetMessageManager()->PostMessage(tool_msg);
			}
			else if(name == "VerticalMoveTool")
			{
				int id = (int) this;
				MessagePtr tool_msg(new ToolChangedMessage("VerticalMoveTool",id));
				EditorManager::GetPtr()->GetMessageManager()->PostMessage(tool_msg);
			}
			else if(name == "RotateTool")
			{
				int id = (int) this;
				MessagePtr tool_msg(new ToolChangedMessage("RotateTool",id));
				EditorManager::GetPtr()->GetMessageManager()->PostMessage(tool_msg);
			}
			else if(name == "PaintTool")
			{
				int id = (int) this;
				MessagePtr tool_msg(new ToolChangedMessage("TerrainDeformTool",id));
				EditorManager::GetPtr()->GetMessageManager()->PostMessage(tool_msg);
			}
			else if(name == "TerrainDeformTool")
			{
				int id = (int) this;
				MessagePtr tool_msg(new ToolChangedMessage("TerrainDeformTool",id));
				EditorManager::GetPtr()->GetMessageManager()->PostMessage(tool_msg);
			}
			else if(name == "GoToPositionTool")
			{
				int id = (int) this;
				MessagePtr tool_msg(new ToolChangedMessage("GoToPositionTool",id));
				EditorManager::GetPtr()->GetMessageManager()->PostMessage(tool_msg);
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
					EditorManager::GetPtr()->GetMessageManager()->PostMessage(tool_msg);
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
					EditorManager::GetPtr()->GetMessageManager()->PostMessage(tool_msg);

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

	bool MouseToolController::IsObjectLocked(SceneObjectWeakPtr obj)
	{
		if(m_LockedObjects.end() != m_LockedObjects.find(obj))
		{
			return true;
		}
		return false;
	}

	bool MouseToolController::IsObjectVisible(SceneObjectWeakPtr obj)
	{
		if(m_InvisibleObjects.end() != m_InvisibleObjects.find(obj))
		{
			return false;
		}
		return true;
	}


	bool MouseToolController::IsObjectStatic(SceneObjectWeakPtr obj)
	{
		if(m_StaticObjects.end() != m_StaticObjects.find(obj))
		{
			return true;
		}
		return false;
	}

	void MouseToolController::UnlockObject(SceneObjectWeakPtr obj)
	{
		std::set<GASS::SceneObjectWeakPtr>::iterator iter = m_LockedObjects.find(obj);
		if(m_LockedObjects.end() != iter)
		{
			m_LockedObjects.erase(iter);
		}
	}


	void MouseToolController::LockObject(SceneObjectWeakPtr obj)
	{
		std::set<GASS::SceneObjectWeakPtr>::iterator iter = m_LockedObjects.find(obj);
		if(m_LockedObjects.end() == iter)
		{
			m_LockedObjects.insert(obj);
		}
	}
	void MouseToolController::OnObjectVisible(ObjectVisibleMessagePtr message)
	{
		if(message->GetVisible())
		{
			std::set<GASS::SceneObjectWeakPtr>::iterator iter = m_InvisibleObjects.find(message->GetSceneObject());
			if(m_InvisibleObjects.end() != iter)
			{
				m_InvisibleObjects.erase(iter);
			}
		}
		else
		{
			std::set<GASS::SceneObjectWeakPtr>::iterator iter = m_InvisibleObjects.find(message->GetSceneObject());
			if(m_InvisibleObjects.end() == iter)
			{
				m_InvisibleObjects.insert(message->GetSceneObject());
			}
		}
	}

	void MouseToolController::OnObjectLock(ObjectLockMessagePtr message)
	{
		if(message->GetLock())
		{
			LockObject(message->GetSceneObject());
		}
		else
		{
			UnlockObject(message->GetSceneObject());
		}
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
		CameraComponentPtr cam = GetActiveCamera();

		//save ray direction
		if(cam)
			cam->GetCameraToViewportRay(cursor_pos.x, cursor_pos.y,info.m_RayStart,info.m_RayDir);

		GASS::CollisionResult gizmo_result = CameraRaycast(cam, cursor_pos, raycast_distance, 2);


		int from_id = int(this);

		if(gizmo_result.Coll)
		{
			SceneObjectPtr col_obj(gizmo_result.CollSceneObject,boost::detail::sp_nothrow_tag());
			if(col_obj)
			{
				//MessagePtr cursor_msg(new CursorMoved3DMessage(Vec2(cursor_pos_x,cursor_pos_y),gizmo_result.CollPosition, col_obj,from_id));
				//EditorManager::GetPtr()->GetMessageManager()->PostMessage(cursor_msg);
				info.m_3DPos = gizmo_result.CollPosition;
				info.m_ObjectUnderCursor = gizmo_result.CollSceneObject;
				//std::cout << result.CollPosition << std::endl;
			}
		}
		else
		{
			GASS::CollisionResult mesh_result  = CameraRaycast(cam, cursor_pos, raycast_distance, 1);
			if(mesh_result.Coll)
			{
				SceneObjectPtr col_obj(mesh_result.CollSceneObject,boost::detail::sp_nothrow_tag());
				if(col_obj)
				{
					//MessagePtr cursor_msg(new CursorMoved3DMessage(Vec2(viewport_pos_x,viewport_pos_y),mesh_result.CollPosition, col_obj,from_id));
					//EditorManager::GetPtr()->GetMessageManager()->PostMessage(cursor_msg);
					info.m_3DPos = mesh_result.CollPosition;
					info.m_ObjectUnderCursor = mesh_result.CollSceneObject;
					//std::cout << result.CollPosition << std::endl;
					//if(!m_EnableGizmo)
					//	MoveTo(m_CursorInfo);
				}
			}
		}
		return info;
		//if(m_EnableGizmo)
		//	MoveTo(m_CursorInfo);
	}

	


	void MouseToolController::SetGridSpacing(Float value)
	{
		m_GridSpacing = value;
		EditorManager::Get().GetMessageManager()->PostMessage(MessagePtr(new GridMessage(m_GridSize,m_GridSpacing)));
	}

	void MouseToolController::SetGridSize(Float value)
	{
		m_GridSize = value;
		EditorManager::Get().GetMessageManager()->PostMessage(MessagePtr(new GridMessage(m_GridSize,m_GridSpacing)));
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


	void MouseToolController::OnSnapSettingsMessage(SnapSettingsMessagePtr message)
	{
		m_SnapMovment = message->GetMovementSnap();
		m_SnapAngle = message->GetRotationSnap();

	}

	void MouseToolController::OnSnapModeMessage(SnapModeMessagePtr message)
	{
		m_EnableMovmentSnap = message->MovementSnapEnabled();
		m_EnableAngleSnap = message->RotationSnapEnabled();
	}


	SceneObjectPtr MouseToolController::GetPointerObject()
	{
		SceneObjectPtr pointer(m_PointerObject,boost::detail::sp_nothrow_tag());
		if(!pointer &&  GetScene())
		{
			GASS::SceneObjectPtr scene_object = GetScene()->LoadObjectFromTemplate("PointerObject",GetScene()->GetRootSceneObject());
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
		//fade mouse delta
		//m_CursorInfo.m_Delta.x *= 0.9;
		//m_CursorInfo.m_Delta.y *= 0.9;

		//CheckScenePosition();
		//RequestScenePosition();
		//GetCursorInfo();
		m_Delta = delta;
		//debug message
		/*SceneObjectPtr obj_under_cursor(m_CursorInfo.m_ObjectUnderCursor,boost::detail::sp_nothrow_tag());
		if(obj_under_cursor)
		{
		std::stringstream ss;
		ss << " Cursor pos:" << m_CursorInfo.m_3DPos << " 2d:" << m_CursorInfo.m_ScreenPos << "\n";
		const std::string message = "Object under cursor:" + obj_under_cursor->GetName() + ss.str();
		SimEngine::Get().GetSimSystemManager()->PostMessage(MessagePtr( new DebugPrintMessage(message)));
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
}

