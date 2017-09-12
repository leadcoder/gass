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


#include "Core/Reflection/GASSProperty.h"

namespace GASS
{
	//Specialized template implementation to catch std::string
	template <>
	bool GetValueFromString<std::string>(std::string &res,const std::string &s)
	{
		res = s;
		return true;
	}

	//Specialized template implementation to catch bool
	template <>
	bool GetValueFromString<bool>(bool &res,const std::string &s)
	{
		std::stringstream ss;
		if (s.find("0") == std::string::npos && s.find("1") == std::string::npos)
			ss.setf(std::ios::boolalpha);
		ss << s;
		ss >> res;
		return !ss.fail();
	}
	
	template <>
    bool GetStringFromValue(const float &val, std::string &res)
    {
        std::stringstream ss;
	    //sstream.unsetf(std::ios::skipws);
		//sstream.setf(std::ios::boolalpha);
		//sstream.setf(0,std::ios::floatfield);
		//ss.unsetf(std::ios::floatfield);
		//ss.precision(std::numeric_limits<float>::digits10 + 1);
		ss << std::setprecision(std::numeric_limits<float>::digits10 + 1) << val;
		res = ss.str();
		return !ss.fail();
    }

	template <>
	bool GetStringFromValue(const double &val, std::string &res)
	{
		std::stringstream ss;
		ss << std::setprecision(std::numeric_limits<double>::digits10 + 1) << val;
		res = ss.str();
		return !ss.fail();
	}

	template <>
	bool GetStringFromValue(const std::vector<float> &val, std::string &res)
	{
		std::stringstream ss;
		ss << std::setprecision(std::numeric_limits<float>::digits10 + 1) << val;
		res = ss.str();
		return !ss.fail();
	}

	template <>
	bool GetStringFromValue(const std::vector<double> &val, std::string &res)
	{
		std::stringstream ss;
		ss << std::setprecision(std::numeric_limits<double>::digits10 + 1) << val;
		res = ss.str();
		return !ss.fail();
	}
}
