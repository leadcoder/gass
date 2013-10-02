
#ifndef AI_TARGET__COMPONENT
#define AI_TARGET__COMPONENT

#include "Sim/GASS.h"
#include "Core/Utils/GASSFilePath.h"
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
		ADD_PROPERTY(bool,Enable)
	protected:
		void OnTransChanged(TransformationNotifyMessagePtr);
		void OnActivate(DoorMessagePtr message);
		bool IsInside(SceneObjectWeakPtr obj) const;
		bool RemoveInside(SceneObjectWeakPtr obj);
	private:
		ADD_PROPERTY(bool,Debug)
		ADD_PROPERTY(Vec2,RandomDelay)
		ADD_PROPERTY(Vec2,RandomVelocity)
		void SetType(const std::string &value);
		std::string GetType() const;
		bool m_Initialized;
		Vec3 m_Position;
		Float m_CurrentTime;
		AITargetComponent::TargetType m_Type;
	};

	typedef SPTR<AITargetComponent> AITargetComponentPtr;
}
#endif