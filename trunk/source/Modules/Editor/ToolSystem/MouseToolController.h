#pragma once

#include "../EditorCommon.h"
#include "../EditorMessages.h"
#include "Sim/Systems/Collision/GASSICollisionSystem.h"
#include "Sim/Scene/GASSCoreSceneMessages.h"
#include "Sim/Scene/GASSGraphicsSceneMessages.h"
#include "Sim/Systems/Messages/GASSCoreSystemMessages.h"
#include "Sim/Systems/Input/GASSControlSetting.h"
#include "Sim/Systems/Input/GASSIInputSystem.h"
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

	class EditorModuleExport MouseToolController : public boost::enable_shared_from_this<MouseToolController>, public IMessageListener, public IMouseListener
	{
		friend class IMouseTool;
	public:
		
		MouseToolController();
		virtual ~MouseToolController(void);

		void Init();
		void AddTool(IMouseTool* tool);
		bool SelectTool(const std::string &tool_name);
		void SetActive(bool value) {m_Active = value;}
		void CreateSceneObject(const std::string name, const Vec2 &mouse_pos);
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
		
		bool GetUseTerrainNormalOnDrop() const {return m_UseTerrainNormalOnDrop;}
		void SetUseTerrainNormalOnDrop(bool value) {m_UseTerrainNormalOnDrop = value;}
		void SetRayPickDistance(float value) {m_RayPickDistance = value;}
		double GetDeltaTime() const {return m_Delta;}

		//IMouseListener
		bool MouseMoved(const MouseData &data);
		bool MousePressed(const MouseData &data, MouseButtonId id );
		bool MouseReleased(const MouseData &data, MouseButtonId id );
	private:
		void NextTool();
		void PrevTool();

		CursorInfo GetCursorInfo(const Vec2 &cursor_pos, Float raycast_distance);
		

		void MoveTo(const CursorInfo &info);
		void MouseDown(const CursorInfo &info);
		void MouseUp(const CursorInfo &info);
		
		GASS::CollisionResult CameraRaycast(CameraComponentPtr cam, const Vec2 &viewport_pos, Float raycast_distance, int col_bits);
		void OnFocusChanged(WindowFocusChangedMessagePtr message);
		//void OnCursorMoved(CursorMoved2DMessagePtr message);
		//void OnSnapSettingsMessage(SnapSettingsMessagePtr message);
		//void OnSnapModeMessage(SnapModeMessagePtr message);

		//helper
		void CreateObjectFromTemplateAtPosition(const std::string &obj_name, const GASS::Vec3 &pos, const GASS::Quaternion &rot);

		
		SceneObjectPtr GetPointerObject();
		void OnInput(GASS::ControllerMessagePtr message);
		
		std::vector<IMouseTool*> m_Tools;
		IMouseTool* m_ActiveTool;
		GASS::ControlSetting* m_EditorControlSetting;
		
		bool m_Active;
		
		GASS::CollisionHandle m_ColMeshHandle;
		GASS::CollisionHandle m_ColGizmoHandle;

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
		Vec2 m_MBRScreenPos;
		GASS::SceneObjectWeakPtr m_PointerObject;
	};
}

