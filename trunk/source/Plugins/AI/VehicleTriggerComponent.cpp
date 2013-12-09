#include "VehicleTriggerComponent.h"
#include "VehicleControllerComponent.h"
#include "VehicleGroupControllerComponent.h"
#include "AISceneManager.h"
#include "Plugins/Base/CoreMessages.h"

namespace GASS
{
	std::vector<SceneObjectPtr> VehicleTriggerComponentControllerEnumeration(BaseReflectionObjectPtr obj)
	{
		SPTR<VehicleTriggerComponent> comp = DYNAMIC_PTR_CAST<VehicleTriggerComponent>(obj);
		return  comp->_GetControllerEnumeration();
	}

	std::vector<SceneObjectPtr>  VehicleTriggerComponent::_GetControllerEnumeration() const
	{
		std::vector<SceneObjectPtr> ret;
		SceneObjectPtr so = GetSceneObject();
		if(so)
		{
			IComponentContainer::ComponentVector comps;
			so->GetScene()->GetRootSceneObject()->GetComponentsByClass<VehicleControllerComponent>(comps);
			for(int i = 0 ; i < comps.size();i++)
			{
				VehicleControllerComponentPtr comp = DYNAMIC_PTR_CAST<VehicleControllerComponent>(comps[i]);
				if(comp->GetSceneObject())
				{
					SceneObjectPtr so = DYNAMIC_PTR_CAST<SceneObject>(comp->GetOwner());
					ret.push_back(so);
				}
			}
		}
		return ret;
	}

	std::vector<SceneObjectPtr> VehicleTriggerComponentGroupEnumeration(BaseReflectionObjectPtr obj)
	{
		SPTR<VehicleTriggerComponent> comp = DYNAMIC_PTR_CAST<VehicleTriggerComponent>(obj);
		return  comp->_GetGroupEnumeration();
	}

	std::vector<SceneObjectPtr>  VehicleTriggerComponent::_GetGroupEnumeration() const
	{
		std::vector<SceneObjectPtr> ret;
		SceneObjectPtr so = GetSceneObject();
		if(so)
		{
			IComponentContainer::ComponentVector comps;
			so->GetScene()->GetRootSceneObject()->GetComponentsByClass<VehicleGroupControllerComponent>(comps);
			for(int i = 0 ; i < comps.size();i++)
			{
				VehicleGroupControllerComponentPtr comp = DYNAMIC_PTR_CAST<VehicleGroupControllerComponent>(comps[i]);
				if(comp->GetSceneObject())
				{
					SceneObjectPtr so = DYNAMIC_PTR_CAST<SceneObject>(comp->GetOwner());
					ret.push_back(so);
				}
			}
		}
		return ret;
	}



	VehicleTriggerComponent::VehicleTriggerComponent(void) : m_Initialized(false),
		m_Active(false),
		m_Repeatedly(false),
		m_Update(false),
		m_Strict(false),
		m_AreaType(TAT_ELLIPSOID),
		m_AreaSize(40,40,10)
	{

	}	

	VehicleTriggerComponent::~VehicleTriggerComponent(void)
	{

	}

	void VehicleTriggerComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("VehicleTriggerComponent",new Creator<VehicleTriggerComponent, IComponent>);
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("VehicleTriggerComponent", OF_VISIBLE)));
		RegisterProperty<TriggerAreaTypeBinder>("AreaType", &VehicleTriggerComponent::GetAreaType, &VehicleTriggerComponent::SetAreaType,
			EnumerationProxyPropertyMetaDataPtr(new EnumerationProxyPropertyMetaData("Area type",PF_VISIBLE,&TriggerAreaTypeBinder::GetStringEnumeration)));
		RegisterProperty<Vec3>("AreaSize", &VehicleTriggerComponent::GetAreaSize, &VehicleTriggerComponent::SetAreaSize,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Area size",PF_VISIBLE  | PF_EDITABLE)));

		RegisterProperty<bool>("Repeatedly", &VehicleTriggerComponent::GetRepeatedly, &VehicleTriggerComponent::SetRepeatedly,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Choose if this trigger should be active repeatedly or once",PF_VISIBLE  | PF_EDITABLE)));
		RegisterProperty<bool>("Present", &VehicleTriggerComponent::GetPresent, &VehicleTriggerComponent::SetPresent,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Choose if this trigger should be active when objects are inside or out side area",PF_VISIBLE  | PF_EDITABLE)));

		RegisterProperty<bool>("Strict", &VehicleTriggerComponent::GetStrict, &VehicleTriggerComponent::SetStrict,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Choose if this trigger should be active when all or someone is trus",PF_VISIBLE  | PF_EDITABLE)));

		RegisterVectorProperty<SceneObjectRef>("ActivationGroups", &VehicleTriggerComponent::GetActivationGroups, &VehicleTriggerComponent::SetActivationGroups,
			SceneObjectEnumerationProxyPropertyMetaDataPtr(new SceneObjectEnumerationProxyPropertyMetaData("Groups",PF_VISIBLE,VehicleTriggerComponentGroupEnumeration,true)));

		RegisterVectorProperty<SceneObjectRef>("ActivationControllers", &VehicleTriggerComponent::GetActivationControllers, &VehicleTriggerComponent::SetActivationControllers,
			SceneObjectEnumerationProxyPropertyMetaDataPtr(new SceneObjectEnumerationProxyPropertyMetaData("Controllers",PF_VISIBLE,VehicleTriggerComponentControllerEnumeration,true)));

	}

	void VehicleTriggerComponent::OnInitialize()
	{

		GetSceneObject()->RegisterForMessage(REG_TMESS(VehicleTriggerComponent::OnTransformation,TransformationNotifyMessage,0));

		SceneManagerListenerPtr listener = shared_from_this();
		GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<AISceneManager>()->Register(listener);
		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(VehicleTriggerComponent::OnScenarioEvent,ScenarioStateRequest,0));
		m_Initialized = true;
		//update mesh
		_UpdateArea();
	}

	void VehicleTriggerComponent::OnTransformation(TransformationNotifyMessagePtr message)
	{
		Vec3 pos = message->GetPosition();
		Quaternion rot = message->GetRotation();
		//Mat4 trans;
		m_InverseTransform.SetTransformation(pos,rot,Vec3(1,1,1));
		m_InverseTransform = m_InverseTransform.Invert();
	}

	void VehicleTriggerComponent::OnScenarioEvent(ScenarioStateRequestPtr message)
	{
		if(message->GetState() == SS_PLAY)
		{
			_UpdateArea();
			_OnPlay();
			m_Update = true;
		}
		else if(message->GetState() == SS_STOP)
		{
			m_Update = false;
		}
	}

	void VehicleTriggerComponent::_OnPlay()
	{
		Reset();

		//Get all vehicles and add them as triggers

		for(int i = 0 ; i < m_ActivationControllers.size();i++)
		{
			ActivationObject ao;
			ao.Inside = false;
			ao.Object = m_ActivationControllers[i].GetRefObject();
			m_AllActivators.push_back(ao);
		}

		for(int i = 0 ; i < m_ActivationGroups.size();i++)
		{
			if(m_ActivationGroups[i].GetRefObject())
			{
				IComponentContainerTemplate::ComponentVector components;
				m_ActivationGroups[i].GetRefObject()->GetComponentsByClass<VehicleControllerComponent>(components);
				for(int j = 0;  j < components.size(); j++)
				{
					VehicleControllerComponentPtr comp = DYNAMIC_PTR_CAST<VehicleControllerComponent>(components[j]);
					//no filters!
					ActivationObject ao;
					ao.Inside = false;
					ao.Object = comp->GetSceneObject();
					m_AllActivators.push_back(ao);
				}
			}
		}

		//Add groups
		/*IComponentContainer::ComponentVector comps;
		GetSceneObject()->GetScene()->GetRootSceneObject()->GetComponentsByClass<VehicleControllerComponent>(comps);
		for(int i = 0 ; i < comps.size();i++)
		{
		VehicleControllerComponentPtr comp = DYNAMIC_PTR_CAST<VehicleControllerComponent>(comps[i]);
		//no filters!
		ActivationObject ao;
		ao.Inside = false;
		ao.Object = comp->GetSceneObject();
		m_AllActivators.push_back(ao);
		}*/
	}

	void VehicleTriggerComponent::SceneManagerTick(double delta_time)
	{
		if(!m_Update) 
			return;
		if(m_AllActivators.size() == 0)
			return;

		ActivationVector::iterator  iter = m_AllActivators.begin();
		bool some_one_entered = false;
		bool some_one_left = false;
		bool some_one_inside = false;
		bool all_inside = true;

		while(iter != m_AllActivators.end())
		{
			SceneObjectPtr obj((*iter).Object,NO_THROW);
			if(obj)
			{
				VehicleControllerComponentPtr vehicle = obj->GetFirstComponentByClass<VehicleControllerComponent>();
				const Vec3 pos = vehicle->GetVehiclePos();
				{
					if(_IsPointInside(pos))
					{
						some_one_inside = true;
						if(!(*iter).Inside)
						{
							//enter
							(*iter).Inside = true;
							some_one_entered = true;
							//GetSceneObject()->PostMessage(MessagePtr(new TriggerEnterMessage(obj)));
						}
					}	
					else
					{
						all_inside = false;
						if((*iter).Inside)
						{
							(*iter).Inside = false;
							some_one_left = true;
							//GetSceneObject()->PostMessage(MessagePtr(new TriggerExitMessage(obj)));
						}
					}
				}
				iter++;
			}
			else
			{
				iter = m_AllActivators.erase(iter);
			}
		}

		bool activate = false;

		if(m_Strict)
		{
			if(m_Present && all_inside)
			{
				activate = true;
			}

			if(!m_Present && !some_one_inside)
			{
				activate = true;
			}
		}
		else
		{
			if(m_Present && some_one_inside)
			{
				activate = true;
			}

			if(!m_Present && !all_inside)
			{
				activate = true;
			}
		}

		if(activate)
		{
			SetActive(true);
		}
		else
		{
			if(m_Repeatedly)
				SetActive(false);
		}
	}

	void VehicleTriggerComponent::SetActive(bool value)
	{
		m_Active = value;	
	}

	bool VehicleTriggerComponent::IsActive() const
	{
		return m_Active;	
	}

	void VehicleTriggerComponent::Reset()
	{
		m_AllActivators.clear();
		m_Active = false;
	}

	void VehicleTriggerComponent::_UpdateArea()
	{
		if(m_Initialized)
		{
			ColorRGBA color(0.6,0,0,1);
			GraphicsSubMeshPtr sub_mesh_data;

			switch(m_AreaType.GetValue())
			{
			case TAT_BOX:
				sub_mesh_data = GraphicsSubMesh::GenerateWireframeBox(m_AreaSize, color, "WhiteTransparentNoLighting");
				break;
			case TAT_RECTANGLE:
				sub_mesh_data = GraphicsSubMesh::GenerateWireframeRectangle(Vec2(m_AreaSize.x,m_AreaSize.z), color, "WhiteTransparentNoLighting");
				break;
			case TAT_ELLIPSOID:
				sub_mesh_data = GraphicsSubMesh::GenerateWireframeEllipsoid(m_AreaSize*0.5, color, "WhiteTransparentNoLighting", 30);
				break;
			case TAT_ELLIPSE:
				sub_mesh_data = GraphicsSubMesh::GenerateWireframeEllipse(Vec2(m_AreaSize.x*0.5,m_AreaSize.z*0.5), color, "WhiteTransparentNoLighting",30);
				break;
			}
			
			GraphicsMeshPtr mesh_data(new GraphicsMesh());
			mesh_data->SubMeshVector.push_back(sub_mesh_data);
			MessagePtr mesh_message(new ManualMeshDataMessage(mesh_data));
			GetSceneObject()->PostMessage(mesh_message);
		}
	}

	bool VehicleTriggerComponent::_IsPointInside(const Vec3 &point)
	{
		Vec3 trans_point = m_InverseTransform * point;
		Vec3 size = m_AreaSize*0.5;
		switch(m_AreaType.GetValue())
		{
		case TAT_BOX:
			{
				return (trans_point.x < size.x && trans_point.x > -size.x &&
					trans_point.z < size.z && trans_point.z > -size.z &&
					trans_point.y < size.y && trans_point.y > -size.y);
			}
			break;
		case TAT_RECTANGLE:
			{
				return (trans_point.x < size.x && trans_point.x > -size.x &&
					trans_point.z < size.z && trans_point.z > -size.z);
			}
			break;
		case TAT_ELLIPSOID:
			{
				trans_point.x = trans_point.x / size.x;
				trans_point.z = trans_point.z / size.z;
				trans_point.y = trans_point.y / size.y;;
				return (trans_point.Length() < 1.0);
			}break;
		case TAT_ELLIPSE:
			{
				trans_point.x = trans_point.x / size.x;
				trans_point.z = trans_point.z / size.z;
				trans_point.y = 0;
				return (trans_point.Length() < 1.0);
			}
			break;
		}
		return true;
	}
}
