#include "AIMovingPlatformComponent.h"
#include "TriggerComponent.h"
#include "AITargetComponent.h"
#include "Sim/Interface/GASSIWaypointListComponent.h"


namespace GASS
{
	AIMovingPlatformComponent::AIMovingPlatformComponent(void) : m_Initialized(false), m_CurrentTime(0),
		m_TotDist(0), 
		m_Velocity(0),
		m_DesiredVelocity(0),
		m_Acceleration(6)
	{

	}	

	AIMovingPlatformComponent::~AIMovingPlatformComponent(void)
	{

	}

	void AIMovingPlatformComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("AIMovingPlatformComponent",new Creator<AIMovingPlatformComponent, IComponent>);
		RegisterProperty<std::string>("WaypointList", &GetWaypointList, &SetWaypointList);
		RegisterProperty<Float>("DesiredVelocity", &GetDesiredVelocity, &SetDesiredVelocity);
		RegisterProperty<Float>("Acceleration", &GetAcceleration, &SetAcceleration);
	}


	void AIMovingPlatformComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(AIMovingPlatformComponent::OnLocationLoaded,LocationLoadedMessage,1)); //load after agent
		GetSceneObject()->RegisterForMessage(REG_TMESS(AIMovingPlatformComponent::OnSpeedMessage,DesiredSpeedMessage,0));
		SceneManagerListenerPtr listener = shared_from_this();
		GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<AISceneManager>()->Register(listener);
	}


	void AIMovingPlatformComponent::OnLocationLoaded(LocationLoadedMessagePtr message)
	{
		m_Initialized = true;
		SetWaypointList(m_WaypointListName);
	}

	
	
	void AIMovingPlatformComponent::SceneManagerTick(double delta_time)
	{
	
		//Float error = m_DesiredVelocity-m_Velocity;
		if(m_DesiredVelocity < m_Velocity)
		{
			m_Velocity  -= m_Acceleration*delta_time;
			if(m_Velocity < m_DesiredVelocity)
				m_Velocity = m_DesiredVelocity;
		}
		else if(m_DesiredVelocity > m_Velocity)
		{
			m_Velocity  += m_Acceleration*delta_time;
			if(m_Velocity > m_DesiredVelocity)
				m_Velocity = m_DesiredVelocity;
		}
		//m_CurrentTime += delta_time;
		m_TotDist += m_Velocity*delta_time;
		if(m_TotDist >= m_SplineAnimation.m_Length)
		{
			//if(m_Loop)
			{
				m_TotDist = 0;
			}
			/*else
			{
				m_TotDist = m_SplineAnimation.m_Length;
				return;
			}*/
		}
		if(m_TotDist < 0)
			m_TotDist = 0;


		Vec3 new_pos(0,0,0);
		
		if(m_SplineAnimation.m_NodeVector.size() > 0)
		{
			Mat4 transformation = m_SplineAnimation.GetTransformation(m_TotDist,Vec3(0,1,0));
			new_pos = transformation.GetTranslation();
			GetSceneObject()->PostMessage(MessagePtr(new WorldPositionMessage(new_pos)));
			Quaternion rot;
			transformation.SetTranslation(0,0,0);
			rot.FromRotationMatrix(transformation);
			GetSceneObject()->PostMessage(MessagePtr(new WorldRotationMessage(rot)));
		}
	}


	void AIMovingPlatformComponent::OnSpeedMessage(DesiredSpeedMessagePtr message)
	{
		m_DesiredVelocity = message->GetSpeed();
	}

	Float AIMovingPlatformComponent::GetDesiredVelocity() const
	{
		return m_DesiredVelocity;
	}


	void AIMovingPlatformComponent::SetDesiredVelocity(Float value) 
	{
		m_DesiredVelocity = value;
	}

	std::string AIMovingPlatformComponent::GetWaypointList() const
	{
		return m_WaypointListName;
	}

	void AIMovingPlatformComponent::SetWaypointList(const std::string &value)
	{
		m_WaypointListName = value;

		if(m_WaypointListName != "" && m_Initialized)
		{
			IWaypointListComponent*  wpl = FindWaypointList(m_WaypointListName);
			if(wpl)
			{
				std::vector<Vec3> points =  wpl->GetWaypoints(false);

				for(size_t i = 0; i < points.size();i++)
				{
					m_SplineAnimation.AddNode(points[i]);
				}
			}
		}
	}


	IWaypointListComponent* AIMovingPlatformComponent::FindWaypointList(const std::string &name) const
	{
		std::vector<SceneObjectPtr> objs;
		GetSceneObject()->GetScene()->GetRootSceneObject()->GetChildrenByName(objs,name);
		if(objs.size() > 0)
		{
			SceneObjectPtr obj = objs.front();
			SPTR<IWaypointListComponent> wpl_comp = obj->GetFirstComponentByClass<IWaypointListComponent>();
			if(wpl_comp)
			{
				return wpl_comp.get();
			}
		}

		return NULL;
	}
	
}


