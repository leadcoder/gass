#include "EditorSceneManager.h"
#include "EditorSystem.h"
#include "Core/Utils/GASSException.h"
#include "Core/Math/GASSMath.h"
#include "Core/MessageSystem/GASSMessageManager.h"

#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"
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
		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(EditorSceneManager::OnCameraChanged, CameraChangedEvent, 0));
		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(EditorSceneManager::OnPostSceneLoaded, PostSceneLoadEvent, 0));
	}

	void EditorSceneManager::OnInit()
	{
		SystemListenerPtr listener = shared_from_this();
		EditorSystemPtr system = SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystemByClass<EditorSystem>();
		system->Register(listener);
		m_LockTerrainObjects = system->GetLockTerrainObjects();

		ScenePtr scene = GetScene();
		m_MouseTools->Init();
		SetObjectSite(scene->GetRootSceneObject());

		//Load some objects, why???

		GASS::SceneObjectPtr scene_object = scene->LoadObjectFromTemplate("SelectionObject", scene->GetRootSceneObject());
	}

	void EditorSceneManager::OnPostSceneLoaded(PostSceneLoadEventPtr message)
	{
		if (m_LockTerrainObjects) //add static objects
		{
			AddStaticObject(message->GetScene()->GetRootSceneObject(), true);
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

	void EditorSceneManager::CreateCamera(const std::string &template_name)
	{
		std::string ctn = template_name;
		ScenePtr scene = GetScene();
		
		//Vec3 pos = scene->GetStartPos();
		Quaternion rot = scene->GetStartRot().GetQuaternion();
		EditorSystemPtr system = SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystemByClass<EditorSystem>();
		if (ctn == "")
			ctn = system->GetDefaultCameraTemplate();
		
		SceneObjectPtr free_obj = scene->LoadObjectFromTemplate(ctn, scene->GetRootSceneObject());
		
		if (!free_obj)
		{
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Failed to find camera template named:" + ctn, "EditorSceneManager::CreateCamera");
		}

		if (free_obj)
		{
			free_obj->SendImmediateRequest(PositionRequestPtr(new PositionRequest(scene->GetStartPos())));
			free_obj->SendImmediateRequest(RotationRequestPtr(new RotationRequest(rot)));
			SystemMessagePtr camera_msg(new ChangeCameraRequest(free_obj->GetFirstComponentByClass<ICameraComponent>()));
			SimEngine::Get().GetSimSystemManager()->PostMessage(camera_msg);
		}
	}

	void EditorSceneManager::OnCameraChanged(CameraChangedEventPtr message)
	{
		CameraComponentPtr camera = message->GetViewport()->GetCamera();
		SceneObjectPtr cam_obj = GASS_DYNAMIC_PTR_CAST<BaseSceneComponent>(camera)->GetSceneObject();

		m_ActiveCameraObject = cam_obj;
		m_ActiveCamera = camera;
	}

	void EditorSceneManager::AddStaticObject(SceneObjectPtr obj, bool rec)
	{
		m_StaticObjects.insert(obj);
		if (rec)
		{
			ComponentContainer::ComponentContainerIterator iter = obj->GetChildren();
			while (iter.hasMoreElements())
			{
				SceneObjectPtr child = GASS_STATIC_PTR_CAST<SceneObject>(iter.getNext());
				AddStaticObject(child, rec);
			}
		}
	}

	std::vector<SceneObjectWeakPtr> EditorSceneManager::GetSelectedObjects() const
	{
		return m_SelectedObjects;
	}

	void EditorSceneManager::SetSelectedObjects(const SelectionVector &selection)
	{
		m_SelectedObjects = selection;
		GetScene()->PostMessage(SceneMessagePtr(new EditorSelectionChangedEvent(m_SelectedObjects, GASS_PTR_TO_INT(this))));
	}

	SceneObjectPtr EditorSceneManager::GetFirstSelectedObject() const
	{
		if(m_SelectedObjects.size() > 0)
			return m_SelectedObjects[0].lock();
		else
			return SceneObjectPtr();
	}

	
	bool EditorSceneManager::IsSelected(SceneObjectPtr obj)
	{
		std::vector<SceneObjectWeakPtr>::iterator iter = m_SelectedObjects.begin();
		while(iter != m_SelectedObjects.end())
		{
			SceneObjectPtr so = (*iter).lock();
			if(so == obj)
			{
				return true;
			}
			else
				++iter;
		}
		return false;
	}

	void EditorSceneManager::UnselectSceneObject(SceneObjectPtr obj)
	{
		std::vector<SceneObjectWeakPtr>::iterator iter = m_SelectedObjects.begin();
		while(iter != m_SelectedObjects.end())
		{
			SceneObjectPtr so = (*iter).lock();
			if(so == obj)
			{
				iter = 	m_SelectedObjects.erase(iter);
				GetScene()->PostMessage(SceneMessagePtr(new EditorSelectionChangedEvent(m_SelectedObjects, GASS_PTR_TO_INT(this))));
			}
			else
				++iter;
		}
	}

	

	void EditorSceneManager::UnselectAllSceneObjects()
	{
		m_SelectedObjects.clear();
		GetScene()->PostMessage(SceneMessagePtr(new EditorSelectionChangedEvent(m_SelectedObjects, GASS_PTR_TO_INT(this))));
	}

	void EditorSceneManager::SelectSceneObject(SceneObjectPtr obj)
	{
		if (!IsSelected(obj))
		{
			m_SelectedObjects.push_back(obj);
			//notify listeners
			GetScene()->PostMessage(SceneMessagePtr(new EditorSelectionChangedEvent(m_SelectedObjects, GASS_PTR_TO_INT(this))));
		}
	}

	void EditorSceneManager::UnlockObject(SceneObjectWeakPtr obj)
	{
		SceneObjectSet::iterator iter = m_LockedObjects.find(obj);
		if (m_LockedObjects.end() != iter)
		{
			m_LockedObjects.erase(iter);
			GetScene()->PostMessage(SceneMessagePtr(new ObjectLockChangedEvent(SceneObjectPtr(obj), false)));
		}
	}

	void EditorSceneManager::LockObject(SceneObjectWeakPtr obj)
	{
		SceneObjectSet::iterator iter = m_LockedObjects.find(obj);
		if (m_LockedObjects.end() == iter)
		{
			m_LockedObjects.insert(obj);
			GetScene()->PostMessage(SceneMessagePtr(new ObjectLockChangedEvent(SceneObjectPtr(obj), true)));
		}
	}

	bool EditorSceneManager::IsObjectLocked(SceneObjectWeakPtr obj)
	{
		if (m_LockedObjects.end() != m_LockedObjects.find(obj))
		{
			return true;
		}
		return false;
	}

	bool EditorSceneManager::IsObjectVisible(SceneObjectWeakPtr obj)
	{
		if (m_InvisibleObjects.end() != m_InvisibleObjects.find(obj))
		{
			return false;
		}
		return true;
	}

	void EditorSceneManager::UnhideObject(SceneObjectWeakPtr obj)
	{
		SceneObjectSet::iterator iter = m_InvisibleObjects.find(obj);
		if (m_InvisibleObjects.end() != iter)
		{
			m_InvisibleObjects.erase(iter);
			ScenePtr(m_Scene)->PostMessage(SceneMessagePtr(new ObjectVisibilityChangedEvent(SceneObjectPtr(obj), true)));
		}
	}

	void EditorSceneManager::HideObject(SceneObjectWeakPtr obj)
	{
		SceneObjectSet::iterator iter = m_InvisibleObjects.find(obj);
		if (m_InvisibleObjects.end() == iter)
		{
			m_InvisibleObjects.insert(obj);
			GetScene()->PostMessage(SceneMessagePtr(new ObjectVisibilityChangedEvent(SceneObjectPtr(obj), false)));
		}
	}

	bool EditorSceneManager::IsObjectStatic(SceneObjectWeakPtr obj)
	{
		//return false;

		if (m_StaticObjects.end() != m_StaticObjects.find(obj))
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
		return m_CurrentSite.lock();
	}

	void EditorSceneManager::MoveCameraToObject(SceneObjectPtr obj)
	{
		SceneObjectPtr cam_obj = m_ActiveCameraObject.lock();
		CameraComponentPtr cam = m_ActiveCamera.lock();
		if (cam_obj && cam)
		{
			BaseReflectionObjectPtr cam_props = GASS_DYNAMIC_PTR_CAST<BaseReflectionObject>(cam);
			bool ortho_mode = false;

			float object_size = 10;
			Vec3 object_pos(0, 0, 0);
			GeometryComponentPtr gc = obj->GetFirstComponentByClass<IGeometryComponent>(true);

			if (gc)
			{
				object_size = static_cast<float>(gc->GetBoundingSphere().m_Radius * 4);
				//object_pos = (gc->GetBoundingBox().m_Min + gc->GetBoundingBox().m_Max)*0.5;
				LocationComponentPtr lc = obj->GetFirstComponentByClass<ILocationComponent>();
				if (lc)
					object_pos = lc->GetWorldPosition();

				//LogManager::getSingleton().stream() << "zoom box min" << gc->GetBoundingBox().m_Min;
				//LogManager::getSingleton().stream() << "zoom box max" << gc->GetBoundingBox().m_Max;

			}
			else
			{
				LocationComponentPtr lc = obj->GetFirstComponentByClass<ILocationComponent>();
				if (lc)
					object_pos = lc->GetWorldPosition();
				else
					return;
			}

			if (cam_props)
			{
				cam_props->GetPropertyValue<bool>("Ortho", ortho_mode);
			}

			if (ortho_mode)
			{
				//Vec3 pos = lc->GetWorldPosition();
				Vec3 cam_pos = cam_obj->GetFirstComponentByClass<ILocationComponent>()->GetWorldPosition();
				object_pos.y = cam_pos.y;
				cam_obj->PostRequest(PositionRequestPtr(new PositionRequest(object_pos)));
				CameraParameterRequestPtr zoom_msg(new CameraParameterRequest(CameraParameterRequest::CAMERA_ORTHO_WIN_SIZE, object_size));
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
				rot_mat.MakeIdentity();
				rot_mat.SetZAxis(dir);
				Vec3 rvec = Vec3(dir.z, 0, -dir.x);
				rvec.Normalize();
				rot_mat.SetXAxis(rvec);
				Vec3 up = Vec3::Cross(dir, rvec);
				up.Normalize();
				rot_mat.SetYAxis(up);
				cam_rot.FromRotationMatrix(rot_mat);

				cam_obj->PostRequest(RotationRequestPtr(new RotationRequest(cam_rot)));
				cam_obj->PostRequest(PositionRequestPtr(new PositionRequest(object_pos)));
			}
		}
	}
}
