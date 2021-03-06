#pragma once

#include "Sim/GASSCommon.h"
#include "IMouseTool.h"
#include "CursorInfo.h"
#include "../EditorMessages.h"

namespace GASS
{
	class SceneObject;
	class MouseToolController;

	class EditorModuleExport CreateTool : public IMouseTool
	{
	public:
		CreateTool(MouseToolController* controller);
		virtual ~CreateTool(void);

		//IMouseTool interface
		virtual void MouseMoved(const MouseData &data, const SceneCursorInfo &info);
		virtual void MouseDown(const MouseData &data, const SceneCursorInfo &info);
		virtual void MouseUp(const MouseData &data, const SceneCursorInfo &info);
		virtual std::string GetName() { return TID_CREATE; }
		virtual void Stop() {};
		virtual void Start() {};
		void SetParentObject(SceneObjectPtr object) { m_ParentObject = object; }
		void SetTemplateName(const std::string &name) { m_ObjectName = name; }
		void SetAllowWaypointInsert(bool value) {m_AllowWPInsert = value;}
		bool GetAllowWaypointInsert() const { return m_AllowWPInsert; }
	private:
		bool m_MouseIsDown;
		GASS::Vec3 m_Offset;
		MouseToolController* m_Controller;
		std::string m_ObjectName;
		GASS::SceneObjectWeakPtr m_ParentObject; 
		bool m_AllowWPInsert;
	};
}