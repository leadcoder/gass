#pragma once
#include "Sim/GASSCommon.h"
#include "IMouseTool.h"
#include "CursorInfo.h"

namespace GASS
{
	class MouseToolController;
	class SceneObject;
	using SceneObjectWeakPtr = std::weak_ptr<SceneObject>;

	class EditorModuleExport VerticalMoveTool : public IMouseTool
	{
	public:
		VerticalMoveTool(MouseToolController* controller);
		~VerticalMoveTool(void) override;

		//IMouseTool interface
		void MouseMoved(const MouseData &data, const SceneCursorInfo &info) override;
		void MouseDown(const MouseData &data, const SceneCursorInfo &info) override;
		void MouseUp(const MouseData &data, const SceneCursorInfo &info) override;
		std::string GetName() override {return TID_VERTICAL_MOVE;}
		void Stop() override {};
		void Start() override {};
	private:
		bool m_MouseIsDown;
		SceneObjectWeakPtr m_SelectedObject;
		MouseToolController* m_Controller;

	};
}