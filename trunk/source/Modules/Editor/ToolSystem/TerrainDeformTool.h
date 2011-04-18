#pragma once

#include "../EditorCommon.h"
#include "../EditorMessages.h"
#include <set>
#include "IMouseTool.h"
#include "CursorInfo.h"
#include "Sim/Systems/Input/ControlSetting.h"

namespace GASS
{
	class SceneObject;
	class MouseToolController;
	class IMessage;
	typedef boost::shared_ptr<SceneObject> SceneObjectPtr;
	typedef boost::shared_ptr<IMessage> MessagePtr;


	class EditorModuleExport TerrainDeformTool : public IMouseTool
	{
	public:
		TerrainDeformTool(MouseToolController* controller);
		virtual ~TerrainDeformTool(void);

		//IMouseTool interface
		virtual void MoveTo(const CursorInfo &info);
		virtual void MouseDown(const CursorInfo &info);
		virtual void MouseUp(const CursorInfo &info);
		virtual std::string GetName() {return "TerrainDeformTool";}
		virtual void Stop();
		virtual void Start();
	private:
		GASS::SceneObjectPtr GetMasterGizmo();
		void SetGizmoVisiblity(bool value);
		void OnSceneObjectSelected(GASS::ObjectSelectedMessagePtr message);
		void SendMessageRec(GASS::SceneObjectPtr obj,GASS::MessagePtr msg);
		void OnInput(ControllerMessagePtr message);
	

		bool m_MouseIsDown;
		GASS::SceneObjectWeakPtr m_SelectedObject;
		MouseToolController* m_Controller;
		GASS::SceneObjectWeakPtr m_MasterGizmoObject;
		bool m_Active;
		float m_BrushSize;
		float m_BrushInnerSize;
		float m_Intensity;

	};
}