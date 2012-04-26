#pragma once
#include "Sim/Components/GASSBaseSceneComponent.h"
#include "Plugins/Base/CoreMessages.h"
#include "Sim/Components/Graphics/GASSMeshData.h"
#include "Sim/Scene/GASSCoreSceneMessages.h"
#include "Sim/Scene/GASSGraphicsSceneMessages.h"
#include "Sim/Scene/GASSGraphicsSceneObjectMessages.h"

namespace GASS
{
	class EditorModuleExport GizmoComponent : public Reflection<GizmoComponent,BaseSceneComponent>
	{
	public:
		GizmoComponent();
		virtual ~GizmoComponent();
		static void RegisterReflection();
		virtual void OnCreate();
		
		Vec3 GetPosition(const Vec3 &ray_start, const Vec3 &ray_dir);
		Quaternion GetRotation(float detla);
		void SetActive(bool active)  {m_Active =active;}
		bool GetActive() const {return m_Active;}
		std::string GetSpaceMode() const {return m_Mode;}
	private:
		void BuildMesh();
		void OnSnapModeMessage(SnapModeMessagePtr message);
		void OnSnapSettingsMessage(SnapSettingsMessagePtr message);
		void OnGridMessage(GridMessagePtr message);
		void OnCameraParameter(CameraParameterMessagePtr message);
		std::string GetType() const {return m_Type;}
		void SetType(const std::string &value) {m_Type = value;}
		void OnLoad(LoadCoreComponentsMessagePtr message);
		void OnUnload(UnloadComponentsMessagePtr message);
		float GetSize() const{return m_Size;}
		void SetSize(float value){m_Size =value;}
		Vec4 GetColor() const{return m_Color;}
		void SetColor(const Vec4 &value){m_Color =value;}
		void OnSceneObjectSelected(ObjectSelectedMessagePtr message);
		void OnNewCursorInfo(CursorMoved3DMessagePtr message);
		void OnTransformation(TransformationNotifyMessagePtr message);
		void OnCameraMoved(TransformationNotifyMessagePtr message);
		void OnChangeCamera(ChangeCameraMessagePtr message);
		void UpdateScale();
		void OnSelectedTransformation(TransformationNotifyMessagePtr message);
		void OnWorldPosition(WorldPositionMessagePtr message);
		Vec3 ProjectPointOnAxis(const Vec3 &axis_origin, const Vec3 &axis_dir, const Vec3 &p);
		void OnEditMode(EditModeMessagePtr message);
		Float GizmoComponent::SnapValue(Float value, Float snap);
	
		Quaternion m_BaseRot;
		ManualMeshDataPtr m_MeshData;	

		Vec4 m_Color;
		float m_Size;
		std::string m_Type;

		//helpers
		Float m_LastDist;
		bool m_Highlight;
		GASS::SceneObjectWeakPtr m_ActiveCameraObject;
		GASS::SceneObjectWeakPtr m_SelectedObject;
		std::string m_Mode;
		Float m_GridDist;
		bool m_Active;
	};

	typedef boost::shared_ptr<GizmoComponent> GizmoComponentPtr;
}


