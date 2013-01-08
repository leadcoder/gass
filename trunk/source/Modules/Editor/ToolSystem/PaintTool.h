#pragma once

#include "../EditorCommon.h"
#include "../EditorMessages.h"
#include <set>
#include "IMouseTool.h"
#include "CursorInfo.h"

namespace GASS
{
	class SceneObject;
	class MouseToolController;
	class IMessage;
	typedef boost::shared_ptr<SceneObject> SceneObjectPtr;
	typedef boost::shared_ptr<IMessage> MessagePtr;


	class EditorModuleExport PaintTool : public IMouseTool
	{
	public:
		PaintTool(MouseToolController* controller);
		virtual ~PaintTool(void);

		//IMouseTool interface
		virtual void MouseMoved(const CursorInfo &info);
		virtual void MouseDown(const CursorInfo &info);
		virtual void MouseUp(const CursorInfo &info);
		virtual std::string GetName() {return TID_PAINT;}
		virtual void Stop();
		virtual void Start();
	private:
		GASS::SceneObjectPtr GetMasterGizmo();
		void SetGizmoVisiblity(bool value);
		void OnSceneObjectSelected(GASS::ObjectSelectionChangedEventPtr message);
		void SendMessageRec(GASS::SceneObjectPtr obj,GASS::MessagePtr msg);
	

		bool m_MouseIsDown;
		GASS::SceneObjectWeakPtr m_SelectedObject;
		MouseToolController* m_Controller;
		GASS::SceneObjectWeakPtr m_MasterGizmoObject;
		bool m_Active;

	};
}