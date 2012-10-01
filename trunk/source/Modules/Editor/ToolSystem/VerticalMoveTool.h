#pragma once

#include "../EditorCommon.h"
#include <set>
#include "IMouseTool.h"
#include "CursorInfo.h"

namespace GASS
{
	class MouseToolController;
	class SceneObject;
	typedef boost::weak_ptr<SceneObject> SceneObjectWeakPtr;

	class EditorModuleExport VerticalMoveTool : public IMouseTool
	{
	public:
		VerticalMoveTool(MouseToolController* controller);
		virtual ~VerticalMoveTool(void);

		//IMouseTool interface
		virtual void MoveTo(const CursorInfo &info);
		virtual void MouseDown(const CursorInfo &info);
		virtual void MouseUp(const CursorInfo &info);
		virtual std::string GetName() {return TID_VERTICAL_MOVE;}
		virtual void Stop() {};
		virtual void Start() {};
	private:
		bool m_MouseIsDown;
		GASS::SceneObjectWeakPtr m_SelectedObject;
		MouseToolController* m_Controller;

	};
}