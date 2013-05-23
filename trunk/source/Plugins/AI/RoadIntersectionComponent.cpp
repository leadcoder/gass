#include "RoadIntersectionComponent.h"
#include "RoadSegmentComponent.h"
#include "AISceneManager.h"
#include "Plugins/Base/CoreMessages.h"
#include "Plugins/Game/GameMessages.h"



namespace GASS
{
	RoadIntersectionComponent::RoadIntersectionComponent(void) :
		m_Initialized(false),
		m_CurrentGreen(0)
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
		GetSceneObject()->RegisterForMessage(REG_TMESS(RoadIntersectionComponent::OnToggleTrafficLight,ToggleTrafficLightMessage,0));
	
		float start_toggle = 2 + float(rand()%10);

		GetSceneObject()->PostMessage(MessagePtr(new ToggleTrafficLightMessage(-1,start_toggle)));
		m_Initialized = true;
	}

	void RoadIntersectionComponent::AddLight(const TrafficLight &light)
	{
		m_Lights.push_back(light);
	}


	void RoadIntersectionComponent::OnToggleTrafficLight(ToggleTrafficLightMessagePtr message)
	{
		
		GetSceneObject()->PostMessage(MessagePtr(new ToggleTrafficLightMessage(-1,10)));
		if(m_Lights.size() > 1)
		{
			
			TrafficLight* light = &m_Lights[m_CurrentGreen];
			light->m_Stop = true;

			m_CurrentGreen++;
			m_CurrentGreen = m_CurrentGreen % m_Lights.size();

			light = &m_Lights[m_CurrentGreen];
			light->m_Stop = false;


			std::vector<RoadSegmentComponentPtr>::iterator iter = m_Connections.begin();
			while(iter != m_Connections.end())
			{
				(*iter)->UpdateMesh();
				iter++;
			}
		}
		//update all meshes?
	}

	void RoadIntersectionComponent::AddRoad(RoadSegmentComponentPtr road)
	{

		//auto generate lights
		if(m_Connections.size() == 2)
		{
			//check if this road line up with present roads
			Vec3 dir1 = GetRoadDir(road);
			Vec3 dir2 = GetRoadDir(m_Connections[0]);
			Vec3 dir3 = GetRoadDir(m_Connections[1]);
			
			Float a1 = Math::Dot(dir1,dir2);
			Float a2 = Math::Dot(dir1,dir3);
			Float a3 = Math::Dot(dir2,dir3);

			if(fabs(a1) > 0.5)
			{
				TrafficLight light1;
				light1.m_Roads.push_back(road);
				light1.m_Roads.push_back(m_Connections[0]);
				m_Lights.push_back(light1);
				TrafficLight light2;
				light2.m_Roads.push_back(m_Connections[1]);
				m_Lights.push_back(light2);
			}
			else if(fabs(a2) > 0.5)
			{
				TrafficLight light1;
				light1.m_Roads.push_back(road);
				light1.m_Roads.push_back(m_Connections[1]);
				m_Lights.push_back(light1);
				TrafficLight light2;
				light2.m_Roads.push_back(m_Connections[0]);
				m_Lights.push_back(light2);
			}
			else
			{
				TrafficLight light1;
				light1.m_Roads.push_back(m_Connections[0]);
				light1.m_Roads.push_back(m_Connections[1]);
				m_Lights.push_back(light1);
				TrafficLight light2;
				light2.m_Roads.push_back(road);
				m_Lights.push_back(light2);
			}

		}
		//auto generate lights
		if(m_Connections.size() == 3)
		{
			m_Lights.clear();
			//check if this road line up with present roads
			Vec3 dir1 = GetRoadDir(road);
			Vec3 dir2 = GetRoadDir(m_Connections[0]);
			Vec3 dir3 = GetRoadDir(m_Connections[1]);
			Vec3 dir4 = GetRoadDir(m_Connections[2]);
			
			Float a1 = Math::Dot(dir1,dir2);
			Float a2 = Math::Dot(dir1,dir3);
			Float a3 = Math::Dot(dir1,dir4);

			if(fabs(a1) > 0.5)
			{
				TrafficLight light1;
				light1.m_Roads.push_back(road);
				light1.m_Roads.push_back(m_Connections[0]);
				m_Lights.push_back(light1);
				TrafficLight light2;
				light2.m_Roads.push_back(m_Connections[1]);
				light2.m_Roads.push_back(m_Connections[2]);
				m_Lights.push_back(light2);
			}
			else if(fabs(a2) > 0.5)
			{
				TrafficLight light1;
				light1.m_Roads.push_back(road);
				light1.m_Roads.push_back(m_Connections[1]);
				m_Lights.push_back(light1);
				TrafficLight light2;
				light2.m_Roads.push_back(m_Connections[0]);
				light2.m_Roads.push_back(m_Connections[2]);
				m_Lights.push_back(light2);
			}
			else 
			{
				TrafficLight light1;
				light1.m_Roads.push_back(road);
				light1.m_Roads.push_back(m_Connections[2]);
				m_Lights.push_back(light1);
				TrafficLight light2;
				light2.m_Roads.push_back(m_Connections[0]);
				light2.m_Roads.push_back(m_Connections[1]);
				m_Lights.push_back(light2);
			}
		}
		//sort clockwise?
		m_Connections.push_back(road);
	}

	Vec3  RoadIntersectionComponent::GetRoadDir(RoadSegmentComponentPtr road)
	{
		Vec3 start_pos = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->GetWorldPosition();
		Vec3 end_pos;
		
		bool start = road->StartInIntersection(DYNAMIC_PTR_CAST<RoadIntersectionComponent>(shared_from_this()));
		if(start)
		{
			end_pos = road->GetEndNode()->GetFirstComponentByClass<ILocationComponent>()->GetWorldPosition();
		}
		else
		{
			end_pos = road->GetStartNode()->GetFirstComponentByClass<ILocationComponent>()->GetWorldPosition();
		}
		Vec3 dir = end_pos - start_pos;
		dir.Normalize();
		return dir;
	}


	TurnDir RoadIntersectionComponent::CheckTurn(RoadSegmentComponentPtr in_road,RoadSegmentComponentPtr out_road)
	{
		TurnDir turn_dir;
		Vec3  dir1 = GetRoadDir(in_road);
		Vec3  dir2 = GetRoadDir(out_road);

		Float a = Math::Dot(dir1,dir2);
		if(fabs(a) < 0.5)
		{
			Vec3 cross = Math::Cross(dir1,dir2);
			if(cross.y < 0)
				turn_dir = TURN_RIGHT;
			else
				turn_dir = TURN_LEFT;
		}
		else
			turn_dir = TURN_NONE;

		return turn_dir;
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

	bool RoadIntersectionComponent::GetTrafficLight(RoadSegmentComponentPtr road, TrafficLight &light)
	{
		for(size_t i = 0; i < m_Lights.size();i++)
		{
			for(size_t j = 0; j < m_Lights[i].m_Roads.size();j++)
			{
				if(m_Lights[i].m_Roads[j] == road)
				{
					light=m_Lights[i];
					return true;
				}
			}
		}
		return false;
	}


	
}
