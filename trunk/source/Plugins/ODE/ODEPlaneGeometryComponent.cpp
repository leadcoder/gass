/****************************************************************************
* This file is part of GASS.                                                *
* See http://code.google.com/p/gass/                                 *
*                                                                           *
* Copyright (c) 2008-2009 GASS team. See Contributors.txt for details.      *
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

#ifdef WIN32
#define NOMINMAX
#include <algorithm>
#endif

#include "Plugins/ODE/ODEPlaneGeometryComponent.h"
#include "Plugins/ODE/ODEPhysicsSceneManager.h"
#include "Plugins/ODE/ODEBodyComponent.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/ComponentSystem/BaseComponentContainerTemplateManager.h"

#include "Core/MessageSystem/MessageManager.h"
#include "Core/Math/AABox.h"
#include "Core/Utils/Log.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Scenario/Scene/SceneObjectManager.h"
#include "Sim/Scenario/Scene/SceneObjectTemplate.h"
#include "Sim/Components/Graphics/Geometry/IGeometryComponent.h"
#include "Sim/Components/Graphics/Geometry/IMeshComponent.h"
#include "Sim/Components/Graphics/Geometry/ITerrainComponent.h"
#include "Sim/Components/Graphics/ILocationComponent.h"
#include "Sim/SimEngine.h"
#include <boost/bind.hpp>

namespace GASS
{
	ODEPlaneGeometryComponent::ODEPlaneGeometryComponent() : m_Normal(0,1,0), m_PlaneOffset(0)
	{

	}

	ODEPlaneGeometryComponent::~ODEPlaneGeometryComponent()
	{
		
	}

	void ODEPlaneGeometryComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("PhysicsPlaneGeometryComponent",new Creator<ODEPlaneGeometryComponent, IComponent>);
	}

	void ODEPlaneGeometryComponent::OnCreate()
	{
		ODEBaseGeometryComponent::OnCreate();
	}

	dGeomID ODEPlaneGeometryComponent::CreateODEGeom()
	{
		Vec3 plane_normal = GetSceneObject()->GetSceneObjectManager()->GetScenarioScene()->GetSceneUp();
		return dCreatePlane(GetSpace(), plane_normal.x, plane_normal.y, plane_normal.z,m_PlaneOffset);
	}

	void ODEPlaneGeometryComponent::SetNormal(const Vec3 &normal)
	{
		if(normal.x > 0 || normal.y > 0 || normal.z > 0)
		{
			m_Normal = normal;
			if(m_GeomID)
			{
				dGeomPlaneSetParams(m_GeomID,normal.x,normal.y,normal.z,m_PlaneOffset);
				//dGeomBoxSetLengths(m_GeomID, m_Size.x, m_Size.y, m_Size.z);
				//UpdateBodyMass();
				//UpdateDebug();
			}
		}
	}

	Vec3 ODEPlaneGeometryComponent::GetNormal() const
	{
		return m_Normal;
	}
}
