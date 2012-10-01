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
	class IMessage;
	typedef boost::shared_ptr<SceneObject> SceneObjectPtr;
	typedef boost::shared_ptr<IMessage> MessagePtr;


	class EditorModuleExport RotateTool : public IMouseTool
	{
	public:
		RotateTool(MouseToolController* controller);
		virtual ~RotateTool(void);

		//IMouseTool interface
		virtual void MoveTo(const CursorInfo &info);
		virtual void MouseDown(const CursorInfo &info);
		virtual void MouseUp(const CursorInfo &info);
		virtual std::string GetName() {return TID_ROTATE;}
		virtual void Stop();
		virtual void Start();
	private:
		bool CheckIfEditable(SceneObjectPtr obj);
		void SetGizmoVisiblity(bool value);
		GASS::SceneObjectPtr GetMasterGizmo();
		void OnSceneObjectSelected(ObjectSelectionChangedMessagePtr message);
		void SendMessageRec(GASS::SceneObjectPtr obj,GASS::MessagePtr msg);

		bool m_MouseIsDown;
		GASS::SceneObjectWeakPtr m_SelectedObject;
		MouseToolController* m_Controller;

		GASS::SceneObjectWeakPtr m_MasterGizmoObject;
		GASS::SceneObjectWeakPtr m_CurrentGizmo;

		bool m_UseGizmo;
		GASS::Vec2 m_MouseDownPos;
		bool m_RotateY;
		bool  m_Active;

	};
}




