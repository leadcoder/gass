#include "VehicleBehaviorComponent.h"
#include "VehicleTriggerComponent.h"
#include "AISceneManager.h"
#include "Plugins/Base/CoreMessages.h"
#include "Plugins/Game/GameMessages.h"

namespace GASS
{
	
	std::vector<SceneObjectPtr> VehicleBehaviorComponentTriggerEnumeration(BaseReflectionObjectPtr obj)
	{
		SPTR<VehicleBehaviorComponent> comp = DYNAMIC_PTR_CAST<VehicleBehaviorComponent>(obj);
		return  comp->_GetTriggerEnumeration();
	}
	std::vector<SceneObjectPtr> VehicleBehaviorComponentSyncEnumeration(BaseReflectionObjectPtr obj)
	{
		SPTR<VehicleBehaviorComponent> comp = DYNAMIC_PTR_CAST<VehicleBehaviorComponent>(obj);
		return  comp->_GetSyncEnumeration();
	}

	std::vector<SceneObjectPtr>  VehicleBehaviorComponent::_GetSyncEnumeration() const
	{
		std::vector<SceneObjectPtr> ret;
		SceneObjectPtr so = GetSceneObject();
		if(so)
		{
			IComponentContainer::ComponentVector comps;
			so->GetScene()->GetRootSceneObject()->GetComponentsByClass<VehicleBehaviorComponent>(comps);
			for(int i = 0 ; i < comps.size();i++)
			{
				VehicleBehaviorComponentPtr comp = DYNAMIC_PTR_CAST<VehicleBehaviorComponent>(comps[i]);
				if(comp->GetSceneObject() && comp->GetSceneObject()->GetParentSceneObject() != GetSceneObject()->GetParentSceneObject())
				{
					//also check that we not already are in sync
					bool already_synced = false;
					SceneObjectRef sync_obj = comp->GetSynchronize();
					if(sync_obj.GetRefObject())
					{
						if(sync_obj.GetRefObject() == GetSceneObject())
						{
							already_synced = true;
						}
					}
					if(!already_synced)
					{
						SceneObjectPtr so = DYNAMIC_PTR_CAST<SceneObject>(comp->GetOwner());
						ret.push_back(so);
					}
				}
			}
		}
		return ret;
	}

	std::vector<SceneObjectPtr>  VehicleBehaviorComponent::_GetTriggerEnumeration() const
	{
		std::vector<SceneObjectPtr> ret;
		SceneObjectPtr so = GetSceneObject();
		if(so)
		{
			IComponentContainer::ComponentVector comps;
			so->GetScene()->GetRootSceneObject()->GetComponentsByClass<VehicleTriggerComponent>(comps);
			for(int i = 0 ; i < comps.size();i++)
			{
				VehicleTriggerComponentPtr comp = DYNAMIC_PTR_CAST<VehicleTriggerComponent>(comps[i]);
				if(comp->GetSceneObject())
				{
					SceneObjectPtr so = DYNAMIC_PTR_CAST<SceneObject>(comp->GetOwner());
					ret.push_back(so);
				}
			}
		}
		return ret;
	}

	VehicleBehaviorComponent::VehicleBehaviorComponent(void) : m_Initialized(false),
		m_Formation(FT_UNCHANGED),
		m_SpeedMode(ST_UNCHANGED),
		m_RegularSpeedValue(5),
		m_Radius(5),
		m_Delay(0,0),
		m_Complete(false)
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

		RegisterProperty<Vec2>("Delay", &VehicleBehaviorComponent::GetDelay, &VehicleBehaviorComponent::SetDelay,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Random time interval to wait at waypoint before going to next. (to equal values disable any random behavior) Unit is [s]",PF_VISIBLE  | PF_EDITABLE)));

		std::vector<std::string> radius_enumeration;
		//predefined radius
		radius_enumeration.push_back("1"); 
		radius_enumeration.push_back("2"); 
		radius_enumeration.push_back("5"); 
		radius_enumeration.push_back("10"); 
		radius_enumeration.push_back("20"); 

		RegisterProperty<Float>("WaypointRadius", &VehicleBehaviorComponent::GetWaypointRadius, &VehicleBehaviorComponent::SetWaypointRadius,
			StaticEnumerationPropertyMetaDataPtr(new StaticEnumerationPropertyMetaData("Radius used to consider waypoint reached [m]",PF_VISIBLE | PF_EDITABLE,radius_enumeration)));

		RegisterProperty<SceneObjectRef>("Synchronize", &VehicleBehaviorComponent::GetSynchronize, &VehicleBehaviorComponent::SetSynchronize,
			SceneObjectEnumerationProxyPropertyMetaDataPtr(new SceneObjectEnumerationProxyPropertyMetaData("Synchronize with this waypoint",PF_VISIBLE,VehicleBehaviorComponentSyncEnumeration)));

		RegisterVectorProperty<SceneObjectRef>("Triggers", &VehicleBehaviorComponent::GetTriggers, &VehicleBehaviorComponent::SetTriggers,
			SceneObjectEnumerationProxyPropertyMetaDataPtr(new SceneObjectEnumerationProxyPropertyMetaData("Synchronize with triggers",PF_VISIBLE,VehicleBehaviorComponentTriggerEnumeration,true)));
	}

	void VehicleBehaviorComponent::OnInitialize()
	{
		BaseSceneComponent::InitializeSceneObjectRef();
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
		return m_RegularSpeedValue;
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

	Float VehicleBehaviorComponent::GenerateRandomDelay() const
	{
		Float delay_span = m_Delay.y - m_Delay.x;
		Float norm_rand = (Float)rand()/(Float)RAND_MAX;
		return m_Delay.x + norm_rand * delay_span;;
	}

	void VehicleBehaviorComponent::Reset()
	{
		m_Complete = false;
	}

	void VehicleBehaviorComponent::SetComplete(bool value)
	{
		m_Complete = value;
	}

	bool VehicleBehaviorComponent::GetComplete() const
	{
		return m_Complete;
	}

	bool VehicleBehaviorComponent::IsSyncronized() const
	{
		if(m_Complete)
		{
			if(_CheckTriggers())
			{
				return _CheckWaypoints();
			}
			else
				return false;
		}
		else
			return false;
	}

	bool VehicleBehaviorComponent::_CheckWaypoints() const
	{
		if(m_Synchronize.GetRefObject())
		{
			VehicleBehaviorComponentPtr syn_comp = m_Synchronize.GetRefObject()->GetFirstComponentByClass<VehicleBehaviorComponent>();
			return syn_comp->IsSyncronized();
		}
		return true;
	}

	bool VehicleBehaviorComponent::_CheckTriggers() const
	{
		for(size_t i = 0; i < m_Triggers.size(); i++)
		{
			SceneObjectPtr so = m_Triggers[i].GetRefObject();
			if(so)
			{
				VehicleTriggerComponentPtr trigger_comp = so->GetFirstComponentByClass<VehicleTriggerComponent>();
				if(!trigger_comp->IsActive())
				{
					return false;
				}
			}
		}
		return true;
	}

	void VehicleBehaviorComponent::_UpdateMesh()
	{
		if(m_Initialized)
		{
			ColorRGBA color(0,1,0,1);
			GraphicsSubMeshPtr sub_mesh_data(new GraphicsSubMesh());
			sub_mesh_data->MaterialName = "";
			sub_mesh_data->Type = LINE_LIST;
					
			Vec3 wp_pos = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->GetWorldPosition();
					
			//Add line datat;
			for(size_t i = 0; i < m_Triggers.size(); i++)
			{
				SceneObjectPtr so = m_Triggers[i].GetRefObject();
				if(so)
				{
					sub_mesh_data->PositionVector.push_back(wp_pos);
					LocationComponentPtr location_comp = so->GetFirstComponentByClass<ILocationComponent>();
					sub_mesh_data->PositionVector.push_back(location_comp->GetWorldPosition());
				}
			}
			if(m_Synchronize.GetRefObject())
			{
				LocationComponentPtr location_comp = m_Synchronize.GetRefObject()->GetFirstComponentByClass<ILocationComponent>();
				sub_mesh_data->PositionVector.push_back(wp_pos);
				sub_mesh_data->PositionVector.push_back(location_comp->GetWorldPosition());
			}

			GraphicsMeshPtr mesh_data(new GraphicsMesh());
			mesh_data->SubMeshVector.push_back(sub_mesh_data);
			MessagePtr mesh_message(new ManualMeshDataMessage(mesh_data));
			GetSceneObject()->PostMessage(mesh_message);
		}
	}
}
