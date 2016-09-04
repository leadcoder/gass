#pragma once
#include "Sim/GASSCommon.h"
#include "../EditorMessages.h"
#include "IMouseTool.h"
#include "CursorInfo.h"



namespace GASS
{
	class MouseToolController;
	class SceneObject;
	class IMessage;
	typedef GASS_SHARED_PTR<SceneObject> SceneObjectPtr;
	typedef GASS_SHARED_PTR<IMessage> MessagePtr;

	class EditorModuleExport MoveTool : public IMouseTool 
	{
	public:
		
		MoveTool(MouseToolController* controller);
		virtual ~MoveTool(void);

		//IMouseTool interface
		virtual void MouseMoved(const MouseData &data, const SceneCursorInfo &info);
		virtual void MouseDown(const MouseData &data, const SceneCursorInfo &info);
		virtual void MouseUp(const MouseData &data, const SceneCursorInfo &info);
		virtual std::string GetName() {return TID_MOVE;}
		virtual void Stop();
		virtual void Start();
	private:
		bool CheckIfEditable(SceneObjectPtr obj) const;
		void SetGizmoVisiblity(bool value);
		SceneObjectPtr GetOrCreateGizmo();
		void OnSelectionChanged(EditorSelectionChangedEventPtr message);
		void SendMessageRec(SceneObjectPtr obj, SceneObjectRequestMessagePtr msg);
		bool m_MouseIsDown;
		std::vector<SceneObjectWeakPtr> m_Selected;
		std::vector<SceneObjectWeakPtr> m_SelectionCopy;
		MouseToolController* m_Controller;
		int m_MoveUpdateCount;
		GASS::SceneObjectWeakPtr m_MasterGizmoObject;
		GASS::SceneObjectWeakPtr m_CurrentGizmo;
		std::map<GASS::SceneObjectWeakPtr,Vec3> m_SelectedLocations;
		bool m_UseGizmo;
		Vec2 m_MouseDownPos;
		bool m_GroundSnapMove;
		bool  m_Active;
		Vec3 m_PreviousPos;
		double m_MouseMoveTime;
	};
}