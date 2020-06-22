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

#include "GASSSystem.h"
//#include <cstdarg>
#include <cstdlib>

namespace GASS
{
	std::string System::GetEnvVar(const std::string &name)
	{
		const char* value = std::getenv(name.c_str());
		return value ? value : "";
	}


	bool System::SetEnvVar(const std::string& value)
	{
#ifdef WIN32
		const int ret = _putenv(value.c_str());
		if (ret == -1)
			return false;
		return true;
#else
		char* writable = new char[value.size() + 1];
		std::copy(value.begin(), value.end(), writable);
		writable[value.size()] = '\0'; // don't forget the terminating 0
		putenv(writable);
		//delete[] writable;
		return true;
#endif
	}
}
