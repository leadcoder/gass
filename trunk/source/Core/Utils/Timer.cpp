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

#ifdef WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif
#include "Timer.h"

namespace GASS
{
	Timer::Timer()
	{
#ifdef WIN32
		m_PerfTimerStart = 0;
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
			m_Resolution		= (float) (((double)1.0f)/((double)m_Frequency));
			// Set The Elapsed Time To The Current Time
			m_PerfTimerElapsed	= m_PerfTimerStart;
		}

#else

		m_Resolution	= 1.0f/1000.0f;				// Set Our Timer Resolution To .001f
		m_Frequency		= 1000;					// Set Our Timer Frequency To 1000

		timeval tmpTimeVal;
		gettimeofday(&tmpTimeVal, 0);
		m_MMTimerStart = (double)(tmpTimeVal.tv_sec * 1000 + tmpTimeVal.tv_usec / 1000.0);

#endif
	}

	Timer::~Timer()
	{

	}

	// Get Time In Seconds
	double Timer::GetTime()
	{
#ifdef WIN32

		__int64 time;								// time Will Hold A 64 Bit Integer

		if (m_PerfTimer)						// Are We Using The Performance Timer?
		{
			QueryPerformanceCounter((LARGE_INTEGER *) &time);		// Grab The Current Performance Time
			// Return The Current Time Minus The Start Time Multiplied By The Resolution
			return ( (double) ( time - m_PerfTimerStart) * m_Resolution);
		}
		else
		{
			// Return The Current Time Minus The Start Time Multiplied By The Resolution
			return( (double) ( timeGetTime() - m_MMTimerStart) * m_Resolution);
		}

#else

		timeval curTime;
		gettimeofday(&curTime, 0);
		double ticks = (double)(curTime.tv_sec * 1000 + curTime.tv_usec / 1000.0);
		double newTicks = ticks - m_MMTimerStart;

		// Return The Current Time Minus The Start Time Multiplied By The Resolution And 1000 (To Get MS)
		return( (double) (newTicks  - m_MMTimerStart) * m_Resolution)*1000.0f;
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
		m_MMTimerStart = (double)(curTime.tv_sec * 1000 + curTime.tv_usec / 1000.0);
#endif
	}
}

