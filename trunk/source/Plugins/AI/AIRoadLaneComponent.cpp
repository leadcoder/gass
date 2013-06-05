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
		m_Direction(LD_DOWNSTREAM)
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
		RegisterProperty<SceneObjectID>("NextLane", &AIRoadLaneComponent::GetNextLane, &AIRoadLaneComponent::SetNextLane);
		RegisterProperty<SceneObjectID>("PrevLane", &AIRoadLaneComponent::GetPrevLane, &AIRoadLaneComponent::SetPrevLane);
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
	/*	AIRoadComponentPtr road = GetSceneObject()->GetFirstParentComponentByClass<AIRoadComponent>();
		if(!road)
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Failed to find AIRoadComponent", "AIRoadLaneComponent::UpdateLane");


		AIRoadLaneSectionComponentPtr lane_section = GetSceneObject()->GetFirstParentComponentByClass<AIRoadLaneSectionComponent>();
		if(!lane_section)
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Failed to find AIRoadLaneSectionComponent", "AIRoadLaneComponent::UpdateLane");

		SceneObjectRef road_wps_object = road->GetWaypointsObject();
		std::vector<Vec3> road_wps = road_wps_object->GetFirstComponentByClass<IWaypointListComponent>()->GetWaypoints();

		std::vector<Vec3> lane_wps = GenerateOffset(road_wps,m_Width);

		//get previous lane
		lane_wps = Math::ClipPath(lane_section->GetDistance(), 30,lane_wps);

		ManualMeshDataPtr mesh_data(new ManualMeshData());
		mesh_data->Type = LINE_LIST;
		mesh_data->Material = "WhiteTransparentNoLighting";

		MeshVertex vertex;
		vertex.TexCoord.Set(0,0);
		vertex.Color.Set(0.2,0.2,1,1);
		vertex.Normal = Vec3(0,1,0);

		for(size_t i = 1; i < lane_wps.size(); i++)
		{
			vertex.Pos = lane_wps[i];
			mesh_data->VertexVector.push_back(vertex );
			vertex.Pos = lane_wps[i-1];
			mesh_data->VertexVector.push_back(vertex );
		}
		MessagePtr mesh_message(new ManualMeshDataMessage(mesh_data));
		GetSceneObject()->PostMessage(mesh_message);*/
	}
}
