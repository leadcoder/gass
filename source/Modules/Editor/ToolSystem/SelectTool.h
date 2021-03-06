#pragma once
#include "IMouseTool.h"
#include "CursorInfo.h"

namespace GASS
{
	class MouseToolController;
	class SceneObject;
	typedef GASS_WEAK_PTR<SceneObject> SceneObjectWeakPtr;

	class EditorModuleExport SelectTool : public IMouseTool
	{
	public:
		SelectTool(MouseToolController* controller);
		virtual ~SelectTool(void);

		//IMouseTool interface
		virtual void MouseMoved(const MouseData &data, const SceneCursorInfo &info);
		virtual void MouseDown(const MouseData &data, const SceneCursorInfo &info);
		virtual void MouseUp(const MouseData &data, const SceneCursorInfo &info);
		virtual std::string GetName() {return TID_SELECT;}
		virtual void Stop() {};
		virtual void Start() {};
	private:
		bool m_MouseIsDown;
		//GASS::SceneObjectWeakPtr m_SelectedObject;
		MouseToolController* m_Controller;
	};
}