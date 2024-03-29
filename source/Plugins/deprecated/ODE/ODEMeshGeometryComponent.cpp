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

#include "Plugins/ODE/ODEMeshGeometryComponent.h"
#include "Plugins/ODE/ODEPhysicsSceneManager.h"
#include "Plugins/ODE/ODEBodyComponent.h"
#include "Sim/GASSComponentFactory.h"
#include "Core/Math/GASSAABox.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/Interface/GASSIGeometryComponent.h"
#include "Sim/Interface/GASSIMeshComponent.h"
#include "Sim/Interface/GASSIResourceComponent.h"

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
	}

	void ODEMeshGeometryComponent::OnInitialize()
	{
		ODEBaseGeometryComponent::OnInitialize();
	}

	dGeomID ODEMeshGeometryComponent::CreateODEGeom()
	{
		dGeomID geom_id = nullptr;
		MeshComponentPtr mesh  = GASS_DYNAMIC_PTR_CAST<IMeshComponent>(GetGeometry());
		if(mesh)
		{
			std::string col_mesh_id = GetSceneObject()->GetName();
			ResourceComponentPtr res  = GASS_DYNAMIC_PTR_CAST<IResourceComponent>(GetGeometry());
			if(res)
			{
				col_mesh_id = res->GetResource().Name();
			}
			ODEPhysicsCollisionMesh col_mesh = ODEPhysicsSceneManagerPtr(m_SceneManager)->CreateCollisionMesh(col_mesh_id,mesh);
			geom_id = dCreateTriMesh(GetSpace(), col_mesh.ID, nullptr, nullptr, nullptr);
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
