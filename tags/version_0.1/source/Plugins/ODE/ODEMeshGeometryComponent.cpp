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

#include "Plugins/ODE/ODEMeshGeometryComponent.h"
#include "Plugins/ODE/ODEPhysicsSceneManager.h"
#include "Plugins/ODE/ODEBodyComponent.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/ComponentSystem/BaseComponentContainerTemplateManager.h"

#include "Core/MessageSystem/MessageManager.h"
#include "Core/Math/AABox.h"
#include "Core/Utils/GASSLogManager.h"
#include "Sim/Scenario/Scenario.h"
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
	ODEMeshGeometryComponent::ODEMeshGeometryComponent()
		
	{

	}

	ODEMeshGeometryComponent::~ODEMeshGeometryComponent()
	{
		
	}

	void ODEMeshGeometryComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("PhysicsMeshGeometryComponent",new Creator<ODEMeshGeometryComponent, IComponent>);
	}

	void ODEMeshGeometryComponent::OnCreate()
	{
		ODEBaseGeometryComponent::OnCreate();
	}

	dGeomID ODEMeshGeometryComponent::CreateODEGeom()
	{
		dGeomID geom_id = 0;
		IMeshComponent* mesh  = dynamic_cast<IMeshComponent*>(GetGeometry().get());
		if(mesh)
		{
			ODECollisionMesh col_mesh = ODEPhysicsSceneManagerPtr(m_SceneManager)->CreateCollisionMesh(mesh);
			geom_id = dCreateTriMesh(0, col_mesh.ID, 0, 0, 0);
		}
		return geom_id; 
	}

	void ODEMeshGeometryComponent::UpdateBodyMass()
	{
		if(m_Body && m_Body->GetMassRepresentation() == ODEBodyComponent::MR_GEOMETRY)
		{
			dMass ode_mass;
			Vec3 size = GetGeometry()->GetBoundingBox().GetSize();
			dMassSetBoxTotal(&ode_mass, m_Body->GetMass(), size.x, size.y, size.z);
			m_Body->SetODEMass(ode_mass);
		}
	}
}
