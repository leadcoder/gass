#pragma once

#include "../EditorCommon.h"
#include <set>
#include "IMouseTool.h"
#include "CursorInfo.h"

namespace GASS
{
	class SceneObject;
	class MouseToolController;

	class EditorModuleExport PaintTool : public IMouseTool
	{
	public:
		PaintTool(MouseToolController* controller);
		virtual ~PaintTool(void);

		//IMouseTool interface
		virtual void MoveTo(const CursorInfo &info);
		virtual void MouseDown(const CursorInfo &info);
		virtual void MouseUp(const CursorInfo &info);
		virtual std::string GetName() {return "PaintTool";}
		virtual void Stop();
		virtual void Start();
	private:
		bool m_MouseIsDown;
		GASS::SceneObjectWeakPtr m_SelectedObject;
		MouseToolController* m_Controller;

	};
}