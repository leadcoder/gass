#include "SensorComponent.h"
#include <boost/bind.hpp>
#include "Core/Math/Quaternion.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/ComponentSystem/IComponent.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/IMessage.h"
#include "Sim/SimEngine.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Scenario/Scene/SceneObjectManager.h"
#include "Sim/Components/Graphics/ILocationComponent.h"
#include "Sim/Systems/SimSystemManager.h"
#include "Plugins/Game/Components/Sensor/SignatureComponent.h"


namespace GASS
{
	SensorComponent::SensorComponent(void) :
		m_Initialized(false),
		m_DefaultMaxDetectionDistance(100000),
		m_UpdateFrequency(10),
		m_CurrentTime(0)
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
	}

	void SensorComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(SensorComponent::OnLoad,LoadGFXComponentsMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(SensorComponent::OnUnload,UnloadComponentsMessage,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(SensorComponent::OnTransChanged,TransformationNotifyMessage,0));
		GetSceneObject()->GetSceneObjectManager()->GetScenarioScene()->RegisterForMessage(REG_TMESS( SensorComponent::OnSceneObjectCreated,SceneObjectCreatedNotifyMessage,0));
	}

	void SensorComponent::OnLoad(MessagePtr message)
	{
		m_Initialized = true;
		SimEngine::GetPtr()->GetRuntimeController()->Register(this);
	}

	void SensorComponent::OnUnload(MessagePtr message)
	{
		SimEngine::GetPtr()->GetRuntimeController()->Unregister(this);
		m_Initialized = false;
	}

	void SensorComponent::OnSceneObjectCreated(SceneObjectCreatedNotifyMessagePtr message)
	{
		SignatureComponentPtr signature = message->GetSceneObject()->GetFirstComponentByClass<SignatureComponent>();
		if(signature)
		{
			signature->GetSceneObject()->RegisterForMessage(REG_TMESS(SensorComponent::OnTransChanged,TransformationNotifyMessage,0));
			m_AllObjects.push_back(signature);
		}
	}
	
	void SensorComponent::OnTransChanged(TransformationNotifyMessagePtr message)
	{
		m_Position = message->GetPosition();
	}

	void SensorComponent::Update(double delta_time)
	{
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
			return true;
		return false;
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

	TaskGroup SensorComponent::GetTaskGroup() const
	{
		return "SENSOR_TASK_GROUP";
	}
}
