#pragma once
#include "Sim/GASSCommon.h"
#include "Modules/Editor/EditorSceneManager.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/GASSGraphicsMesh.h"
#include "Sim/Messages/GASSGraphicsSystemMessages.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"

namespace GASS
{

	enum GizmoType
	{
		GT_AXIS,
		GT_PLANE
	};

	START_ENUM_BINDER(GizmoType, GizmoTypeBinder)
		BIND(GT_AXIS)
		BIND(GT_PLANE)
	END_ENUM_BINDER(GizmoType, GizmoTypeBinder)

	class EditorModuleExport GizmoComponent : public Reflection<GizmoComponent,BaseSceneComponent>
	{
	public:
		GizmoComponent();
		~GizmoComponent() override;
		static void RegisterReflection();
		void OnInitialize() override;
		void OnDelete() override;
		Vec3 GetPosition(const Ray &ray);
		Quaternion GetRotation(Float detla);
		void SetActive(bool active)  {m_Active =active;}
		bool GetActive() const {return m_Active;}
		GizmoEditMode GetMode() const {return m_Mode;}
		void SceneManagerTick(double /*delta_time*/) override;
	private:
		//getters/setter
		GizmoTypeBinder GetType() const { return m_Type; }
		void SetType(const GizmoTypeBinder &value) { m_Type = value; }
		float GetSize() const { return m_Size; }
		void SetSize(float value) { m_Size = value; }
		ColorRGBA GetColor() const { return m_Color; }
		void SetColor(const ColorRGBA &value) { m_Color = value; }
		
		//Events
		void OnLocationLoaded(LocationLoadedEventPtr message);
		void OnNewCursorInfo(CursorMovedOverSceneEventPtr message);
		void OnTransformation(TransformationChangedEventPtr message);
		void OnCameraChanged(CameraChangedEventPtr message);

		//editor selection changed
		void OnSelectionChanged(EditorSelectionChangedEventPtr message);
		void OnSelectedTransformation(TransformationChangedEventPtr message);
		void OnEditMode(EditModeChangedEventPtr message);
	
		//Helpers
		void Move(const Vec3 &pos);
		void Rotate(const Quaternion &pos);
		void Scale(const Vec3 &scale);
		void UpdateScale();
		void BuildMesh();
		Vec3 ProjectPointOnAxis(const Vec3 &axis_origin, const Vec3 &axis_dir, const Vec3 &p) const;
		Float SnapValue(Float value, Float snap);
		SceneObjectPtr GetFirstSelected();
		void SetSelection(const std::vector<SceneObjectWeakPtr> &selection);

		Quaternion m_BaseRot;
		ColorRGBA m_Color;
		float m_Size{5};
		GizmoTypeBinder m_Type;
		
		Float m_LastDist{0};
		bool m_Highlight{true};
		GASS::SceneObjectWeakPtr m_ActiveCameraObject;
		std::vector<SceneObjectWeakPtr> m_Selection;
		GizmoEditMode m_Mode{GM_LOCAL};
		Float m_GridDist{1.0};
		bool m_Active{false};
		EditorSceneManagerPtr m_EditorSceneManager;
		std::string m_RegularMat;
		std::string m_HighlightMat;
		GASS::Vec3 m_PreviousPos;
		Quaternion m_PreviousRot;
		bool m_TrackTransformation{true};
		bool m_TrackSelectedTransform{true};
	};

	using GizmoComponentPtr = std::shared_ptr<GizmoComponent>;
}


