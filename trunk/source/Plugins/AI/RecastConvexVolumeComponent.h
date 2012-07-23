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

#pragma once
#include "Sim/GASS.h"
#include "Plugins/Base/CoreMessages.h"
#include "RecastNavigationMeshComponent.h"
#include "AIMessages.h"
#include "Detour/DetourNavMesh.h"

namespace GASS
{
	
	class RecastConvexVolumeComponent : public Reflection<RecastConvexVolumeComponent,BaseSceneComponent>
	{
	public:
		RecastConvexVolumeComponent();
		virtual ~RecastConvexVolumeComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
		SamplePolyAreas GetArea() const {return m_Area;}
	protected:
		void SetAreaType(const std::string value);
		std::string GetAreaType() const;
		void OnTransformation(TransformationNotifyMessagePtr message);
		void OnUnload(UnloadComponentsMessagePtr message);
		void OnLoad(LocationLoadedMessagePtr message);
		SamplePolyAreas m_Area;
		bool m_Initialized;
	};
	typedef boost::shared_ptr<RecastConvexVolumeComponent> RecastConvexVolumeComponentPtr;
	typedef boost::weak_ptr<RecastConvexVolumeComponent> RecastConvexVolumeComponentWeakPtr;
}

