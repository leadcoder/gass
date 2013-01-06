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
#include "WaypointListComponent.h"
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
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/Interface/GASSICollisionSystem.h"

#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"

#include "Sim/Messages/GASSPhysicsSceneObjectMessages.h"
#include "Sim/Interface/GASSILocationComponent.h"
#include "Sim/GASSMeshData.h"
#include "WaypointComponent.h"

#include <iostream>
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
		m_LineColor(0,0,1,1)
	{

	}

	WaypointListComponent::~WaypointListComponent()
	{

	}

	void WaypointListComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("WaypointListComponent",new Creator<WaypointListComponent, IComponent>);
		RegisterProperty<float>("Radius", &WaypointListComponent::GetRadius, &WaypointListComponent::SetRadius);
		RegisterProperty<bool>("EnableSpline", &WaypointListComponent::GetEnableSpline, &WaypointListComponent::SetEnableSpline);
		RegisterProperty<bool>("AutoUpdateTangents", &WaypointListComponent::GetAutoUpdateTangents, &WaypointListComponent::SetAutoUpdateTangents);
		RegisterProperty<bool>("ShowWaypoints", &WaypointListComponent::GetShowWaypoints, &WaypointListComponent::SetShowWaypoints);
		RegisterProperty<bool>("ShowPathLine", &WaypointListComponent::GetShowPathLine, &WaypointListComponent::SetShowPathLine);
		RegisterProperty<int>("SplineSteps", &WaypointListComponent::GetSplineSteps, &WaypointListComponent::SetSplineSteps);
		RegisterProperty<FilePath>("Export", &WaypointListComponent::GetExport, &WaypointListComponent::SetExport);
	}

	void WaypointListComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(WaypointListComponent::OnUpdate,UpdateWaypointListMessage,1));

		m_Initialized = true;
		
		UpdatePath();
		SetShowWaypoints(m_ShowWaypoints);
	}

	int WaypointListComponent::GetSplineSteps()const
	{
		return m_SplineSteps;
	}

	void WaypointListComponent::SetSplineSteps(int steps)
	{
		m_SplineSteps = steps;
		if(m_Initialized)
			GetSceneObject()->PostMessage(MessagePtr(new UpdateWaypointListMessage()));
	}


	float WaypointListComponent::GetRadius()const
	{
		return m_Radius;
	}

	void WaypointListComponent::SetRadius(float radius)
	{
		m_Radius = radius;
		if(m_Initialized)
			GetSceneObject()->PostMessage(MessagePtr(new UpdateWaypointListMessage()));
	}

	bool WaypointListComponent::GetEnableSpline() const
	{
		return m_EnableSpline;
	}

	void WaypointListComponent::SetEnableSpline(bool value)
	{
		m_EnableSpline = value;
		if(m_Initialized)
			GetSceneObject()->PostMessage(MessagePtr(new UpdateWaypointListMessage()));
		//UpdatePath();
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
			IComponentContainer::ComponentContainerIterator children = GetSceneObject()->GetChildren();
			while(children.hasMoreElements())
			{
				SceneObjectPtr child_obj =  boost::shared_static_cast<SceneObject>(children.getNext());
				WaypointComponentPtr comp = child_obj->GetFirstComponentByClass<WaypointComponent>();
				if(comp)
				{
					child_obj->PostMessage(MessagePtr(new VisibilityMessage(m_ShowWaypoints)));
					child_obj->PostMessage(MessagePtr(new CollisionSettingsMessage(m_ShowWaypoints)));

					SceneObjectPtr tangent = child_obj->GetFirstChildByName("Tangent",false);
					if(tangent)
					{
						tangent->PostMessage(MessagePtr(new VisibilityMessage(m_ShowWaypoints)));
						tangent->PostMessage(MessagePtr(new CollisionSettingsMessage(m_ShowWaypoints)));
					}
				}
			}
		}
	}

	void WaypointListComponent::OnUpdate(UpdateWaypointListMessagePtr message)
	{
		UpdatePath();
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
			ManualMeshDataPtr mesh_data(new ManualMeshData());
			MeshVertex vertex;
			mesh_data->Material = "WhiteTransparentNoLighting";

			vertex.TexCoord.Set(0,0);
			vertex.Color = m_LineColor;
			mesh_data->Type = LINE_STRIP;
			
			for(size_t i = 0; i < wps.size(); i++)
			{
				vertex.Pos = wps[i];
				mesh_data->VertexVector.push_back(vertex);
			}


			if(mesh_data->VertexVector.size() > 0)
			{
				MessagePtr mesh_message(new ManualMeshDataMessage(mesh_data));
				GetSceneObject()->PostMessage(mesh_message);
			}
		}
		//create absolute positions
		LocationComponentPtr location = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
		Vec3 world_pos = location->GetWorldPosition();
		for(size_t i = 0; i < wps.size(); i++)
		{
			wps[i] += world_pos;
		}

		GetSceneObject()->PostMessage(MessagePtr(new WaypointListUpdatedMessage(wps)));

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
		IComponentContainer::ComponentContainerIterator children = GetSceneObject()->GetChildren();
		while(children.hasMoreElements())
		{
			SceneObjectPtr child_obj =  boost::shared_static_cast<SceneObject>(children.getNext());
			WaypointComponentPtr comp = child_obj->GetFirstComponentByClass<WaypointComponent>();
			if(comp)
			{
				LocationComponentPtr wp_location = child_obj->GetFirstComponentByClass<ILocationComponent>();
				pos_vec.push_back(wp_location->GetPosition()+offset);
				wp_vec.push_back(comp);
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
					wp_vec[i]->SetTangent(tangent);
					wp_vec[i]->SetTangentLength(weight);
				}
			}
			//update tangents
			pos_vec.clear();

			Vec3 last_point(0,0,0);

			for(int  i = 0; i < spline.GetPoints().size(); i++)
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
			GetSceneObject()->PostMessage(MessagePtr(new UpdateWaypointListMessage()));
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
