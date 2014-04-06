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


#include "Core/Common.h"
#include "GASSStringUtils.h"
#include "GASSLogManager.h"
#include "GASSException.h"
#ifndef WIN32
    #include <cxxabi.h>
#endif

namespace GASS
{
	StringUtils::StringUtils()
	{

	}

	StringUtils::~StringUtils()
	{

	}


	std::string StringUtils::RemoveQuotation(char* str)
	{
		std::string ret = str;
		ret = ret.substr(1,ret.length()-2);
		return ret;
	}

	std::string StringUtils::RemoveQuotation(const std::string &str)
	{
		std::string ret = str;
		ret = ret.substr(1,ret.length()-2);
		return ret;
	}


	std::string StringUtils::ToUpper(const std::string&str)
	{
		std::string new_str = str;
		std::transform(str.begin(), str.end(), new_str.begin(),::toupper);
		return new_str;
	}


	std::string StringUtils::Replace(const std::string &str, const std::string &find, const std::string &replacement)
	{
		std::string::size_type  pos = 0;
		std::string::size_type  look_here = 0;
		std::string new_str = str;

		//if(find.find(replacement)) // what we are going to replace already exist replecement string -> infinite while
		while ((pos = new_str.find(find,look_here)) != std::string::npos)
		{
			new_str.replace(pos, find.size(), replacement);
			look_here = pos + replacement.size();
		}
		return new_str;
	}


	std::string StringUtils::Demangle(const std::string &name)
	{
#ifdef WIN32
		if(std::string::npos == name.find("class"))
			return name;
		std::string ret = name.substr(6);
#else
		int status;
		std::string ret = abi::__cxa_demangle(name.c_str(), 0, 0, &status);
#endif
		//remove namespace
		size_t pos = ret.find("::");
		if(pos != -1)
		{
			ret =  ret.substr(pos+2);
		}
		return ret;
	}


}
