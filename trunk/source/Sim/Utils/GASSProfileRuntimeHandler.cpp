#include "GASSProfileRuntimeHandler.h"
#include "Sim/Systems/GASSSimSystemManager.h"
#include "Sim/GASSSimEngine.h"

namespace GASS
{
	ProfileRuntimeHandler::ProfileRuntimeHandler()
	{
		
	}

	ProfileRuntimeHandler::~ProfileRuntimeHandler()
	{

	}

	void ProfileRuntimeHandler::BeginOutput(float tTime)
	{

		float fps = 1.0f/tTime; 
		std::stringstream sstream;
		sstream << "FPS:" << fps << std::endl;
		std::string fps_mess = sstream.str();
		SimEngine::Get().GetSimSystemManager()->PostMessage(MessagePtr(new DebugPrintMessage(fps_mess)));

		
		std::string mess = "Cur: Min: Avg: Max: Time Calls: Profile Name:";
		SimEngine::Get().GetSimSystemManager()->PostMessage(MessagePtr(new DebugPrintMessage(mess)));


		//Font::DebugPrint("Cur: Min: Avg: Max: Time Calls: Profile Name:");
		//Font::DebugPrint("-------------------------------");
	}

	void ProfileRuntimeHandler::Sample(float fCurrent,float fMin, float fAvg, float fMax, float tAvg, int callCount, std::string name, int parentCount)
	{
		char namebuf[256], indentedName[256];
		char avg[16], min[16], max[16], num[16], time[16], cur[16];

		sprintf(cur, "%3.1f", fCurrent);
		sprintf(avg, "%3.1f", fAvg);
		sprintf(min, "%3.1f", fMin);
		sprintf(max, "%3.1f", fMax);
		sprintf(time,"%3.1f", tAvg);
		sprintf(num, "%3d", callCount);

		if(strlen(avg) < 4) avg[3] = ' '; avg[4] = NULL;
		if(strlen(min) < 4) min[3] = ' '; min[4] = NULL;
		if(strlen(max) < 4) max[3] = ' '; max[4] = NULL;
		if(strlen(time) < 4) time[3] = ' '; time[4] = NULL;


		strcpy( indentedName, name.c_str());
		for( int indent=0; indent<parentCount; ++indent )
		{
			sprintf(namebuf, " %s", indentedName);
			strcpy( indentedName, namebuf);
		}

		std::stringstream sstream;
		sstream << cur << " " << min << " " << avg << " " << max << " " << time << " " << num << " " << indentedName;
		std::string mess = sstream.str();
		SimEngine::Get().GetSimSystemManager()->PostMessage(MessagePtr(new DebugPrintMessage(mess)));
	}

	void ProfileRuntimeHandler::EndOutput()
	{
		//Font::DebugPrint("\n");
	}
}
