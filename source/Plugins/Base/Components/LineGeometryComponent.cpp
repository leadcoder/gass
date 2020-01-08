/****************************************************************************
* This file is part of GASS.                                                *
* See https://github.com/leadcoder/gass                                     *
*                                                                           *
* Copyright (c) 2008-2016 GASS team. See Contributors.txt for details.      *
*                                                                           *
* GASS is free software: you can redistribute it and/or modify              *
* it under the terms of the GNU Lesser General Public License as published  *
* by the Free Software Foundation, either version 3 of the License, or      *
* (at your option) any later version.                                       *
*                                                                           *
* GASS is distributed in the hope that it will be useful,                   *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU Lesser General Public License for more details.                       *
*                                                                           *
* You should have received a copy of the GNU Lesser General Public License  *
* along with GASS. If not, see <http://www.gnu.org/licenses/>.              *
*****************************************************************************/


#include "LineGeometryComponent.h"
#include "Core/Math/GASSMath.h"
#include "Core/Math/GASSSplineAnimation.h"
#include "Core/Math/GASSTriangle.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/ComponentSystem/GASSComponent.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/ComponentSystem/GASSComponentContainerFactory.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/Interface/GASSILocationComponent.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/GASSGraphicsMesh.h"
#include "Sim/Interface/GASSIWaypointListComponent.h"
#include "Sim/Interface/GASSITerrainComponent.h"
#include "Sim/GASSGraphicsMaterial.h"

namespace GASS
{

	std::vector<std::string> GetLineMaterials()
	{
		GASS::GraphicsSystemPtr gfx_system = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<IGraphicsSystem>();
		std::vector<std::string> content = gfx_system->GetMaterialNames("GASS_ROAD_MATERIALS");
		return content;
	}

	LineGeometryComponent::LineGeometryComponent() : m_Initialized(false),
		m_Offset(0.3f),
		m_Material("MuddyRoadWithTracks"),
		m_Width(10),
		m_ClampToTerrain(true),
		m_TileScale(1,1),
		m_FadeStart(false),
		m_FadeEnd(false),
		m_Color(1,1,1,1),
		m_RotateTexture(false),
		m_CustomDitchTexturePercent(0)
	{

	}

	LineGeometryComponent::~LineGeometryComponent()
	{

	}

	void LineGeometryComponent::RegisterReflection()
	{
		ComponentFactory::Get().Register<LineGeometryComponent>();
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("LineGeometryComponent", OF_VISIBLE)));

		RegisterGetSet("Width", &GASS::LineGeometryComponent::GetWidth, &GASS::LineGeometryComponent::SetWidth,PF_VISIBLE | PF_EDITABLE,"");
		RegisterMember("Color", &GASS::LineGeometryComponent::m_Color,PF_VISIBLE | PF_EDITABLE,"");
		RegisterMember("Offset", &GASS::LineGeometryComponent::m_Offset,PF_VISIBLE | PF_EDITABLE,"");
		RegisterMember("TileScale", &GASS::LineGeometryComponent::m_TileScale,PF_VISIBLE | PF_EDITABLE,"");
		auto prop = RegisterGetSet("Material", &GASS::LineGeometryComponent::GetMaterial, &GASS::LineGeometryComponent::SetMaterial, PF_VISIBLE | PF_EDITABLE, "Road material from GASS_ROAD_MATERIALS resource group");
		prop->SetOptionsFunction(&GetLineMaterials);
		RegisterMember("FadeStart", &GASS::LineGeometryComponent::m_FadeStart,PF_VISIBLE | PF_EDITABLE,"");
		RegisterMember("FadeEnd", &GASS::LineGeometryComponent::m_FadeEnd,PF_VISIBLE | PF_EDITABLE,"");
		RegisterMember("RotateTexture", &GASS::LineGeometryComponent::m_RotateTexture,PF_VISIBLE | PF_EDITABLE,"");
		RegisterMember("WapointListObject", &GASS::LineGeometryComponent::m_WapointListObject);
	}

	void LineGeometryComponent::OnInitialize()
	{
		if(m_WapointListObject.IsValid())
		{
			m_WapointListObject->RegisterForMessage(REG_TMESS(LineGeometryComponent::OnUpdate,WaypointListUpdatedMessage,1));
		}
		else
		{
			GASS_SHARED_PTR<IWaypointListComponent> wpl = GetSceneObject()->GetFirstComponentByClass<IWaypointListComponent>();
			if(!wpl)
				GASS_LOG(LWARNING) << "LineComponent depends on WaypointListComponent";
			GetSceneObject()->RegisterForMessage(REG_TMESS(LineGeometryComponent::OnUpdate,WaypointListUpdatedMessage,1));
		}

		m_Initialized = true;
	}

	void LineGeometryComponent::SetMaterial(const std::string &value)
	{
		m_Material = value;
		UpdateMesh();
	}

	std::string  LineGeometryComponent::GetMaterial() const
	{
		return m_Material;
	}

	void LineGeometryComponent::OnUpdate(WaypointListUpdatedMessagePtr message)
	{
		UpdateMesh();
	}

	void LineGeometryComponent::UpdateMesh()
	{
		if(!m_Initialized)
			return;

		HeightmapTerrainComponentPtr terrain = GetSceneObject()->GetScene()->GetRootSceneObject()->GetFirstComponentByClass<IHeightmapTerrainComponent>(true);
		LocationComponentPtr location = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
		const Vec3 origo = location->GetWorldPosition();

		WaypointListComponentPtr wpl;

		if(m_WapointListObject.IsValid())
		{
			wpl = m_WapointListObject->GetFirstComponentByClass<IWaypointListComponent>();

		}
		else
			wpl = GetSceneObject()->GetFirstComponentByClass<IWaypointListComponent>();

		if(!wpl)
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Couldn't find waypoint list", "LineComponent::UpdateMesh");

		std::vector<Vec3> points = wpl->GetWaypoints();

		if(points.size() < 2)
			return;

		GraphicsMeshPtr mesh_data(new GraphicsMesh());
		GraphicsSubMeshPtr sub_mesh_data(new GraphicsSubMesh());
		mesh_data->SubMeshVector.push_back(sub_mesh_data);

		std::vector<Vec4> tex_coords;


		sub_mesh_data->MaterialName = m_Material;
		sub_mesh_data->Type = TRIANGLE_LIST;



		// for keeping track of the sides
		Vec3 lr_vector;

		Vec3 vertex;
		Vec3 last_pos;

		int num_horizontal_pts = 3;
		int vertex_offset;

		Float v_coord = 0;
		Vec3 uv_old_pos = points[0];
		Vec3 uv_new_pos;
		Vec3 front;
		//Vec3 pos
		Float u_coord[3] = {0,0.5,1};
		Float lr_vector_multiplier[3] = {m_Width*0.5, 0, -m_Width*0.5};

		Float curr_height;
		Vec3 curr_vertices[3];

		// for some reason the first section may only have 1 segment... in this case we need to have last_pos defined
		last_pos = points[0];

		for(int i = 0 ;  i < static_cast<int>(points.size()) ;i++)
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
			Float vertex_alpha = m_Color.a;
			// positive is left vector
			if( i== 0 && points.size() > 1)
			{
				front = (points[1] - vertex);
				front.y = 0;
				if(m_FadeStart)
					vertex_alpha = 0;
			}
			else if(i < static_cast<int>(points.size()) - 1)
			{
				Vec3 v1 = (vertex-last_pos);
				Vec3 v2 = (points[i+1] - vertex);
				v1.Normalize();
				v2.Normalize();
				front = v1 + v2;

				front.Normalize();
				width_mult = Vec3::Dot(v1,front);
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
			lr_vector = Vec3::Cross(front,Vec3(0,1,0));
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
				for (int j = 0; j < num_horizontal_pts; j++)
					curr_vertices[j].y = terrain->GetHeightAtWorldLocation(curr_vertices[j].x + origo.x,curr_vertices[j].z + origo.z) + m_Offset - origo.y;
			}
			else
			{
				for (int j = 0; j < num_horizontal_pts; j++)
					curr_vertices[j].y = curr_height + m_Offset;
			}
			last_pos = curr_vertices[1];

			for (int j = 1; j < num_horizontal_pts; j++)
			{
				Vec3 pos = curr_vertices[j-1];
				ColorRGBA color = m_Color;
				color.a = vertex_alpha;
				Vec4 tex_coord;
				if(m_RotateTexture)
					tex_coord.Set(u_coord[j-1]*m_TileScale.x,v_coord,0,0);
				else
					tex_coord.Set(pos.x*m_TileScale.x,pos.z*m_TileScale.y,0,0);

				tex_coords.push_back(tex_coord);
				sub_mesh_data->PositionVector.push_back(pos);
				sub_mesh_data->ColorVector.push_back(color);

				pos =curr_vertices[j];
				if(m_RotateTexture)
					tex_coord.Set(u_coord[j]*m_TileScale.x,v_coord,0,0);
				else
					tex_coord.Set(pos.x*m_TileScale.x,pos.z*m_TileScale.y,0,0);

				tex_coords.push_back(tex_coord);
				sub_mesh_data->PositionVector.push_back(pos);
				sub_mesh_data->ColorVector.push_back(color);
			}
		}

		for (int n = 0; n < static_cast<int>(points.size())-1; n++)
		{
			int num_horizontal_vertex = num_horizontal_pts*2-2;
			// create four: 0,1,4 0,4,3 1,2,5 1,5,4 (0->6 + offset)
			for (vertex_offset = n*num_horizontal_vertex; vertex_offset < (n+1)*num_horizontal_vertex-1; vertex_offset += 2) // vertex_offset
			{
				unsigned short face[3*2] = { static_cast<unsigned short>(vertex_offset + 0),
					static_cast<unsigned short>(vertex_offset + num_horizontal_vertex+1),
					static_cast<unsigned short>(vertex_offset + 1),
					static_cast<unsigned short>(vertex_offset + 0),
					static_cast<unsigned short>(vertex_offset + num_horizontal_vertex),
					static_cast<unsigned short>(vertex_offset + num_horizontal_vertex+1)};

				for (int j=0; j<6; j++)
					sub_mesh_data->IndexVector.push_back(face[j]);
			}
		}

		sub_mesh_data->NormalVector.resize(sub_mesh_data->PositionVector.size());
		sub_mesh_data->TangentVector.resize(sub_mesh_data->PositionVector.size());

		//Do normals
		for(size_t i = 0 ;  i < sub_mesh_data->IndexVector.size(); i+=6)
		{
			Vec3* v1 = &sub_mesh_data->PositionVector[sub_mesh_data->IndexVector[i]];
			Vec3* v2 = &sub_mesh_data->PositionVector[sub_mesh_data->IndexVector[i+1]];
			Vec3* v3 = &sub_mesh_data->PositionVector[sub_mesh_data->IndexVector[i+2]];
			Vec3 normal = Triangle(*v1, *v2, *v3).GetNormal();
			Vec3 tangent = *v3 - *v1;
			tangent.Normalize();

			(sub_mesh_data->NormalVector[sub_mesh_data->IndexVector[i]]) = normal;
			(sub_mesh_data->NormalVector[sub_mesh_data->IndexVector[i+1]]) = normal;
			(sub_mesh_data->NormalVector[sub_mesh_data->IndexVector[i+2]]) = normal;

			(sub_mesh_data->TangentVector[sub_mesh_data->IndexVector[i]]) = tangent;
			(sub_mesh_data->TangentVector[sub_mesh_data->IndexVector[i+1]]) = tangent;
			(sub_mesh_data->TangentVector[sub_mesh_data->IndexVector[i+2]]) = tangent;


			(sub_mesh_data->NormalVector[sub_mesh_data->IndexVector[i+3]]) = normal;
			(sub_mesh_data->NormalVector[sub_mesh_data->IndexVector[i+4]]) = normal;
			(sub_mesh_data->NormalVector[sub_mesh_data->IndexVector[i+5]]) = normal;

			(sub_mesh_data->TangentVector[sub_mesh_data->IndexVector[i+3]]) = tangent;
			(sub_mesh_data->TangentVector[sub_mesh_data->IndexVector[i+4]]) = tangent;
			(sub_mesh_data->TangentVector[sub_mesh_data->IndexVector[i+5]]) = tangent;
		}
		sub_mesh_data->TexCoordsVector.push_back(tex_coords);
		GetSceneObject()->PostRequest(ManualMeshDataRequestPtr(new ManualMeshDataRequest(mesh_data)));
	}
}
