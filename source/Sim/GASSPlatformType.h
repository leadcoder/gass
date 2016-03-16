
#pragma once


#include "Sim/GASSCommon.h"
#include "Core/Utils/GASSEnumBinder.h"
#include "Sim/Interface/GASSIPlatformComponent.h"

namespace GASS
{
	//MOVED to GASSIPlatformComponent
	/*enum PlatformType
	{
		PT_HUMAN,
		PT_CAR,
		PT_TANK,
		PT_TRUCK,
		PT_AIRCRAFT,
		PT_BUILDING,
		PT_WEAPON_SYSTEM,
	};*/

	START_ENUM_BINDER(PlatformType,PlatformTypeReflection)
		BIND(PT_HUMAN)
		BIND(PT_CAR)
		BIND(PT_TANK)
		BIND(PT_TRUCK)
		BIND(PT_AIRCRAFT)
		BIND(PT_BUILDING)
		BIND(PT_WEAPON_SYSTEM)
	END_ENUM_BINDER(PlatformType,PlatformTypeReflection)


	/*class  GASSExport PlatformTypeReflection : public SingleEnumBinder<PlatformType,PlatformTypeReflection>
	{
	public:
		PlatformTypeReflection();
		PlatformTypeReflection(PlatformType team);
		virtual ~PlatformTypeReflection();
		static void Register();
	};
	typedef GASS_SHARED_PTR<PlatformTypeReflection> PlatformTypeReflectionPtr;*/
}
