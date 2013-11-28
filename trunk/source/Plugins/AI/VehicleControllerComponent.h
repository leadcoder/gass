
#ifndef AI_VEHICLE_CONTROLLER_COMPONENT
#define AI_VEHICLE_CONTROLLER_COMPONENT

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
	private:
		void SetVehicleTemplate(const std::string &template_name);
		std::string GetVehicleTemplate() const;
		bool m_Initialized;
		SceneObjectWeakPtr m_Vehicle;
		std::string  m_VehicleTemplate;
	};

	typedef SPTR<VehicleControllerComponent> VehicleControllerComponentPtr;
}
#endif