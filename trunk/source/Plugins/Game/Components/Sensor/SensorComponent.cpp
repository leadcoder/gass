#include "SensorComponent.h"
#include <boost/bind.hpp>
#include "Core/Math/GASSQuaternion.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/ComponentSystem/GASSIComponent.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/Scene/GASSScene.h"
#include "Sim/Scene/GASSSceneObject.h"

#include "Sim/Components/Graphics/GASSILocationComponent.h"
#include "Sim/Systems/GASSSimSystemManager.h"
#include "Plugins/Game/Components/Sensor/SignatureComponent.h"
#include "Plugins/Game/GameSceneManager.h"


namespace GASS
{
	SensorComponent::SensorComponent(void) :
		m_Initialized(false),
		m_DefaultMaxDetectionDistance(100000),
		m_UpdateFrequency(10),
		m_CurrentTime(0),
		m_Debug(false),
		m_Position(0,0,0)
	{
		
	}	

	SensorComponent::~SensorComponent(void)
	{

	}

	void SensorComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("SensorComponent",new Creator<SensorComponent, IComponent>);
		REG_ATTRIBUTE(Float,DefaultMaxDetectionDistance,SensorComponent)
		REG_ATTRIBUTE(Float,UpdateFrequency,SensorComponent)
		REG_ATTRIBUTE(bool,Debug,SensorComponent)
		
	}

	void SensorComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(SensorComponent::OnLoad,LoadComponentsMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(SensorComponent::OnUnload,UnloadComponentsMessage,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(SensorComponent::OnTransChanged,TransformationNotifyMessage,0));
		GetSceneObject()->GetScene()->RegisterForMessage(REG_TMESS( SensorComponent::OnSceneObjectCreated,SceneObjectCreatedNotifyMessage,0));
	}

	void SensorComponent::OnLoad(LoadComponentsMessagePtr message)
	{
		SceneManagerListenerPtr listener = shared_from_this();
		GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<GameSceneManager>()->Register(listener);
		
		m_Initialized = true;

		// get all signature objects and add them
		IComponentContainer::ComponentVector comps;
		GetSceneObject()->GetScene()->GetRootSceneObject()->GetComponentsByClass<SignatureComponent>(comps,true);
		for(size_t i = 0; i < comps.size(); i++)
		{
			SignatureComponentPtr sig = boost::shared_dynamic_cast<SignatureComponent>(comps[i]);
			m_AllObjects.push_back(sig);
		}
		
	}

	void SensorComponent::OnUnload(MessagePtr message)
	{
		m_Initialized = false;
	}

	void SensorComponent::OnSceneObjectCreated(SceneObjectCreatedNotifyMessagePtr message)
	{
		SignatureComponentPtr signature = message->GetSceneObject()->GetFirstComponentByClass<SignatureComponent>();
		if(signature)
		{
			//signature->GetSceneObject()->RegisterForMessage(REG_TMESS(SensorComponent::OnTransChanged,TransformationNotifyMessage,0));
			m_AllObjects.push_back(signature);
		}
	}
	
	void SensorComponent::OnTransChanged(TransformationNotifyMessagePtr message)
	{
		m_Position = message->GetPosition();
	}

	void SensorComponent::SceneManagerTick(double delta_time)
	{
		//debug?
		if(m_Debug)
		{
			MessagePtr message(new DrawCircleMessage(m_Position,m_DefaultMaxDetectionDistance,Vec4(1,0,0,1),20,false));
			SimEngine::Get().GetSimSystemManager()->PostMessage(message);
		}

		//update sensor data at x hz 
		m_CurrentTime += delta_time;
		if(m_CurrentTime > 1.0/m_UpdateFrequency)
		{
			//reset
			m_CurrentTime = 0;

			std::vector<SignatureComponentWeakPtr>::const_iterator iter = m_AllObjects.begin();
			while(iter != m_AllObjects.end())
			{
				SignatureComponentPtr signature(*iter,boost::detail::sp_nothrow_tag());
				if(signature)
				{
					const Vec3 pos = signature->GetPosition();
					
					const Float dist = (m_Position - pos).Length();
					const Float detection_distance = GetDetectionDistance(signature->GetPlatformType().Get(), signature->GetRadarCrossSection());
					if(dist < detection_distance)
					{
						//check line of sight?
						DetectionData data;
						data.Pos = pos;
						data.Rotation = signature->GetRotation();
						data.Velocity = signature->GetVelocity();
						data.Type = signature->GetPlatformType().Get();
						data.ID = signature->GetSceneObject()->GetName();
						if(IsNewTarget(signature))
						{
							//send new message that target is found
							GetSceneObject()->PostMessage(MessagePtr(new SensorGotTargetMessage(data)));
						}
						m_DetectedObjects[signature] = data;
					}
					else
					{
						RemoveTarget(signature);
					}
					iter++;
				}
				else
				{
					//remove object
					iter = m_AllObjects.erase(iter);
					RemoveTarget(signature);
				}
			}
			//compile detection data
			std::map<SignatureComponentWeakPtr,DetectionData>::const_iterator detection_iter;
			detection_iter = m_DetectedObjects.begin();
			DetectionVector all_targets;
			while(detection_iter != m_DetectedObjects.end())
			{
				all_targets.push_back(detection_iter->second);
				detection_iter++;
			}
			//Send detection message
			GetSceneObject()->PostMessage(MessagePtr(new SensorMessage(all_targets)));

			
		}
	}
	bool SensorComponent::IsNewTarget(SignatureComponentWeakPtr sig) const
	{
		if(m_DetectedObjects.find(sig) != m_DetectedObjects.end())
			return false;
		return true;
	}

	bool SensorComponent::RemoveTarget(SignatureComponentWeakPtr sig)
	{
		std::map<SignatureComponentWeakPtr,DetectionData>::iterator iter;
		iter = m_DetectedObjects.find(sig);
		if(iter != m_DetectedObjects.end())
		{
			GetSceneObject()->PostMessage(MessagePtr(new SensorLostTargetMessage(iter->second)));
			m_DetectedObjects.erase(iter);
			
			//send Message that target is lost
			return true;
		}
		return false;
	}

	Float SensorComponent::GetDetectionDistance(PlatformType signature, Float radar_cross_section)
	{
		switch(signature)
		{
		case PT_HUMAN:
		case PT_CAR:
		case PT_TANK:
		case PT_TRUCK:
		case PT_AIRCRAFT:
		case PT_BUILDING:
		case PT_WEAPON_SYSTEM:
			break;
		}
		return m_DefaultMaxDetectionDistance*radar_cross_section;
	}

}
