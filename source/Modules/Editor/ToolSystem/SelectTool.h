#pragma once
#include "IMouseTool.h"
#include "CursorInfo.h"

namespace GASS
{
	class MouseToolController;
	class SceneObject;
	using SceneObjectWeakPtr = std::weak_ptr<SceneObject>;

	class EditorModuleExport SelectTool : public IMouseTool
	{
	public:
		SelectTool(MouseToolController* controller);
		~SelectTool(void) override;

		//IMouseTool interface
		void MouseMoved(const MouseData &data, const SceneCursorInfo &info) override;
		void MouseDown(const MouseData &data, const SceneCursorInfo &info) override;
		void MouseUp(const MouseData &data, const SceneCursorInfo &info) override;
		std::string GetName() override {return TID_SELECT;}
		void Stop() override {};
		void Start() override {};
	private:
		bool m_MouseIsDown;
		//GASS::SceneObjectWeakPtr m_SelectedObject;
		MouseToolController* m_Controller;
	};
}