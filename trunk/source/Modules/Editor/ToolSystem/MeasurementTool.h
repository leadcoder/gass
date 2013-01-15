#pragma once

#include "../EditorCommon.h"
#include <set>
#include "IMouseTool.h"
#include "CursorInfo.h"

namespace GASS
{
	class MouseToolController;
	class SceneObject;
	class IComponent;
	typedef boost::weak_ptr<SceneObject> SceneObjectWeakPtr;
	typedef boost::shared_ptr<SceneObject> SceneObjectPtr;
	typedef boost::weak_ptr<IComponent> ComponentWeakPtr;


	class EditorModuleExport MeasurementTool : public IMouseTool
	{
	public:
		MeasurementTool(MouseToolController* controller);
		virtual ~MeasurementTool(void);

		//IMouseTool interface
		virtual void MouseMoved(const MouseData &data, const SceneCursorInfo &info);
		virtual void MouseDown(const MouseData &data, const SceneCursorInfo &info);
		virtual void MouseUp(const MouseData &data, const SceneCursorInfo &info);
		virtual std::string GetName() {return TID_MEASUREMENT;}
		virtual void Stop() {};
		virtual void Start() {};
	private:
		SceneObjectPtr GetOrCreateRulerObject();
		void UpdateLine(const GASS::Vec3 &start, const GASS::Vec3 &end);
		bool m_MouseIsDown;
		GASS::SceneObjectWeakPtr m_RulerObject;
		MouseToolController* m_Controller;
		GASS::ComponentWeakPtr m_TextComp;
		GASS::Vec3 m_StartPos;

	};
}
