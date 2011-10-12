#include <boost/bind.hpp>
#include "Plugins/Game/PlatformTeam.h"

namespace GASS
{
	
	template<> std::map<std::string ,PlatformTeam> EnumBinder<PlatformTeam,PlatformTeamReflection>::m_Names;
	template<> std::map<PlatformTeam, std::string> EnumBinder<PlatformTeam,PlatformTeamReflection>::m_Types;

	PlatformTeamReflection::PlatformTeamReflection() : EnumBinder<PlatformTeam,PlatformTeamReflection>()
	{

	}

	PlatformTeamReflection::PlatformTeamReflection(PlatformTeam type) : EnumBinder<PlatformTeam,PlatformTeamReflection>(type)
	{

	}

	PlatformTeamReflection::~PlatformTeamReflection()
	{

	}

	void PlatformTeamReflection::Register()
	{
		Bind("RED_TEAM", RED_TEAM);
		Bind("BLUE_TEAM", BLUE_TEAM);
		Bind("YELLOW_TEAM", YELLOW_TEAM);
	}
}