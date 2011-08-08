#pragma once

#include <set>
#include "IMouseTool.h"
#include "CursorInfo.h"
#include "../EditorMessages.h"
#include "Sim/Scenario/Scene/SceneObject.h"

namespace GASS
{
	class SceneObject;
	class MouseToolController;

	class EditorModuleExport CreateTool :  public IMouseTool
	{
	public:
		CreateTool(MouseToolController* controller);
		virtual ~CreateTool(void);

		//IMouseTool interface
		virtual void MoveTo(const CursorInfo &info);
		virtual void MouseDown(const CursorInfo &info);
		virtual void MouseUp(const CursorInfo &info);
		virtual std::string GetName() {return "CreateTool";}
		virtual void Stop() {};
		virtual void Start() {};
	private:
		void OnToolChanged(ToolChangedMessagePtr message);
		bool m_MouseIsDown;
		GASS::SceneObjectWeakPtr m_SelectedObject;
		GASS::Vec3 m_Offset;
		MouseToolController* m_Controller;
		bool m_FirstMoveUpdate;

		std::string m_ObjectName;
		GASS::SceneObjectWeakPtr m_ParentObject; 
	};
}