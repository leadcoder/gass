
#ifndef AI_TARGET__COMPONENT
#define AI_TARGET__COMPONENT

#include "Sim/GASS.h"
#include "Core/Utils/GASSFilePath.h"
#include "Plugins/Game/PlatformType.h"
#include "Plugins/Base/CoreMessages.h"
#include <set>

namespace GASS
{
	
	class VehicleControllerComponent :  public Reflection<VehicleControllerComponent,BaseSceneComponent> 
	{
		
	public:
		VehicleControllerComponent(void);
		~VehicleControllerComponent(void);
		static void RegisterReflection();
		virtual void OnInitialize();
	protected:
		void OnTriggerEnter(TriggerEnterMessagePtr message);
	private:
		double GetDelay() const;
		double GetVelocity() const;

		ADD_PROPERTY(Vec2,RandomDelay)
		ADD_PROPERTY(Vec2,RandomVelocity)
		ADD_PROPERTY(bool,Enable)

		bool m_Initialized;
	};

	typedef SPTR<VehicleControllerComponent> VehicleControllerComponentPtr;
}
#endif