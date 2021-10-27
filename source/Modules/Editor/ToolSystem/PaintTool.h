#pragma once
#include "Sim/GASSCommon.h"
#include "../EditorMessages.h"
#include "IMouseTool.h"
#include "CursorInfo.h"

namespace GASS
{
	class SceneObject;
	class MouseToolController;
	class IMessage;
	using SceneObjectPtr = std::shared_ptr<SceneObject>;
	using MessagePtr = std::shared_ptr<IMessage>;

	class EditorModuleExport PaintTool : public IMouseTool
	{
	public:
		PaintTool(MouseToolController* controller);
		~PaintTool(void) override;

		//IMouseTool interface
		void MouseMoved(const MouseData &data, const SceneCursorInfo &info) override;
		void MouseDown(const MouseData &data, const SceneCursorInfo &info) override;
		void MouseUp(const MouseData &data, const SceneCursorInfo &info) override;
		std::string GetName() override {return TID_PAINT;}
		void Stop() override;
		void Start() override;
	private:
		SceneObjectPtr GetMasterGizmo();
		void SetGizmoVisiblity(bool value);
		void OnSelectionChanged(EditorSelectionChangedEventPtr message);
		void SendMessageRec(SceneObjectPtr obj,SceneObjectRequestMessagePtr msg);

		bool m_MouseIsDown;
		SceneObjectWeakPtr m_SelectedObject;
		MouseToolController* m_Controller;
		SceneObjectWeakPtr m_MasterGizmoObject;
		bool m_Active;
	};
}