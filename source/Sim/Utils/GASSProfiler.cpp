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


#include "GASSProfiler.h"
#include "Sim/GASSSimEngine.h"

namespace GASS
{
	int ProfileSample::m_LastOpenedSample=-1;
	int ProfileSample::m_OpenSampleCount=0;
	ProfileSample::ProfileSampleData ProfileSample::m_Samples[MAX_PROFILER_SAMPLES];
	IProfilerOutputHandler *ProfileSample::m_OutputHandler=nullptr;
	double ProfileSample::m_RootBegin=0.0;
	double ProfileSample::m_RootEnd=0.0;
	bool ProfileSample::m_ProfilerIsRunning=true;
	Timer* ProfileSample::m_Timer= new Timer();

	ProfileSample::ProfileSample(std::string sampleName)  
	{
		if(!m_ProfilerIsRunning)return;
		//find the sample
		int i=0;
		int store_index=-1;
		for(i=0;i<MAX_PROFILER_SAMPLES;++i)
		{
			if(!m_Samples[i].m_IsValid)
			{
				if(store_index<0)store_index=i;
			}else{
				if(m_Samples[i].m_Name==sampleName)
				{
					//this is the sample we want
					//check that it's not already open
					assert(!m_Samples[i].m_IsOpen && "Tried to profile a sample which was already being profiled");
					//first, store it's index
					m_SampleIndex=i;
					//the parent sample is the last opened sample
					m_ParentIndex=m_LastOpenedSample;
					m_LastOpenedSample=i;
					m_Samples[i].m_ParentCount=m_OpenSampleCount;
					++m_OpenSampleCount;
					m_Samples[i].m_IsOpen=true;
					++m_Samples[i].m_CallCount;
					m_Samples[i].m_StartTime = GetTime();
					//if this has no parent, it must be the 'main loop' sample, so do the global timer
					if(m_ParentIndex<0)m_RootBegin=m_Samples[i].m_StartTime;
					return;
				}
			}
		}
		//we've not found it, so it must be a new sample
		//use the storeIndex value to store the new sample
		assert(store_index>=0 && "Profiler has run out of sample slots!");
		m_Samples[store_index].m_IsValid=true;
		m_Samples[store_index].m_Name=sampleName;
		m_SampleIndex=store_index;
		m_ParentIndex=m_LastOpenedSample;
		m_LastOpenedSample=store_index;
		//m_Samples[i].m_ParentCount=m_OpenSampleCount;
		m_Samples[store_index].m_ParentCount = m_OpenSampleCount;
		m_OpenSampleCount++;
		m_Samples[store_index].m_IsOpen=true;
		m_Samples[store_index].m_CallCount=1;

		m_Samples[store_index].m_TotalTime=0.0;
		m_Samples[store_index].m_ChildTime=0.0;
		m_Samples[store_index].m_StartTime=GetTime();
		if(m_ParentIndex<0)m_RootBegin=m_Samples[store_index].m_StartTime;
	}

	ProfileSample::~ProfileSample()
	{
		if(!m_ProfilerIsRunning)return;
		const double f_end_time= GetTime();
		//phew... ok, we're done timing
		m_Samples[m_SampleIndex].m_IsOpen=false;
		//calculate the time taken this profile, for ease of use later on
		const double f_time_taken = f_end_time - m_Samples[m_SampleIndex].m_StartTime;

		if(m_ParentIndex>=0)
		{
			m_Samples[m_ParentIndex].m_ChildTime+=f_time_taken;
		}else{
			//no parent, so this is the end of the main loop sample
			m_RootEnd=f_end_time;
		}
		m_Samples[m_SampleIndex].m_TotalTime+=f_time_taken;
		m_LastOpenedSample=m_ParentIndex;
		--m_OpenSampleCount;
	}

	double ProfileSample::GetTime()
	{
		return m_Timer->GetTime();
	}

	void ProfileSample::Output()
	{
		
		if(!m_ProfilerIsRunning)return;

		assert(m_OutputHandler && "Profiler has no output handler set");

		m_OutputHandler->BeginOutput(m_RootEnd - m_RootBegin);
		for(int i=0;i<MAX_PROFILER_SAMPLES; ++i)
		{
			if(m_Samples[i].m_IsValid)
			{
				double sample_time, percentage;
				//calculate the time spend on the sample itself (excluding children)
				sample_time = m_Samples[i].m_TotalTime-m_Samples[i].m_ChildTime;
				percentage = ( sample_time / ( m_RootEnd - m_RootBegin ) ) * 100.0f;

				//add it to the sample's values
				double total_pc;
				total_pc=m_Samples[i].m_AveragePc*m_Samples[i].m_DataCount;
				total_pc+=percentage; m_Samples[i].m_DataCount++;
				m_Samples[i].m_AveragePc=total_pc/m_Samples[i].m_DataCount;
				if((m_Samples[i].m_MinPc==-1)||(percentage<m_Samples[i].m_MinPc))m_Samples[i].m_MinPc=percentage;
				if((m_Samples[i].m_MaxPc==-1)||(percentage>m_Samples[i].m_MaxPc))m_Samples[i].m_MaxPc=percentage;

				//output these values
				m_OutputHandler->Sample(percentage,m_Samples[i].m_MinPc,
					m_Samples[i].m_AveragePc,
					m_Samples[i].m_MaxPc,
					sample_time,
					m_Samples[i].m_CallCount,
					m_Samples[i].m_Name,
					m_Samples[i].m_ParentCount);

				//reset the sample for next time
				m_Samples[i].m_CallCount=0;
				m_Samples[i].m_TotalTime=0;
				m_Samples[i].m_ChildTime=0;
			}
		}
		m_OutputHandler->EndOutput();
	}

	void ProfileSample::ResetSample(std::string strName)
	{
		for(int i=0;i<MAX_PROFILER_SAMPLES; ++i)
		{
			if((m_Samples[i].m_IsValid)&&(m_Samples[i].m_Name==strName))
			{
				//found it
				//reset avg/min/max ONLY
				//because the sample may be running
				m_Samples[i].m_MaxPc=m_Samples[i].m_MinPc=-1;
				m_Samples[i].m_DataCount=0;
				return;
			}
		}
	}	

	void ProfileSample::ResetAll()
	{
		for(int i=0;i<MAX_PROFILER_SAMPLES;++i)
		{
			if(m_Samples[i].m_IsValid)
			{
				m_Samples[i].m_MaxPc=m_Samples[i].m_MinPc=-1;
				m_Samples[i].m_DataCount=0;
				if(!m_Samples[i].m_IsOpen)m_Samples[i].m_IsValid=false;
			}
		}
	}
}
