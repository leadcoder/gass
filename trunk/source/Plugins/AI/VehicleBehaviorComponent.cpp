#include "VehicleBehaviorComponent.h"
#include "AISceneManager.h"
#include "Plugins/Base/CoreMessages.h"
#include "Plugins/Game/GameMessages.h"



namespace GASS
{
	VehicleBehaviorComponent::VehicleBehaviorComponent(void) : m_Initialized(false),
		m_Formation(FT_UNCHANGED),
		m_SpeedMode(ST_UNCHANGED),
		m_RegularSpeedValue(5),
		m_Radius(5)
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
			EnumerationProxyPropertyMetaDataPtr(new EnumerationProxyPropertyMetaData("Formation type",PF_VISIBLE,&FormationTypeBinder::GetStringEnumeration)));

		RegisterProperty<SpeedTypeBinder>("SpeedMode", &VehicleBehaviorComponent::GetSpeedMode, &VehicleBehaviorComponent::SetSpeedMode,
			EnumerationProxyPropertyMetaDataPtr(new EnumerationProxyPropertyMetaData("Speed type, the actual speed based on the RegularSpeedValue attribute",PF_VISIBLE,&SpeedTypeBinder::GetStringEnumeration)));

		RegisterProperty<Float>("RegularSpeedValue", &VehicleBehaviorComponent::GetRegularSpeedValue, &VehicleBehaviorComponent::SetRegularSpeedValue,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Speed value used in regular speed mode, other speed modes divided or multiply this value. [m/s]",PF_VISIBLE  | PF_EDITABLE)));


		
		std::vector<std::string> radius_enumeration;
		//predefined radius
		radius_enumeration.push_back("1"); 
		radius_enumeration.push_back("2"); 
		radius_enumeration.push_back("5"); 
		radius_enumeration.push_back("10"); 
		radius_enumeration.push_back("20"); 

		RegisterProperty<Float>("WaypointRadius", &VehicleBehaviorComponent::GetWaypointRadius, &VehicleBehaviorComponent::SetWaypointRadius,
			StaticEnumerationPropertyMetaDataPtr(new StaticEnumerationPropertyMetaData("Radius used to consider waypoint reached [m]",PF_VISIBLE | PF_EDITABLE,radius_enumeration)));
			//BasePropertyMetaDataPtr(new BasePropertyMetaData("Radius used to consider waypoint reached [m]",PF_VISIBLE  | PF_EDITABLE)));
	}

	void VehicleBehaviorComponent::OnInitialize()
	{
		m_Initialized = true;
		SetWaypointRadius(m_Radius);
	}

	Float VehicleBehaviorComponent::GetSpeed() const
	{
		switch(m_SpeedMode.GetValue())
		{
		case ST_SLOW:
			return m_RegularSpeedValue*0.5;
			break;
		case ST_REGULAR:
			return m_RegularSpeedValue;
			break;
		case ST_FAST:
			return m_RegularSpeedValue*2;
			break;
		}
		return 0;
	}

	void VehicleBehaviorComponent::SetWaypointRadius(Float radius)
	{
		m_Radius = radius;
		if(m_Initialized)
		{
			BaseSceneComponentPtr sphere_comp  = DYNAMIC_PTR_CAST<BaseSceneComponent>(GetSceneObject()->GetFirstComponentByClass("SphereGeometryComponent"));
			if(sphere_comp)
				sphere_comp->SetPropertyByType("Radius",m_Radius);
		}
		
		//update sphere size
	}

	Float VehicleBehaviorComponent::GetWaypointRadius() const
	{
		return m_Radius;
	}
	
}
