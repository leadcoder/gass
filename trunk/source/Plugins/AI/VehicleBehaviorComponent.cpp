#include "VehicleBehaviorComponent.h"
#include "AISceneManager.h"
#include "Plugins/Base/CoreMessages.h"
#include "Plugins/Game/GameMessages.h"



namespace GASS
{
	VehicleBehaviorComponent::VehicleBehaviorComponent(void) : m_Initialized(false)
	{
		
	}	

	VehicleBehaviorComponent::~VehicleBehaviorComponent(void)
	{

	}

	void VehicleBehaviorComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("VehicleBehaviorComponent",new Creator<VehicleBehaviorComponent, IComponent>);
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("VehicleBehaviorComponent", OF_VISIBLE)));
	}

	void VehicleBehaviorComponent::OnInitialize()
	{
		m_Initialized = true;
	}
}
