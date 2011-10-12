
#ifndef SENSOR_COMPONENT
#define SENSOR_COMPONENT

#include "Core/Utils/FilePath.h"
#include "Sim/GASS.h"
#include "Plugins/Game/GameMessages.h"
#include "Plugins/Game/PlatformType.h"

namespace GASS
{
	class SignatureComponent;
	typedef boost::weak_ptr<SignatureComponent> SignatureComponentWeakPtr;

	class SensorComponent :  public Reflection<SensorComponent,BaseSceneComponent> , public ITaskListener
	{
	public:
		SensorComponent(void);
		~SensorComponent(void);
		static void RegisterReflection();
		void OnCreate();

		//ITaskListener interface
		void Update(double delta);
		TaskGroup GetTaskGroup() const;
	protected:
		void OnLoad(MessagePtr message);
		void OnUnload(MessagePtr message);
		void OnTransChanged(TransformationNotifyMessagePtr);
		Float GetDetectionDistance(PlatformType signature, Float radar_cross_section);
		void OnSceneObjectCreated(SceneObjectCreatedNotifyMessagePtr message);
		bool IsNewTarget(SignatureComponentWeakPtr sig) const;
		bool RemoveTarget(SignatureComponentWeakPtr sig);
	private:
		ADD_ATTRIBUTE(Float,DefaultMaxDetectionDistance)
		ADD_ATTRIBUTE(Float,UpdateFrequency)

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