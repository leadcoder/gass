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


namespace GASS
{
	WaypointListComponent::WaypointListComponent() : m_Radius(0), m_EnableSpline(false), m_Initialized(false), m_AutoUpdateTangents(true), m_SplineSteps(10)
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
		RegisterProperty<int>("SplineSteps", &WaypointListComponent::GetSplineSteps, &WaypointListComponent::SetSplineSteps);
	}

	void WaypointListComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(WaypointListComponent::OnLoad,LoadCoreComponentsMessage,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(WaypointListComponent::OnUnload,UnloadComponentsMessage,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(WaypointListComponent::OnUpdate,UpdateWaypointListMessage,1));

	}

	int WaypointListComponent::GetSplineSteps()const
	{
		return m_SplineSteps;
	}

	void WaypointListComponent::SetSplineSteps(int steps)
	{
		m_SplineSteps = steps;
		UpdatePath();
	}


	float WaypointListComponent::GetRadius()const
	{
		return m_Radius;
	}

	void WaypointListComponent::SetRadius(float radius)
	{
		m_Radius = radius;
		UpdatePath();
	}

	bool WaypointListComponent::GetEnableSpline() const
	{
		return m_EnableSpline;
	}

	void WaypointListComponent::SetEnableSpline(bool value)
	{
		m_EnableSpline = value;
		UpdatePath();
	}

	void WaypointListComponent::OnUnload(UnloadComponentsMessagePtr message)
	{

	}

	void WaypointListComponent::OnLoad(LoadCoreComponentsMessagePtr message)
	{
		m_Initialized = true;
		UpdatePath();
	}

	void WaypointListComponent::OnUpdate(UpdateWaypointListMessagePtr message)
	{
		UpdatePath();
	}

	void WaypointListComponent::UpdatePath()
	{
		if(!m_Initialized)
			return;
		//collect all children and update path
		//const double line_steps = 115;
		ManualMeshDataPtr mesh_data(new ManualMeshData());
		MeshVertex vertex;
		mesh_data->Material = "WhiteTransparentNoLighting";

		vertex.TexCoord.Set(0,0);
		vertex.Color = Vec4(1,1,1,1);
		mesh_data->Type = LINE_STRIP;

		std::vector<Vec3> wps = WaypointListComponent::GetWaypoints();

		
		//LocationComponentPtr line_location = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
		//Vec3 line_pos = line_location->GetWorldPosition();
		for(size_t i = 0; i < wps.size(); i++)
		{
			vertex.Pos = wps[i];
			mesh_data->VertexVector.push_back(vertex);
		}


		if(mesh_data->VertexVector.size() > 0)
		{
			MessagePtr mesh_message(new ManualMeshDataMessage(mesh_data));
			//GetSceneObject()->PostMessage(mesh_message);
		}
	}

	std::vector<Vec3> WaypointListComponent::GetWaypoints() const
	{
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
				pos_vec.push_back(wp_location->GetPosition());
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
		UpdatePath();
	}

}