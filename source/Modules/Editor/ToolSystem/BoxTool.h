#pragma once
#include "Sim/GASSCommon.h"
#include "IMouseTool.h"
#include "CursorInfo.h"

namespace GASS
{
	class MouseToolController;
	class SceneObject;
	class Component;
	using SceneObjectWeakPtr = std::weak_ptr<SceneObject>;
	using SceneObjectPtr = std::shared_ptr<SceneObject>;
	using ComponentWeakPtr = std::weak_ptr<Component>;


	class EditorModuleExport BoxTool : public IMouseTool
	{
	public:
		BoxTool(MouseToolController* controller);
		~BoxTool(void) override;

		//IMouseTool interface
		void MouseMoved(const MouseData& data, const SceneCursorInfo& info) override;
		void MouseDown(const MouseData& data, const SceneCursorInfo& info) override;
		void MouseUp(const MouseData& data, const SceneCursorInfo& info) override;
		std::string GetName() override { return TID_BOX; }
		void Stop() override {};
		void Start() override {};
	private:
		SceneObjectPtr GetOrCreateBoxObject();
		void UpdateBox(const GASS::Vec3& start, const GASS::Vec3& end);
		bool m_MouseIsDown;
		GASS::SceneObjectWeakPtr m_BoxObject;
		MouseToolController* m_Controller;
		GASS::Vec3 m_StartPos;
	};
}
