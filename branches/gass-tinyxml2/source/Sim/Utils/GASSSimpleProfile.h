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

#pragma once

#include "Sim/GASSCommon.h"
#include <tbb/tick_count.h>

#define ENABLE_SPROFILER

namespace GASS
{
	class SimpleProfileData
	{
	public:
		SimpleProfileData() : Count(0),
			Time(0),
			AccTime(0) 
		{

		}
		int Count;
		tbb::tick_count StartTick;
		tbb::tick_count EndTick;
		double Time;
		double AccTime;
		double AvgTime;
	};

	typedef std::map<std::string, SimpleProfileData> SimpleProfileDataMap;

	class SimpleProfileSample
	{
	public:
		SimpleProfileSample(const std::string name,SimpleProfileDataMap *data ) 
		{
			SimpleProfileDataMap::iterator iter = data->find(name);
			if(iter == data->end())
			{
				SimpleProfileData sample;
				sample.StartTick = tbb::tick_count::now();
				sample.Count = 1;
				sample.AvgTime = 0;
				(*data)[name] = sample;
				m_Data = &data->find(name)->second;
			}
			else
			{
				iter->second.StartTick = tbb::tick_count::now();
				iter->second.Count++;
				m_Data = &iter->second;
			}

		}
		virtual ~SimpleProfileSample()
		{
			m_Data->EndTick = tbb::tick_count::now();
			m_Data->Time = (m_Data->EndTick - m_Data->StartTick).seconds();
			m_Data->AccTime += m_Data->Time;
		}

		SimpleProfileData *m_Data;
	};
}

#ifdef ENABLE_SPROFILER
#define SPROFILE(name,data) GASS::SimpleProfileSample profile_sample(name,data);
#else
#define SPROFILE(name,data)
#endif