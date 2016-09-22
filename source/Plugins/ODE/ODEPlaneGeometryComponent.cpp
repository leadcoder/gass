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



#include "Plugins/ODE/ODEPlaneGeometryComponent.h"
#include "Plugins/ODE/ODEBodyComponent.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Sim/GASSSceneObjectTemplate.h"

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
		RegisterProperty<Vec3>("Normal",&ODEPlaneGeometryComponent::GetNormal, &ODEPlaneGeometryComponent::SetNormal);
		RegisterProperty<Float>("PlaneOffset",&ODEPlaneGeometryComponent::GetPlaneOffset, &ODEPlaneGeometryComponent::SetPlaneOffset);
	}

	void ODEPlaneGeometryComponent::OnInitialize()
	{
		ODEBaseGeometryComponent::OnInitialize();
	}

	dGeomID ODEPlaneGeometryComponent::CreateODEGeom()
	{
		//Vec3 plane_normal = GetSceneObject()->GetScene()->GetSceneUp();
		return dCreatePlane(GetSpace(), m_Normal.x, m_Normal.y, m_Normal.z, m_PlaneOffset);
	}

	void ODEPlaneGeometryComponent::UpdateODEGeom()
	{
		Reset();
		m_Body = NULL;
		m_GeomID  = CreateODEGeom();
		m_TransformGeomID = m_GeomID;//dCreateGeomTransform(space);
		dGeomSetData(m_TransformGeomID, (void*)this);
		SetCollisionBits(m_CollisionBits);
		SetCollisionCategory(m_CollisionCategory);
	}

	void ODEPlaneGeometryComponent::SetNormal(const Vec3 &normal)
	{
		m_Normal = normal;
		if(m_GeomID)
		{
			dGeomPlaneSetParams(m_GeomID, m_Normal.x, m_Normal.y, m_Normal.z, m_PlaneOffset);
		}
	}

	Vec3 ODEPlaneGeometryComponent::GetNormal() const
	{
		return m_Normal;
	}

	void ODEPlaneGeometryComponent::SetPlaneOffset(Float offset)
	{
		m_PlaneOffset = offset;
		if(m_GeomID)
		{
			dGeomPlaneSetParams(m_GeomID, m_Normal.x, m_Normal.y, m_Normal.z, m_PlaneOffset);
		}
	}

	Float ODEPlaneGeometryComponent::GetPlaneOffset() const
	{
		return m_PlaneOffset;
	}

	void ODEPlaneGeometryComponent::SetPosition(const Vec3 &/*pos*/)
	{
		//not possible for plane
	}

	void ODEPlaneGeometryComponent::SetRotation(const Quaternion &/*rot*/)
	{
		//not possible for plane
	}
}
 