#pragma once
#include "Sim/GASSCommon.h"
#include "Modules/Editor/EditorSystem.h"
#include "Modules/Editor/EditorSceneManager.h"
#include "Modules/Editor/ToolSystem/MouseToolController.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Plugins/Base/CoreMessages.h"
#include "Sim/GASSGraphicsMesh.h"
#include "Sim/Messages/GASSCoreSceneMessages.h"
#include "Sim/Messages/GASSGraphicsSceneMessages.h"
#include "Sim/Messages/GASSGraphicsSystemMessages.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"

#define GT_AXIS "GT_AXIS"
#define GT_PLANE "GT_PLANE"
#define GT_GRID "GT_GRID"
#define GT_FIXED_GRID "GT_FIXED_GRID"

namespace GASS
{
	class EditorModuleExport GizmoComponent : public Reflection<GizmoComponent,BaseSceneComponent>
	{
	public:
		GizmoComponent();
		virtual ~GizmoComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual void OnDelete();		
		Vec3 GetPosition(const Vec3 &ray_start, const Vec3 &ray_dir);
		Quaternion GetRotation(float detla);
		void SetActive(bool active)  {m_Active =active;}
		bool GetActive() const {return m_Active;}
		GizmoEditMode GetMode() const {return m_Mode;}
	private:
		void OnChangeGridRequest(ChangeGridRequestPtr message);
		void OnCameraParameter(CameraParameterMessagePtr message);
		void OnLocationLoaded(LocationLoadedMessagePtr message);
		void OnNewCursorInfo(CursorMovedOverSceneEventPtr message);
		void OnTransformation(TransformationNotifyMessagePtr message);
		void OnCameraMoved(TransformationNotifyMessagePtr message);
		void OnCameraChanged(CameraChangedEventPtr message);
		void OnSceneObjectSelected(ObjectSelectionChangedEventPtr message);
		void OnSelectedTransformation(TransformationNotifyMessagePtr message);
		void OnWorldPosition(WorldPositionMessagePtr message);
		void OnEditMode(EditModeChangedEventPtr message);

		void SetSelection(SceneObjectPtr  object);
		void BuildMesh();
		std::string GetType() const {return m_Type;}
		void SetType(const std::string &value) {m_Type = value;}
		float GetSize() const{return m_Size;}
		void SetSize(float value){m_Size =value;}
		ColorRGBA GetColor() const{return m_Color;}
		void SetColor(const ColorRGBA &value){m_Color =value;}
		void UpdateScale();
		Vec3 ProjectPointOnAxis(const Vec3 &axis_origin, const Vec3 &axis_dir, const Vec3 &p);
		Float GizmoComponent::SnapValue(Float value, Float snap);
	
		Quaternion m_BaseRot;
		GraphicsMeshPtr m_MeshData;	
		ColorRGBA m_Color;
		float m_Size;
		std::string m_Type;

		//helpers
		Float m_LastDist;
		bool m_Highlight;
		GASS::SceneObjectWeakPtr m_ActiveCameraObject;
		GASS::SceneObjectWeakPtr m_SelectedObject;
		GizmoEditMode m_Mode;
		Float m_GridDist;
		bool m_Active;
		EditorSceneManagerPtr m_EditorSceneManager;
		std::string m_RegularMat;
		std::string m_HighlightMat;

	};

	typedef SPTR<GizmoComponent> GizmoComponentPtr;
}


