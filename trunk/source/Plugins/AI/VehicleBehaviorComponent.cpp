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
					/*SceneObjectRef sync_obj = comp->GetSynchronize();
					if(sync_obj.GetRefObject())
					{
						if(sync_obj.GetRefObject() == GetSceneObject())
						{
							already_synced = true;
						}
					}*/
					if(!already_synced)
					{
						SceneObjectPtr so = DYNAMIC_PTR_CAST<SceneObject>(comp->GetOwner());
						ret.push_back(so);
					}
				}
			}
		}
		//add null object!
		//ret.push_back(SceneObjectPtr());
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

		RegisterVectorProperty<SceneObjectRef>("SynchronizedWaypoints", &VehicleBehaviorComponent::GetSynchronizedWaypoints, &VehicleBehaviorComponent::SetSynchronizedWaypoints,
			SceneObjectEnumerationProxyPropertyMetaDataPtr(new SceneObjectEnumerationProxyPropertyMetaData("Synchronize with this waypoint",PF_VISIBLE,VehicleBehaviorComponentSyncEnumeration,true)));

		RegisterVectorProperty<SceneObjectRef>("Triggers", &VehicleBehaviorComponent::GetTriggers, &VehicleBehaviorComponent::SetTriggers,
			SceneObjectEnumerationProxyPropertyMetaDataPtr(new SceneObjectEnumerationProxyPropertyMetaData("Synchronize with triggers",PF_VISIBLE,VehicleBehaviorComponentTriggerEnumeration,true)));
	}

	void VehicleBehaviorComponent::OnInitialize()
	{
		BaseSceneComponent::InitializeSceneObjectRef();

		//Check that all triggers are valid!
		std::vector<SceneObjectRef>::iterator iter = m_Triggers.begin();
		while(iter != m_Triggers.end())
		{
			SceneObjectPtr so = (*iter).GetRefObject();
			if(so)
			{
				iter++;
			}
			else
				iter = m_Triggers.erase(iter);

		}

		SceneManagerListenerPtr listener = shared_from_this();
		GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<AISceneManager>()->Register(listener);
		m_ConnectionLines = GetSceneObject()->GetChildByID("CONNECTION_LINES");
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

	void VehicleBehaviorComponent::SetSynchronizedWaypoints(const std::vector<SceneObjectRef> &value) 
	{
		m_SynchronizedWaypoints = value;

		for(size_t  i = 0;  i < m_SynchronizedWaypoints.size(); i++)
		{
			SceneObjectRef ref = m_SynchronizedWaypoints[i];
			if(ref.IsValid())
			{
				VehicleBehaviorComponentPtr syn_comp = ref.GetRefObject()->GetFirstComponentByClass<VehicleBehaviorComponent>();
				std::vector<SceneObjectRef> new_values = syn_comp->GetSynchronizedWaypoints();
				bool present = false;
				for(size_t  j = 0;  j < new_values.size(); j++)
				{
					if(new_values[j].GetRefObject() == GetSceneObject())
						present = true;
				}
				if(!present)
				{

					new_values.push_back(SceneObjectRef(GetSceneObject()));
					syn_comp->SetSynchronizedWaypoints(new_values);
				}
			}
		}
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
			//if(_CheckTriggers())
			return _CheckWaypoints();
		}
		else
			return false;
	}

	bool VehicleBehaviorComponent::_CheckWaypoints() const
	{
		bool sync = true;
		for(size_t  i = 0;  i < m_SynchronizedWaypoints.size(); i++)
		{
			SceneObjectRef ref = m_SynchronizedWaypoints[i];
			if(ref.IsValid())
			{
				VehicleBehaviorComponentPtr syn_comp = ref.GetRefObject()->GetFirstComponentByClass<VehicleBehaviorComponent>();
				sync = sync && syn_comp->GetComplete();
			}
		}
		return sync;
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


	void VehicleBehaviorComponent::SceneManagerTick(double delta_time)
	{
		_UpdateConnectionLines();
	}

	void VehicleBehaviorComponent::_UpdateConnectionLines()
	{
		if(m_Initialized)
		{
			SceneObjectPtr line_obj = _GetConnectionLines();
			if(line_obj)
			{
				ColorRGBA color(0,1,0,1);
				Float arrow_size = 1;
				GraphicsSubMeshPtr sub_mesh_data(new GraphicsSubMesh());
				sub_mesh_data->MaterialName = "";
				sub_mesh_data->Type = LINE_LIST;
				Vec3 wp_pos = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->GetWorldPosition();

				//offset 1dm
				wp_pos.y += 0.1;

				//Add line datat;
				for(size_t i = 0; i < m_Triggers.size(); i++)
				{
					SceneObjectPtr so = m_Triggers[i].GetRefObject();
					if(so)
					{
						LocationComponentPtr location_comp = so->GetFirstComponentByClass<ILocationComponent>();
						Vec3 end_pos = location_comp->GetWorldPosition();
						//offset 1dm
						end_pos.y += 0.1;
						sub_mesh_data->AddArrow(wp_pos,end_pos,arrow_size,color);
					}
				}
				for(size_t i = 0; i < m_SynchronizedWaypoints.size(); i++)
				{
					SceneObjectPtr so = m_SynchronizedWaypoints[i].GetRefObject();
					if(so)
					{
						LocationComponentPtr location_comp = so->GetFirstComponentByClass<ILocationComponent>();
						Vec3 end_pos = location_comp->GetWorldPosition();
						//offset 1dm
						end_pos.y += 0.1;
						sub_mesh_data->AddArrow(wp_pos,end_pos,arrow_size,color);
					}
				}

				/*if(m_Synchronize.GetRefObject())
				{
					LocationComponentPtr location_comp = m_Synchronize.GetRefObject()->GetFirstComponentByClass<ILocationComponent>();
					Vec3 end_pos = location_comp->GetWorldPosition();
					//offset 1dm
					end_pos.y += 0.1;
					sub_mesh_data->AddArrow(wp_pos,end_pos,arrow_size,color);
				}*/

				GraphicsMeshPtr mesh_data(new GraphicsMesh());
				mesh_data->SubMeshVector.push_back(sub_mesh_data);
				MessagePtr mesh_message(new ManualMeshDataMessage(mesh_data));
				line_obj->PostMessage(mesh_message);
			}
		}
	}
}
