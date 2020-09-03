
#pragma once


#include "Sim/GASSCommon.h"
#include "Core/Utils/GASSEnumBinder.h"
#include "Sim/Interface/GASSIPlatformComponent.h"

namespace GASS
{
	START_ENUM_BINDER(PlatformType,PlatformTypeReflection)
		BIND(PT_HUMAN)
		BIND(PT_CAR)
		BIND(PT_TANK)
		BIND(PT_TRUCK)
		BIND(PT_AIRCRAFT)
		BIND(PT_BUILDING)
		BIND(PT_WEAPON_SYSTEM)
	END_ENUM_BINDER(PlatformType,PlatformTypeReflection)
}
