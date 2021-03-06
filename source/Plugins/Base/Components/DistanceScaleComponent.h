#pragma once
#include "Sim/GASSCommon.h"
#include "Modules/Editor/EditorSystem.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/GASSGraphicsMesh.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSGraphicsSystemMessages.h"

namespace GASS
{
	class DistanceScaleComponent : public Reflection<DistanceScaleComponent,BaseSceneComponent>
	{
	public:
		DistanceScaleComponent();
		~DistanceScaleComponent() override;
		static void RegisterReflection();
		void OnInitialize() override;
		void OnDelete() override;
	private:
		void OnTransformation(TransformationChangedEventPtr message);
		void OnCameraMoved(TransformationChangedEventPtr message);
		void OnCameraChanged(CameraChangedEventPtr message);
		void OnCameraParameter(CameraParameterRequestPtr message);

		float GetMaxDistance() const{return m_MaxDistance;}
		void SetMaxDistance(float value){m_MaxDistance =value;}
		float GetMinDistance() const{return m_MinDistance;}
		void SetMinDistance(float value){m_MinDistance =value;}
		bool GetScaleLocation()const {return  m_ScaleLocation;}
		void SetScaleLocation(bool value) {m_ScaleLocation = value;}
		
		//helpers
		void UpdateScale();

		Float m_LastDist;
		GASS::SceneObjectWeakPtr m_ActiveCameraObject;
		float m_MaxDistance;
		float m_MinDistance;
		bool m_ScaleLocation;
		float m_FOV;
	};
	typedef GASS_SHARED_PTR<DistanceScaleComponent> DistanceScaleComponentPtr;
}


