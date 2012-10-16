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

#include "RecastDoorComponent.h"

#include "Recast/Recast.h"
#include "Detour/DetourNavMeshQuery.h"
#include "InputGeom.h"
#include "tinyxml.h"

namespace GASS
{
	RecastDoorComponent::RecastDoorComponent() : m_Initialized(false),
		m_Open(true),
		m_PolyRefCount(0)
	{

	}

	RecastDoorComponent::~RecastDoorComponent()
	{
		
	}

	void RecastDoorComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("RecastDoorComponent",new Creator<RecastDoorComponent, IComponent>);
		RegisterProperty<bool>("Open", &RecastDoorComponent::GetOpen, &RecastDoorComponent::SetOpen);
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
		GetSceneObject()->RegisterForMessage(REG_TMESS(RecastDoorComponent::OnLoad,LocationLoadedMessage,3));
		GetSceneObject()->RegisterForMessage(REG_TMESS(RecastDoorComponent::OnTransformation,TransformationNotifyMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(RecastDoorComponent::OnDoorMessage,DoorMessage,0));
	}

	void RecastDoorComponent::OnTransformation(TransformationNotifyMessagePtr message)
	{
		m_Pos = message->GetPosition();
		m_PolyRefCount = 0;
	}

	void RecastDoorComponent::OnDoorMessage(DoorMessagePtr message)
	{
		//SetOpen(message->GetOpen());
	}

	void RecastDoorComponent::OnLoad(LocationLoadedMessagePtr message)
	{
		//assume only onw nav mesh
		m_NavMeshComp = GetSceneObject()->GetScene()->GetRootSceneObject()->GetFirstComponentByClass<RecastNavigationMeshComponent>(true);
		UpdatePolyState(m_Open);
		m_Initialized = true;
	}

	
	void RecastDoorComponent::UpdatePolyState(bool value)
	{
		RecastNavigationMeshComponentPtr nav_mesh(m_NavMeshComp,boost::detail::sp_nothrow_tag());

		if(!nav_mesh)
		{
			m_NavMeshComp = GetSceneObject()->GetScene()->GetRootSceneObject()->GetFirstComponentByClass<RecastNavigationMeshComponent>(true);
			nav_mesh = RecastNavigationMeshComponentPtr (m_NavMeshComp,boost::detail::sp_nothrow_tag());
		}
		
		if (nav_mesh && nav_mesh->GetNavMesh() && nav_mesh->GetNavMeshQuery())
		{
			if(m_PolyRefCount == 0)
			{
				GeometryComponentPtr geom = GetSceneObject()->GetFirstComponentByClass<IGeometryComponent>();
				if(geom)
				{
					boost::any any_size;
					ComponentPtr comp = GetSceneObject()->GetFirstComponentByClass("BoxGeometryComponent",false);
					BaseReflectionObjectPtr bro = boost::shared_dynamic_cast<BaseReflectionObject>(comp);
					bro->GetPropertyByType("Size",any_size);
					Vec3 size = boost::any_cast<Vec3>(any_size);
					size = size* 0.5;
					dtQueryFilter filter;
					float ext[3];
					float point[3];
					point[0] =m_Pos.x; point[1] =m_Pos.y; point[2] =m_Pos.z;
					ext[0] = size.x; ext[1] = size.y; ext[2] = size.z;
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
