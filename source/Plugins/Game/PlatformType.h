
#ifndef PLATFORM_TYPE
#define PLATFORM_TYPE


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

	class PlatformTypeReflection : public SingleEnumBinder<PlatformType,PlatformTypeReflection>
	{
	public:
		PlatformTypeReflection();
		PlatformTypeReflection(PlatformType team);
		virtual ~PlatformTypeReflection();
		static void Register();
	};
	typedef GASS_SHARED_PTR<PlatformTypeReflection> PlatformTypeReflectionPtr;
}
#endif