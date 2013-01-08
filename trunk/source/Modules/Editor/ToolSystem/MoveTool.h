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
	

	class EditorModuleExport MoveTool : public IMouseTool 
	{
	public:
		
		MoveTool(MouseToolController* controller);
		virtual ~MoveTool(void);

		//IMouseTool interface
		virtual void MouseMoved(const CursorInfo &info);
		virtual void MouseDown(const CursorInfo &info);
		virtual void MouseUp(const CursorInfo &info);
		virtual std::string GetName() {return TID_MOVE;}
		virtual void Stop();
		virtual void Start();
	private:
		bool CheckIfEditable(SceneObjectPtr obj);
		void SetGizmoVisiblity(bool value);
		SceneObjectPtr GetOrCreateGizmo();
		void OnSceneObjectSelected(ObjectSelectionChangedEventPtr message);
		void SendMessageRec(GASS::SceneObjectPtr obj,GASS::MessagePtr msg);
		bool m_MouseIsDown;
		SceneObjectWeakPtr m_SelectedObject;
		Vec3 m_Offset;
		MouseToolController* m_Controller;
		int m_MoveUpdateCount;

		GASS::SceneObjectWeakPtr m_MasterGizmoObject;
		GASS::SceneObjectWeakPtr m_CurrentGizmo;

		bool m_UseGizmo;
		Vec2 m_MouseDownPos;
		bool m_GroundSnapMove;
		bool  m_Active;
		bool m_SnapToMouse;

	};
}