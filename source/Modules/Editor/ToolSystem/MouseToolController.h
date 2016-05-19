#pragma once
#include "Sim/GASSCommon.h"
#include "Modules/Editor/EditorCommon.h"
#include "Modules/Editor/EditorMessages.h"
#include "Sim/GASSGeometryFlags.h"
#include "Sim/Interface/GASSICollisionSceneManager.h"
#include "Sim/Messages/GASSCoreSceneMessages.h"
#include "Sim/Messages/GASSGraphicsSceneMessages.h"
#include "Sim/Messages/GASSCoreSystemMessages.h"
#include "Sim/Interface/GASSIControlSettingsSystem.h"
#include "Sim/Interface/GASSIInputSystem.h"
#include "Sim/GASSGeometryFlags.h"
#include "CursorInfo.h"
#include "IMouseTool.h"
#include <string>
#include <vector>
#include <set>



namespace GASS
{
	class ControlSetting;
	class SceneObject;
	class EditorSceneManager;
	class Scene;
	class ICameraComponent;
	typedef GASS_WEAK_PTR<SceneObject> SceneObjectWeakPtr;
	typedef GASS_SHARED_PTR<SceneObject> SceneObjectPtr;
	typedef GASS_WEAK_PTR<ICameraComponent> CameraComponentWeakPtr;
	typedef GASS_SHARED_PTR<ICameraComponent> CameraComponentPtr;
	class EditorSystem; 

	class EditorModuleExport MouseToolController : public GASS_ENABLE_SHARED_FROM_THIS<MouseToolController>, public IMessageListener, public IMouseListener , public IKeyListener
	{
		friend class IMouseTool;
	public:
		MouseToolController(EditorSceneManager* sm);
		~MouseToolController(void);
		void Init();
		void AddTool(IMouseTool* tool);
		bool SelectTool(const std::string &tool_name);
		IMouseTool* GetTool(const std::string &tool_name);
		void SetActive(bool value) {m_Active = value;}
		void CreateSceneObject(const std::string name, const Vec2 &mouse_pos);
		void SetGridSpacing(Float value); 
		void SetGridSize(Float value); 
		void SetSnapMovment(Float value);
		void SetSnapAngle(Float value);
		void SetEditMode(GizmoEditMode value);
		GizmoEditMode  GetEditMode() const {return m_EditMode;}
		Float GetGridSpacing() const {return m_GridSpacing;}
		Float GetGridSize() const {return m_GridSize;}
		Float GetSnapMovment() const {return m_SnapMovment;}
		Float GetSnapAngle() const {return m_SnapAngle;}
		int GetEnableGizmo() const {return m_EnableGizmo;}
		void SetEnableGizmo(int value);
		Float SnapPosition(Float value);
		Float SnapAngle(Float value);
		void Update(double delta);
		bool GetUseTerrainNormalOnDrop() const {return m_UseTerrainNormalOnDrop;}
		void SetUseTerrainNormalOnDrop(bool value) {m_UseTerrainNormalOnDrop = value;}
		void SetRayPickDistance(float value) {m_RayPickDistance = value;}
		float GetRayPickDistance() const {return m_RayPickDistance;}
		EditorSceneManager* GetEditorSceneManager() const {return m_EditorSceneManager;}
		void SelectHelper(SceneObjectPtr obj) const;

		//IMouseListener
		bool MouseMoved(const MouseData &data);
		bool MousePressed(const MouseData &data, MouseButtonId id );
		bool MouseReleased(const MouseData &data, MouseButtonId id );

		//IKeyListener
		bool KeyPressed( int key, unsigned int text);
		bool KeyReleased( int key, unsigned int text);		
	
	
		bool IsShiftDown() const {return m_ShiftDown;}
		bool IsCtrlDown() const { return m_CtrlDown; }

		SceneCursorInfo GetSceneCursorInfo(const Vec2 &cursor_pos, Float raycast_distance);
	private:
		void OnInput(GASS::ControllSettingsMessagePtr message);
		void NextTool();
		void PrevTool();
		GASS::CollisionResult CameraRaycast(CameraComponentPtr cam, const Vec2 &viewport_pos, Float raycast_distance, GeometryFlags col_bits);

		//helper
		void CreateObjectFromTemplateAtPosition(const std::string &obj_name, const GASS::Vec3 &pos, const GASS::Quaternion &rot);
		GASS::SceneObjectPtr GetPointerObject();

		std::vector<IMouseTool*> m_Tools;
		IMouseTool* m_ActiveTool;
		std::string m_ControlSettingName;
		bool m_Active;
		
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
		bool m_ShiftDown;
		bool m_CtrlDown;
		GizmoEditMode m_EditMode;
		Vec2 m_MBRScreenPos;
		GASS::SceneObjectWeakPtr m_PointerObject;
		EditorSceneManager* m_EditorSceneManager;
	};
}

