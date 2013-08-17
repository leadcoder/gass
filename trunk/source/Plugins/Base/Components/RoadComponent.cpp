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


#include "RoadComponent.h"
#include "Core/Utils/GASSLogManager.h"
#include "Core/Math/GASSQuaternion.h"
#include "Core/Math/GASSSplineAnimation.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/ComponentSystem/GASSIComponent.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/ComponentSystem/GASSIComponent.h"
#include "Core/ComponentSystem/GASSBaseComponentContainerTemplateManager.h"
#include "Core/ComponentSystem/GASSComponentContainerFactory.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/Interface/GASSILocationComponent.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/GASSMeshData.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/Interface/GASSICollisionSceneManager.h"
#include "Sim/Interface/GASSIWaypointListComponent.h"
#include "Sim/Interface/GASSITerrainComponent.h"
#include "Sim/GASSGraphicsMaterial.h"

namespace GASS
{
	template<> std::map<std::string ,TerrainLayer> SingleEnumBinder<TerrainLayer,TerrainLayerBinder>::m_NameToEnumMap;
	template<> std::map<TerrainLayer,std::string> SingleEnumBinder<TerrainLayer,TerrainLayerBinder>::m_EnumToNameMap;

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
		m_ClampToTerrain(true),
		m_TileScale(1,10),
		m_CAP(false),
		m_FadeStart(false),
		m_FadeEnd(false),
		m_CustomDitchTexturePercent(0)
	{

	}

	RoadComponent::~RoadComponent()
	{

	}

	void RoadComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("RoadComponent",new Creator<RoadComponent, IComponent>);
		GetClassRTTI()->SetMetaData(ObjectMetaDataPtr(new ObjectMetaData("RoadComponent", OF_VISIBLE)));
		RegisterProperty<bool>("FlattenTerrain", &GASS::RoadComponent::GetFlattenTerrain, &GASS::RoadComponent::SetFlattenTerrain,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<bool>("PaintTerrain", &GASS::RoadComponent::GetPaintTerrain, &GASS::RoadComponent::SetPaintTerrain,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<float>("TerrainFlattenWidth", &GASS::RoadComponent::GetTerrainFlattenWidth, &GASS::RoadComponent::SetTerrainFlattenWidth,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<float>("TerrainPaintWidth", &GASS::RoadComponent::GetTerrainPaintWidth, &GASS::RoadComponent::SetTerrainPaintWidth,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<float>("TerrainPaintIntensity", &GASS::RoadComponent::GetTerrainPaintIntensity, &GASS::RoadComponent::SetTerrainPaintIntensity,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<TerrainLayerBinder>("TerrainPaintLayer", &GASS::RoadComponent::GetTerrainPaintLayer, &GASS::RoadComponent::SetTerrainPaintLayer,
			EnumerationProxyPropertyMetaDataPtr(new EnumerationProxyPropertyMetaData("Select paint layer",PF_VISIBLE,&TerrainLayerBinder::GetStringEnumeration)));
			
		RegisterProperty<float>("RoadWidth", &GASS::RoadComponent::GetRoadWidth, &GASS::RoadComponent::SetRoadWidth,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<float>("RoadOffset", &GASS::RoadComponent::GetRoadOffset, &GASS::RoadComponent::SetRoadOffset,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<float>("DitchWidth", &GASS::RoadComponent::GetDitchWidth, &GASS::RoadComponent::SetDitchWidth,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<Vec2>("TileScale", &GASS::RoadComponent::GetTileScale, &GASS::RoadComponent::SetTileScale,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<bool>("UseSkirts", &GASS::RoadComponent::GetUseSkirts, &GASS::RoadComponent::SetUseSkirts,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<bool>("ClampToTerrain", &GASS::RoadComponent::GetClampToTerrain, &GASS::RoadComponent::SetClampToTerrain,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<std::string>("Material", &GASS::RoadComponent::GetMaterial, &GASS::RoadComponent::SetMaterial,
			GraphicsMaterialPropertyMetaDataPtr(new GraphicsMaterialPropertyMetaData("Road material from GASS_ROAD_MATERIALS resource group",PF_VISIBLE, "GASS_ROAD_MATERIALS")));
		RegisterProperty<float>("CustomDitchTexturePercent", &GASS::RoadComponent::GetCustomDitchTexturePercent, &GASS::RoadComponent::SetCustomDitchTexturePercent,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<bool>("CAP", &GASS::RoadComponent::GetCAP, &GASS::RoadComponent::SetCAP,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<bool>("FadeStart", &GASS::RoadComponent::GetFadeStart, &GASS::RoadComponent::SetFadeStart,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<bool>("FadeEnd", &GASS::RoadComponent::GetFadeEnd, &GASS::RoadComponent::SetFadeEnd,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
	}

	void RoadComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(RoadComponent::OnUpdate,UpdateWaypointListMessage,1));
		
		//get waypoint list
		SPTR<IWaypointListComponent> wpl = GetSceneObject()->GetFirstComponentByClass<IWaypointListComponent>();
		if(!wpl)
			LogManager::getSingleton().stream() << "WARNING:RoadComponent depends on WaypointListComponent";

		m_Initialized = true;
	}

	void RoadComponent::SetMaterial(const std::string &value)
	{
		m_Material = value;
		UpdateRoadMesh();
	}

	std::string  RoadComponent::GetMaterial() const 
	{
		return m_Material;
	}

	void RoadComponent::SetPaintTerrain(bool value)
	{
		if(!m_Initialized)
			return;

		WaypointListComponentPtr wpl = GetSceneObject()->GetFirstComponentByClass<IWaypointListComponent>();
		if(wpl)
		{

			std::vector<Vec3> points = wpl->GetWaypoints();
			LocationComponentPtr location = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
			const Vec3 origo = location->GetWorldPosition();
			//create absolute positions
			for(int i = 0 ; i < points.size(); i++)
				points[i] = points[i] + origo;

			HeightmapTerrainComponentPtr terrain = GetSceneObject()->GetScene()->GetRootSceneObject()->GetFirstComponentByClass<IHeightmapTerrainComponent>(true);
			if(terrain)
			{
				BaseSceneComponentPtr bsc = DYNAMIC_PTR_CAST<BaseSceneComponent>(terrain);
				if(bsc->GetSceneObject())
				{
					bsc->GetSceneObject()->GetParentSceneObject()->PostMessage(MessagePtr(new RoadMessage(points,0,m_TerrainPaintWidth,m_TerrainPaintIntensity,m_TerrainPaintLayer.GetValue())));
				}
			}

			SceneObjectPtr last_obj;
			IComponentContainer::ComponentVector components;
			GetSceneObject()->GetScene()->GetRootSceneObject()->GetComponentsByClass(components, "GrassLayerComponent", true);
			GetSceneObject()->GetScene()->GetRootSceneObject()->GetComponentsByClass(components, "TreeGeometryComponent", true);
			for(int i = 0 ;  i < components.size(); i++)
			{
				BaseSceneComponentPtr bsc = DYNAMIC_PTR_CAST<BaseSceneComponent>(components[i]);
				if(last_obj != bsc->GetSceneObject())
					bsc->GetSceneObject()->PostMessage(MessagePtr(new RoadMessage(points,0,m_TerrainPaintWidth,m_TerrainPaintIntensity,m_TerrainPaintLayer.GetValue())));
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

		WaypointListComponentPtr wpl = GetSceneObject()->GetFirstComponentByClass<IWaypointListComponent>();
		if(wpl)
		{

			std::vector<Vec3> points = wpl->GetWaypoints();
			LocationComponentPtr location = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
			const Vec3 origo = location->GetWorldPosition();
			//create absolute positions
			for(int i = 0 ; i < points.size(); i++)
				points[i] = points[i] + origo;

			HeightmapTerrainComponentPtr terrain = GetSceneObject()->GetScene()->GetRootSceneObject()->GetFirstComponentByClass<IHeightmapTerrainComponent>(true);
			if(terrain)
			{
				BaseSceneComponentPtr bsc = DYNAMIC_PTR_CAST<BaseSceneComponent>(terrain);
				bsc->GetSceneObject()->GetParentSceneObject()->PostMessage(MessagePtr(new RoadMessage(points,m_TerrainFlattenWidth,0,0,m_TerrainPaintLayer.GetValue())));
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


		HeightmapTerrainComponentPtr terrain = GetSceneObject()->GetScene()->GetRootSceneObject()->GetFirstComponentByClass<IHeightmapTerrainComponent>(true);
		LocationComponentPtr location = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
		const Vec3 origo = location->GetWorldPosition();
	
		WaypointListComponentPtr wpl = GetSceneObject()->GetFirstComponentByClass<IWaypointListComponent>();

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
		if(m_CustomDitchTexturePercent != 0)
			u = m_CustomDitchTexturePercent;
		//Float u_coord[5] = {0,0.03125,0.5,1-0.03125,1};
		Float u_coord[5] = {0,u,0.5,(1-u),1};
		Float lr_vector_multiplier[5] = {tot_width*0.5, m_RoadWidth*0.5, 0, -m_RoadWidth*0.5, -tot_width*0.5}; 

		Float curr_height; 
		Vec3 curr_vertices[5]; 

		// for some reason the first section may only have 1 segment... in this case we need to have last_pos defined 
		last_pos = points[0];

		for(int i = 0 ;  i < points.size() ;i++)
		{
			uv_new_pos = points[i]; 
			v_coord += (uv_new_pos - uv_old_pos).Length()/m_TileScale.y; 
			uv_old_pos = uv_new_pos; 

			// move left or right 
			// calc lrVector 
			vertex = points[i];   // vertex is being used as a temporary variable 
			//curr_height = mTerrainMesh->getHeight(vertex); 
			curr_height = vertex.y;

			last_pos.y = vertex.y = 0; 
			Float width_mult = 1.0;
			Float vertex_alpha = 1;
			// positive is left vector 
			if( i== 0 && points.size() > 1)
			{
				front = (points[1] - vertex); 
				front.y = 0;
				if(m_FadeStart)
					vertex_alpha = 0;
			}
			else if(i < points.size() - 1)
			{
				Vec3 v1 = (vertex-last_pos);
				Vec3 v2 = (points[i+1] - vertex);
				v1.Normalize();
				v2.Normalize();
				front = v1 + v2;

				front.Normalize();
				width_mult = Math::Dot(v1,front);
				if(width_mult > 0)
					width_mult = 1.0/width_mult;
			}
			else
			{
				front = (vertex-last_pos);
				if(m_FadeEnd)
					vertex_alpha = 0;
			}
			front.Normalize();
			lr_vector = Math::Cross(front,Vec3(0,1,0)); 
			lr_vector.Normalize(); 
			// end of lrVector calculation 

			for (int j = 0; j < num_horizontal_pts; j++) 
			{ 
				// create this side piece 
				curr_vertices[j] = vertex; 
				curr_vertices[j] += lr_vector*lr_vector_multiplier[j]*width_mult; 
			} 


			if(terrain && m_ClampToTerrain)
			{
				for (int j = 1; j < num_horizontal_pts-1; j++) 
					curr_vertices[j].y = terrain->GetHeightAtWorldLocation(curr_vertices[j].x + origo.x,curr_vertices[j].z + origo.z) + m_RoadOffset - origo.y;

				if(!m_UseSkirts)
				{
					curr_vertices[0].y = terrain->GetHeightAtWorldLocation(curr_vertices[0].x + origo.x,curr_vertices[0].z + origo.z) - origo.y;
					curr_vertices[num_horizontal_pts-1].y = terrain->GetHeightAtWorldLocation(curr_vertices[num_horizontal_pts-1].x + origo.x,curr_vertices[num_horizontal_pts-1].z + origo.z)-origo.y;
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
			/*for (int j = 0; j < num_horizontal_pts; j++) 
			{ 
				MeshVertex mesh_vertex;
				mesh_vertex.Pos = curr_vertices[j];
				//mesh_vertex.Normal.Set(0,1,0);
				mesh_vertex.Color.Set(1,1,1,1);
				mesh_vertex.TexCoord.x = u_coord[j]*m_TileScale.x;
				mesh_vertex.TexCoord.y = v_coord;
				mesh_data->VertexVector.push_back(mesh_vertex);
			}*/


			for (int j = 1; j < num_horizontal_pts; j++) 
			{ 
				MeshVertex mesh_vertex;
				mesh_vertex.Pos = curr_vertices[j-1];
				mesh_vertex.Normal.Set(0,1,0);
				mesh_vertex.Color.Set(1,1,1,vertex_alpha);
				mesh_vertex.TexCoord.x = u_coord[j-1]*m_TileScale.x;
				mesh_vertex.TexCoord.y = v_coord;
				mesh_data->VertexVector.push_back(mesh_vertex);

				mesh_vertex.Pos = curr_vertices[j];
				mesh_vertex.Normal.Set(0,1,0);
				mesh_vertex.Color.Set(1,1,1,vertex_alpha);
				mesh_vertex.TexCoord.x = u_coord[j]*m_TileScale.x;
				mesh_vertex.TexCoord.y = v_coord;
				mesh_data->VertexVector.push_back(mesh_vertex);
			}
		} 


		for (int n = 0; n < points.size()-1; n++) 
		{
			int num_horizontal_vertex = num_horizontal_pts*2-2;
			// create four: 0,1,4 0,4,3 1,2,5 1,5,4 (0->6 + offset) 
			for (vertex_offset = n*num_horizontal_vertex; vertex_offset < (n+1)*num_horizontal_vertex-1; vertex_offset += 2) // vertex_offset 
			{ 
				unsigned short face[3*2] = {vertex_offset + 0,vertex_offset + num_horizontal_vertex+1, vertex_offset + 1, 
					vertex_offset + 0,vertex_offset + num_horizontal_vertex,vertex_offset + num_horizontal_vertex+1}; 

				for (int j=0; j<6; j++)
					mesh_data->IndexVector.push_back(face[j]);
			} 
		}

		//Do normals
		for(size_t i = 0 ;  i < mesh_data->IndexVector.size(); i+=6)
		{
			MeshVertex* v1 = &mesh_data->VertexVector[mesh_data->IndexVector[i]];
			MeshVertex* v2 = &mesh_data->VertexVector[mesh_data->IndexVector[i+1]];
			MeshVertex* v3 = &mesh_data->VertexVector[mesh_data->IndexVector[i+2]];
			Vec3 normal = Math::GetNormal(v1->Pos,v2->Pos,v3->Pos);
			Vec3 tangent = v3->Pos - v1->Pos;
			tangent.Normalize();

			v1->Tangent = tangent;
			v2->Tangent = tangent;
			v3->Tangent = tangent;

			v1->Normal = normal;
			v2->Normal = normal;
			v3->Normal = normal;

			(&mesh_data->VertexVector[mesh_data->IndexVector[i+3]])->Normal = normal;
			(&mesh_data->VertexVector[mesh_data->IndexVector[i+4]])->Normal = normal;
			(&mesh_data->VertexVector[mesh_data->IndexVector[i+5]])->Normal = normal;

			(&mesh_data->VertexVector[mesh_data->IndexVector[i+3]])->Tangent = tangent;
			(&mesh_data->VertexVector[mesh_data->IndexVector[i+4]])->Tangent = tangent;
			(&mesh_data->VertexVector[mesh_data->IndexVector[i+5]])->Tangent = tangent;
		}

		
		if(m_CAP)
		{
			//CAP under
			int num_horizontal_vertex = num_horizontal_pts*2-2;
			for (int n = 0; n < points.size()-1; n++) 
			{
				vertex_offset = n*num_horizontal_vertex;
				mesh_data->IndexVector.push_back(vertex_offset + num_horizontal_vertex - 1 );
				mesh_data->IndexVector.push_back(vertex_offset + num_horizontal_vertex + num_horizontal_vertex - 1 );
				mesh_data->IndexVector.push_back(vertex_offset);

				mesh_data->IndexVector.push_back(vertex_offset + num_horizontal_vertex + num_horizontal_vertex - 1);
				mesh_data->IndexVector.push_back(vertex_offset + num_horizontal_vertex);
				mesh_data->IndexVector.push_back(vertex_offset);
			}

			//CAP START and END
			float offset = (points.size()-1)*num_horizontal_vertex;
			for (int i = 0; i < num_horizontal_vertex-1 ; i++ ) 
			{
				//start cap
				MeshVertex m0 = mesh_data->VertexVector[0];
				MeshVertex m1 = mesh_data->VertexVector[i];
				MeshVertex m2 = mesh_data->VertexVector[i+1];
				
				Vec3 normal = Math::GetNormal(m0.Pos,m1.Pos,m2.Pos);
				
				m0.Normal = normal;
				m1.Normal = normal;
				m2.Normal = normal;
				Vec3 t1 = m1.Pos - m0.Pos;
				Vec3 t2 = m2.Pos - m0.Pos;
				

				m0.TexCoord.x = 0;
				m0.TexCoord.y = 0;

				m1.TexCoord.y = t1.y;
				t1.y = 0;
				m1.TexCoord.x = t1.Length();

				m2.TexCoord.y = t2.y;
				t2.y = 0;
				m2.TexCoord.x = t2.Length();

				int start_ind = mesh_data->VertexVector.size();
				mesh_data->IndexVector.push_back(start_ind);
				mesh_data->IndexVector.push_back(start_ind+1);
				mesh_data->IndexVector.push_back(start_ind+2);
				mesh_data->VertexVector.push_back(m0);
				mesh_data->VertexVector.push_back(m1);
				mesh_data->VertexVector.push_back(m2);

				//end cap

				
				m0 = mesh_data->VertexVector[offset];
				m1 = mesh_data->VertexVector[offset + i];
				m2 = mesh_data->VertexVector[offset +i+1];
				normal = Math::GetNormal(m0.Pos,m1.Pos,m2.Pos);
				
				m0.Normal = normal;
				m1.Normal = normal;
				m2.Normal = normal;
				t1 = m1.Pos - m0.Pos;
				t2 = m2.Pos - m0.Pos;
				
				m0.TexCoord.x = 0;
				m0.TexCoord.y = 0;

				m1.TexCoord.y = t1.y;
				t1.y = 0;
				m1.TexCoord.x = t1.Length();

				m2.TexCoord.y = t2.y;
				t2.y = 0;
				m2.TexCoord.x = t2.Length();

				start_ind = mesh_data->VertexVector.size();
				mesh_data->IndexVector.push_back(start_ind);
				mesh_data->IndexVector.push_back(start_ind+1);
				mesh_data->IndexVector.push_back(start_ind+2);
				mesh_data->VertexVector.push_back(m2);
				mesh_data->VertexVector.push_back(m1);
				mesh_data->VertexVector.push_back(m0);
			} 
		}


		

		/*for (int n = 0; n < points.size()-1; n++) 
		{ 
			// create four: 0,1,4 0,4,3 1,2,5 1,5,4 (0->6 + offset) 
			for (vertex_offset = n*num_horizontal_pts; vertex_offset < (n+1)*num_horizontal_pts-1; vertex_offset++) // vertex_offset 
			{ 
				unsigned short face[3*2] = {vertex_offset + 0,vertex_offset + num_horizontal_pts+1, vertex_offset + 1, 
					vertex_offset + 0,vertex_offset + num_horizontal_pts,vertex_offset + num_horizontal_pts+1}; 

				for (int j=0; j<6; j++) 
					mesh_data->IndexVector.push_back(face[j]);
				
			} 
		} */
		MessagePtr mesh_message(new ManualMeshDataMessage(mesh_data));
		GetSceneObject()->PostMessage(mesh_message);
	}
}
