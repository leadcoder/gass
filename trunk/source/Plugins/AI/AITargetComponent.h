
#ifndef AI_TARGET__COMPONENT
#define AI_TARGET__COMPONENT

#include "Core/Utils/GASSFilePath.h"
#include "Sim/GASS.h"
#include "Plugins/Game/PlatformType.h"
#include "AIMessages.h"

#include <set>

namespace GASS
{
	
	class AITargetComponent :  public Reflection<AITargetComponent,BaseSceneComponent> 
	{
		
	public:
		enum TargetType
		{
			RANDOM_TARGET,
			RANDOM_RESPAWN_TARGET,
			PLATFORM_TARGET,
			VELOCITY_TARGET,
			UNDEFINED_TARGET
		};

		AITargetComponent(void);
		~AITargetComponent(void);
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual void SceneManagerTick(double delta_time);

		double GetDelay() const;
		double GetVelocity() const;
		AITargetComponent::TargetType GetTargetType() {return m_Type;}
		ADD_ATTRIBUTE(bool,Enable)
	protected:
		void OnTransChanged(TransformationNotifyMessagePtr);
		void OnActivate(DoorMessagePtr message);
		bool IsInside(SceneObjectWeakPtr obj) const;
		bool RemoveInside(SceneObjectWeakPtr obj);
	private:
		ADD_ATTRIBUTE(bool,Debug)
		ADD_ATTRIBUTE(Vec2,RandomDelay)
		ADD_ATTRIBUTE(Vec2,RandomVelocity)
		void SetType(const std::string &value);
		std::string GetType() const;
		bool m_Initialized;
		Vec3 m_Position;
		Float m_CurrentTime;
		AITargetComponent::TargetType m_Type;
	};

	typedef boost::shared_ptr<AITargetComponent> AITargetComponentPtr;
}
#endif