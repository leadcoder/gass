#include "EditorSceneManager.h"
#include "EditorSystem.h"
#include "Core/Utils/GASSLogManager.h"
#include "Core/Utils/GASSException.h"

#include "Core/MessageSystem/GASSMessageManager.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/GASSSceneManagerFactory.h"
#include "Sim/Interface/GASSILocationComponent.h"
#include "Sim/Interface/GASSIGeometryComponent.h"
#include "Sim/Interface/GASSIViewport.h"
#include "Sim/Interface/GASSICameraComponent.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "ToolSystem/MouseToolController.h"

namespace GASS
{
	EditorSceneManager::EditorSceneManager() : m_SceneObjectsSelectable(false)
	{
		m_MouseTools = MouseToolControllerPtr(new MouseToolController(this));
	}

	EditorSceneManager::~EditorSceneManager(void)
	{
		m_LockedObjects.clear();
		m_StaticObjects.clear();
		m_InvisibleObjects.clear();
	}

	void EditorSceneManager::RegisterReflection()
	{

	}

	void EditorSceneManager::OnCreate()
	{
		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(EditorSceneManager::OnCameraChanged,CameraChangedEvent,0));
		GetScene()->RegisterForMessage(REG_TMESS(EditorSceneManager::OnLoad ,LoadSceneManagersRequest,2));
	}

	void EditorSceneManager::OnLoad(LoadSceneManagersRequestPtr message)
	{
		ScenePtr scene = GetScene();
		m_MouseTools->Init();
		SystemListenerPtr listener = shared_from_this();
		EditorSystemPtr system =  SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystem<EditorSystem>();
		system->Register(listener);

		
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
		SetObjectSite(scene->GetRootSceneObject());
		GASS::SceneObjectPtr scene_object = scene->LoadObjectFromTemplate("SelectionObject",scene->GetRootSceneObject());
	}

	void EditorSceneManager::SystemTick(double delta_time)
	{
		GetMouseToolController()->Update(delta_time);
		BaseSceneManager::SystemTick(delta_time);
	}
	
	void EditorSceneManager::OnCameraChanged(CameraChangedEventPtr message)
	{
		CameraComponentPtr camera = message->GetViewport()->GetCamera();
		SceneObjectPtr cam_obj = boost::shared_dynamic_cast<BaseSceneComponent>(camera)->GetSceneObject();

		m_ActiveCameraObject = cam_obj;
		m_ActiveCamera = camera;
	}

	void EditorSceneManager::AddStaticObject(SceneObjectPtr obj, bool rec)
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

	SceneObjectPtr EditorSceneManager::GetSelectedObject() const
	{
		return SceneObjectPtr(m_SelectedObject,boost::detail::sp_nothrow_tag());
	}

	void EditorSceneManager::SelectSceneObject(SceneObjectPtr obj)
	{
		if(obj != GetSelectedObject())
		{
			m_SelectedObject = obj;
			//notify listeners
			int from_id = (int) this;
			SceneMessagePtr selection_msg(new ObjectSelectionChangedEvent(obj,from_id));
			GetScene()->PostMessage(selection_msg);
		}
	}

	void EditorSceneManager::UnlockObject(SceneObjectWeakPtr obj)
	{
		std::set<GASS::SceneObjectWeakPtr>::iterator iter = m_LockedObjects.find(obj);
		if(m_LockedObjects.end() != iter)
		{
			m_LockedObjects.erase(iter);
			GetScene()->PostMessage(SceneMessagePtr(new ObjectLockChangedEvent(SceneObjectPtr(obj),false)));
		}
	}

	void EditorSceneManager::LockObject(SceneObjectWeakPtr obj)
	{
		std::set<GASS::SceneObjectWeakPtr>::iterator iter = m_LockedObjects.find(obj);
		if(m_LockedObjects.end() == iter)
		{
			m_LockedObjects.insert(obj);
			GetScene()->PostMessage(SceneMessagePtr(new ObjectLockChangedEvent(SceneObjectPtr(obj),true)));
		}
	}

	bool EditorSceneManager::IsObjectLocked(SceneObjectWeakPtr obj)
	{
		if(m_LockedObjects.end() != m_LockedObjects.find(obj))
		{
			return true;
		}
		return false;
	}

	bool EditorSceneManager::IsObjectVisible(SceneObjectWeakPtr obj)
	{
		if(m_InvisibleObjects.end() != m_InvisibleObjects.find(obj))
		{
			return false;
		}
		return true;
	}

	void EditorSceneManager::UnhideObject(SceneObjectWeakPtr obj)
	{
		std::set<GASS::SceneObjectWeakPtr>::iterator iter = m_InvisibleObjects.find(obj);
		if(m_InvisibleObjects.end() != iter)
		{
			m_InvisibleObjects.erase(iter);
			ScenePtr(m_Scene)->PostMessage(SceneMessagePtr(new ObjectVisibilityChangedEvent(SceneObjectPtr(obj),true)));
		}
	}

	void EditorSceneManager::HideObject(SceneObjectWeakPtr obj)
	{
		std::set<GASS::SceneObjectWeakPtr>::iterator iter = m_InvisibleObjects.find(obj);
		if(m_InvisibleObjects.end() == iter)
		{
			m_InvisibleObjects.insert(obj);
			GetScene()->PostMessage(SceneMessagePtr(new ObjectVisibilityChangedEvent(SceneObjectPtr(obj),false)));
		}
	}

	bool EditorSceneManager::IsObjectStatic(SceneObjectWeakPtr obj)
	{
		if(m_StaticObjects.end() != m_StaticObjects.find(obj))
		{
			return true;
		}
		return false;
	}

	void EditorSceneManager::SetObjectSite(SceneObjectPtr obj)
	{
		m_CurrentSite = obj;
		GetScene()->PostMessage(SceneMessagePtr(new ObjectSiteChangedEvent(obj)));
	}

	SceneObjectPtr EditorSceneManager::GetObjectSite() const
	{
		return SceneObjectPtr(m_CurrentSite,boost::detail::sp_nothrow_tag());
	}

	void EditorSceneManager::MoveCameraToObject(SceneObjectPtr obj)	
	{
		SceneObjectPtr cam_obj (m_ActiveCameraObject,boost::detail::sp_nothrow_tag());
		CameraComponentPtr cam(m_ActiveCamera,boost::detail::sp_nothrow_tag());
		if(cam_obj && cam)
		{
			BaseReflectionObjectPtr cam_props = boost::shared_dynamic_cast<BaseReflectionObject>(cam);
			bool ortho_mode = false;

			float object_size = 10;
			Vec3 object_pos(0,0,0);
			GeometryComponentPtr gc = obj->GetFirstComponentByClass<IGeometryComponent>(true);
			
			if(gc)
			{
				object_size = gc->GetBoundingSphere().m_Radius*4;
				object_pos = (gc->GetBoundingBox().m_Min + gc->GetBoundingBox().m_Max)*0.5;

				//LogManager::getSingleton().stream() << "zoom box min" << gc->GetBoundingBox().m_Min;	
				//LogManager::getSingleton().stream() << "zoom box max" << gc->GetBoundingBox().m_Max;	

			}
			else
			{
				LocationComponentPtr lc = obj->GetFirstComponentByClass<ILocationComponent>();
				if(lc)
					object_pos = lc->GetWorldPosition();
				else 
					return;
			}

			if(cam_props)
			{
				boost::any temp;
				cam_props->GetPropertyByType("Ortho",temp);
				ortho_mode = boost::any_cast<bool>(temp);
			}

			if(ortho_mode)
			{
				//Vec3 pos = lc->GetWorldPosition();
				Vec3 cam_pos = cam_obj->GetFirstComponentByClass<ILocationComponent>()->GetWorldPosition();
				object_pos.y = cam_pos.y;
				cam_obj->PostMessage(MessagePtr(new PositionMessage(object_pos)));
				MessagePtr zoom_msg(new CameraParameterMessage(CameraParameterMessage::CAMERA_ORTHO_WIN_SIZE,object_size));
				cam_obj->PostMessage(zoom_msg);
			}
			else
			{
				
				Vec3 cam_pos = cam_obj->GetFirstComponentByClass<ILocationComponent>()->GetWorldPosition();
				Quaternion cam_rot;
				Vec3 dir = object_pos - cam_pos;
				dir.Normalize(); 
				dir.y = -0.7;
				dir.Normalize();

				object_pos = object_pos - dir*object_size;
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
				cam_obj->PostMessage(MessagePtr(new PositionMessage(object_pos)));
			}
		}
	}
}