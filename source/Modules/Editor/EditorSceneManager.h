#pragma once
#include "Sim/GASSCommon.h"
#include "Core/Utils/GASSSingleton.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/Messages/GASSCoreSceneMessages.h"
#include "Sim/Messages/GASSGraphicsSystemMessages.h"
#include "Sim/Messages/GASSCoreSystemMessages.h"
#include "Sim/Interface/GASSICameraComponent.h"
#include "Sim/GASSBaseSceneManager.h"
#include "GUISchemaLoader.h"

namespace GASS
{

	enum GizmoEditMode
	{
		GM_LOCAL,
		GM_WORLD,
	};

	
	class MouseToolController;
	class MessageManager;
	typedef GASS_SHARED_PTR<MouseToolController> MouseToolControllerPtr;

	class EditorModuleExport EditorSceneManager :  public Reflection<EditorSceneManager, BaseSceneManager>
	{
	public:
		typedef std::vector<GASS::SceneObjectWeakPtr> SelectionVector;
#ifndef GASS_USE_BOOST_PTR
		typedef std::set<GASS::SceneObjectWeakPtr, std::owner_less<GASS::SceneObjectWeakPtr> > SceneObjectSet;
#else
		typedef std::set<GASS::SceneObjectWeakPtr> SceneObjectSet;
#endif


		EditorSceneManager();
		virtual ~EditorSceneManager(void);
		static  void RegisterReflection();
		virtual void OnCreate();
		virtual void OnInit();
		virtual void OnShutdown();
		virtual bool GetSerialize() const {return false;}
		void SystemTick(double delta_time);

		MouseToolControllerPtr GetMouseToolController() const {return m_MouseTools;}
		void SetSelectedObjects(const SelectionVector &selection);
		SelectionVector GetSelectedObjects() const;
		SceneObjectPtr GetFirstSelectedObject() const;

		void SelectSceneObject(SceneObjectPtr obj);
		
		void UnselectSceneObject(SceneObjectPtr obj);
		void UnselectAllSceneObjects();
		bool IsSelected(SceneObjectPtr obj);

		bool IsObjectLocked(SceneObjectWeakPtr obj);
		void UnlockObject(SceneObjectWeakPtr obj);
		void LockObject(SceneObjectWeakPtr obj);

		bool IsObjectVisible(SceneObjectWeakPtr obj);
		void UnhideObject(SceneObjectWeakPtr obj);
		void HideObject(SceneObjectWeakPtr obj);
		bool IsObjectStatic(SceneObjectWeakPtr obj);
		
		void SetObjectSite(SceneObjectPtr obj);
		SceneObjectPtr GetObjectSite() const;
		void MoveCameraToObject(SceneObjectPtr obj);
		void CreateCamera(const std::string &template_name = "");
		CameraComponentPtr GetActiveCamera() const {return m_ActiveCamera.lock();}
		SceneObjectPtr GetActiveCameraObject() const {return m_ActiveCameraObject.lock();}
		const SceneObjectSet&  GetInvisibleObjects() const { return m_InvisibleObjects; }
	protected:
		void OnPostSceneLoaded(PostSceneLoadEventPtr message);
		void AddStaticObject(SceneObjectPtr obj, bool rec);
		void OnCameraChanged(CameraChangedEventPtr message);
		CameraComponentWeakPtr m_ActiveCamera;
		SceneObjectWeakPtr m_ActiveCameraObject;
		MouseToolControllerPtr m_MouseTools;
		SelectionVector m_SelectedObjects;
		SceneObjectSet m_LockedObjects;
		SceneObjectSet m_InvisibleObjects;
		SceneObjectSet m_StaticObjects;
		bool m_LockTerrainObjects;
		SceneObjectWeakPtr m_CurrentSite;
	};
	typedef GASS_SHARED_PTR<EditorSceneManager> EditorSceneManagerPtr;
}
