#include <boost/bind.hpp>
#include "Plugins/Game/PlatformType.h"

namespace GASS
{
	
	template<> std::map<std::string ,PlatformType> EnumBinder<PlatformType,PlatformTypeReflection>::m_Names;
	template<> std::map<PlatformType, std::string> EnumBinder<PlatformType,PlatformTypeReflection>::m_Types;

	PlatformTypeReflection::PlatformTypeReflection() : EnumBinder<PlatformType,PlatformTypeReflection>()
	{

	}

	PlatformTypeReflection::PlatformTypeReflection(PlatformType type) : EnumBinder<PlatformType,PlatformTypeReflection>(type)
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