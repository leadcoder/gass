#include "GASSProfileRuntimeHandler.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/GASSSimEngine.h"
#include <string.h>

namespace GASS
{
	ProfileRuntimeHandler::ProfileRuntimeHandler()
	{

	}

	ProfileRuntimeHandler::~ProfileRuntimeHandler()
	{

	}

	void ProfileRuntimeHandler::BeginOutput(double tTime)
	{
		const double fps = 1.0f/tTime;
		GASS_PRINT("FPS:" << fps)
		GASS_PRINT("Cur: Min: Avg: Max: Time Calls: Profile Name:")
	}

	void ProfileRuntimeHandler::Sample(double fCurrent, double fMin, double fAvg, double fMax, double tAvg, int callCount, std::string name, int parentCount)
	{
		char namebuf[256], indented_name[256];
		char avg[16], min[16], max[16], num[16], time[16], cur[16];

		sprintf(cur, "%3.1f", fCurrent);
		sprintf(avg, "%3.1f", fAvg);
		sprintf(min, "%3.1f", fMin);
		sprintf(max, "%3.1f", fMax);
		sprintf(time, "%3.1f", tAvg);
		sprintf(num, "%3d", callCount);

		if (strlen(avg) < 4) avg[3] = ' '; avg[4] = 0;
		if (strlen(min) < 4) min[3] = ' '; min[4] = 0;
		if (strlen(max) < 4) max[3] = ' '; max[4] = 0;
		if (strlen(time) < 4) time[3] = ' '; time[4] = 0;


		strcpy(indented_name, name.c_str());
		for (int indent = 0; indent < parentCount; ++indent)
		{
			sprintf(namebuf, " %s", indented_name);
			strcpy(indented_name, namebuf);
		}
		GASS_PRINT(cur << " " << min << " " << avg << " " << max << " " << time << " " << num << " " << indented_name)
	}

	void ProfileRuntimeHandler::EndOutput()
	{
		//Font::DebugPrint("\n");
	}
}
