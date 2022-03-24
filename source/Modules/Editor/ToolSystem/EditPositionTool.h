#pragma once
#include "Sim/GASSCommon.h"
#include "../EditorMessages.h"
#include "IMouseTool.h"
#include "CursorInfo.h"

namespace GASS
{
	class MouseToolController;
	class SceneObject;
	using SceneObjectWeakPtr = std::weak_ptr<SceneObject>;

	class EditorModuleExport EditPositionTool: public IMouseTool
	{
	public:
		EditPositionTool(MouseToolController* controller);
		~EditPositionTool(void) override;

		//IMouseTool interface
		void MouseMoved(const MouseData &data, const SceneCursorInfo &info) override;
		void MouseDown(const MouseData &data, const SceneCursorInfo &info) override;
		void MouseUp(const MouseData &data, const SceneCursorInfo &info) override;
		std::string GetName() override {return TID_EDIT_POS;}
		void Stop() override {};
		void Start() override {};
	private:
		void OnSelectionChanged(EditorSelectionChangedEventPtr message);

		bool m_MouseIsDown;
		std::vector<SceneObjectWeakPtr> m_Selection;
		MouseToolController* m_Controller;
	};
}