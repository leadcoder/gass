#pragma once
#include "Sim/GASSBaseSceneComponent.h"
#include "Plugins/Base/CoreMessages.h"
#include "Sim/GASSMeshData.h"
#include "Sim/Messages/GASSCoreSceneMessages.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"

namespace GASS
{
	class EditorModuleExport DistanceScaleComponent : public Reflection<DistanceScaleComponent,BaseSceneComponent>
	{
	public:
		DistanceScaleComponent();
		virtual ~DistanceScaleComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual void OnDelete();
	private:
		void OnCameraParameter(CameraParameterMessagePtr message);
		float GetMaxDistance() const{return m_MaxDistance;}
		void SetMaxDistance(float value){m_MaxDistance =value;}
		float GetMinDistance() const{return m_MinDistance;}
		void SetMinDistance(float value){m_MinDistance =value;}
		bool GetScaleLocation()const {return  m_ScaleLocation;}
		void SetScaleLocation(bool value) {m_ScaleLocation = value;}

	
		void OnTransformation(TransformationNotifyMessagePtr message);
		void OnCameraMoved(TransformationNotifyMessagePtr message);
		void OnChangeCamera(ChangeCameraRequestPtr message);
		void UpdateScale();
		void OnSelectedTransformation(TransformationNotifyMessagePtr message);
		void OnWorldPosition(WorldPositionMessagePtr message);
	
		//helpers
		Float m_LastDist;
		GASS::SceneObjectWeakPtr m_ActiveCameraObject;
		

		float m_MaxDistance;
		float m_MinDistance;
		bool m_ScaleLocation;
	};

	typedef boost::shared_ptr<DistanceScaleComponent> DistanceScaleComponentPtr;
}


