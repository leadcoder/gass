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



#include "Core/RTC/GASSTBBManager.h"
#include "Core/RTC/GASSTaskNode.h"
#include "Core/Utils/GASSException.h"
#include "Core/Utils/GASSLogManager.h"
#include <tbb/task_scheduler_init.h>

namespace GASS
{
	TBBManager::TBBManager() : m_Scheduler(NULL)
	{

	}

	TBBManager::~TBBManager()
	{	

	}

	void TBBManager::Init(int num_threads)
	{
		//int nthread = tbb::task_scheduler_init::automatic;
		int  default_num_t = tbb::task_scheduler_init::default_num_threads();
		if(num_threads == -1)
			num_threads = default_num_t;
		
		m_Scheduler = new tbb::task_scheduler_init(num_threads);
	}
}
