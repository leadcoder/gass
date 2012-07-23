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

#include "RecastConvexVolumeComponent.h"

#include "Recast/Recast.h"
#include "Detour/DetourNavMeshQuery.h"
#include "InputGeom.h"
#include "tinyxml.h"

namespace GASS
{
	RecastConvexVolumeComponent::RecastConvexVolumeComponent() : m_Initialized(false), m_Area(SAMPLE_POLYAREA_GROUND)
	{

	}

	RecastConvexVolumeComponent::~RecastConvexVolumeComponent()
	{
		
	}

	void RecastConvexVolumeComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("RecastConvexVolumeComponent",new Creator<RecastConvexVolumeComponent, IComponent>);
		RegisterProperty<std::string>("AreaType", &RecastConvexVolumeComponent::GetAreaType, &RecastConvexVolumeComponent::SetAreaType);
	}

	void RecastConvexVolumeComponent::SetAreaType(const std::string value)
	{
		m_Area = GetAreaFromName(value);
	}

	std::string RecastConvexVolumeComponent::GetAreaType() const
	{
		return GetAreaName(m_Area);
	}

	void RecastConvexVolumeComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(RecastConvexVolumeComponent::OnLoad,LocationLoadedMessage,3));
		GetSceneObject()->RegisterForMessage(REG_TMESS(RecastConvexVolumeComponent::OnUnload,UnloadComponentsMessage,0));
	}

	void RecastConvexVolumeComponent::OnLoad(LocationLoadedMessagePtr message)
	{
		//assume only onw nav mesh
		m_Initialized = true;
	}

	void RecastConvexVolumeComponent::OnUnload(UnloadComponentsMessagePtr message)
	{
		
	}
	
}
