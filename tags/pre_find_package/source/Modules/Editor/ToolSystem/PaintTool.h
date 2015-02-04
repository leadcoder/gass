#pragma once
#include "Sim/GASSCommon.h"
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
	typedef SPTR<SceneObject> SceneObjectPtr;
	typedef SPTR<IMessage> MessagePtr;


	class EditorModuleExport PaintTool : public IMouseTool
	{
	public:
		PaintTool(MouseToolController* controller);
		virtual ~PaintTool(void);

		//IMouseTool interface
		virtual void MouseMoved(const MouseData &data, const SceneCursorInfo &info);
		virtual void MouseDown(const MouseData &data, const SceneCursorInfo &info);
		virtual void MouseUp(const MouseData &data, const SceneCursorInfo &info);
		virtual std::string GetName() {return TID_PAINT;}
		virtual void Stop();
		virtual void Start();
	private:
		SceneObjectPtr GetMasterGizmo();
		void SetGizmoVisiblity(bool value);
		void OnSceneObjectSelected(ObjectSelectionChangedEventPtr message);
		void SendMessageRec(SceneObjectPtr obj,SceneObjectRequestMessagePtr msg);

		bool m_MouseIsDown;
		GASS::SceneObjectWeakPtr m_SelectedObject;
		MouseToolController* m_Controller;
		GASS::SceneObjectWeakPtr m_MasterGizmoObject;
		bool m_Active;

	};
}