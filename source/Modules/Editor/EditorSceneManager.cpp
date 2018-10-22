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
	
	void EditorSceneManager::OnPreSystemUpdate(double delta_time)
	{
		GetMouseToolController()->Update(delta_time);
		_UpdateListeners(delta_time);
	}

	void EditorSceneManager::OnPostSystemUpdate(double delta_time)
	{
		
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
			free_obj->GetFirstComponentByClass<ICameraComponent>()->ShowInViewport();
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

	bool GetObjectPosAndSize(SceneObjectPtr obj, double &object_radius, Vec3 &object_pos)
	{
		GeometryComponentPtr gc = obj->GetFirstComponentByClass<IGeometryComponent>(true);
		if (gc)
		{
			object_radius = gc->GetBoundingSphere().m_Radius;
			LocationComponentPtr lc = obj->GetFirstComponentByClass<ILocationComponent>();
			if (lc)
				object_pos = lc->GetWorldPosition();
			else
				return false;
		}
		else
		{
			LocationComponentPtr lc = obj->GetFirstComponentByClass<ILocationComponent>();
			if (lc)
				object_pos = lc->GetWorldPosition();
			else
				return false;
		}
		return true;
	}

	Quaternion GetGeocentricRotation(const Vec3 &object_pos, EulerRotation rel_rot)
	{
		const Vec3 look_dir = object_pos.NormalizedCopy();
		const double earth_radius = 6371000;
		const Vec3 north = (Vec3(0, earth_radius, 0) - object_pos).NormalizedCopy();
		const Vec3 right_vec = -look_dir.Cross(north);
		const Vec3 z = (-Vec3::Cross(look_dir, right_vec)).NormalizedCopy();
		const Vec3 x = (Vec3::Cross(look_dir, z)).NormalizedCopy();
		Quaternion qrot;
		qrot.FromAxes(x, look_dir, z);
		//add optional tilt
		
		const Quaternion rel_cam_rot = Quaternion::CreateFromEulerYXZ(rel_rot.GetAxisRotation());
		qrot = qrot*rel_cam_rot;
		return qrot;
	}

	void EditorSceneManager::MoveCameraToObject(SceneObjectPtr obj)
	{
		SceneObjectPtr cam_obj = m_ActiveCameraObject.lock();
		if (cam_obj)
		{
			//Check if we have osgEarth manipulator component, then send fly request
			if (cam_obj->GetFirstComponentByClassName("OSGEarthCameraManipulatorComponent"))
			{
				cam_obj->PostRequest(CameraFlyToObjectRequestPtr(new CameraFlyToObjectRequest(obj)));
			}
			else
			{
				double object_radius = 10;
				Vec3 object_pos(0, 0, 0);
				if (GetObjectPosAndSize(obj, object_radius, object_pos))
				{
					//calculate distance from object
					const double dist_to_object = std::max(object_radius * 4, 10.0);
					const bool geocentric = cam_obj->GetScene()->GetGeocentric();
					LocationComponentPtr location = cam_obj->GetFirstComponentByClass<ILocationComponent>();
					if (geocentric)
					{
						const Quaternion camera_rot = GetGeocentricRotation(object_pos, EulerRotation(0, 30, 0));
						const Vec3 camera_pos = object_pos + camera_rot.GetYAxis() * dist_to_object;
						location->SetWorldPosition(camera_pos);
						location->SetWorldRotation(camera_rot);
					}
					else
					{
						//object_radius = std::max(object_radius, 10.0);
						const EulerRotation rel_rot(0, -30, 0);
						const Quaternion camera_rot = Quaternion::CreateFromEulerYXZ(rel_rot.GetAxisRotation());
						const Vec3 camera_pos = object_pos + camera_rot.GetZAxis() * dist_to_object;
						location->SetWorldPosition(camera_pos);
						location->SetWorldRotation(camera_rot);
					}
				}
			}
		}
	}
}
