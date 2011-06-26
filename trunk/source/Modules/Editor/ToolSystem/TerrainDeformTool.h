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
		enum TerrainEditMode
		{
			TEM_DEFORM,
			TEM_SMOOTH,
			TEM_FLATTEN,
			TEM_LAYER_PAINT,
			TEM_VEGETATION_PAINT
		};

		TerrainDeformTool(MouseToolController* controller);
		virtual ~TerrainDeformTool(void);

		//IMouseTool interface
		virtual void MoveTo(const CursorInfo &info);
		virtual void MouseDown(const CursorInfo &info);
		virtual void MouseUp(const CursorInfo &info);
		virtual std::string GetName() {return "TerrainDeformTool";}
		virtual void Stop();
		virtual void Start();
		void SetBrushSize(float value);
		void SetBrushInnerSize(float value);
		float GetBrushSize() const {return m_BrushSize;}
		void SetIntensity(float value);
		void SetNoise(float value);
		void SetModMode(TerrainEditMode value) {m_TEM = value;}
		void SetLayerTexture(const std::string &texture, float tiling);
		void SetActiveLayer(TerrainLayer layer) {m_ActiveLayer = layer;}
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
		float m_Noise;
		float m_InvertBrush;
		TerrainEditMode m_TEM;
		TerrainLayer m_ActiveLayer;

	};
}