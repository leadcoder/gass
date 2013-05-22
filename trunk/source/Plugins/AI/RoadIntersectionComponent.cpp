#include "RoadIntersectionComponent.h"
#include "AISceneManager.h"
#include "Plugins/Base/CoreMessages.h"
#include "Plugins/Game/GameMessages.h"



namespace GASS
{
	RoadIntersectionComponent::RoadIntersectionComponent(void) :
		m_Initialized(false)
	{
		
	}	

	RoadIntersectionComponent::~RoadIntersectionComponent(void)
	{

	}

	void RoadIntersectionComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("RoadIntersectionComponent",new Creator<RoadIntersectionComponent, IComponent>);
		//RegisterVectorProperty<SceneObjectRef>("RoadSegments", &RoadIntersectionComponent::GetRoadSegments, &RoadIntersectionComponent::SetRoadSegments);
	}

	void RoadIntersectionComponent::OnInitialize()
	{
		//GetSceneObject()->RegisterForMessage(REG_TMESS(RoadIntersectionComponent::OnTriggerEnter,TriggerEnterMessage,0));
		
		m_Initialized = true;
	}

	void RoadIntersectionComponent::AddRoad(RoadSegmentComponentPtr road)
	{
		//sort clockwise?
		m_Connections.push_back(road);

		/*Vec3 start_pos = GetCeneObject()->GetFirstComponentByClass<ILocationComponent>()->GetWorldPosition();
		Vec3 end_pos;
		bool start = road->StartInIntersection();
		if(start)
		{
			end_pos = road->GetEndNode()->GetFirstComponentByClass<ILocationComponent>()->GetWorldPosition();
		}
		else
		{
			end_pos = road->GetStartNode()->GetFirstComponentByClass<ILocationComponent>()->GetWorldPosition();
		}
		
		Vec3 dir = end_pos - start_pos;
		dir.Normalize();*/

	}

	void RoadIntersectionComponent::RemoveRoad(RoadSegmentComponentPtr road)
	{
		std::vector<RoadSegmentComponentPtr>::iterator iter = m_Connections.begin();
		while(iter != m_Connections.end())
		{
			if(*iter == road )
				iter = m_Connections.erase(iter);
			else
				iter++;
		}
	}

	RoadSegmentComponentPtr RoadIntersectionComponent::GetRandomRoad(RoadSegmentComponentPtr road)
	{
		std::vector<RoadSegmentComponentPtr>::iterator iter = m_Connections.begin();
		std::vector<RoadSegmentComponentPtr> roads;
		while(iter != m_Connections.end())
		{
			if(*iter != road)
				roads.push_back(*iter);
			iter++;
		}
		if(roads.size() == 0)
			return road;
		else
			return roads[rand()%roads.size()];
	}

	bool RoadIntersectionComponent::CheckIfRedLight(RoadSegmentComponentPtr road)
	{
		std::vector<RoadSegmentComponentPtr>::iterator iter = m_Connections.begin();
		std::vector<RoadSegmentComponentPtr> roads;
		while(iter != m_Connections.end())
		{
			if(*iter == road)
			{

			}
		}
	}
	
}
