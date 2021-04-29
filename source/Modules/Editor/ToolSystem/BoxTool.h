#pragma once
#include "Sim/GASSCommon.h"
#include "IMouseTool.h"
#include "CursorInfo.h"

namespace GASS
{
	class MouseToolController;
	class SceneObject;
	class Component;
	typedef GASS_WEAK_PTR<SceneObject> SceneObjectWeakPtr;
	typedef GASS_SHARED_PTR<SceneObject> SceneObjectPtr;
	typedef GASS_WEAK_PTR<Component> ComponentWeakPtr;


	class EditorModuleExport BoxTool : public IMouseTool
	{
	public:
		BoxTool(MouseToolController* controller);
		virtual ~BoxTool(void);

		//IMouseTool interface
		virtual void MouseMoved(const MouseData& data, const SceneCursorInfo& info);
		virtual void MouseDown(const MouseData& data, const SceneCursorInfo& info);
		virtual void MouseUp(const MouseData& data, const SceneCursorInfo& info);
		virtual std::string GetName() { return TID_BOX; }
		virtual void Stop() {};
		virtual void Start() {};
	private:
		SceneObjectPtr GetOrCreateBoxObject();
		void UpdateBox(const GASS::Vec3& start, const GASS::Vec3& end);
		bool m_MouseIsDown;
		GASS::SceneObjectWeakPtr m_BoxObject;
		MouseToolController* m_Controller;
		GASS::Vec3 m_StartPos;
	};
}
