/****************************************************************************
* This file is part of GASS.                                                *
* See http://code.google.com/p/gass/                                 *
*                                                                           *
* Copyright (c) 2008-2009 GASS team. See Contributors.txt for details.      *
*                                                                           *
* GASS is free software: you can redistribute it and/or modify              *
* it under the terms of the GNU Lesser General Public License as published  *
* by the Free Software Foundation, either version 3 of the License, or      *
* (at your option) any later version.                                       *
*                                                                           *
* GASS is distributed in the hope that it will be useful,                   *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU Lesser General Public License for more details.                       *
*                                                                           *
* You should have received a copy of the GNU Lesser General Public License  *
* along with GASS. If not, see <http://www.gnu.org/licenses/>.              *
*****************************************************************************/


#include "GASSProfiler.h"
#include "Sim/GASSSimEngine.h"

namespace GASS
{
	int ProfileSample::lastOpenedSample=-1;
	int ProfileSample::openSampleCount=0;
	ProfileSample::profileSample ProfileSample::samples[MAX_PROFILER_SAMPLES];
	IProfilerOutputHandler *ProfileSample::outputHandler=0;
	float ProfileSample::rootBegin=0.0f;
	float ProfileSample::rootEnd=0.0f;
	bool ProfileSample::bProfilerIsRunning=true;
	Timer* ProfileSample::m_Timer= new Timer();

	ProfileSample::ProfileSample(std::string sampleName)
	{
		if(!bProfilerIsRunning)return;
		//find the sample
		int i=0;
		int storeIndex=-1;
		for(i=0;i<MAX_PROFILER_SAMPLES;++i)
		{
			if(!samples[i].bIsValid)
			{
				if(storeIndex<0)storeIndex=i;
			}else{
				if(samples[i].name==sampleName)
				{
					//this is the sample we want
					//check that it's not already open
					assert(!samples[i].bIsOpen && "Tried to profile a sample which was already being profiled");
					//first, store it's index
					iSampleIndex=i;
					//the parent sample is the last opened sample
					iParentIndex=lastOpenedSample;
					lastOpenedSample=i;
					samples[i].parentCount=openSampleCount;
					++openSampleCount;
					samples[i].bIsOpen=true;
					++samples[i].callCount;
					samples[i].startTime= GetTime();
					//if this has no parent, it must be the 'main loop' sample, so do the global timer
					if(iParentIndex<0)rootBegin=samples[i].startTime;
					return;
				}
			}
		}
		//we've not found it, so it must be a new sample
		//use the storeIndex value to store the new sample
		assert(storeIndex>=0 && "Profiler has run out of sample slots!");
		samples[storeIndex].bIsValid=true;
		samples[storeIndex].name=sampleName;
		iSampleIndex=storeIndex;
		iParentIndex=lastOpenedSample;
		lastOpenedSample=storeIndex;
		samples[i].parentCount=openSampleCount;
		openSampleCount++;
		samples[storeIndex].bIsOpen=true;
		samples[storeIndex].callCount=1;

		samples[storeIndex].totalTime=0.0f;
		samples[storeIndex].childTime=0.0f;
		samples[storeIndex].startTime=GetTime();
		if(iParentIndex<0)rootBegin=samples[storeIndex].startTime;
	}

	ProfileSample::~ProfileSample()
	{
		if(!bProfilerIsRunning)return;
		float fEndTime= GetTime();
		//phew... ok, we're done timing
		samples[iSampleIndex].bIsOpen=false;
		//calculate the time taken this profile, for ease of use later on
		float fTimeTaken = fEndTime - samples[iSampleIndex].startTime;

		if(iParentIndex>=0)
		{
			samples[iParentIndex].childTime+=fTimeTaken;
		}else{
			//no parent, so this is the end of the main loop sample
			rootEnd=fEndTime;
		}
		samples[iSampleIndex].totalTime+=fTimeTaken;
		lastOpenedSample=iParentIndex;
		--openSampleCount;
	}

	float ProfileSample::GetTime()
	{
		return m_Timer->GetTime();
	}

	void ProfileSample::Output()
	{
		
		if(!bProfilerIsRunning)return;

		assert(outputHandler && "Profiler has no output handler set");

		outputHandler->BeginOutput(rootEnd-rootBegin);
		for(int i=0;i<MAX_PROFILER_SAMPLES; ++i)
		{
			if(samples[i].bIsValid)
			{
				float sampleTime, percentage;
				//calculate the time spend on the sample itself (excluding children)
				sampleTime = samples[i].totalTime-samples[i].childTime;
				percentage = ( sampleTime / ( rootEnd - rootBegin ) ) * 100.0f;

				//add it to the sample's values
				float totalPc;
				totalPc=samples[i].averagePc*samples[i].dataCount;
				totalPc+=percentage; samples[i].dataCount++;
				samples[i].averagePc=totalPc/samples[i].dataCount;
				if((samples[i].minPc==-1)||(percentage<samples[i].minPc))samples[i].minPc=percentage;
				if((samples[i].maxPc==-1)||(percentage>samples[i].maxPc))samples[i].maxPc=percentage;

				//output these values
				outputHandler->Sample(percentage,samples[i].minPc,
					samples[i].averagePc,
					samples[i].maxPc,
					sampleTime,
					samples[i].callCount,
					samples[i].name,
					samples[i].parentCount);

				//reset the sample for next time
				samples[i].callCount=0;
				samples[i].totalTime=0;
				samples[i].childTime=0;
			}
		}
		outputHandler->EndOutput();
	}

	void ProfileSample::ResetSample(std::string strName)
	{
		for(int i=0;i<MAX_PROFILER_SAMPLES; ++i)
		{
			if((samples[i].bIsValid)&&(samples[i].name==strName))
			{
				//found it
				//reset avg/min/max ONLY
				//because the sample may be running
				samples[i].maxPc=samples[i].minPc=-1;
				samples[i].dataCount=0;
				return;
			}
		}
	}	

	void ProfileSample::ResetAll()
	{
		for(int i=0;i<MAX_PROFILER_SAMPLES;++i)
		{
			if(samples[i].bIsValid)
			{
				samples[i].maxPc=samples[i].minPc=-1;
				samples[i].dataCount=0;
				if(!samples[i].bIsOpen)samples[i].bIsValid=false;
			}
		}
	}
}
