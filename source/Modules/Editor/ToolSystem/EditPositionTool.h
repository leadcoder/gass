#pragma once
#include "Sim/GASSCommon.h"
#include "../EditorMessages.h"
#include "IMouseTool.h"
#include "CursorInfo.h"

namespace GASS
{
	class MouseToolController;
	class SceneObject;
	typedef GASS_WEAK_PTR<SceneObject> SceneObjectWeakPtr;

	class EditorModuleExport EditPositionTool: public IMouseTool
	{
	public:
		EditPositionTool(MouseToolController* controller);
		virtual ~EditPositionTool(void);

		//IMouseTool interface
		virtual void MouseMoved(const MouseData &data, const SceneCursorInfo &info);
		virtual void MouseDown(const MouseData &data, const SceneCursorInfo &info);
		virtual void MouseUp(const MouseData &data, const SceneCursorInfo &info);
		virtual std::string GetName() {return TID_EDIT_POS;}
		virtual void Stop() {};
		virtual void Start() {};
	private:
		void OnSelectionChanged(EditorSelectionChangedEventPtr message);

		bool m_MouseIsDown;
		std::vector<SceneObjectWeakPtr> m_Selection;
		MouseToolController* m_Controller;
	};
}