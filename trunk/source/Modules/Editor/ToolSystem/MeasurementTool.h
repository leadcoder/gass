#pragma once

#include "../EditorCommon.h"
#include <set>
#include "IMouseTool.h"
#include "CursorInfo.h"

namespace GASS
{
	class MouseToolController;
	class SceneObject;
	class ILineComponent;
	class IComponent;
	typedef boost::weak_ptr<SceneObject> SceneObjectWeakPtr;
	typedef boost::weak_ptr<ILineComponent> LineComponentWeakPtr;
	typedef boost::weak_ptr<IComponent> ComponentWeakPtr;


	class EditorModuleExport MeasurementTool : public IMouseTool
	{
	public:
		MeasurementTool(MouseToolController* controller);
		virtual ~MeasurementTool(void);

		//IMouseTool interface
		virtual void MoveTo(const CursorInfo &info);
		virtual void MouseDown(const CursorInfo &info);
		virtual void MouseUp(const CursorInfo &info);
		virtual std::string GetName() {return "MeasurementTool";}
		virtual void Stop() {};
		virtual void Start() {};
	private:
		void UpdateLine(const GASS::Vec3 &start, const GASS::Vec3 &end);

		bool m_MouseIsDown;
		GASS::SceneObjectWeakPtr m_RulerObject;
		MouseToolController* m_Controller;
		GASS::LineComponentWeakPtr m_RulerLine;
		GASS::ComponentWeakPtr m_TextComp;
		GASS::Vec3 m_StartPos;

	};
}
