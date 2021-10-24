#pragma once
#include "Sim/GASSCommon.h"
#include "Core/Utils/GASSSingleton.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/Messages/GASSCoreSceneMessages.h"
#include "Sim/Messages/GASSGraphicsSystemMessages.h"
#include "Sim/Messages/GASSCoreSystemMessages.h"
#include "Sim/Interface/GASSICameraComponent.h"
#include "Sim/GASSBaseSceneManager.h"
#include "Modules/Editor/EditorCommon.h"

namespace GASS
{

	enum GizmoEditMode
	{
		GM_LOCAL,
		GM_WORLD,
	};

	
	class MouseToolController;
	class MessageManager;
	using MouseToolControllerPtr = std::shared_ptr<MouseToolController>;

	class EditorModuleExport EditorSceneManager : public Reflection<EditorSceneManager, BaseSceneManager>
	{
	public:
		using SelectionVector = std::vector<GASS::SceneObjectWeakPtr>;
		using SceneObjectSet = std::set<GASS::SceneObjectWeakPtr, std::owner_less<GASS::SceneObjectWeakPtr>>;

		EditorSceneManager(SceneWeakPtr scene);
		~EditorSceneManager(void) override;
		static  void RegisterReflection();
		void OnPostConstruction() override;
		void OnSceneCreated() override;
		void OnSceneShutdown() override;
		bool GetSerialize() const override {return false;}
		
		void OnUpdate(double delta_time) override;

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
	using EditorSceneManagerPtr = std::shared_ptr<EditorSceneManager>;
}
