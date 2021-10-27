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

	class EditorModuleExport RotateTool : public IMouseTool
	{
	public:
		RotateTool(MouseToolController* controller);
		~RotateTool(void) override;

		//IMouseTool interface
		void MouseMoved(const MouseData &data, const SceneCursorInfo &info) override;
		void MouseDown(const MouseData &data, const SceneCursorInfo &info) override;
		void MouseUp(const MouseData &data, const SceneCursorInfo &info) override;
		std::string GetName() override {return TID_ROTATE;}
		void Stop() override;
		void Start() override;
	private:
		bool CheckIfEditable(SceneObjectPtr obj) const;
		void SetGizmoVisiblity(bool value);
		SceneObjectPtr GetMasterGizmo();
		void OnSelectionChanged(EditorSelectionChangedEventPtr message);
		void SendMessageRec(SceneObjectPtr obj, SceneObjectRequestMessagePtr msg);

		bool m_MouseIsDown;
		std::vector<SceneObjectWeakPtr> m_Selected;
		MouseToolController* m_Controller;
		SceneObjectWeakPtr m_MasterGizmoObject;
		SceneObjectWeakPtr m_CurrentGizmo;

		bool m_UseGizmo;
		Vec2 m_MouseDownPos;
		bool m_RotateY;
		bool  m_Active;
	};
}




