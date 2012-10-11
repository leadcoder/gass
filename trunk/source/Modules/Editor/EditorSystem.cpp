#include "EditorSystem.h"
#include "Core/Utils/GASSLogManager.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/Systems/GASSSimSystemManager.h"
#include "Sim/Components/Graphics/GASSILocationComponent.h"
#include "Sim/Components/Graphics/Geometry/GASSIGeometryComponent.h"
#include "Sim/Scene/GASSGraphicsSceneObjectMessages.h"



#include "ToolSystem/MouseToolController.h"
namespace GASS
{
	EditorSystem::EditorSystem()	: m_GUISettings(new GUISchemaLoader),
		m_SceneObjectsSelectable(false)
	{
		m_MouseTools = MouseToolControllerPtr(new MouseToolController(this));
	}

	EditorSystem::~EditorSystem(void)
	{
		delete m_GUISettings;
		m_LockedObjects.clear();
		m_StaticObjects.clear();
		m_InvisibleObjects.clear();
	}

	void EditorSystem::RegisterReflection()
	{
		SystemFactory::GetPtr()->Register("EditorSystem",new GASS::Creator<EditorSystem, ISystem>);
		RegisterProperty<float>("MouseRayPickDistance", &GASS::EditorSystem::GetRayPickDistance, &GASS::EditorSystem::SetRayPickDistance);
		RegisterProperty<bool>("AutoRotateObjectOnDrop", &GASS::EditorSystem::GetUseTerrainNormalOnDrop, &GASS::EditorSystem::SetUseTerrainNormalOnDrop);
	}

	void EditorSystem::Init()
	{
		m_MouseTools->Init();
		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(EditorSystem::OnSceneLoaded,SceneLoadedNotifyMessage,0));
		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(EditorSystem::OnNewScene,SceneAboutToLoadNotifyMessage,0));
		//Register at rtc
		SimEngine::Get().GetRuntimeController()->Register(shared_from_this(),m_TaskNodeName);
	}

	void EditorSystem::SetRayPickDistance(float value) 
	{
		m_MouseTools->SetRayPickDistance(value);
	}

	float EditorSystem::GetRayPickDistance() const 
	{
		return m_MouseTools->GetRayPickDistance();
	}

	void EditorSystem::SetUseTerrainNormalOnDrop(bool value) 
	{
		m_MouseTools->SetUseTerrainNormalOnDrop(value);
	}

	bool EditorSystem::GetUseTerrainNormalOnDrop() const 
	{
		return m_MouseTools->GetUseTerrainNormalOnDrop();
	}

	void EditorSystem::Update(double delta_time)
	{
		GetMouseToolController()->Update(delta_time);
	}

	void EditorSystem::SetPaths(const FilePath &execution_folder,
							 const FilePath &appdata_folder,
							 const FilePath &mydocuments_folder)
	{
		m_ExecutionFolder = execution_folder;
		m_AppDataFolder = appdata_folder;
		m_MyDocumentsFolder = m_MyDocumentsFolder;
	}

	void EditorSystem::AddStaticObject(SceneObjectPtr obj, bool rec)
	{
		m_StaticObjects.insert(obj);
		if(rec)
		{
			IComponentContainer::ComponentContainerIterator iter = obj->GetChildren();
			while(iter.hasMoreElements())
			{
				SceneObjectPtr child = boost::shared_static_cast<SceneObject>(iter.getNext());
				AddStaticObject(child,rec);
			}
		}
	}

	void EditorSystem::OnSceneLoaded(SceneLoadedNotifyMessagePtr message)
	{
		ScenePtr scene = message->GetScene();
		if(!m_SceneObjectsSelectable) //add static objects
		{
			IComponentContainer::ComponentContainerIterator iter = scene->GetRootSceneObject()->GetChildren();
			while(iter.hasMoreElements())
			{
				//Lock recursive?
				SceneObjectPtr obj = boost::shared_static_cast<SceneObject>(iter.getNext());
				AddStaticObject(obj, true);
			}
		}
		m_Scene = scene;

		//load selection object
		GASS::SceneObjectPtr scene_object = scene->LoadObjectFromTemplate("SelectionObject",scene->GetRootSceneObject());
	}

	void EditorSystem::OnNewScene(GASS::SceneAboutToLoadNotifyMessagePtr message)
	{
		GASS::ScenePtr scene = message->GetScene();
		scene->RegisterForMessage(REG_TMESS(EditorSystem::OnChangeCamera,ChangeCameraMessage,0));
	}

	void EditorSystem::OnChangeCamera(ChangeCameraMessagePtr message)
	{
		SceneObjectPtr cam_obj =  message->GetCamera();
		m_ActiveCameraObject = cam_obj;
		m_ActiveCamera = cam_obj->GetFirstComponentByClass<ICameraComponent>();
	}

	SceneObjectPtr EditorSystem::GetSelectedObject() const
	{
		return SceneObjectPtr(m_SelectedObject,boost::detail::sp_nothrow_tag());
	}

	void EditorSystem::SelectSceneObject(SceneObjectPtr obj)
	{
		if(obj != GetSelectedObject())
		{
			m_SelectedObject = obj;
			//notify listeners
			int from_id = (int) this;
			MessagePtr selection_msg(new ObjectSelectionChangedMessage(obj,from_id));
			GetSimSystemManager()->PostMessage(selection_msg);
		}
	}

	void EditorSystem::UnlockObject(SceneObjectWeakPtr obj)
	{
		std::set<GASS::SceneObjectWeakPtr>::iterator iter = m_LockedObjects.find(obj);
		if(m_LockedObjects.end() != iter)
		{
			m_LockedObjects.erase(iter);
			GetSimSystemManager()->PostMessage(MessagePtr(new ObjectLockChangedMessage(SceneObjectPtr(obj),false)));
		}
	}

	void EditorSystem::LockObject(SceneObjectWeakPtr obj)
	{
		std::set<GASS::SceneObjectWeakPtr>::iterator iter = m_LockedObjects.find(obj);
		if(m_LockedObjects.end() == iter)
		{
			m_LockedObjects.insert(obj);
			GetSimSystemManager()->PostMessage(MessagePtr(new ObjectLockChangedMessage(SceneObjectPtr(obj),true)));
		}
	}

	bool EditorSystem::IsObjectLocked(SceneObjectWeakPtr obj)
	{
		if(m_LockedObjects.end() != m_LockedObjects.find(obj))
		{
			return true;
		}
		return false;
	}

	bool EditorSystem::IsObjectVisible(SceneObjectWeakPtr obj)
	{
		if(m_InvisibleObjects.end() != m_InvisibleObjects.find(obj))
		{
			return false;
		}
		return true;
	}

	void EditorSystem::UnhideObject(SceneObjectWeakPtr obj)
	{
		std::set<GASS::SceneObjectWeakPtr>::iterator iter = m_InvisibleObjects.find(obj);
		if(m_InvisibleObjects.end() != iter)
		{
			m_InvisibleObjects.erase(iter);
			GetSimSystemManager()->PostMessage(MessagePtr(new ObjectVisibilityChangedMessage(SceneObjectPtr(obj),true)));
		}
	}

	void EditorSystem::HideObject(SceneObjectWeakPtr obj)
	{
		std::set<GASS::SceneObjectWeakPtr>::iterator iter = m_InvisibleObjects.find(obj);
		if(m_InvisibleObjects.end() == iter)
		{
			m_InvisibleObjects.insert(obj);
			GetSimSystemManager()->PostMessage(MessagePtr(new ObjectVisibilityChangedMessage(SceneObjectPtr(obj),false)));
		}
	}

	bool EditorSystem::IsObjectStatic(SceneObjectWeakPtr obj)
	{
		if(m_StaticObjects.end() != m_StaticObjects.find(obj))
		{
			return true;
		}
		return false;
	}

	void EditorSystem::SetObjectSite(SceneObjectPtr obj)
	{
		m_CurrentSite = obj;
		GetSimSystemManager()->PostMessage(MessagePtr(new ObjectSiteChangedMessage(obj)));
	}

	SceneObjectPtr EditorSystem::GetObjectSite() const
	{
		return SceneObjectPtr(m_CurrentSite,boost::detail::sp_nothrow_tag());
	}

	void EditorSystem::MoveCameraToObject(SceneObjectPtr obj)	
	{
		LocationComponentPtr lc = obj->GetFirstComponentByClass<ILocationComponent>();
		SceneObjectPtr cam_obj (m_ActiveCameraObject,boost::detail::sp_nothrow_tag());
		CameraComponentPtr cam(m_ActiveCamera,boost::detail::sp_nothrow_tag());
		if(cam_obj && cam && lc)
		{
			BaseReflectionObjectPtr cam_props = boost::shared_dynamic_cast<BaseReflectionObject>(cam);
			bool ortho_mode = false;

			float object_size = 10;
			GeometryComponentPtr gc = obj->GetFirstComponentByClass<IGeometryComponent>();
			if(gc)
			{
				object_size = gc->GetBoundingSphere().m_Radius*4;
			}

			if(cam_props)
			{
				boost::any temp;
				cam_props->GetPropertyByType("Ortho",temp);
				ortho_mode = boost::any_cast<bool>(temp);
			}

			if(ortho_mode)
			{
				Vec3 pos = lc->GetWorldPosition();
				Vec3 cam_pos = cam_obj->GetFirstComponentByClass<ILocationComponent>()->GetWorldPosition();
				pos.y = cam_pos.y;
				cam_obj->PostMessage(MessagePtr(new PositionMessage(pos)));
				MessagePtr zoom_msg(new CameraParameterMessage(CameraParameterMessage::CAMERA_ORTHO_WIN_SIZE,object_size));
				cam_obj->PostMessage(zoom_msg);
			}
			else
			{
				Vec3 pos = lc->GetWorldPosition();
				Vec3 cam_pos = cam_obj->GetFirstComponentByClass<ILocationComponent>()->GetWorldPosition();
				Quaternion cam_rot;
				Vec3 dir = pos - cam_pos;
				dir.Normalize(); 
				dir.y = -0.7;
				dir.Normalize();

				pos = pos - dir*object_size;
				dir = -dir;
				Mat4 rot_mat;
				rot_mat.Identity();
				rot_mat.SetViewDirVector(dir);
				Vec3 rvec = Vec3(dir.z,0,-dir.x);
				rvec.Normalize();
				rot_mat.SetRightVector(rvec);
				Vec3 up = Math::Cross(dir,rvec);
				up.Normalize();
				rot_mat.SetUpVector(up);
				cam_rot.FromRotationMatrix(rot_mat);

				cam_obj->PostMessage(MessagePtr(new RotationMessage(cam_rot)));
				cam_obj->PostMessage(MessagePtr(new PositionMessage(pos)));
			}
		}
	}
}