/****************************************************************************
* This file is part of GASS.                                                *
* See https://github.com/leadcoder/gass                                     *
*                                                                           *
* Copyright (c) 2008-2016 GASS team. See Contributors.txt for details.      *
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
#define MAX_PROFILER_SAMPLES 100
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
		static IProfilerOutputHandler *m_OutputHandler;
		static bool m_ProfilerIsRunning;
	protected:
		//index into the array of samples
		int m_SampleIndex;
		int m_ParentIndex;

		double GetTime();

		static struct ProfileSampleData
		{
			ProfileSampleData()
			{
				m_IsValid=false; 
				m_DataCount=0;
				m_AveragePc=m_MinPc=m_MaxPc=-1;
			}

			bool m_IsValid;		//whether or not this sample is valid (for use with fixed-size arrays)
			bool m_IsOpen;		//is this sample currently being profiled?
			unsigned int m_CallCount;	//number of times this sample has been profiled this frame
			std::string m_Name;	//name of the sample

			double m_StartTime;	//starting time on the clock, in seconds
			double m_TotalTime;	//total time recorded across all profiles of this sample
			double m_ChildTime;	//total time taken by children of this sample

			int m_ParentCount;	//number of parents this sample has (useful for indenting)

			double m_AveragePc;	//average percentage of game loop time taken up
			double m_MinPc;		//minimum percentage of game loop time taken up
			double m_MaxPc;		//maximum percentage of game loop time taken up
			unsigned long m_DataCount;//number of percentage values that have been stored
		} m_Samples[MAX_PROFILER_SAMPLES];
		static int m_LastOpenedSample;
		static int m_OpenSampleCount;
		static double m_RootBegin;
		static double m_RootEnd;
		static Timer* m_Timer;
	};

	class GASSExport IProfilerOutputHandler
	{
	public:
		virtual void BeginOutput(double tTotal)=0;
		virtual void Sample(double fCur,double fMin, double fAvg, double fMax, double tAvg, int callCount, std::string name, int parentCount)=0;
		virtual void EndOutput()=0;
	};

#ifdef PROFILER
#define PROFILE(name) ProfileSample _profile_sample(name);
#else
#define PROFILE(name)
#endif

}

#endif
