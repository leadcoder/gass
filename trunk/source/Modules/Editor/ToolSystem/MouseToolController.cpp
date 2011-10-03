

#include <boost/bind.hpp>
#include "MouseToolController.h"
#include "../EditorManager.h"
#include "IMouseTool.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/ComponentSystem/IComponent.h"
#include "Sim/Scenario/Scenario.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Scenario/Scene/SceneObjectManager.h"
#include "Sim/Systems/Input/IInputSystem.h"
#include "Sim/SimEngine.h"
#include "Sim/Systems/Input/ControlSettingsManager.h"
#include "Sim/Systems/Input/ControlSetting.h"
#include "Sim/Systems/SimSystemManager.h"
#include "Sim/Components/Graphics/ICameraComponent.h"
#include "Sim/Components/Graphics/MeshData.h"
#include "Sim/Scenario/Scene/Messages/GraphicsSceneObjectMessages.h"


namespace GASS
{
	MouseToolController::MouseToolController(bool scenario_objects_selectable): m_ScenarioObjectsSelectable(scenario_objects_selectable),
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
		m_UseTerrainNormalOnDrop(false)
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
			Log::Error("Failed to find EditorInputSettings in ControlSettingsManager");

		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(MouseToolController::OnSceneLoaded,ScenarioSceneLoadedNotifyMessage,0));
		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(MouseToolController::OnNewScene,ScenarioSceneAboutToLoadNotifyMessage,0));
		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(MouseToolController::OnUnloadScene,ScenarioSceneUnloadNotifyMessage,0));
	}


	void MouseToolController::OnFocusChanged(WindowFocusChangedMessagePtr message)
	{
		std::string window = message->GetWindow();
		if(window == "RenderWindow")
		{
			m_Active = message->GetFocus();
		}
	}

	void MouseToolController::OnSceneLoaded(ScenarioSceneLoadedNotifyMessagePtr message)
	{
		ScenarioScenePtr scene = message->GetScenarioScene();
		IComponentContainer::ComponentContainerIterator iter = scene->GetObjectManager()->GetSceneRoot()->GetChildren();
		while(iter.hasMoreElements())
		{
			//Lock recursive?
			SceneObjectPtr obj = boost::shared_static_cast<SceneObject>(iter.getNext());
			//EditorManager::GetPtr()->GetMessageManager()->SendImmediate(MessagePtr(new ObjectLockMessage(obj,true)));
			if(!m_ScenarioObjectsSelectable)
				m_StaticObjects.insert(obj);

			
	
		}
		m_Scene = scene;


		//load selection object
		GASS::SceneObjectPtr scene_object = scene->GetObjectManager()->LoadFromTemplate("SelectionObject");
	}

	void MouseToolController::OnNewScene(GASS::ScenarioSceneAboutToLoadNotifyMessagePtr message)
	{
		GASS::ScenarioScenePtr scene = message->GetScenarioScene();
		scene->RegisterForMessage(REG_TMESS(MouseToolController::OnChangeCamera,ChangeCameraMessage,0));

		m_ColMeshHandle = 0;
		m_ColGizmoHandle = 0;
		
	}

	void MouseToolController::OnUnloadScene(GASS::ScenarioSceneUnloadNotifyMessagePtr message)
	{
		m_LockedObjects.clear();
		m_StaticObjects.clear();
		m_InvisibleObjects.clear();
		ScenarioScenePtr scene = message->GetScenarioScene();
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
		m_CursorInfo.m_ScreenPos = message->GetScreenPosition();

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
				if(down)
					MouseDown(m_CursorInfo);
				else 
					MouseUp(m_CursorInfo);
			}
		}
	}

	void MouseToolController::OnInput(ControllerMessagePtr message)
	{
		if(m_Active)
		{
			std::string name = message->GetController();
			float value = message->GetValue();
			
			if(name == "MouseX")
			{
				m_CursorInfo.m_Delta.x = value;
			}
			else if(name == "MouseY")
			{
				m_CursorInfo.m_Delta.y = value;
			}
			else if(name == "ChangeTool")
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
						new_tool = m_Tools.size()-1;

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
			m_ActiveTool->MoveTo(m_CursorInfo);
	}

	void MouseToolController::MouseDown(const CursorInfo &info)
	{
		if(m_ActiveTool)
			m_ActiveTool->MouseDown(m_CursorInfo);
	}

	void MouseToolController::MouseUp(const CursorInfo &info)
	{
		if(m_ActiveTool)
			m_ActiveTool->MouseUp(m_CursorInfo);
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

	void MouseToolController::RequestScenePosition()
	{
		float norm_x = m_CursorInfo.m_ScreenPos.x;
		float norm_y = m_CursorInfo.m_ScreenPos.y;
		SceneObjectPtr cam_obj  = GetActiveCameraObject();
		CameraComponentPtr cam = GetActiveCamera();
		if(cam_obj && cam)
		{
			ScenarioScenePtr scene = cam_obj->GetSceneObjectManager()->GetScenarioScene();
			Vec3 ray_start;
			Vec3 ray_direction;
			cam->GetCameraToViewportRay(norm_x,norm_y,ray_start,ray_direction);
			m_CursorInfo.m_RayDir = ray_direction;
			m_CursorInfo.m_RayStart = ray_start;
			ray_direction = ray_direction*m_RayPickDistance;
			GASS::CollisionSystemPtr col_sys = GASS::SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystem<GASS::ICollisionSystem>();

			GASS::CollisionRequest request;
			request.LineStart = ray_start;
			request.LineEnd = ray_start + ray_direction;
			request.Type = COL_LINE;
			request.Scene = scene;
			request.ReturnFirstCollisionPoint = false;
			request.CollisionBits = 2;

			if(m_ColGizmoHandle == 0)
				m_ColGizmoHandle = col_sys->Request(request);

			request.CollisionBits = 1;

			if(m_ColMeshHandle == 0)
				m_ColMeshHandle = col_sys->Request(request);
		}
	}

	bool MouseToolController::ForceScenePosition()
	{
		float norm_x = m_CursorInfo.m_ScreenPos.x;
		float norm_y = m_CursorInfo.m_ScreenPos.y;

		bool new_pos = false; 
		SceneObjectPtr cam_obj  = GetActiveCameraObject();
		CameraComponentPtr cam = GetActiveCamera();
		if(cam_obj && cam)
		{
			ScenarioScenePtr scene = cam_obj->GetSceneObjectManager()->GetScenarioScene();
			Vec3 ray_start;
			Vec3 ray_direction;
			cam->GetCameraToViewportRay(norm_x,norm_y,ray_start,ray_direction);

			m_CursorInfo.m_RayDir = ray_direction;
			m_CursorInfo.m_RayStart = ray_start;

			ray_direction = ray_direction*m_RayPickDistance;
			GASS::CollisionSystemPtr col_sys = GASS::SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystem<GASS::ICollisionSystem>();

			//Check for gizmo first
			GASS::CollisionRequest request;
			request.LineStart = ray_start;
			request.LineEnd = ray_start + ray_direction;
			request.Type = COL_LINE;
			request.Scene = scene;
			request.ReturnFirstCollisionPoint = false;
			request.CollisionBits = 2;

			GASS::CollisionResult result;
			result.Coll = false;
			if(col_sys)
				col_sys->Force(request,result);

			/*if(result.Coll)
			{
			SceneObjectPtr hit_obj(result.CollSceneObject);
			if(hit_obj)
			{
			GizmoComponentPtr gc = hit_obj->GetFirstComponentByClass<GizmoComponent>();
			if(gc)
			//					gc->HighLight();
			}
			}*/

			if(!result.Coll)
			{
				request.CollisionBits = 1;
				if(col_sys)
					col_sys->Force(request,result);
			}

			if(result.Coll)
			{
				float norm_x = m_CursorInfo.m_ScreenPos.x;
				float norm_y = m_CursorInfo.m_ScreenPos.y;
				int from_id = (int)this;
				Vec2 screen_pos(norm_x,norm_y);
				MessagePtr cursor_msg(new CursorMoved3DMessage(screen_pos,result.CollPosition, SceneObjectPtr(result.CollSceneObject),from_id));
				EditorManager::GetPtr()->GetMessageManager()->PostMessage(cursor_msg);

				m_CursorInfo.m_3DPos = result.CollPosition;
				m_CursorInfo.m_ObjectUnderCursor = result.CollSceneObject;

				new_pos= true;

				/*SceneObjectPtr pointer = GetPointerObject();
				if(pointer)
				pointer->PostMessage(MessagePtr(new WorldPositionMessage(result.CollPosition)));*/
			}

		}
		return new_pos;
	}


	bool MouseToolController::CheckScenePosition()
	{
		GASS::CollisionResult gizmo_result;
		GASS::CollisionResult mesh_result;
		gizmo_result.Coll = false;
		mesh_result.Coll = false;
		if(m_ColGizmoHandle)
		{
			GASS::CollisionSystemPtr col_sys = GASS::SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystem<GASS::ICollisionSystem>();
			if(col_sys->Check(m_ColGizmoHandle,gizmo_result))
			{
				m_ColGizmoHandle = 0;
			}
		}

		if(m_ColMeshHandle)
		{
			GASS::CollisionSystemPtr col_sys = GASS::SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystem<GASS::ICollisionSystem>();
			if(col_sys->Check(m_ColMeshHandle,mesh_result))
			{
				m_ColMeshHandle = 0;
			}
		}

		float norm_x = m_CursorInfo.m_ScreenPos.x;
		float norm_y = m_CursorInfo.m_ScreenPos.y;
		int from_id = (int)this;
		Vec2 screen_pos(norm_x,norm_y);

		if(gizmo_result.Coll)
		{
			
			SceneObjectPtr col_obj(gizmo_result.CollSceneObject,boost::detail::sp_nothrow_tag());
			if(col_obj)
			{
				MessagePtr cursor_msg(new CursorMoved3DMessage(screen_pos,gizmo_result.CollPosition, col_obj,from_id));
				EditorManager::GetPtr()->GetMessageManager()->PostMessage(cursor_msg);
				m_CursorInfo.m_3DPos = gizmo_result.CollPosition;
				m_CursorInfo.m_ObjectUnderCursor = gizmo_result.CollSceneObject;
				//std::cout << result.CollPosition << std::endl;
				//MoveTo(m_CursorInfo);
			}
		}
		else if(mesh_result.Coll)
		{
			SceneObjectPtr col_obj(mesh_result.CollSceneObject,boost::detail::sp_nothrow_tag());
			if(col_obj)
			{
				MessagePtr cursor_msg(new CursorMoved3DMessage(screen_pos,mesh_result.CollPosition, col_obj,from_id));
				EditorManager::GetPtr()->GetMessageManager()->PostMessage(cursor_msg);
				m_CursorInfo.m_3DPos = mesh_result.CollPosition;
				m_CursorInfo.m_ObjectUnderCursor = mesh_result.CollSceneObject;
				//std::cout << result.CollPosition << std::endl;
				if(!m_EnableGizmo)
					MoveTo(m_CursorInfo);
			}
		}

		if(m_EnableGizmo)
			MoveTo(m_CursorInfo);
		//SceneObjectPtr obj_under_cursor(m_CursorInfo.m_ObjectUnderCursor,boost::detail::sp_nothrow_tag());
		//	if(obj_under_cursor)
		//		std::cout << obj_under_cursor->GetName() << std::endl;
		return gizmo_result.Coll || mesh_result.Coll;
	}


	void MouseToolController::SetGridSpacing(Float value)
	{
		m_GridSpacing = value;
		EditorManager::Get().GetMessageManager()->PostMessage(MessagePtr(new GridMessage(m_GridSize,m_GridSpacing)));
	}

	void MouseToolController::SetGridSize(Float value)
	{
		m_GridSize= value;
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
			GASS::SceneObjectPtr scene_object = GetScene()->GetObjectManager()->LoadFromTemplate("PointerObject");
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
		m_CursorInfo.m_Delta.x *= 0.9;
		m_CursorInfo.m_Delta.y *= 0.9;

		CheckScenePosition();
		RequestScenePosition();



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

