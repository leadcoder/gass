#pragma once
#include "Sim/GASSCommon.h"
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
	typedef SPTR<SceneObject> SceneObjectPtr;
	typedef SPTR<IMessage> MessagePtr;


	class EditorModuleExport RotateTool : public IMouseTool
	{
	public:
		RotateTool(MouseToolController* controller);
		virtual ~RotateTool(void);

		//IMouseTool interface
		virtual void MouseMoved(const MouseData &data, const SceneCursorInfo &info);
		virtual void MouseDown(const MouseData &data, const SceneCursorInfo &info);
		virtual void MouseUp(const MouseData &data, const SceneCursorInfo &info);
		virtual std::string GetName() {return TID_ROTATE;}
		virtual void Stop();
		virtual void Start();
	private:
		bool CheckIfEditable(SceneObjectPtr obj);
		void SetGizmoVisiblity(bool value);
		SceneObjectPtr GetMasterGizmo();
		void OnSceneObjectSelected(ObjectSelectionChangedEventPtr message);
		void SendMessageRec(SceneObjectPtr obj, SceneObjectRequestMessagePtr msg);

		bool m_MouseIsDown;
		SceneObjectWeakPtr m_SelectedObject;
		MouseToolController* m_Controller;
		SceneObjectWeakPtr m_MasterGizmoObject;
		SceneObjectWeakPtr m_CurrentGizmo;

		bool m_UseGizmo;
		GASS::Vec2 m_MouseDownPos;
		bool m_RotateY;
		bool  m_Active;
	};
}




