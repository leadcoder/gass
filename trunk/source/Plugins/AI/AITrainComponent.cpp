#include "AITrainComponent.h"
#include "TriggerComponent.h"
#include "AITargetComponent.h"
#include "AIMessages.h"
#include "Sim/Components/Common/GASSIWaypointListComponent.h"


namespace GASS
{
	AITrainComponent::AITrainComponent(void) : m_Initialized(false), m_CurrentTime(0),
		m_TotDist(0), 
		m_Velocity(0),
		m_DesiredVelocity(20),
		m_StopDistance(250),
		m_AccelerationDistance(20),
		m_StopTime(20),
		m_DoorDelay(5),
		m_TrackDist(500),
		
		m_TimeStop(0),
		m_Stopped(false),
		m_DoorOpen(false)
		
	{

	}	

	AITrainComponent::~AITrainComponent(void)
	{

	}

	void AITrainComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("AITrainComponent",new Creator<AITrainComponent, IComponent>);
		RegisterProperty<std::string>("WaypointList", &GetWaypointList, &SetWaypointList);
		RegisterProperty<Float>("StopDistance", &GetStopDistance, &SetStopDistance);
		RegisterProperty<Float>("TrackDistance", &GetTrackDistance, &SetTrackDistance);
		
		RegisterProperty<Float>("DesiredVelocity", &GetDesiredVelocity, &SetDesiredVelocity);
		RegisterProperty<Float>("AccelerationDistance", &GetAccelerationDistance, &SetAccelerationDistance);
		RegisterProperty<Float>("StopTime", &GetStopTime, &SetStopTime);
		RegisterProperty<Float>("DoorDelay", &GetDoorDelay, &SetDoorDelay);
		RegisterProperty<SceneObjectID>("TrainObject", &GetTrainObject, &SetTrainObject);
	}


	void AITrainComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(AITrainComponent::OnLoad,LocationLoadedMessage,1)); //load after agent
		GetSceneObject()->RegisterForMessage(REG_TMESS(AITrainComponent::OnSpeedMessage,DesiredSpeedMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(AITrainComponent::OnTransformationChanged,TransformationNotifyMessage ,0));

		SceneManagerListenerPtr listener = shared_from_this();
		GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<AISceneManager>()->Register(listener);

		BaseSceneComponent::OnInitialize();
	}

	void AITrainComponent::OnLoad(LocationLoadedMessagePtr message)
	{
		m_Initialized = true;

		m_Train = GetSceneObject()->GetChildByID(m_TrainObject);

		if(m_WaypointListName == "")
		{
			//create own spline
			CreateSpline();
		}
		else
			SetWaypointList(m_WaypointListName);
	}
	
	

	void AITrainComponent::OnTransformationChanged(TransformationNotifyMessagePtr message)
	{
		m_Pos = message->GetPosition();
		m_Rot = message->GetRotation();
	}


	void AITrainComponent::CreateSpline()
	{
		m_SplineAnimation.m_NodeVector.clear();
		m_SplineAnimation.m_Spline.Clear();
		m_SplineAnimation.m_Length = 0;
		
		
		m_SplineAnimation.AddNode(Vec3(-m_TrackDist*0.5,0,0));
		m_SplineAnimation.AddNode(Vec3(m_TrackDist*0.5,0,0));
	}

	void AITrainComponent::SetTrackDistance(Float dist)
	{
		m_TrackDist = dist;

		if(m_Initialized)
		{
			CreateSpline();
		}
	}

	void AITrainComponent::SceneManagerTick(double delta_time)
	{
		Float start_stop_at = m_StopDistance-m_AccelerationDistance;
		Float end_start_at = m_StopDistance+m_AccelerationDistance;

		
		if(!m_Stopped)
		{
			if(m_TotDist > start_stop_at && m_TotDist < m_StopDistance)
			{
				Float dist_left = m_TotDist  - start_stop_at;
				Float vel = 1.0-dist_left/m_AccelerationDistance;
				m_TotDist += vel*m_DesiredVelocity*delta_time;
				if(fabs(m_TotDist - m_StopDistance) < 0.01)
				{
					m_Stopped = true;
					m_DoorOpen = true;
					//at rest, send message recursive?
					MessagePtr message(new DoorMessage(true));
					RecPost(message,GetSceneObject());
					
				}
			}
			else if(m_TotDist > m_StopDistance && m_TotDist < end_start_at)
			{
				Float dist_left = m_TotDist - m_StopDistance;
				Float vel = dist_left/m_AccelerationDistance;
				m_TotDist += vel*m_DesiredVelocity*delta_time;;
			}
			else 
				m_TotDist += m_DesiredVelocity*delta_time;
		}
		else
		{
			m_TotDist = m_StopDistance+0.01;

			if(m_TimeStop > (m_StopTime-m_DoorDelay))
			{
				if(m_DoorOpen)
				{
					m_DoorOpen = false;
					MessagePtr message(new DoorMessage(false));

					//post to all children
					RecPost(message,GetSceneObject());
				}
			}

			if(m_TimeStop > m_StopTime)
			{
				m_Stopped = false;
				m_TimeStop = 0;
			}
			else
			{
				m_TimeStop += delta_time;
			}
		}


		if(m_TotDist >= m_SplineAnimation.m_Length)
		{
			m_TotDist = 0;
		}

		if(m_TotDist < 0)
			m_TotDist = 0;

		Vec3 new_pos(0,0,0);
		
		if(m_SplineAnimation.m_NodeVector.size() > 0)
		{
			Mat4 transformation = m_SplineAnimation.GetTransformation(m_TotDist,Vec3(0,1,0));
			Mat4 rot_correction;
			rot_correction.Identity();
			rot_correction.RotateY(Math::Deg2Rad(90));

			Mat4 local_tans;
			local_tans.SetTransformation(m_Pos,m_Rot,Vec3(1,1,1));
			transformation = (rot_correction*transformation)*local_tans;
			new_pos = transformation.GetTranslation();
			SceneObjectPtr train(m_Train);
			if(train)
			{
				train->PostMessage(MessagePtr(new WorldPositionMessage(new_pos)));
				Quaternion rot;
				transformation.SetTranslation(0,0,0);
				rot.FromRotationMatrix(transformation);
				train->PostMessage(MessagePtr(new WorldRotationMessage(rot)));
			}
		}
	}


	void AITrainComponent::RecPost(MessagePtr message,SceneObjectPtr obj)
	{
			BaseComponentContainer::ComponentContainerIterator children = obj->GetChildren();
			while(children.hasMoreElements())
			{
				SceneObjectPtr child = boost::shared_static_cast<SceneObject>(children.getNext());
				child->PostMessage(message);
				RecPost(message,child);
			}
	}



	void AITrainComponent::OnSpeedMessage(DesiredSpeedMessagePtr message)
	{
		m_DesiredVelocity = message->GetSpeed();
	}

	Float AITrainComponent::GetDesiredVelocity() const
	{
		return m_DesiredVelocity;
	}

	void AITrainComponent::SetDesiredVelocity(Float value) 
	{
		m_DesiredVelocity = value;
	}

	std::string AITrainComponent::GetWaypointList() const
	{
		return m_WaypointListName;
	}

	void AITrainComponent::SetWaypointList(const std::string &value)
	{
		m_WaypointListName = value;

		if(m_WaypointListName != "" && m_Initialized)
		{
			IWaypointListComponent*  wpl = FindWaypointList(m_WaypointListName);
			if(wpl)
			{
				std::vector<Vec3> points =  wpl->GetWaypoints(false);

				m_SplineAnimation.m_NodeVector.clear();
				m_SplineAnimation.m_Spline.Clear();
				m_SplineAnimation.m_Length = 0;

				for(size_t i = 0; i < points.size();i++)
				{
					m_SplineAnimation.AddNode(points[i]);
				}
			}
		}
	}

	IWaypointListComponent* AITrainComponent::FindWaypointList(const std::string &name) const
	{
		std::vector<SceneObjectPtr> objs;
		GetSceneObject()->GetScene()->GetRootSceneObject()->GetChildrenByName(objs,name);
		if(objs.size() > 0)
		{
			SceneObjectPtr obj = objs.front();
			boost::shared_ptr<IWaypointListComponent> wpl_comp = obj->GetFirstComponentByClass<IWaypointListComponent>();
			if(wpl_comp)
			{
				return wpl_comp.get();
			}
		}
		return NULL;
	}
	
}


