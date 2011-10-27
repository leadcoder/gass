
#ifndef PLATFORM_TEAM
#define PLATFORM_TEAM

#include "Sim/Common.h"
#include "Sim/Utils/EnumBinder.h"


namespace GASS
{
	enum PlatformTeam
	{
		RED_TEAM,
		BLUE_TEAM,
		YELLOW_TEAM
	};

	class PlatformTeamReflection : public EnumBinder<PlatformTeam,PlatformTeamReflection>
	{
	public:
		PlatformTeamReflection();
		PlatformTeamReflection(PlatformTeam type);
		virtual ~PlatformTeamReflection();
		static void Register();
	};
	typedef boost::shared_ptr<PlatformTeamReflection> PlatformTeamReflectionPtr;
}
#endif