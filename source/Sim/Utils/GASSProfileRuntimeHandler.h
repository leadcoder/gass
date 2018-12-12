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

#if !defined PROFILE_RUNTIME_HANDLER
#define PROFILE_RUNTIME_HANDLER

class Log;
#include "GASSProfiler.h"

namespace GASS
{
	class ProfileRuntimeHandler : public IProfilerOutputHandler
	{
	public:
		ProfileRuntimeHandler();
		~ProfileRuntimeHandler() override;
		void BeginOutput(double tTime) override;
		void EndOutput() override;
		void Sample(double fCur, double fMin, double fAvg, double fMax, double tAvg, int callCount, std::string name, int parentCount) override;
	};
}

#endif // !defined(AFX_PROFILELOGHANDLER_H__CAD57C2F_2BF7_492C_8ED3_EFE606EF3EAC__INCLUDED_)
