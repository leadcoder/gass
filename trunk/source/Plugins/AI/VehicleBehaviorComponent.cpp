#include "VehicleBehaviorComponent.h"
#include "AISceneManager.h"
#include "Plugins/Base/CoreMessages.h"
#include "Plugins/Game/GameMessages.h"



namespace GASS
{
	VehicleBehaviorComponent::VehicleBehaviorComponent(void) : m_Initialized(false),
		m_Formation(FT_UNCHANGED),
		m_Speed(9)
	{
		
	}	

	VehicleBehaviorComponent::~VehicleBehaviorComponent(void)
	{

	}

	void VehicleBehaviorComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("VehicleBehaviorComponent",new Creator<VehicleBehaviorComponent, IComponent>);
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("VehicleBehaviorComponent", OF_VISIBLE)));
		
		RegisterProperty<FormationTypeBinder>("Formation", &VehicleBehaviorComponent::GetFormation, &VehicleBehaviorComponent::SetFormation,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Formation",PF_VISIBLE  | PF_EDITABLE)));

		RegisterProperty<Float>("Speed", &VehicleBehaviorComponent::GetSpeed, &VehicleBehaviorComponent::SetSpeed,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Speed",PF_VISIBLE  | PF_EDITABLE)));
	}

	void VehicleBehaviorComponent::OnInitialize()
	{
		m_Initialized = true;
	}
}
