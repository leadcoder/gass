#pragma once

#include "../EditorCommon.h"
#include "../EditorMessages.h"
#include <set>
#include "IMouseTool.h"
#include "CursorInfo.h"

namespace GASS
{
	class MouseToolController;
	class SceneObject;
	typedef boost::weak_ptr<SceneObject> SceneObjectWeakPtr;

	class EditorModuleExport EditPositionTool: public IMouseTool
	{
	public:
		EditPositionTool(MouseToolController* controller);
		virtual ~EditPositionTool(void);

		//IMouseTool interface
		virtual void MoveTo(const CursorInfo &info);
		virtual void MouseDown(const CursorInfo &info);
		virtual void MouseUp(const CursorInfo &info);
		virtual std::string GetName() {return "EditPositionTool";}
		virtual void Stop() {};
		virtual void Start() {};
	private:
		void OnSceneObjectSelected(ObjectSelectionChangedMessagePtr message);
		bool m_MouseIsDown;
		GASS::SceneObjectWeakPtr m_SelectedObject;
		MouseToolController* m_Controller;


	};
}