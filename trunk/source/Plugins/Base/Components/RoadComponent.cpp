/****************************************************************************
*                                                                           *
* GASS                                                                *
* Copyright (C)2003 - 2005 Johan Hedstrom                                   *
* Email: GASS@gmail.com                                               *
* Web page: http://n00b.dyndns.org/GASS                               *
*                                                                           *
* GASS is only used with knowledge from the author. This software     *
* is not allowed to redistribute without permission from the author.        *
* For further license information, please turn to the product home page or  *
* contact author. Abuse against the GASS license is prohibited by law.*
*                                                                           *
*****************************************************************************/ 

#include <boost/bind.hpp>
#include "RoadComponent.h"
#include "Core/Utils/Log.h"
#include "Core/Math/Quaternion.h"
#include "Core/Math/SplineAnimation.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/ComponentSystem/IComponent.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/IMessage.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/ComponentSystem/IComponent.h"
#include "Core/ComponentSystem/BaseComponentContainerTemplateManager.h"
#include "Core/ComponentSystem/ComponentContainerFactory.h"
#include "Sim/Scheduling/IRuntimeController.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Scenario/Scene/SceneObjectManager.h"
#include "Sim/Components/Graphics/ILocationComponent.h"
#include "Sim/SimEngine.h"
#include "Sim/Systems/SimSystemManager.h"
#include "Sim/Components/Graphics/MeshData.h"
#include "Sim/Systems/SimSystemManager.h"
#include "Sim/Systems/Collision/ICollisionSystem.h"
#include "WaypointComponent.h"
#include "WaypointListComponent.h"
#include "Sim/Components/Graphics/Geometry/ITerrainComponent.h"

namespace GASS
{

	template<> std::map<std::string ,TerrainLayer> EnumBinder<TerrainLayer,TerrainLayerBinder>::m_Names;
	template<> std::map<TerrainLayer,std::string> EnumBinder<TerrainLayer,TerrainLayerBinder>::m_Types;

	RoadComponent::RoadComponent() : m_Initialized(false), 
		m_TerrainPaintIntensity(0.01), 
		m_RoadOffset(0.3), 
		m_TerrainFlattenWidth(30), 
		m_TerrainPaintWidth(20),
		m_Material("MuddyRoadWithTracks"),
		m_RoadWidth(10),
		m_DitchWidth(1),
		m_UseSkirts(false),
		m_TerrainPaintLayer(TL_2),
		m_ClampToTerrain(true)
	{

	}

	RoadComponent::~RoadComponent()
	{

	}

	void RoadComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("RoadComponent",new Creator<RoadComponent, IComponent>);
		RegisterProperty<bool>("FlattenTerrain", &GASS::RoadComponent::GetFlattenTerrain, &GASS::RoadComponent::SetFlattenTerrain);
		RegisterProperty<bool>("PaintTerrain", &GASS::RoadComponent::GetPaintTerrain, &GASS::RoadComponent::SetPaintTerrain);
		RegisterProperty<float>("TerrainFlattenWidth", &GASS::RoadComponent::GetTerrainFlattenWidth, &GASS::RoadComponent::SetTerrainFlattenWidth);
		RegisterProperty<float>("TerrainPaintWidth", &GASS::RoadComponent::GetTerrainPaintWidth, &GASS::RoadComponent::SetTerrainPaintWidth);
		RegisterProperty<float>("TerrainPaintIntensity", &GASS::RoadComponent::GetTerrainPaintIntensity, &GASS::RoadComponent::SetTerrainPaintIntensity);
		RegisterProperty<TerrainLayerBinder>("TerrainPaintLayer", &GASS::RoadComponent::GetTerrainPaintLayer, &GASS::RoadComponent::SetTerrainPaintLayer);
		RegisterProperty<float>("RoadWidth", &GASS::RoadComponent::GetRoadWidth, &GASS::RoadComponent::SetRoadWidth);
		RegisterProperty<float>("RoadOffset", &GASS::RoadComponent::GetRoadOffset, &GASS::RoadComponent::SetRoadOffset);
		RegisterProperty<float>("DitchWidth", &GASS::RoadComponent::GetDitchWidth, &GASS::RoadComponent::SetDitchWidth);
		RegisterProperty<bool>("UseSkirts", &GASS::RoadComponent::GetUseSkirts, &GASS::RoadComponent::SetUseSkirts);
		RegisterProperty<bool>("ClampToTerrain", &GASS::RoadComponent::GetClampToTerrain, &GASS::RoadComponent::SetClampToTerrain);
		RegisterProperty<std::string>("Material", &GASS::RoadComponent::GetMaterial, &GASS::RoadComponent::SetMaterial);
	}

	void RoadComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(RoadComponent::OnLoad,LoadCoreComponentsMessage,2));
		GetSceneObject()->RegisterForMessage(REG_TMESS(RoadComponent::OnUnload,UnloadComponentsMessage,2));
		GetSceneObject()->RegisterForMessage(REG_TMESS(RoadComponent::OnUpdate,UpdateWaypointListMessage,1));
	}

	void RoadComponent::SetMaterial(const std::string value)
	{
		m_Material = value;
		UpdateRoadMesh();
	}

	std::string RoadComponent::GetMaterial() const 
	{
		return m_Material;
	}


	void RoadComponent::OnUnload(UnloadComponentsMessagePtr message)
	{

	}

	void RoadComponent::OnLoad(LoadCoreComponentsMessagePtr message)
	{
		m_Initialized = true;
		//get waypoint list
		WaypointListComponentPtr wpl = GetSceneObject()->GetFirstComponentByClass<WaypointListComponent>();
		if(!wpl)
			Log::Warning("RoadComponent depends on WaypointListComponent");
	}

	void RoadComponent::SetPaintTerrain(bool value)
	{
		if(!m_Initialized)
			return;


		WaypointListComponentPtr wpl = GetSceneObject()->GetFirstComponentByClass<WaypointListComponent>();
		if(wpl)
		{

			std::vector<Vec3> points = wpl->GetWaypoints();
			LocationComponentPtr location = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
			const Vec3 origo = location->GetWorldPosition();
			//create absolute positions
			for(int i = 0 ; i < points.size(); i++)
				points[i] = points[i] + origo;


			TerrainComponentPtr terrain = GetSceneObject()->GetSceneObjectManager()->GetSceneRoot()->GetFirstComponentByClass<ITerrainComponent>(true);
			if(terrain)
			{
				BaseSceneComponentPtr bsc = boost::shared_dynamic_cast<BaseSceneComponent>(terrain);
				bsc->GetSceneObject()->PostMessage(MessagePtr(new RoadMessage(points,0,m_TerrainPaintWidth,m_TerrainPaintIntensity,m_TerrainPaintLayer.Get())));
			}


			SceneObjectPtr last_obj;
			IComponentContainer::ComponentVector components;
			GetSceneObject()->GetSceneObjectManager()->GetSceneRoot()->GetComponentsByClass(components, "GrassLayerComponent", true);
			for(int i = 0 ;  i < components.size(); i++)
			{
				BaseSceneComponentPtr bsc = boost::shared_dynamic_cast<BaseSceneComponent>(components[i]);
				if(last_obj != bsc->GetSceneObject())
					bsc->GetSceneObject()->PostMessage(MessagePtr(new RoadMessage(points,0,m_TerrainPaintWidth,m_TerrainPaintIntensity,m_TerrainPaintLayer.Get())));
				last_obj = bsc->GetSceneObject();
			}
		}
	}
	bool RoadComponent::GetPaintTerrain() const
	{
		return false;
	}


	void RoadComponent::SetFlattenTerrain(bool value)
	{
		if(!m_Initialized)
			return;

		WaypointListComponentPtr wpl = GetSceneObject()->GetFirstComponentByClass<WaypointListComponent>();
		if(wpl)
		{

			std::vector<Vec3> points = wpl->GetWaypoints();
			LocationComponentPtr location = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
			const Vec3 origo = location->GetWorldPosition();
			//create absolute positions
			for(int i = 0 ; i < points.size(); i++)
				points[i] = points[i] + origo;


			TerrainComponentPtr terrain = GetSceneObject()->GetSceneObjectManager()->GetSceneRoot()->GetFirstComponentByClass<ITerrainComponent>(true);
			if(terrain)
			{
				BaseSceneComponentPtr bsc = boost::shared_dynamic_cast<BaseSceneComponent>(terrain);
				bsc->GetSceneObject()->PostMessage(MessagePtr(new RoadMessage(points,m_TerrainFlattenWidth,0,0,m_TerrainPaintLayer.Get())));
			}
		}
	}

	bool RoadComponent::GetFlattenTerrain() const
	{
		return false;
	}


	void RoadComponent::OnUpdate(UpdateWaypointListMessagePtr message)
	{
		UpdateRoadMesh();
	}



	

	void RoadComponent::UpdateRoadMesh() 
	{ 
		if(!m_Initialized)
			return;


		TerrainComponentPtr terrain = GetSceneObject()->GetSceneObjectManager()->GetSceneRoot()->GetFirstComponentByClass<ITerrainComponent>(true);
		LocationComponentPtr location = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
		const Vec3 origo = location->GetWorldPosition();
	
		WaypointListComponentPtr wpl = GetSceneObject()->GetFirstComponentByClass<WaypointListComponent>();

		ManualMeshDataPtr mesh_data(new ManualMeshData());

		mesh_data->Material = m_Material;
		mesh_data->Type = TRIANGLE_LIST;

		std::vector<Vec3> points = wpl->GetWaypoints();

		// for keeping track of the sides 
		Vec3 lr_vector; 

		Vec3 vertex; 
		Vec3 last_pos; 

		int num_horizontal_pts = 5; 
		int vertex_offset; 

		Float v_coord = 0; 
		Vec3 uv_old_pos = points[0]; 
		Vec3 uv_new_pos; 
		Vec3 pos, front; 


		float tot_width = m_RoadWidth + 2*m_DitchWidth;
		Float u = m_DitchWidth/tot_width;
		//Float u_coord[5] = {0,0.03125,0.5,1-0.03125,1};
		Float u_coord[5] = {0,u,0.5,1-u,1};
		Float lr_vector_multiplier[5] = {tot_width*0.5, m_RoadWidth*0.5, 0, -m_RoadWidth*0.5, -tot_width*0.5}; 

		Float curr_height; 
		Vec3 curr_vertices[5]; 

		// for some reason the first section may only have 1 segment... in this case we need to have last_pos defined 
		last_pos = points[0];

		for(int i = 0 ;  i < points.size() ;i++)
		{
			uv_new_pos = points[i]; 
			v_coord += (uv_new_pos - uv_old_pos).Length()/10; 
			uv_old_pos = uv_new_pos; 

			// move left or right 
			// calc lrVector 
			vertex = points[i];   // vertex is being used as a temporary variable 
			//curr_height = mTerrainMesh->getHeight(vertex); 
			curr_height = vertex.y;

			last_pos.y = vertex.y = 0; 

			// positive is left vector 
			if( i== 0 && points.size() > 1)
			{
				front = (points[1] - vertex); 
				front.y = 0;
			}
			else
				front = (vertex-last_pos); 

			lr_vector = Math::Cross(front,Vec3(0,1,0)); 
			lr_vector.Normalize(); 
			// end of lrVector calculation 

			for (int j = 0; j < num_horizontal_pts; j++) 
			{ 
				// create this side piece 
				curr_vertices[j] = vertex; 
				curr_vertices[j] += lr_vector*lr_vector_multiplier[j]; 

				// update height 
				//curr_vertices[j].y = mTerrainMesh->getHeight(curr_vertices[j]); 
				/*curr_vertices[j].y = vertex.y; 
				if (curr_vertices[j].y > curr_height && j != 0 && j != num_horizontal_pts-1)
				{
					curr_height = curr_vertices[j].y; 
				}*/
			} 


			if(terrain && m_ClampToTerrain)
			{
				for (int j = 1; j < num_horizontal_pts-1; j++) 
					curr_vertices[j].y = terrain->GetHeight(curr_vertices[j].x + origo.x,curr_vertices[j].z + origo.z) + m_RoadOffset - origo.y;

				if(!m_UseSkirts)
				{
					curr_vertices[0].y = terrain->GetHeight(curr_vertices[0].x + origo.x,curr_vertices[0].z + origo.z) - origo.y;
					curr_vertices[num_horizontal_pts-1].y = terrain->GetHeight(curr_vertices[num_horizontal_pts-1].x + origo.x,curr_vertices[num_horizontal_pts-1].z + origo.z)-origo.y;
				}
			}
			else
			{
				for (int j = 1; j < num_horizontal_pts-1; j++) 
					curr_vertices[j].y = curr_height + m_RoadOffset; 

				if(!m_UseSkirts)
				{
					curr_vertices[0].y = curr_height;
					curr_vertices[num_horizontal_pts-1].y = curr_height;
				}
			}

			

			last_pos = curr_vertices[2]; 

			// finally add vertices 
			for (int j = 0; j < num_horizontal_pts; j++) 
			{ 
				MeshVertex mesh_vertex;
				mesh_vertex.Pos = curr_vertices[j];
				mesh_vertex.Color.Set(1,1,1,1);
				mesh_vertex.TexCoord.x = u_coord[j];
				mesh_vertex.TexCoord.y = v_coord;
				// get vertex from created values 
				mesh_data->VertexVector.push_back(mesh_vertex);
				//m_RoadObject->position(curr_vertices[j]); 
				//m_RoadObject->normal(0,1,0);
				//m_RoadObject->textureCoord(u_coord[j], v_coord); 
				//mesh_data->IndexVector

			
			} 
		} 

		for (int n = 0; n < points.size()-1; n++) 
		{ 
			// create four: 0,1,4 0,4,3 1,2,5 1,5,4 (0->6 + offset) 
			for (vertex_offset = n*num_horizontal_pts; vertex_offset < (n+1)*num_horizontal_pts-1; vertex_offset++) // vertex_offset 
			{ 
				unsigned short face[3*2] = {vertex_offset + 0,vertex_offset + num_horizontal_pts+1, vertex_offset + 1, 
					vertex_offset + 0,vertex_offset + num_horizontal_pts,vertex_offset + num_horizontal_pts+1}; 

				for (int j=0; j<6; j++) 
					mesh_data->IndexVector.push_back(face[j]);
				
			} 
		} 
		MessagePtr mesh_message(new ManualMeshDataMessage(mesh_data));
		GetSceneObject()->PostMessage(mesh_message);
	}
}
