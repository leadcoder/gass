#pragma once

#include "Sim/GASSCommon.h"
#include "IMouseTool.h"
#include "CursorInfo.h"
#include "../EditorMessages.h"
#include "Sim/Interface/GASSIGraphEdgeComponent.h"

namespace GASS
{
	class SceneObject;
	class MouseToolController;

	enum GraphToolMode
	{
		GTM_ADD,
		GTM_INSERT
	};

	//class GraphEdgeComponent;
	//typedef GASS_SHARED_PTR<GraphEdgeComponent> GraphEdgeComponentPtr;
	
	class EditorModuleExport GraphTool :  public IMouseTool
	{
	public:
		GraphTool(MouseToolController* controller);
		~GraphTool(void) override;

		//IMouseTool interface
		void MouseMoved(const MouseData &data, const SceneCursorInfo &info) override;
		void MouseDown(const MouseData &data, const SceneCursorInfo &info) override;
		void MouseUp(const MouseData &data, const SceneCursorInfo &info) override;
		std::string GetName() override {return TID_GRAPH;}
		void Stop() override {};
		void Start() override {};
		void SetParentObject(SceneObjectPtr object) {m_ParentObject = object;}
		void SetConnetionObject(SceneObjectPtr object) {m_PrevObject = object;}
		void SetNodeTemplateName(const std::string &name) {m_NodeObjectName= name;}
		void SetEdgeTemplateName(const std::string &name) {m_EdgeObjectName= name;}
		void SetMode(GraphToolMode mode) {m_Mode= mode;}
	private:
		void TryInsert(SceneObjectPtr new_obj, const Vec3 &obj_pos, SceneObjectPtr parent_obj) const;
		GraphEdgeComponentPtr GetClosestEdge(SceneObjectPtr graph_obj, const Vec3 &pos, Float treshhold_dist) const;
		void OnToolChanged(ToolChangedEventPtr message);
		bool m_MouseIsDown;
		GASS::Vec3 m_Offset;
		MouseToolController* m_Controller;
		std::string m_NodeObjectName;
		std::string m_EdgeObjectName;
		GASS::SceneObjectWeakPtr m_ParentObject; 
		GASS::SceneObjectWeakPtr m_PrevObject; 
		GraphToolMode m_Mode;
	};
}