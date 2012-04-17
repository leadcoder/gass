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

#ifndef GASS_PROFILER_H
#define GASS_PROFILER_H

#include "Sim/GASSCommon.h"
#include "Core/Utils/GASSTimer.h"
#define MAX_PROFILER_SAMPLES 50
//enable profiler
//#define PROFILER

namespace GASS
{
	class IProfilerOutputHandler;

	class ProfileSample;

	class GASSExport ProfileSample
	{
	public:
		ProfileSample(std::string sampleName);
		~ProfileSample();
		static void Output();
		static void ResetSample(std::string sampleName);
		static void ResetAll();
		static IProfilerOutputHandler *outputHandler;
		static bool bProfilerIsRunning;
	protected:
		//index into the array of samples
		int iSampleIndex;
		int iParentIndex;

		float GetTime();

		static struct profileSample
		{
			profileSample()
			{
				bIsValid=false; 
				dataCount=0;
				averagePc=minPc=maxPc=-1;
			}

			bool bIsValid;		//whether or not this sample is valid (for use with fixed-size arrays)
			bool bIsOpen;		//is this sample currently being profiled?
			unsigned int callCount;	//number of times this sample has been profiled this frame
			std::string name;	//name of the sample

			float startTime;	//starting time on the clock, in seconds
			float totalTime;	//total time recorded across all profiles of this sample
			float childTime;	//total time taken by children of this sample

			int parentCount;	//number of parents this sample has (useful for indenting)

			float averagePc;	//average percentage of game loop time taken up
			float minPc;		//minimum percentage of game loop time taken up
			float maxPc;		//maximum percentage of game loop time taken up
			unsigned long dataCount;//number of percentage values that have been stored
		} samples[MAX_PROFILER_SAMPLES];
		static int lastOpenedSample;
		static int openSampleCount;
		static float rootBegin;
		static float rootEnd;
		static Timer* m_Timer;
	};

	class GASSExport IProfilerOutputHandler
	{
	public:
		virtual void BeginOutput(float tTotal)=0;
		virtual void Sample(float fCur,float fMin, float fAvg, float fMax, float tAvg, int callCount, std::string name, int parentCount)=0;
		virtual void EndOutput()=0;
	};

#ifdef PROFILER
#define PROFILE(name) ProfileSample _profile_sample(name);
#else
#define PROFILE(name)
#endif

}

#endif
