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


#include "Core/Common.h"
#include "GASSStringUtils.h"
#include "Core/Utils/GASSException.h"

#include <cctype>
#ifndef _MSC_VER
#include <cxxabi.h>
#endif

namespace GASS
{
	namespace StringUtils
	{
		std::string RemoveQuotation(const char* str)
		{
			std::string ret = str;
			ret = ret.substr(1, ret.length() - 2);
			return ret;
		}

		std::string RemoveQuotation(const std::string &str)
		{
			std::string ret = str;
			ret = ret.substr(1, ret.length() - 2);
			return ret;
		}

		unsigned char CharToUpper(char ch)
		{
			return static_cast<unsigned char>(std::toupper(static_cast<unsigned char>(ch)));
		}

		unsigned char CharToLower(char ch)
		{
			return static_cast<unsigned char>(std::tolower(static_cast<unsigned char>(ch)));
		}

		std::string ToUpper(const std::string&str)
		{
			std::string new_str = str;
			std::transform(str.begin(), str.end(), new_str.begin(), CharToUpper);
			return new_str;
		}

		std::string ToLower(const std::string &str)
		{
			std::string new_str = str;
			std::transform(str.begin(), str.end(), new_str.begin(), CharToLower);
			return new_str;
		}

		int CompareNoCase(const std::string &str1, const std::string &str2)
		{
			return ToLower(str1).compare(ToLower(str2));
		}

		std::string Replace(const std::string &str, const std::string &find, const std::string &replacement)
		{
			std::string::size_type  pos;
			std::string::size_type  look_here = 0;
			std::string new_str = str;

			// what we are going to replace already exist replacement string, throw to indicate this.
			if (replacement.find(find) != std::string::npos)
				GASS_EXCEPT(Exception::ERR_INTERNAL_ERROR, "String to replace with contains string searched for - will result in infinite loop!", "StringUtils::Replace");

			while ((pos = new_str.find(find, look_here)) != std::string::npos)
			{
				new_str.replace(pos, find.size(), replacement);
				look_here = pos + replacement.size();
			}
			return new_str;
		}

		std::string Demangle(const std::string &name)
		{
#ifdef _MSC_VER
			if (std::string::npos == name.find("class"))
				return name;
			std::string ret = name.substr(6);
#else
			int status;
			std::string ret = abi::__cxa_demangle(name.c_str(), 0, 0, &status);
#endif
			//remove namespace
			const size_t pos = ret.find("::");
			if (pos != std::string::npos)
			{
				ret = ret.substr(pos + 2);
			}
			return ret;
		}

		//Specialized template implementation to catch std::string
		template <>
		bool GetValueFromString<std::string>(std::string &res, const std::string &s)
		{
			res = s;
			return true;
		}

		//Specialized template implementation to catch bool
		template <>
		bool GetValueFromString<bool>(bool &res, const std::string &s)
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
			// When converting to float/double, clamp values to max precision data type (digits10) can guarantee + 1 value digit.
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
}
