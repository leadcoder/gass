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

#ifdef WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif
#include "GASSTimer.h"
namespace GASS
{
	Timer::Timer() 
	{
#ifdef WIN32
		
		// Check To See If A Performance Counter Is Available
		// If One Is Available The Timer Frequency Will Be Updated
		if (!QueryPerformanceFrequency((LARGE_INTEGER *) &m_Frequency))
		{
			// No Performace Counter Available
			m_PerfTimer	= false;				// Set Performance Timer To FALSE
			m_MMTimerStart	= timeGetTime();			// Use timeGetTime() To Get Current Time
			m_Resolution	= 1.0f/1000.0f;				// Set Our Timer Resolution To .001f
			m_Frequency		= 1000;					// Set Our Timer Frequency To 1000
			m_MMTimerElapsed	= m_MMTimerStart;			// Set The Elapsed Time To The Current Time
		}
		else
		{
			// Performance Counter Is Available, Use It Instead Of The Multimedia Timer
			// Get The Current Time And Store It In performance_timer_start
			QueryPerformanceCounter((LARGE_INTEGER *) &m_PerfTimerStart);
			m_PerfTimer		= true;				// Set Performance Timer To TRUE
			// Calculate The Timer Resolution Using The Timer Frequency
			m_Resolution		= static_cast<float>(1.0/static_cast<double>(m_Frequency));
			// Set The Elapsed Time To The Current Time
			m_PerfTimerElapsed	= m_PerfTimerStart;
		}

#else

		m_Resolution	= 1.0f/1000.0f;				// Set Our Timer Resolution To .001f
		m_Frequency		= 1000;					// Set Our Timer Frequency To 1000

		timeval tmpTimeVal;
		gettimeofday(&tmpTimeVal, 0);
		m_StartTime = (double)(tmpTimeVal.tv_sec + tmpTimeVal.tv_usec / 1000000.0);

		//std::cout << "Start time is:" << tmpTimeVal.tv_sec << " " << tmpTimeVal.tv_sec << std::endl;
		//std::cout << "Start time is:" << m_StartTime << std::endl;

#endif
	}

	// Get Time In Seconds
	double Timer::GetTime() const
	{
#ifdef WIN32

		__int64 time = 0;								// time Will Hold A 64 Bit Integer

		if (m_PerfTimer)						// Are We Using The Performance Timer?
		{
			QueryPerformanceCounter((LARGE_INTEGER *) &time);		// Grab The Current Performance Time
			// Return The Current Time Minus The Start Time Multiplied By The Resolution
			return static_cast<double>( time - m_PerfTimerStart) * m_Resolution;
		}
		else
		{
			// Return The Current Time Minus The Start Time Multiplied By The Resolution
			return static_cast<double>( timeGetTime() - m_MMTimerStart) * m_Resolution;
		}

#else

		timeval curTime;
		gettimeofday(&curTime, 0);
		double ticks = (double)(curTime.tv_sec + curTime.tv_usec / 1000000.0);
		double newTicks = ticks - m_StartTime;
		//std::cout << "time is:" << newTicks << std::endl;
		//std::cout << "time is:" << curTime.tv_sec << " " << curTime.tv_usec << std::endl;


		// Return The Current Time Minus The Start Time Multiplied By The Resolution And 1000 (To Get MS)
		return( (double) (newTicks));
#endif

	}

	void Timer::Reset()
	{
#ifdef WIN32
		if ( m_PerfTimer == true )
			QueryPerformanceCounter(( LARGE_INTEGER* )&m_PerfTimerStart);
		else
			m_MMTimerStart = timeGetTime();
#else
		timeval curTime;
		gettimeofday(&curTime, 0);
		m_StartTime = (double)(curTime.tv_sec + curTime.tv_usec / 1000000.0);
		//std::cout << "Start time is:" << m_StartTime << std::endl;
		//std::cout << "Start time is:" << curTime.tv_sec << " " << curTime.tv_usec << std::endl;

#endif
	}
}

