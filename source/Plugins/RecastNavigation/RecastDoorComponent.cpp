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

#include "RecastDoorComponent.h"
#include "RecastIncludes.h"
#include "Core/Serialize/tinyxml2.h"

namespace GASS
{
	RecastDoorComponent::RecastDoorComponent() 
		
	{

	}

	RecastDoorComponent::~RecastDoorComponent()
	{
		
	}

	void RecastDoorComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register<RecastDoorComponent>();
		RegisterGetSet("Open", &RecastDoorComponent::GetOpen, &RecastDoorComponent::SetOpen);
	}

	void RecastDoorComponent::SetOpen(bool value)
	{
		m_Open = value;
		if(m_Initialized)
			UpdatePolyState(m_Open);
	}

	bool RecastDoorComponent::GetOpen() const
	{
		return m_Open;
	}

	void RecastDoorComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(RecastDoorComponent::OnLoad,LocationLoadedEvent,3));
		GetSceneObject()->RegisterForMessage(REG_TMESS(RecastDoorComponent::OnTransformation,TransformationChangedEvent,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(RecastDoorComponent::OnDoorMessage,DoorMessage,0));
	}

	void RecastDoorComponent::OnTransformation(TransformationChangedEventPtr message)
	{
		m_Pos = message->GetPosition();
		m_PolyRefCount = 0;
	}

	void RecastDoorComponent::OnDoorMessage(DoorMessagePtr /*message*/)
	{
		//SetOpen(message->GetOpen());
	}

	void RecastDoorComponent::OnLoad(LocationLoadedEventPtr /*message*/)
	{
		//assume only onw nav mesh
		m_NavMeshComp = GetSceneObject()->GetScene()->GetRootSceneObject()->GetFirstComponentByClass<RecastNavigationMeshComponent>(true);
		UpdatePolyState(m_Open);
		m_Initialized = true;
	}

	
	void RecastDoorComponent::UpdatePolyState(bool value)
	{
		RecastNavigationMeshComponentPtr nav_mesh = m_NavMeshComp.lock();

		if(!nav_mesh)
		{
			m_NavMeshComp = GetSceneObject()->GetScene()->GetRootSceneObject()->GetFirstComponentByClass<RecastNavigationMeshComponent>(true);
			nav_mesh = m_NavMeshComp.lock();
		}
		
		if (nav_mesh && nav_mesh->GetNavMesh() && nav_mesh->GetNavMeshQuery())
		{
			if(m_PolyRefCount == 0)
			{
				GeometryComponentPtr geom = GetSceneObject()->GetFirstComponentByClass<IGeometryComponent>();
				if(geom)
				{
					GASS_ANY any_size;
					ComponentPtr comp = GetSceneObject()->GetFirstComponentByClassName("BoxGeometryComponent",false);
					BaseReflectionObjectPtr bro = GASS_DYNAMIC_PTR_CAST<BaseReflectionObject>(comp);
					bro->GetPropertyAsAny("Size", any_size);
					Vec3 size = GASS_ANY_CAST<Vec3>(any_size);
					size = size* 0.5;
					dtQueryFilter filter;
					float ext[3];
					float point[3];
					point[0] = static_cast<float>(m_Pos.x);
					point[1] = static_cast<float>(m_Pos.y);
					point[2] = static_cast<float>(m_Pos.z);
					ext[0] = static_cast<float>(size.x);
					ext[1] = static_cast<float>(size.y); 
					ext[2] = static_cast<float>(size.z);
					float tgt[3];
					nav_mesh->GetNavMeshQuery()->findNearestPoly(point, ext, &filter, &m_PolyRefs[0], tgt);
					if(m_PolyRefs[0])
						m_PolyRefCount = 1;
					//nav_mesh->GetNavMeshQuery()->queryPolygons(point, ext,&filter,m_PolyRefs,&m_PolyRefCount, MAX_REF_POLYS);
				}
			}

			for(int i = 0; i < m_PolyRefCount; i++)
			{
				unsigned short flags = 0;
				if (dtStatusSucceed(nav_mesh->GetNavMesh()->getPolyFlags(m_PolyRefs[i], &flags)))
				{
					if(value)
						flags &= ~SAMPLE_POLYFLAGS_DISABLED;
					else
						flags |= SAMPLE_POLYFLAGS_DISABLED;
						
					nav_mesh->GetNavMesh()->setPolyFlags(m_PolyRefs[i], flags);

					//debug
					nav_mesh->UpdateNavMeshVis();
				}
			}
		}
	}
	
}
