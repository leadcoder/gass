#pragma once

#include "Sim/GASSCommon.h"
#include <set>
#include "IMouseTool.h"
#include "CursorInfo.h"
#include "../EditorMessages.h"
#include "Sim/GASSSceneObject.h"

namespace GASS
{
	class SceneObject;
	class MouseToolController;

	class EditorModuleExport GraphTool :  public IMouseTool
	{
	public:
		GraphTool(MouseToolController* controller);
		virtual ~GraphTool(void);

		//IMouseTool interface
		virtual void MouseMoved(const MouseData &data, const SceneCursorInfo &info);
		virtual void MouseDown(const MouseData &data, const SceneCursorInfo &info);
		virtual void MouseUp(const MouseData &data, const SceneCursorInfo &info);
		virtual std::string GetName() {return TID_GRAPH;}
		virtual void Stop() {};
		virtual void Start() {};
		void SetParentObject(SceneObjectPtr object) {m_ParentObject = object;}
		void SetConnetionObject(SceneObjectPtr object) {m_PrevObject = object;}
		void SetNodeTemplateName(const std::string &name) {m_NodeObjectName= name;}
		void SetEdgeTemplateName(const std::string &name) {m_EdgeObjectName= name;}
	private:
		void OnToolChanged(ToolChangedEventPtr message);
		bool m_MouseIsDown;
		GASS::Vec3 m_Offset;
		MouseToolController* m_Controller;
		bool m_FirstMoveUpdate;
		std::string m_NodeObjectName;
		std::string m_EdgeObjectName;
		GASS::SceneObjectWeakPtr m_ParentObject; 
		GASS::SceneObjectWeakPtr m_PrevObject; 
	};
}