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
	#define MAX_REF_POLYS 256
	class RecastDoorComponent : public Reflection<RecastDoorComponent,BaseSceneComponent>
	{
	public:
		RecastDoorComponent();
		virtual ~RecastDoorComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
	protected:
		void OnTransformation(TransformationNotifyMessagePtr message);
		void OnUnload(UnloadComponentsMessagePtr message);
		void OnLoad(LocationLoadedMessagePtr message);
		void OnDoorMessage(DoorMessagePtr message);
		void UpdatePolyState(bool value);
		void SetOpen(bool value);
		bool GetOpen() const;
	
		bool m_Initialized;
		bool m_Open;
		Vec3 m_Pos;
		RecastNavigationMeshComponentWeakPtr m_NavMeshComp;
		dtPolyRef m_PolyRefs[MAX_REF_POLYS];
		int m_PolyRefCount;
	};
	typedef boost::shared_ptr<RecastDoorComponent> RecastDoorComponentPtr;
	typedef boost::weak_ptr<RecastDoorComponent> RecastDoorComponentWeakPtr;
}

