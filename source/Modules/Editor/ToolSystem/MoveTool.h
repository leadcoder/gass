#pragma once
#include "Sim/GASSCommon.h"
#include "../EditorMessages.h"
#include "IMouseTool.h"
#include "CursorInfo.h"



namespace GASS
{
	class MouseToolController;
	class SceneObject;
	class IMessage;
	using SceneObjectPtr = std::shared_ptr<SceneObject>;
	using MessagePtr = std::shared_ptr<IMessage>;

	class EditorModuleExport MoveTool : public IMouseTool 
	{
	public:
		
		MoveTool(MouseToolController* controller);
		~MoveTool(void) override;

		//IMouseTool interface
		void MouseMoved(const MouseData &data, const SceneCursorInfo &info) override;
		void MouseDown(const MouseData &data, const SceneCursorInfo &info) override;
		void MouseUp(const MouseData &data, const SceneCursorInfo &info) override;
		std::string GetName() override {return TID_MOVE;}
		void Stop() override;
		void Start() override;
	private:
		bool CheckIfEditable(SceneObjectPtr obj) const;
		void SetGizmoVisiblity(bool value);
		SceneObjectPtr GetOrCreateGizmo();
		void OnSelectionChanged(EditorSelectionChangedEventPtr message);
		void SendMessageRec(SceneObjectPtr obj, SceneObjectRequestMessagePtr msg);
		bool m_MouseIsDown;
		std::vector<SceneObjectWeakPtr> m_Selected;
		std::vector<SceneObjectWeakPtr> m_SelectionCopy;
		MouseToolController* m_Controller;
		int m_MoveUpdateCount;
		GASS::SceneObjectWeakPtr m_MasterGizmoObject;
		GASS::SceneObjectWeakPtr m_CurrentGizmo;

		using SelectionMap = std::map<GASS::SceneObjectWeakPtr, Vec3, std::owner_less<GASS::SceneObjectWeakPtr>>;
		SelectionMap m_SelectedLocations;
		
		bool m_UseGizmo;
		Vec2 m_MouseDownPos;
		bool m_GroundSnapMove;
		bool  m_Active;
		Vec3 m_PreviousPos;
		double m_MouseMoveTime;
	};
}