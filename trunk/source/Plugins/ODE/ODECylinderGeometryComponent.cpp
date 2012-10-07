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

#include "Plugins/ODE/ODECylinderGeometryComponent.h"
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
	ODECylinderGeometryComponent::ODECylinderGeometryComponent(): m_Radius(1),
			m_Length(3)
		
	{

	}

	ODECylinderGeometryComponent::~ODECylinderGeometryComponent()
	{
		
	}

	void ODECylinderGeometryComponent::RegisterReflection()
	{
		RegisterProperty<Float>("Radius", &GASS::ODECylinderGeometryComponent::GetRadius, &GASS::ODECylinderGeometryComponent::SetRadius);
		RegisterProperty<Float>("Length", &GASS::ODECylinderGeometryComponent::GetLength, &GASS::ODECylinderGeometryComponent::SetLength);
	}

	void ODECylinderGeometryComponent::OnInitialize()
	{
		ODEBaseGeometryComponent::OnInitialize();
	}

	void ODECylinderGeometryComponent::SetSizeFromMesh(bool value)
	{
		m_SizeFromMesh = value;
		if(m_SizeFromMesh && IsInitialized())
		{
			GeometryComponentPtr geom  = GetGeometry();
			if(geom)
			{
				AABox aabox = geom->GetBoundingBox();
				Vec3 size = aabox.GetSize();
				float radius=std::max(size.x/2.f,size.y/2.f);
				float length=size.z-radius;
				SetRadius(radius);
				SetLength(length);
			}
		}
	}

	dGeomID ODECylinderGeometryComponent::CreateODEGeom()
	{
		return dCreateCCylinder (0, m_Radius, m_Length);
	}

	void ODECylinderGeometryComponent::SetRadius(Float radius)
	{
		if(radius > 0 )
		{
			m_Radius = radius;
			if(m_GeomID)
				dGeomCCylinderSetParams(m_GeomID,m_Radius,m_Length);
		}
	}

	Float ODECylinderGeometryComponent::GetRadius() const
	{
		return m_Radius;
	}

	void ODECylinderGeometryComponent::SetLength(Float length)
	{
		if(length > 0 )
		{
			m_Length = length;

			if(m_GeomID)
				dGeomCCylinderSetParams(m_GeomID,m_Radius,m_Length);
		}
	}

	Float ODECylinderGeometryComponent::GetLength() const
	{
		return m_Length;
	}

	void ODECylinderGeometryComponent::UpdateBodyMass()
	{
		if(m_Body && m_Body->GetMassRepresentation() == ODEBodyComponent::MR_GEOMETRY)
		{
			dMass ode_mass;
			dMassSetCylinderTotal(&ode_mass, m_Body->GetMass(),3, m_Radius, m_Length);
			m_Body->SetODEMass(ode_mass);
		}
	}
	
	void ODECylinderGeometryComponent::UpdateDebug()
	{
		if(m_Debug)
		{
			if(m_GeomID)
			{
				/*dVector3 temp_size;
				dGeomBoxGetLengths (m_GeomID, temp_size);
				Vec3 size(temp_size[0],temp_size[1],temp_size[2]);
				const dReal* pos =  dGeomGetPosition(m_GeomID);
				CreateDebugBox(size,Vec3(pos[0],pos[1],pos[2]));*/
			}
		}
		
	}
}
