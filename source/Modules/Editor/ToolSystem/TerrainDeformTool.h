#pragma once
#include "Sim/GASSCommon.h"
#include "../EditorMessages.h"
#include "IMouseTool.h"
#include "CursorInfo.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Interface/GASSIControlSettingsSystem.h"


namespace GASS
{
	class SceneObject;
	class MouseToolController;
	class IMessage;
	using SceneObjectPtr = std::shared_ptr<SceneObject>;
	using MessagePtr = std::shared_ptr<IMessage>;


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
		~TerrainDeformTool(void) override;

		//IMouseTool interface
		void MouseMoved(const MouseData &data, const SceneCursorInfo &info) override;
		void MouseDown(const MouseData &data, const SceneCursorInfo &info) override;
		void MouseUp(const MouseData &data, const SceneCursorInfo &info) override;
		void Update(double delta) override;

		std::string GetName() override {return TID_TERRAIN;}
		void Stop() override;
		void Start() override;
		void SetBrushSize(float value);
		void SetBrushInnerSize(float value);
		float GetBrushSize() const {return m_BrushSize;}
		void SetIntensity(float value);
		void SetNoise(float value);
		void SetModMode(TerrainEditMode value) {m_TEM = value;}
		TerrainEditMode GetModMode() const {return m_TEM;}
		void SetActiveLayer(TerrainLayer layer) {m_ActiveLayer = layer;}
	private:
		SceneObjectPtr GetOrCreateGizmo();
		void SetGizmoVisiblity(bool value);
		void OnSelectionChanged(EditorSelectionChangedEventPtr message);
		void SendMessageRec(SceneObjectPtr obj,SceneObjectRequestMessagePtr msg);
		void OnInput(ControllSettingsMessagePtr message);
	
		bool m_MouseIsDown;
		std::vector<SceneObjectWeakPtr> m_Selection;
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
		Vec3 m_CursorPos;
	};
}