#include "EditorSceneManager.h"
#include "EditorSystem.h"
#include "Core/Utils/GASSLogManager.h"
#include "Core/Utils/GASSException.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/ComponentSystem/GASSComponentContainerTemplateManager.h"
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
	EditorSceneManager::EditorSceneManager() : m_LockTerrainObjects(true)
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
		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(EditorSceneManager::OnPostSceneLoaded,PostSceneLoadEvent,0));
	}

	void EditorSceneManager::OnInit()
	{
		SystemListenerPtr listener = shared_from_this();
		EditorSystemPtr system =  SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystemByClass<EditorSystem>();
		system->Register(listener);
		m_LockTerrainObjects = system->GetLockTerrainObjects();

		ScenePtr scene = GetScene();
		m_MouseTools->Init();
		SetObjectSite(scene->GetRootSceneObject());

		//Load some objects, why???

		GASS::SceneObjectPtr scene_object = scene->LoadObjectFromTemplate("SelectionObject",scene->GetRootSceneObject());
	}

	void EditorSceneManager::OnPostSceneLoaded(PostSceneLoadEventPtr message)
	{
		if(m_LockTerrainObjects) //add static objects
		{
			AddStaticObject(message->GetScene()->GetRootSceneObject(),true);
		}
		SetObjectSite(message->GetScene()->GetRootSceneObject());
	}

	void EditorSceneManager::OnShutdown()
	{

	}

	void EditorSceneManager::SystemTick(double delta_time)
	{
		GetMouseToolController()->Update(delta_time);
		BaseSceneManager::SystemTick(delta_time);
	}

	void EditorSceneManager::CreateCamera()
	{
		ScenePtr scene = GetScene();
		//load top camera
		Vec3 vel(0,0,0);
		Vec3 pos = scene->GetStartPos();
		Quaternion rot(scene->GetStartRot());
		SceneObjectPtr free_obj = scene->LoadObjectFromTemplate("FreeCameraObject",scene->GetRootSceneObject());

		if(!free_obj) //If no FreeCameraObject template found, create one
		{
			SceneObjectTemplatePtr fre_cam_template (new SceneObjectTemplate);
			fre_cam_template->SetName("FreeCameraObject");
			ComponentPtr location_comp (ComponentFactory::Get().Create("LocationComponent"));
			location_comp->SetName("LocationComp");

			ComponentPtr camera_comp (ComponentFactory::Get().Create("CameraComponent"));
			camera_comp->SetName("FreeCameraComp");

			ComponentPtr cc_comp (ComponentFactory::Get().Create("FreeCamControlComponent"));
			cc_comp->SetName("FreeCameraCtrlComp");

			fre_cam_template->AddComponent(location_comp);
			fre_cam_template->AddComponent(camera_comp);
			fre_cam_template->AddComponent(cc_comp);

			SimEngine::Get().GetSceneObjectTemplateManager()->AddTemplate(fre_cam_template);

			free_obj = scene->LoadObjectFromTemplate("FreeCameraObject",scene->GetRootSceneObject());
		}


		if(free_obj)
		{
			free_obj->SendImmediateRequest(PositionRequestPtr(new PositionRequest(scene->GetStartPos())));
			SystemMessagePtr camera_msg(new ChangeCameraRequest(free_obj->GetFirstComponentByClass<ICameraComponent>()));
			SimEngine::Get().GetSimSystemManager()->PostMessage(camera_msg);
		}

		//SceneObjectPtr top_obj = scene->LoadObjectFromTemplate("TopCameraObject",scene->GetRootSceneObject());
		//if(top_obj)
		//	top_obj->SendImmediate(pos_msg);
	}

	void EditorSceneManager::OnCameraChanged(CameraChangedEventPtr message)
	{
		CameraComponentPtr camera = message->GetViewport()->GetCamera();
		SceneObjectPtr cam_obj = DYNAMIC_PTR_CAST<BaseSceneComponent>(camera)->GetSceneObject();

		m_ActiveCameraObject = cam_obj;
		m_ActiveCamera = camera;
	}

	void EditorSceneManager::AddStaticObject(SceneObjectPtr obj, bool rec)
	{
		m_StaticObjects.insert(obj);
		if(rec)
		{
			ComponentContainer::ComponentContainerIterator iter = obj->GetChildren();
			while(iter.hasMoreElements())
			{
				SceneObjectPtr child = STATIC_PTR_CAST<SceneObject>(iter.getNext());
				AddStaticObject(child,rec);
			}
		}
	}

	SceneObjectPtr EditorSceneManager::GetSelectedObject() const
	{
		return SceneObjectPtr(m_SelectedObject,NO_THROW);
	}

	void EditorSceneManager::SelectSceneObject(SceneObjectPtr obj)
	{
		if(obj != GetSelectedObject())
		{
			m_SelectedObject = obj;
			//notify listeners
			int from_id = PTR_TO_INT(this);
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
		return SceneObjectPtr(m_CurrentSite,NO_THROW);
	}

	void EditorSceneManager::MoveCameraToObject(SceneObjectPtr obj)
	{
		SceneObjectPtr cam_obj (m_ActiveCameraObject,NO_THROW);
		CameraComponentPtr cam(m_ActiveCamera,NO_THROW);
		if(cam_obj && cam)
		{
			BaseReflectionObjectPtr cam_props = DYNAMIC_PTR_CAST<BaseReflectionObject>(cam);
			bool ortho_mode = false;

			float object_size = 10;
			Vec3 object_pos(0,0,0);
			GeometryComponentPtr gc = obj->GetFirstComponentByClass<IGeometryComponent>(true);

			if(gc)
			{
				object_size = gc->GetBoundingSphere().m_Radius*4;
				//object_pos = (gc->GetBoundingBox().m_Min + gc->GetBoundingBox().m_Max)*0.5;
				LocationComponentPtr lc = obj->GetFirstComponentByClass<ILocationComponent>();
				if(lc)
					object_pos = lc->GetWorldPosition();

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
				cam_obj->PostRequest(PositionRequestPtr(new PositionRequest(object_pos)));
				CameraParameterRequestPtr zoom_msg(new CameraParameterRequest(CameraParameterRequest::CAMERA_ORTHO_WIN_SIZE,object_size));
				cam_obj->PostRequest(zoom_msg);
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
				rot_mat.SetZAxis(dir);
				Vec3 rvec = Vec3(dir.z,0,-dir.x);
				rvec.Normalize();
				rot_mat.SetXAxis(rvec);
				Vec3 up = Math::Cross(dir,rvec);
				up.Normalize();
				rot_mat.SetYAxis(up);
				cam_rot.FromRotationMatrix(rot_mat);

				cam_obj->PostRequest(RotationRequestPtr(new RotationRequest(cam_rot)));
				cam_obj->PostRequest(PositionRequestPtr(new PositionRequest(object_pos)));
			}
		}
	}
}
