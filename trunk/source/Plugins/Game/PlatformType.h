
#ifndef PLATFORM_TYPE
#define PLATFORM_TYPE


#include "Sim/GASSCommon.h"
#include "Core/Utils/GASSEnumBinder.h"

namespace GASS
{
	enum PlatformType
	{
		PT_HUMAN,
		PT_CAR,
		PT_TANK,
		PT_TRUCK,
		PT_AIRCRAFT,
		PT_BUILDING,
		PT_WEAPON_SYSTEM,
	};

	class PlatformTypeReflection : public EnumBinder<PlatformType,PlatformTypeReflection>
	{
	public:
		PlatformTypeReflection();
		PlatformTypeReflection(PlatformType team);
		virtual ~PlatformTypeReflection();
		static void Register();
	};
	typedef boost::shared_ptr<PlatformTypeReflection> PlatformTypeReflectionPtr;
}
#endif