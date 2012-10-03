
#ifndef SENSOR_COMPONENT
#define SENSOR_COMPONENT

#include "Core/Utils/GASSFilePath.h"
#include "Sim/GASS.h"
#include "Plugins/Game/GameMessages.h"
#include "Plugins/Game/PlatformType.h"

namespace GASS
{
	class SignatureComponent;
	typedef boost::weak_ptr<SignatureComponent> SignatureComponentWeakPtr;

	class SensorComponent :  public Reflection<SensorComponent,BaseSceneComponent> 
	{
	public:
		SensorComponent(void);
		~SensorComponent(void);
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual void SceneManagerTick(double delta_time);
		//ITaskListener interface
		//void Update(double delta);
	protected:
		void OnLoad(LoadComponentsMessagePtr message);
		void OnUnload(MessagePtr message);
		void OnTransChanged(TransformationNotifyMessagePtr);
		Float GetDetectionDistance(PlatformType signature, Float radar_cross_section);
		void OnSceneObjectCreated(PostSceneObjectInitializedPtr message);
		bool IsNewTarget(SignatureComponentWeakPtr sig) const;
		bool RemoveTarget(SignatureComponentWeakPtr sig);
	private:
		ADD_ATTRIBUTE(Float,DefaultMaxDetectionDistance)
		ADD_ATTRIBUTE(Float,UpdateFrequency)
		ADD_ATTRIBUTE(bool,Debug)

		void Update();
		bool m_Initialized;
		Vec3 m_Position;
		Float m_CurrentTime;
		std::vector<SignatureComponentWeakPtr> m_AllObjects;
		std::map<SignatureComponentWeakPtr,DetectionData> m_DetectedObjects;
	};
	typedef boost::shared_ptr<SensorComponent> SensorComponentPtr;
}
#endif