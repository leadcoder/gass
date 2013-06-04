#include "AIRoadIntersectionComponent.h"
#include "AIRoadComponent.h"
#include "AISceneManager.h"
#include "Plugins/Base/CoreMessages.h"
#include "Plugins/Game/GameMessages.h"



namespace GASS
{
	AIRoadIntersectionComponent::AIRoadIntersectionComponent(void) : m_Initialized(false),
		m_AllowLeftTurn(false),
		m_DisableTrafficLight(false)
	{

	}	

	AIRoadIntersectionComponent::~AIRoadIntersectionComponent(void)
	{

	}

	void AIRoadIntersectionComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("AIRoadIntersectionComponent",new Creator<AIRoadIntersectionComponent, IComponent>);
		GetClassRTTI()->SetMetaData(ObjectMetaDataPtr(new ObjectMetaData("AIRoadIntersectionComponent", OF_VISIBLE)));
		RegisterProperty<SceneObjectRef>("ConnectionDebugObject", &AIRoadIntersectionComponent::GetConnectionDebugObject, &AIRoadIntersectionComponent::SetConnectionDebugObject);
	}

	void AIRoadIntersectionComponent::OnInitialize()
	{
		m_Initialized = true;
	}

	void AIRoadIntersectionComponent::AddRoad(AIRoadComponentPtr road)
	{
		m_Connections.push_back(road);
		UpdateConnectionLines();
	}

	void AIRoadIntersectionComponent::RemoveRoad(AIRoadComponentPtr road)
	{
		std::vector<AIRoadComponentPtr>::iterator iter = m_Connections.begin();
		while(iter != m_Connections.end())
		{
			if(*iter == road )
				iter = m_Connections.erase(iter);
			else
				iter++;
		}
		UpdateConnectionLines();
	}

	void AIRoadIntersectionComponent::UpdateConnectionLines()
	{
		//do auto lane connections
		for(size_t i =  0; i < m_Connections.size(); i++)
		{
			std::vector<AIRoadLaneComponentPtr> incomming_lanes = m_Connections[i]->GetIncommingLanes(GetSceneObject());
			for(size_t j =  0; j < m_Connections.size(); j++)
			{
				if(m_Connections[i] != m_Connections[j])
				{
					std::vector<AIRoadLaneComponentPtr> outgoing_lanes = m_Connections[i]->GetOutgoingLanes(GetSceneObject());
					for(size_t k =  0; k < incomming_lanes.size(); k++)
					{
						if(k < outgoing_lanes.size())
						{
							Vec3 start_pos = incomming_lanes[k]->GetEndPos();
							Vec3 end_pos = outgoing_lanes[k]->GetStartPos();
						}
					}
				}
			}
		}

		if(!GetConnectionDebugObject())
			return;
		Float start = 0;
		Float end = 0;
		Float prev = 0;

		ManualMeshDataPtr mesh_data(new ManualMeshData());
		mesh_data->Type = LINE_LIST;
		mesh_data->Material = "WhiteTransparentNoLighting";

		MeshVertex vertex;
		vertex.TexCoord.Set(0,0);
		vertex.Color.Set(0.2,1,1,1);
		vertex.Normal = Vec3(0,1,0);

		Vec3 inter_pos = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->GetWorldPosition();

		if(m_Connections.size() > 0)
		{
			for(size_t i =  0; i < m_Connections.size(); i++)
			{
				Vec3 connection_pos;
				if(m_Connections[i]->StartIn(GetSceneObject()))
					connection_pos = m_Connections[i]->GetStartPoint();
				else
					connection_pos = m_Connections[i]->GetEndPoint();
				vertex.Pos = inter_pos;
				mesh_data->VertexVector.push_back(vertex );
				vertex.Pos = connection_pos;
				mesh_data->VertexVector.push_back(vertex );
			}
			MessagePtr mesh_message(new ManualMeshDataMessage(mesh_data));
			GetConnectionDebugObject()->PostMessage(mesh_message);
		}
	}


	/*Vec3 AIRoadIntersectionComponent::GetRoadDir(AIRoadComponentPtr road)
	{
		Vec3 start_pos = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->GetWorldPosition();
		Vec3 end_pos;

		bool start = road->StartInIntersection(DYNAMIC_PTR_CAST<AIRoadIntersectionComponent>(shared_from_this()));
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

	TurnDir AIRoadIntersectionComponent::CheckTurn(AIRoadComponentPtr in_road,AIRoadComponentPtr out_road)
	{
		TurnDir turn_dir;
		Vec3  dir1 = GetRoadDir(in_road);
		Vec3  dir2 = GetRoadDir(out_road);

		Float a = Math::Dot(dir1,dir2);
		if(fabs(a) < 0.5)
		{
			Vec3 cross = Math::Cross(dir1,dir2);
			if(cross.y < 0)
				turn_dir = TURN_LEFT;
			else
				turn_dir = TURN_RIGHT;
		}
		else
			turn_dir = TURN_NONE;

		return turn_dir;
	}

	void AIRoadIntersectionComponent::RemoveRoad(AIRoadComponentPtr road)
	{
		std::vector<AIRoadComponentPtr>::iterator iter = m_Connections.begin();
		while(iter != m_Connections.end())
		{
			if(*iter == road )
				iter = m_Connections.erase(iter);
			else
				iter++;
		}
	}

	AIRoadComponentPtr AIRoadIntersectionComponent::GetRandomRoad(AIRoadComponentPtr road)
	{
		std::vector<AIRoadComponentPtr>::iterator iter = m_Connections.begin();
		std::vector<AIRoadComponentPtr> roads;
		while(iter != m_Connections.end())
		{
			if(*iter != road)
			{
				
				if(m_AllowLeftTurn || m_Connections.size() < 3)
					roads.push_back(*iter);
				else if(CheckTurn(road,*iter) != TURN_LEFT)
					roads.push_back(*iter);
			}
			iter++;
		}
		if(roads.size() == 0)
			return road;
		else
			return roads[rand()%roads.size()];
	}

	bool AIRoadIntersectionComponent::GetTrafficLight(AIRoadComponentPtr road, TrafficLight &light)
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
	}*/

}
