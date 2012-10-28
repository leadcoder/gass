#include <boost/bind.hpp>
#include "Plugins/Game/PlatformType.h"

namespace GASS
{
	
	template<> std::map<std::string ,PlatformType> SingleEnumBinder<PlatformType,PlatformTypeReflection>::m_NameToEnumMap;
	template<> std::map<PlatformType, std::string> SingleEnumBinder<PlatformType,PlatformTypeReflection>::m_EnumToNameMap;

	PlatformTypeReflection::PlatformTypeReflection() : SingleEnumBinder<PlatformType,PlatformTypeReflection>()
	{

	}

	PlatformTypeReflection::PlatformTypeReflection(PlatformType type) : SingleEnumBinder<PlatformType,PlatformTypeReflection>(type)
	{

	}

	PlatformTypeReflection::~PlatformTypeReflection()
	{

	}

	void PlatformTypeReflection::Register()
	{
		Bind("PT_HUMAN", PT_HUMAN);
		Bind("PT_CAR", PT_CAR);
		Bind("PT_TANK", PT_TANK);
		Bind("PT_TRUCK", PT_TRUCK);
		Bind("PT_AIRCRAFT,", PT_AIRCRAFT);
		Bind("PT_BUILDING", PT_BUILDING);
		Bind("PT_WEAPON_SYSTEM", PT_WEAPON_SYSTEM);
	}
}