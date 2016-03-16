
#pragma once

#include "Sim/GASSCommon.h"
#include "Core/Utils/GASSEnumBinder.h"


namespace GASS
{
	enum PlatformTeam
	{
		RED_TEAM,
		BLUE_TEAM,
		YELLOW_TEAM
	};

	START_ENUM_BINDER(PlatformTeam,PlatformTeamReflection)
		BIND(RED_TEAM)
		BIND(BLUE_TEAM)
		BIND(YELLOW_TEAM)
	END_ENUM_BINDER(PlatformTeam,PlatformTeamReflection)

	/*class GASSExport PlatformTeamReflection : public SingleEnumBinder<PlatformTeam,PlatformTeamReflection>
	{
	public:
		PlatformTeamReflection();
		PlatformTeamReflection(PlatformTeam type);
		virtual ~PlatformTeamReflection();
		static void Register();
	};
	typedef GASS_SHARED_PTR<PlatformTeamReflection> PlatformTeamReflectionPtr;*/
}
