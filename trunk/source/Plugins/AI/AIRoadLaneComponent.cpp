#include "AIRoadLaneComponent.h"
#include "AIRoadComponent.h"
#include "AIRoadLaneSectionComponent.h"
#include "AISceneManager.h"
#include "Plugins/Base/CoreMessages.h"
#include "Plugins/Game/GameMessages.h"
#include "Sim/Interface/GASSIWaypointListComponent.h"

namespace GASS
{
	AIRoadLaneComponent::AIRoadLaneComponent(void) : m_Initialized(false),
		m_Width(4),
		m_Direction(LD_DOWNSTREAM),
		m_LaneID(0)
	{

	}	

	AIRoadLaneComponent::~AIRoadLaneComponent(void)
	{

	}

	void AIRoadLaneComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("AIRoadLaneComponent",new Creator<AIRoadLaneComponent, IComponent>);
		GetClassRTTI()->SetMetaData(ObjectMetaDataPtr(new ObjectMetaData("AIRoadLaneSectionComponent", OF_VISIBLE)));

		RegisterProperty<Float>("Width", &AIRoadLaneComponent::GetWidth, &AIRoadLaneComponent::SetWidth,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));

		RegisterProperty<int>("LaneID", &AIRoadLaneComponent::GetLaneID, &AIRoadLaneComponent::SetLaneID,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));

		//RegisterProperty<SceneObjectID>("NextLane", &AIRoadLaneComponent::GetNextLane, &AIRoadLaneComponent::SetNextLane);
		//RegisterProperty<SceneObjectID>("PrevLane", &AIRoadLaneComponent::GetPrevLane, &AIRoadLaneComponent::SetPrevLane);
		RegisterProperty<SceneObjectRef>("WaypointsObject", &AIRoadLaneComponent::GetWaypointsObject, &AIRoadLaneComponent::SetWaypointsObject);

		RegisterProperty<LaneDirectionBinder>("Direction", &AIRoadLaneComponent::GetDirection, &AIRoadLaneComponent::SetDirection,
			EnumerationProxyPropertyMetaDataPtr(new EnumerationProxyPropertyMetaData("Direction",PF_VISIBLE,&LaneDirectionBinder::GetStringEnumeration)));
	}

	void AIRoadLaneComponent::SetWidth(Float width)
	{
		m_Width = width;
	}

	Float AIRoadLaneComponent::GetWidth() const
	{
		return m_Width;
	}

	void AIRoadLaneComponent::OnInitialize()
	{
		AIRoadComponentPtr road = GetSceneObject()->GetFirstParentComponentByClass<AIRoadComponent>();
		SceneObjectRef wp_object = road->GetWaypointsObject();
		wp_object->RegisterForMessage(REG_TMESS(AIRoadLaneComponent::OnWaypointsChanged,UpdateWaypointListMessage,0));
		m_Initialized = true;
	}
	void AIRoadLaneComponent::OnWaypointsChanged(UpdateWaypointListMessagePtr message)
	{
		//Update all lines
		UpdateLane();
		//Update lane data
	}

	std::vector<Vec3> AIRoadLaneComponent::GenerateOffset(std::vector<Vec3> wps, Float offset)
	{
		std::vector<Vec3> lane;
		for(size_t i = 0; i < wps.size(); i++)
		{
			Vec3 side; //= wps[i+1] - wps[i];

			Float width_mult = 1.0;

			if( i== 0)
			{
				side = (wps[1] - wps[0]); 
				side.y = 0;
			}
			else if(i < wps.size() - 1)
			{
				Vec3 d1 = (wps[i]-wps[i-1]);
				Vec3 d2 = (wps[i+1]-wps[i]);
				d1.Normalize();
				d2.Normalize();
				side = d1 + d2;
				side.Normalize();
				width_mult = Math::Dot(d1,side);
				if(width_mult > 0)
					width_mult = 1.0/width_mult;

			}
			else
			{
				side = wps[i]-wps[i-1];
			}
			side.Normalize();
			side = Math::Cross(side,Vec3(0,1,0)); 
			side.Normalize();
			lane.push_back(wps[i] + side*offset*width_mult);
		}
		return lane;
	}

	void AIRoadLaneComponent::UpdateLane()
	{
	}

	void AIRoadLaneComponent::RegisterLaneObject(LaneObject* object)
	{
		tbb::spin_mutex::scoped_lock lock(m_LaneMutex);
		m_LaneObjects.push_back(object);
	}

	void AIRoadLaneComponent::UnregisterLaneObject(LaneObject* object)
	{
		tbb::spin_mutex::scoped_lock lock(m_LaneMutex);
		std::vector<LaneObject*>::iterator iter = m_LaneObjects.begin();
		while(m_LaneObjects.end() != iter)
		{
			if(*iter == object)
				iter = m_LaneObjects.erase(iter);
			else iter++;
		}
	}



	bool AIRoadLaneComponent::FirstFreeLocation(Vec3 &pos,Quaternion &rot, Float &distance, Float vehicle_separation)
	{
		tbb::spin_mutex::scoped_lock lock(m_LaneMutex);
		double d = 0;
		double tot_dist = 0;
		for(size_t i = 1 ; i < m_Waypoints.size() ; i++)
		{
			Vec3 dir = (m_Waypoints[i] - m_Waypoints[i-1]);
			d = dir.Length();
			
			dir.Normalize();
			Float dist = vehicle_separation;
			while(dist < d)
			{
				bool free = true;
				for(size_t j = 0 ; j < m_LaneObjects.size() ; j++)
				{
					if( fabs((tot_dist + dist) - m_LaneObjects.at(j)->m_Distance) < vehicle_separation)
					{
						free = false;
						break;
					}
				}
				if(free)
				{
					pos = m_Waypoints[i-1] + (dir*dist);
					
					Mat4 rot_mat;
					rot_mat.Identity();
					Vec3 up(0,1,0);
					rot_mat.SetUpVector(up);
					rot_mat.SetViewDirVector(-dir);
					Vec3 right = -Math::Cross(-dir,up);
					rot_mat.SetRightVector(right);
					rot.FromRotationMatrix(rot_mat);

					distance = tot_dist + dist;
					return true;
				}
				dist += vehicle_separation;
			}
			tot_dist += d; 
		}
		return false;
	}


}
