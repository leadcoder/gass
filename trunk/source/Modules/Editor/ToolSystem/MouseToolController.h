#pragma once

#include "../EditorCommon.h"
#include "../EditorMessages.h"
#include "Sim/Systems/Collision/GASSICollisionSystem.h"
#include "Sim/Scene/GASSCoreSceneMessages.h"
#include "Sim/Scene/GASSGraphicsSceneMessages.h"
#include "Sim/Systems/Messages/GASSCoreSystemMessages.h"
#include "Sim/Systems/Input/GASSControlSetting.h"
#include "CursorInfo.h"
#include "IMouseTool.h"
#include <string>
#include <vector>
#include <set>

namespace GASS
{
	class ControlSetting;
	class SceneObject;
	class Scene;
	class ICameraComponent;
	typedef boost::weak_ptr<SceneObject> SceneObjectWeakPtr;
	typedef boost::shared_ptr<SceneObject> SceneObjectPtr;
	typedef boost::weak_ptr<ICameraComponent> CameraComponentWeakPtr;
	typedef boost::shared_ptr<ICameraComponent> CameraComponentPtr;


	class EditorModuleExport MouseToolController : public boost::enable_shared_from_this<MouseToolController>, public IMessageListener
	{
		friend class IMouseTool;
	public:
		
		MouseToolController(bool scene_objects_selectable);
		virtual ~MouseToolController(void);
		void Init();
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
		GASS::ScenePtr GetScene(){return GASS::ScenePtr(m_Scene,boost::detail::sp_nothrow_tag());}
		void SetSceneObjectsSelectable(bool value) {m_SceneObjectsSelectable = value;}
		bool GetSceneObjectsSelectable() const {return m_SceneObjectsSelectable;}
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
		void Update(double delta);
		
		void NextTool();
		void PrevTool();
		void SetActive(bool value) {m_Active = value;}
		double GetDeltaTime() const {return m_Delta;}

		bool GetUseTerrainNormalOnDrop() const {return m_UseTerrainNormalOnDrop;}
		void SetUseTerrainNormalOnDrop(bool value) {m_UseTerrainNormalOnDrop = value;}
		void SetRayPickDistance(float value) {m_RayPickDistance = value;}
		void AddStaticObject(GASS::SceneObjectWeakPtr obj) {m_StaticObjects.insert(obj);}
	private:

		CursorInfo GetCursorInfo(const Vec2 &cursor_pos, Float raycast_distance);
		GASS::CollisionResult CameraRaycast(CameraComponentPtr cam, const Vec2 &viewport_pos, Float raycast_distance, int col_bits);

		//void RequestScenePosition();
		//bool CheckScenePosition();
		//bool ForceScenePosition();
		void OnFocusChanged(WindowFocusChangedMessagePtr message);
		void OnCursorMoved(CursorMoved2DMessagePtr message);
		void OnToolChanged(ToolChangedMessagePtr message);
		void OnMouseButton(MouseButtonMessagePtr message);
		void OnObjectLock(ObjectLockMessagePtr message);
		void OnObjectVisible(ObjectVisibleMessagePtr message);

		void OnNewScene(GASS::SceneAboutToLoadNotifyMessagePtr message);
		void OnUnloadScene(GASS::SceneUnloadNotifyMessagePtr message);
		void OnChangeCamera(GASS::ChangeCameraMessagePtr message);
		void OnSceneLoaded(GASS::SceneLoadedNotifyMessagePtr message);
		void OnSnapSettingsMessage(SnapSettingsMessagePtr message);
		void OnSnapModeMessage(SnapModeMessagePtr message);
		
		SceneObjectPtr GetPointerObject();


		void OnInput(GASS::ControllerMessagePtr message);
		GASS::CameraComponentPtr GetActiveCamera() const {return GASS::CameraComponentPtr(m_ActiveCamera,boost::detail::sp_nothrow_tag());}
		//GASS::SceneObjectPtr GetActiveCameraObject() const {return GASS::SceneObjectPtr(m_ActiveCameraObject,boost::detail::sp_nothrow_tag());}
		std::vector<IMouseTool*> m_Tools;
		IMouseTool* m_ActiveTool;
		//CursorInfo m_CursorInfo;
		GASS::ControlSetting* m_EditorControlSetting;
		std::set<GASS::SceneObjectWeakPtr> m_LockedObjects;
		std::set<GASS::SceneObjectWeakPtr> m_InvisibleObjects;
		std::set<GASS::SceneObjectWeakPtr> m_StaticObjects;

		GASS::CameraComponentWeakPtr m_ActiveCamera;
		GASS::SceneObjectWeakPtr m_ActiveCameraObject;
		bool m_Active;
		GASS::SceneWeakPtr m_Scene;
		GASS::CollisionHandle m_ColMeshHandle;
		GASS::CollisionHandle m_ColGizmoHandle;
		bool m_SceneObjectsSelectable;

		//Global settings
		Float m_GridSpacing;
		Float m_GridSize;
		Float m_SnapMovment;
		Float m_SnapAngle;

		bool m_EnableMovmentSnap;
		bool m_EnableAngleSnap;
		float m_RayPickDistance;
		bool m_EnableGizmo;
		bool m_UseTerrainNormalOnDrop;
		double m_Delta;
		Vec2 m_LastScreenPos;

		GASS::SceneObjectWeakPtr m_PointerObject;
	};
}

