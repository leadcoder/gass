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

#include "Sim/GASSCommon.h"
#include "Core/Common.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Math/GASSVector.h"
#include "Sim/Interface/GASSICameraComponent.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/GASSSceneObject.h"
#include "Plugins/Base/CoreMessages.h"


namespace GASS
{
	class EntityTypeComponent;
	typedef WPTR<EntityTypeComponent> EntityTypeComponentWeakPtr;
	class EntityTypeComponent : public Reflection<EntityTypeComponent,BaseSceneComponent>
	{
	public:
		EntityTypeComponent();
		virtual ~EntityTypeComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
		
		ADD_ATTRIBUTE(int,EntityKind)
		ADD_ATTRIBUTE(int,Domain)
		ADD_ATTRIBUTE(int,CountryCode)
		ADD_ATTRIBUTE(int,Category)
		ADD_ATTRIBUTE(int,Subcategory)
		ADD_ATTRIBUTE(int,Specific)
		ADD_ATTRIBUTE(int,Extra)
	
	};

	typedef SPTR<EntityTypeComponent> EntityTypeComponentPtr;
	
}

