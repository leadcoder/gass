
#ifndef SENSOR_COMPONENT
#define SENSOR_COMPONENT

#include "Sim/GASSCommon.h"
#include "Core/Utils/GASSFilePath.h"
#include "Sim/GASS.h"

#include "Sim/Messages/GASSSensorMessages.h"
#include "Sim/GASSPlatformType.h"

namespace GASS
{
	class SignatureComponent;
	typedef WPTR<SignatureComponent> SignatureComponentWeakPtr;

	class SensorComponent :  public Reflection<SensorComponent,BaseSceneComponent> 
	{
	public:
		SensorComponent(void);
		~SensorComponent(void);
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual void SceneManagerTick(double delta_time);
	protected:
		void OnTransChanged(TransformationChangedEventPtr);
		Float GetDetectionDistance(PlatformType signature, Float radar_cross_section);
		void OnSceneObjectCreated(PostSceneObjectInitializedEventPtr message);
		bool IsNewTarget(SignatureComponentWeakPtr sig) const;
		bool RemoveTarget(SignatureComponentWeakPtr sig);
	private:
		ADD_PROPERTY(Float,DefaultMaxDetectionDistance)
		ADD_PROPERTY(Float,UpdateFrequency)
		ADD_PROPERTY(bool,Debug)

		void Update();
		bool m_Initialized;
		Vec3 m_Position;
		Float m_CurrentTime;
		std::vector<SignatureComponentWeakPtr> m_AllObjects;
		std::map<SignatureComponentWeakPtr,DetectionData> m_DetectedObjects;
	};
	typedef SPTR<SensorComponent> SensorComponentPtr;
}
#endif