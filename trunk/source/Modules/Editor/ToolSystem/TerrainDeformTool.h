#pragma once
#include "Sim/GASSCommon.h"
#include "../EditorCommon.h"
#include "../EditorMessages.h"
#include <set>
#include "IMouseTool.h"
#include "CursorInfo.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Interface/GASSIControlSettingsSystem.h"


namespace GASS
{
	class SceneObject;
	class MouseToolController;
	class IMessage;
	typedef SPTR<SceneObject> SceneObjectPtr;
	typedef SPTR<IMessage> MessagePtr;


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
		virtual void MouseMoved(const MouseData &data, const SceneCursorInfo &info);
		virtual void MouseDown(const MouseData &data, const SceneCursorInfo &info);
		virtual void MouseUp(const MouseData &data, const SceneCursorInfo &info);
		virtual void Update(double delta);

		virtual std::string GetName() {return TID_TERRAIN;}
		virtual void Stop();
		virtual void Start();
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
		void OnSceneObjectSelected(ObjectSelectionChangedEventPtr message);
		void SendMessageRec(SceneObjectPtr obj,SceneObjectRequestMessagePtr msg);
		void OnInput(ControllSettingsMessagePtr message);
	
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
		Vec3 m_CursorPos;

	};
}