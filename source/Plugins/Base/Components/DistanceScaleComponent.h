#pragma once
#include "Sim/GASSCommon.h"
#include "Modules/Editor/EditorSystem.h"
#include "Sim/GASSComponent.h"
#include "Sim/GASSGraphicsMesh.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSGraphicsSystemMessages.h"

namespace GASS
{
	class DistanceScaleComponent : public Reflection<DistanceScaleComponent,Component>
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
	
		float GetMaxDistance() const{return m_MaxDistance;}
		void SetMaxDistance(float value){m_MaxDistance =value;}
		float GetMinDistance() const{return m_MinDistance;}
		void SetMinDistance(float value){m_MinDistance =value;}
		bool GetScaleLocation()const {return  m_ScaleLocation;}
		void SetScaleLocation(bool value) {m_ScaleLocation = value;}
		
		//helpers
		void UpdateScale();

		Float m_LastDist{0};
		GASS::SceneObjectWeakPtr m_ActiveCameraObject;
		float m_MaxDistance{30000.0f};
		float m_MinDistance{0.1f};
		bool m_ScaleLocation{false};
		float m_FOV{45.0f};
	};
	using DistanceScaleComponentPtr = std::shared_ptr<DistanceScaleComponent>;
}


