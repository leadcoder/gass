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
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/ComponentSystem/GASSBaseComponentContainerTemplateManager.h"

#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/Math/GASSAABox.h"
#include "Core/Utils/GASSLogManager.h"
#include "Sim/Scene/GASSScene.h"
#include "Sim/Scene/GASSSceneObject.h"

#include "Sim/Scene/GASSSceneObjectTemplate.h"
#include "Sim/Components/Graphics/Geometry/GASSIGeometryComponent.h"
#include "Sim/Components/Graphics/Geometry/GASSIMeshComponent.h"
#include "Sim/Components/Graphics/Geometry/GASSITerrainComponent.h"
#include "Sim/Components/Graphics/GASSILocationComponent.h"
#include "Sim/GASSSimEngine.h"
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

	void ODEMeshGeometryComponent::OnInitialize()
	{
		ODEBaseGeometryComponent::OnInitialize();
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
