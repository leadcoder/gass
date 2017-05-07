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
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/ComponentSystem/GASSComponent.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/ComponentSystem/GASSComponentContainerFactory.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/Messages/GASSPhysicsSceneObjectMessages.h"
#include "Sim/Interface/GASSILocationComponent.h"
#include "Sim/GASSGraphicsMesh.h"
#include "Plugins/Base/CoreMessages.h"
#include "WaypointComponent.h"
#include <sstream>
#include <fstream>


namespace GASS
{
	WaypointListComponent::WaypointListComponent() : m_Radius(0),
		m_EnableSpline(false),
		m_Initialized(false),
		m_AutoUpdateTangents(true),
		m_SplineSteps(10),
		m_ShowWaypoints(true),
		m_ShowPathLine(false),
		m_LineColor(0,0,1,1),
		m_WaypointTemplate("Waypoint"),
		m_Closed(false),
		m_AutoRotateWaypoints(false)
	{

	}

	WaypointListComponent::~WaypointListComponent()
	{

	}

	void WaypointListComponent::RegisterReflection()
	{

		std::vector<std::string> ext;
		ext.push_back("txt");

		ComponentFactory::GetPtr()->Register("WaypointListComponent",new Creator<WaypointListComponent, Component>);
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("WaypointListComponent", OF_VISIBLE)));
		RegisterProperty<float>("Radius", &WaypointListComponent::GetRadius, &WaypointListComponent::SetRadius,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<bool>("EnableSpline", &WaypointListComponent::GetEnableSpline, &WaypointListComponent::SetEnableSpline,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<bool>("AutoUpdateTangents", &WaypointListComponent::GetAutoUpdateTangents, &WaypointListComponent::SetAutoUpdateTangents,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<bool>("ShowWaypoints", &WaypointListComponent::GetShowWaypoints, &WaypointListComponent::SetShowWaypoints,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<bool>("ShowPathLine", &WaypointListComponent::GetShowPathLine, &WaypointListComponent::SetShowPathLine,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<int>("SplineSteps", &WaypointListComponent::GetSplineSteps, &WaypointListComponent::SetSplineSteps,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<FilePath>("Export", &WaypointListComponent::GetExport, &WaypointListComponent::SetExport,
			FilePathPropertyMetaDataPtr(new FilePathPropertyMetaData("Export this path to text file",PF_VISIBLE | PF_EDITABLE, FilePathPropertyMetaData::EXPORT_FILE,ext)));
		RegisterProperty<std::string>("WaypointTemplate", &WaypointListComponent::GetWaypointTemplate, &WaypointListComponent::SetWaypointTemplate,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE)));
		RegisterProperty<bool>("Closed", &WaypointListComponent::GetClosed, &WaypointListComponent::SetClosed,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE)));
		RegisterProperty<bool>("AutoRotateWaypoints", &WaypointListComponent::GetAutoRotateWaypoints, &WaypointListComponent::SetAutoRotateWaypoints,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE)));

	}

	void WaypointListComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(WaypointListComponent::OnPostInitializedEvent,PostInitializedEvent,0));
		m_ConnectionLines = GetSceneObject()->GetChildByID("WP_CONNECTION_LINES");
	}

	static const std::string MAT_NAME = "WaypointListLine";

	void WaypointListComponent::OnPostInitializedEvent(PostInitializedEventPtr message)
	{
		//create material for waypoint binding line
		GraphicsSystemPtr gfx_sys = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<IGraphicsSystem>();
		if(!gfx_sys->HasMaterial(MAT_NAME))
		{
			GraphicsMaterial line_mat;
			line_mat.Name = MAT_NAME;
			line_mat.Diffuse.Set(0,0,0,1.0);
			line_mat.Ambient.Set(0,0,0);
			line_mat.SelfIllumination.Set(0.7,1,1);
			line_mat.DepthTest = false;
			line_mat.DepthWrite = false;
			gfx_sys->AddMaterial(line_mat);
		}

		//No need to be attached to parent any more
		GetSceneObject()->PostRequest(AttachToParentRequestPtr(new AttachToParentRequest(false)));
		m_Initialized = true;

		UpdatePath();
		SetShowWaypoints(m_ShowWaypoints);
	}

	std::string WaypointListComponent::GetWaypointTemplate() const {return m_WaypointTemplate;}
	void WaypointListComponent::SetWaypointTemplate(const std::string &name) {m_WaypointTemplate=name;}

	int WaypointListComponent::GetSplineSteps()const
	{
		return m_SplineSteps;
	}

	void WaypointListComponent::SetSplineSteps(int steps)
	{
		m_SplineSteps = steps;
		if(m_Initialized)
			UpdatePath();
	}


	float WaypointListComponent::GetRadius()const
	{
		return m_Radius;
	}

	void WaypointListComponent::SetRadius(float radius)
	{
		m_Radius = radius;
		if(m_Initialized)
			UpdatePath();
	}

	bool WaypointListComponent::GetEnableSpline() const
	{
		return m_EnableSpline;
	}

	void WaypointListComponent::SetEnableSpline(bool value)
	{
		m_EnableSpline = value;
		if(m_Initialized)
			UpdatePath();
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
			ComponentContainer::ComponentContainerIterator children = GetSceneObject()->GetChildren();
			while(children.hasMoreElements())
			{
				SceneObjectPtr child_obj =  GASS_STATIC_PTR_CAST<SceneObject>(children.getNext());
				WaypointComponentPtr comp = child_obj->GetFirstComponentByClass<WaypointComponent>();
				if(comp)
				{
					child_obj->PostRequest(LocationVisibilityRequestPtr(new LocationVisibilityRequest(m_ShowWaypoints)));
					child_obj->PostRequest(CollisionSettingsRequestPtr(new CollisionSettingsRequest(m_ShowWaypoints)));

					SceneObjectPtr tangent = child_obj->GetFirstChildByName("Tangent",false);
					if(tangent)
					{
						tangent->PostRequest(LocationVisibilityRequestPtr(new LocationVisibilityRequest(m_ShowWaypoints)));
						tangent->PostRequest(CollisionSettingsRequestPtr(new CollisionSettingsRequest(m_ShowWaypoints)));
					}
				}
			}
		}
	}

	void WaypointListComponent::UpdatePath()
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
				/*SceneObjectPtr line_obj = _GetConnectionLines();
				if(line_obj)
				{
					line->PostRequest(ManualMeshDataRequestPtr(new ManualMeshDataRequest(mesh_data)));
					//update material
					line->PostRequest(ReplaceMaterialRequestPtr(new ReplaceMaterialRequest(MAT_NAME)));
				}
				else //remove this*/
				{

					GetSceneObject()->PostRequest(ManualMeshDataRequestPtr(new ManualMeshDataRequest(mesh_data)));
					//update material
					GetSceneObject()->PostRequest(ReplaceMaterialRequestPtr(new ReplaceMaterialRequest(MAT_NAME)));
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
		GetSceneObject()->PostEvent(WaypointListUpdatedMessagePtr(new WaypointListUpdatedMessage(wps)));
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
		ComponentContainer::ComponentContainerIterator children = GetSceneObject()->GetChildren();
		while(children.hasMoreElements())
		{
			SceneObjectPtr child_obj =  GASS_STATIC_PTR_CAST<SceneObject>(children.getNext());
			WaypointComponentPtr comp = child_obj->GetFirstComponentByClass<WaypointComponent>();
			if(comp)
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
				rot.SetZAxis(dir);
				rot.SetYAxis(up);
				rot.SetXAxis(-left);
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
				if(!m_AutoUpdateTangents) //get tangents from wp
				{
					spline.GetTangents()[i] = wp_vec[i]->GetTangent();//* wp_vec[i]->GetTangentWeight();
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
					if(i < wp_vec.size())
					{
						wp_vec[i]->SetTangent(tangent);
						wp_vec[i]->SetTangentLength(weight);
					}
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
		if(m_Initialized)
			UpdatePath();
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
