
#ifndef AI_TARGET__COMPONENT
#define AI_TARGET__COMPONENT

#include "Core/Utils/GASSFilePath.h"
#include "Sim/GASS.h"
#include "Plugins/Game/PlatformType.h"
#include "Plugins/Base/CoreMessages.h"
#include <set>

namespace GASS
{
	
	class VelocityTriggerComponent :  public Reflection<VelocityTriggerComponent,BaseSceneComponent> 
	{
		
	public:
		VelocityTriggerComponent(void);
		~VelocityTriggerComponent(void);
		static void RegisterReflection();
		virtual void OnInitialize();
	protected:
		void OnTriggerEnter(TriggerEnterMessagePtr message);
	private:
		double GetDelay() const;
		double GetVelocity() const;

		ADD_ATTRIBUTE(Vec2,RandomDelay)
		ADD_ATTRIBUTE(Vec2,RandomVelocity)
		ADD_ATTRIBUTE(bool,Enable)

		bool m_Initialized;
	};

	typedef SPTR<VelocityTriggerComponent> VelocityTriggerComponentPtr;
}
#endif