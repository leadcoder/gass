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

#include "WaypointListComponent.h"
#include "Core/Math/GASSMath.h"
#include "Core/Math/GASSQuaternion.h"
#include "Core/Math/GASSSplineAnimation.h"
#include "Sim/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/Messages/GASSPhysicsSceneObjectMessages.h"
#include "Sim/Interface/GASSILocationComponent.h"
#include "Sim/Interface/GASSIManualMeshComponent.h"
#include "Sim/GASSGraphicsMesh.h"
#include "Plugins/Base/CoreMessages.h"
#include "Plugins/Base/GASSCoreSceneManager.h"
#include "WaypointComponent.h"
#include <memory>
#include <sstream>
#include <fstream>


namespace GASS
{
	WaypointListComponent::WaypointListComponent() : 
		m_LineColor(0,0,1,1),
		m_WaypointTemplate("Waypoint")
		
	{

	}

	WaypointListComponent::~WaypointListComponent()
	{

	}

	void WaypointListComponent::RegisterReflection()
	{

		std::vector<std::string> ext;
		ext.emplace_back("txt");
		ComponentFactory::Get().Register<WaypointListComponent>();
		GetClassRTTI()->SetMetaData(std::make_shared<ClassMetaData>("WaypointListComponent", OF_VISIBLE));
		RegisterGetSet("Radius", &WaypointListComponent::GetRadius, &WaypointListComponent::SetRadius,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("EnableSpline", &WaypointListComponent::GetEnableSpline, &WaypointListComponent::SetEnableSpline,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("AutoUpdateTangents", &WaypointListComponent::GetAutoUpdateTangents, &WaypointListComponent::SetAutoUpdateTangents,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("ShowWaypoints", &WaypointListComponent::GetShowWaypoints, &WaypointListComponent::SetShowWaypoints,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("ShowPathLine", &WaypointListComponent::GetShowPathLine ,&WaypointListComponent::SetShowPathLine,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("SplineSteps", &WaypointListComponent::GetSplineSteps, &WaypointListComponent::SetSplineSteps,PF_VISIBLE | PF_EDITABLE,"");
		auto prop = RegisterGetSet("Export", &WaypointListComponent::GetExport, &WaypointListComponent::SetExport, PF_VISIBLE | PF_EDITABLE, "Export this path to text file");
		prop->SetMetaData(std::make_shared<FilePathPropertyMetaData>(FilePathPropertyMetaData::EXPORT_FILE,ext));
		RegisterGetSet("WaypointTemplate", &WaypointListComponent::GetWaypointTemplate, &WaypointListComponent::SetWaypointTemplate,PF_VISIBLE,"");
		RegisterGetSet("Closed", &WaypointListComponent::GetClosed, &WaypointListComponent::SetClosed,PF_VISIBLE,"");
		RegisterGetSet("AutoRotateWaypoints", &WaypointListComponent::GetAutoRotateWaypoints, &WaypointListComponent::SetAutoRotateWaypoints,PF_VISIBLE,"");
	}

	static const std::string MAT_NAME = "WaypointListLine";

	void WaypointListComponent::OnInitialize()
	{
		m_ConnectionLines = GetSceneObject()->GetChildByID("WP_CONNECTION_LINES");
		//create material for waypoint binding line
		GraphicsSystemPtr gfx_sys = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<IGraphicsSystem>();
		if (!gfx_sys->HasMaterial(MAT_NAME))
		{
			GraphicsMaterial line_mat;
			line_mat.Name = MAT_NAME;
			line_mat.Diffuse.Set(0, 0, 0, 1.0);
			line_mat.Ambient.Set(0, 0, 0);
			line_mat.SelfIllumination.Set(0.7, 1, 1);
			line_mat.DepthTest = false;
			line_mat.DepthWrite = false;
			gfx_sys->AddMaterial(line_mat);
		}
		RegisterForPostUpdate<CoreSceneManager>();
	}

	std::string WaypointListComponent::GetWaypointTemplate() const 
	{
		return m_WaypointTemplate;
	}

	void WaypointListComponent::SetWaypointTemplate(const std::string &name) 
	{
		m_WaypointTemplate=name;
	}

	void WaypointListComponent::SetShowPathLine(bool value) 
	{ 
		m_ShowPathLine = value; 
		SetDirty(true);
		if (!m_ShowPathLine && m_Initialized)
		{
			auto mm_comp = GetSceneObject()->GetFirstComponentByClass<IManualMeshComponent>();
			if (mm_comp)
				mm_comp->Clear();
		}
	}

	
	int WaypointListComponent::GetSplineSteps()const
	{
		return m_SplineSteps;
	}

	void WaypointListComponent::SetSplineSteps(int steps)
	{
		m_SplineSteps = steps;
		SetDirty(true);
	}

	void WaypointListComponent::SetDirty(bool value)
	{
		m_Dirty = value;
	}

	float WaypointListComponent::GetRadius()const
	{
		return m_Radius;
	}

	void WaypointListComponent::SetRadius(float radius)
	{
		m_Radius = radius;
		SetDirty(true);
	}

	bool WaypointListComponent::GetEnableSpline() const
	{
		return m_EnableSpline;
	}

	void WaypointListComponent::SetEnableSpline(bool value)
	{
		m_EnableSpline = value;
		SetDirty(true);
	}

	bool WaypointListComponent::GetShowWaypoints() const
	{
		return m_ShowWaypoints;
	}

	void WaypointListComponent::SetShowWaypoints(bool value)
	{
		m_ShowWaypoints = value;
		if(m_Initialized)
		{
			std::vector<WaypointComponentPtr> wp_vec;
			auto children = GetSceneObject()->GetChildren();
			while(children.hasMoreElements())
			{
				auto child_obj =  children.getNext();
				WaypointComponentPtr comp = child_obj->GetFirstComponentByClass<WaypointComponent>();
				if(comp)
				{
					child_obj->SetVisible(m_ShowWaypoints);
				}
			}
		}
	}

	void WaypointListComponent::SceneManagerTick(double /*delta_time*/)
	{

		if (!m_Initialized)
		{
			m_Initialized = true;
			SetShowWaypoints(m_ShowWaypoints);
		}
		

		if (m_Dirty)
		{
			NotifyPathUpdated();
			m_Dirty = false;
		}
	}

	void WaypointListComponent::NotifyPathUpdated()
	{
		if(!m_Initialized)
			return;

		std::vector<Vec3> wps = WaypointListComponent::GetWaypoints();

		//collect all children and update path
		//const double line_steps = 115;

		if(m_ShowPathLine)
		{
			GraphicsMeshPtr mesh_data(new GraphicsMesh());
			GraphicsSubMeshPtr sub_mesh_data(new GraphicsSubMesh());
			mesh_data->SubMeshVector.push_back(sub_mesh_data);
			sub_mesh_data->MaterialName = "WhiteTransparentNoLighting";
			sub_mesh_data->Type = LINE_STRIP;

			for(size_t i = 0; i < wps.size(); i++)
			{
				Vec3 pos =wps[i];
				sub_mesh_data->PositionVector.push_back(pos);
				sub_mesh_data->ColorVector.push_back(m_LineColor);
			}

			if(sub_mesh_data->PositionVector.size() > 0)
			{
					auto mm_comp = GetSceneObject()->GetFirstComponentByClass<IManualMeshComponent>();
					if (mm_comp)
					{
						mm_comp->SetMeshData(*mesh_data);
						mm_comp->SetSubMeshMaterial(MAT_NAME);
					}
			}
		}
		//create absolute positions
		LocationComponentPtr location = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
		Vec3 world_pos = location->GetWorldPosition();
		for(size_t i = 0; i < wps.size(); i++)
		{
			wps[i] += world_pos;
		}
		GetSceneObject()->PostEvent(std::make_shared<WaypointListUpdatedMessage>(wps));
	}

	std::vector<Vec3> WaypointListComponent::GetWaypoints(bool relative_position) const
	{
		Vec3 offset(0,0,0);

		if(!relative_position)
		{
			LocationComponentPtr my_location = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
			offset = my_location->GetWorldPosition();
		}

		Spline spline;
		std::vector<Vec3> pos_vec;
		std::vector<WaypointComponentPtr> wp_vec;
		std::vector<SceneObjectPtr> obj_vec;
		auto children = GetSceneObject()->GetChildren();
		while(children.hasMoreElements())
		{
			auto child_obj =  children.getNext();
			WaypointComponentPtr comp = child_obj->GetFirstComponentByClass<WaypointComponent>();
			if(comp &&  comp->IsActive())
			{
				LocationComponentPtr wp_location = child_obj->GetFirstComponentByClass<ILocationComponent>();
				pos_vec.push_back(wp_location->GetPosition()+offset);
				wp_vec.push_back(comp);
				obj_vec.push_back(child_obj);
			}
		}
		if(m_Closed && pos_vec.size() > 2)
		{
			pos_vec.push_back(pos_vec[0]);
		}


		//auto rotate waypoints?
		if(m_AutoRotateWaypoints && wp_vec.size() > 1)
		{
			Mat4 rot;
			rot.MakeIdentity();

			for(size_t i = 0; i < wp_vec.size(); i++)
			{
				Vec3 dir;
				if(i == 0)
				{
					dir = pos_vec[i] - pos_vec[i+1];
				}
				else if(i == wp_vec.size()-1)
				{
					dir = pos_vec[i-1] - pos_vec[i];
				}
				else
					dir = pos_vec[i-1] - pos_vec[i+1];

				dir.Normalize();
				Vec3 left = Vec3::Cross(dir,Vec3(0,1,0));
				left.Normalize();
				Vec3 up = Vec3::Cross(left,dir);
				up.Normalize();
				rot.SetRotationByAxis(-left, up, dir);
				Quaternion q;
				q.FromRotationMatrix(rot);
				wp_vec[i]->Rotate(q);
			}
		}

		const double steps = m_SplineSteps;

		if(m_EnableSpline)
		{
			for(size_t i = 0; i < pos_vec.size(); i++)
			{
				spline.AddPoint(pos_vec[i]);
				/*if(i < pos_vec.size()-1)
				{
					RecursiveIncreaseResolution(pos_vec[i],pos_vec[i+1],spline,min_dist);
				}*/
			}
			spline.RecalcTangents();

			for(size_t  i = 0; i < spline.GetTangents().size(); i++)
			{
				const int wp_index = i % static_cast<int>(wp_vec.size());
				if(!m_AutoUpdateTangents || wp_vec[wp_index]->GetCustomTangent()) //get tangents from wp
				{
					spline.GetTangents()[i] = wp_vec[wp_index]->GetTangent();//* wp_vec[i]->GetTangentWeight();
				}
				else //auto update wp tangent from spline
				{
					Vec3 tangent = spline.GetTangents().at(i);
					Float weight = tangent.Length();

					if(m_Radius > 0) //used fixed radius
					{
						Vec3 norm_tangent = tangent*(1.0/weight);
						spline.GetTangents()[i] = norm_tangent*m_Radius;
					}
					wp_vec[wp_index]->SetTangent(tangent);
				}
			}
			//update tangents
			pos_vec.clear();

			Vec3 last_point(0,0,0);

			for(unsigned int  i = 0; i < static_cast<unsigned int>(spline.GetPoints().size()); i++)
			{
				for(double t = 0; t <= 1; t += 1.0 / steps)
				{
					Vec3 point =  spline.Interpolate(i, t);
					if(point != last_point)
						pos_vec.push_back(point);
					last_point = point;
				}
			}
		}
		return pos_vec;
	}

	void WaypointListComponent::RecursiveIncreaseResolution(const Vec3& line_start,  const Vec3& line_end, SplineAnimation &spline, Float min_dist) const
	{
		Vec3 dir = (line_start - line_end);
		Float dist = dir.Length();
		if(dist > min_dist)
		{
			Vec3 new_pos = (line_start + line_end)*0.5;
			RecursiveIncreaseResolution(line_start,  new_pos, spline, min_dist);
			spline.AddNode(new_pos);
			RecursiveIncreaseResolution(new_pos,line_end, spline, min_dist);
		}
	}

	bool WaypointListComponent::GetAutoUpdateTangents() const
	{
		return m_AutoUpdateTangents;
	}

	void WaypointListComponent::SetAutoUpdateTangents(bool value)
	{
		m_AutoUpdateTangents = value;
		SetDirty(true);
	}

	void WaypointListComponent::SetExport(const FilePath &filename)
	{
		if(!m_Initialized)
			return;
		std::vector<Vec3> wps = WaypointListComponent::GetWaypoints();

		if(wps.size() > 0)
		{
			LocationComponentPtr location = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
			Vec3 world_pos = location->GetWorldPosition();

			std::stringstream ss;
			std::ofstream file_ptr;
			file_ptr.open(filename.GetFullPath().c_str());

			for(size_t i = 0; i < wps.size(); i++)
			{
				ss << (wps[i] + world_pos) << "\n";
			}
			file_ptr << ss.str().c_str();
			file_ptr.close();
		}
	}

	FilePath WaypointListComponent::GetExport() const
	{
		return FilePath("");
	}
}
