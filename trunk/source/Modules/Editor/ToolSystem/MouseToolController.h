#pragma once

#include "../EditorCommon.h"
#include "../EditorMessages.h"
#include "Sim/Systems/Collision/ICollisionSystem.h"
#include "Sim/Scenario/Scene/ScenarioSceneMessages.h"
#include "Sim/Systems/SimSystemMessages.h"
#include "Sim/Systems/Input/ControlSetting.h"
#include "CursorInfo.h"
#include "IMouseTool.h"
#include <string>
#include <vector>
#include <set>

namespace GASS
{
	class ControlSetting;
	class SceneObject;
	class ScenarioScene;
	class ICameraComponent;
	typedef boost::weak_ptr<SceneObject> SceneObjectWeakPtr;
	typedef boost::shared_ptr<SceneObject> SceneObjectPtr;
	typedef boost::weak_ptr<ICameraComponent> CameraComponentWeakPtr;
	typedef boost::shared_ptr<ICameraComponent> CameraComponentPtr;


	class EditorModuleExport MouseToolController 
	{
		friend class IMouseTool;
	public:

		MouseToolController(bool scenario_objects_selectable);
		virtual ~MouseToolController(void);
		void AddTool(IMouseTool* tool);
		bool SelectTool(const std::string &tool_name);
		void MoveTo(const CursorInfo &info);
		void MouseDown(const CursorInfo &info);
		void MouseUp(const CursorInfo &info);
		bool IsObjectLocked(GASS::SceneObjectWeakPtr obj);
		bool IsObjectStatic(GASS::SceneObjectWeakPtr obj);
		bool IsObjectVisible(SceneObjectWeakPtr obj);
		void UnlockObject(SceneObjectWeakPtr obj);
		void LockObject(SceneObjectWeakPtr obj);
		GASS::ScenarioScenePtr GetScene(){return GASS::ScenarioScenePtr(m_Scene,boost::detail::sp_nothrow_tag());}
		void SetScenarioObjectsSelectable(bool value) {m_ScenarioObjectsSelectable = value;}
		bool GetScenarioObjectsSelectable() const {return m_ScenarioObjectsSelectable;}
		GASS::SceneObjectPtr GetActiveCameraObject() const {return GASS::SceneObjectPtr(m_ActiveCameraObject,boost::detail::sp_nothrow_tag());}

		void SetGridSpacing(Float value); 
		void SetGridSize(Float value); 
		void SetSnapMovment(Float value);
		void SetSnapAngle(Float value);

		Float GetGridSpacing() const {return m_GridSpacing;}
		Float GetGridSize() const {return m_GridSize;}
		Float GetSnapMovment() const {return m_SnapMovment;}
		Float GetSnapAngle() const {return m_SnapAngle;}
		int GetEnableGizmo() const {return m_EnableGizmo;}
		void SetEnableGizmo(int value);
		Float SnapPosition(Float value);
		Float SnapAngle(Float value);
		void Update();
		
		void NextTool();
		void PrevTool();
		void SetActive(bool value) {m_Active = value;}


	private:
		void RequestScenePosition();
		bool CheckScenePosition();
		bool ForceScenePosition();
		void OnFocusChanged(WindowFocusChangedMessagePtr message);
		void OnCursorMoved(CursorMoved2DMessagePtr message);
		void OnToolChanged(ToolChangedMessagePtr message);
		void OnMouseButton(MouseButtonMessagePtr message);
		void OnObjectLock(ObjectLockMessagePtr message);
		void OnObjectVisible(ObjectVisibleMessagePtr message);

		void OnNewScene(GASS::ScenarioSceneAboutToLoadNotifyMessagePtr message);
		void OnUnloadScene(GASS::ScenarioSceneUnloadNotifyMessagePtr message);
		void OnChangeCamera(GASS::ChangeCameraMessagePtr message);
		void OnSceneLoaded(GASS::ScenarioSceneLoadedNotifyMessagePtr message);
		void OnSnapSettingsMessage(SnapSettingsMessagePtr message);
		void OnSnapModeMessage(SnapModeMessagePtr message);


		SceneObjectPtr GetPointerObject();


		void OnInput(GASS::ControllerMessagePtr message);
		GASS::CameraComponentPtr GetActiveCamera() const {return GASS::CameraComponentPtr(m_ActiveCamera,boost::detail::sp_nothrow_tag());}
		//GASS::SceneObjectPtr GetActiveCameraObject() const {return GASS::SceneObjectPtr(m_ActiveCameraObject,boost::detail::sp_nothrow_tag());}
		std::vector<IMouseTool*> m_Tools;
		IMouseTool* m_ActiveTool;
		CursorInfo m_CursorInfo;
		GASS::ControlSetting* m_EditorControlSetting;
		std::set<GASS::SceneObjectWeakPtr> m_LockedObjects;
		std::set<GASS::SceneObjectWeakPtr> m_InvisibleObjects;
		std::set<GASS::SceneObjectWeakPtr> m_StaticObjects;

		GASS::CameraComponentWeakPtr m_ActiveCamera;
		GASS::SceneObjectWeakPtr m_ActiveCameraObject;
		bool m_Active;
		GASS::ScenarioSceneWeakPtr m_Scene;
		GASS::CollisionHandle m_ColMeshHandle;
		GASS::CollisionHandle m_ColGizmoHandle;
		bool m_ScenarioObjectsSelectable;

		//Global settings
		Float m_GridSpacing;
		Float m_GridSize;
		Float m_SnapMovment;
		Float m_SnapAngle;

		bool m_EnableMovmentSnap;
		bool m_EnableAngleSnap;
		float m_RayPickDistance;
		bool m_EnableGizmo;

		GASS::SceneObjectWeakPtr m_PointerObject;
	};
}

