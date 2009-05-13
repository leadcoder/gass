/****************************************************************************
* This file is part of GASS.                                                *
* See http://sourceforge.net/projects/gass/                                 *
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

#include "Core/Reflection/Property.h"

namespace GASS
{
	//Specialized template implementation to catch std::string 

	template <>
	bool GetValueFromString<std::string>(std::string &res,const std::string &s)
	{
		res = s;
		return true;
	}

	bool GASSCoreExport GetValueFromString<std::vector<int>>(std::vector<int> &res,const std::string &s)
	{
		GetVectorFromString(res,s);
		return true;
	}
	bool GASSCoreExport GetValueFromString<std::vector<float>>(std::vector<float> &res,const std::string &s)
	{
		GetVectorFromString(res,s);
		return true;
	}
	bool GASSCoreExport GetValueFromString<std::vector<double>>(std::vector<double> &res,const std::string &s)
	{
		GetVectorFromString(res,s);
		return true;
	}
	bool GASSCoreExport GetValueFromString<std::vector<std::string>>(std::vector<std::string> &res,const std::string &s)
	{
		GetVectorFromString(res,s);
		return true;
	}

	template <>
	bool GetStringFromValue<std::vector<std::string>>(const std::vector<std::string> &val,std::string &s)
	{
		GetStringFromVector(val,s);
		return true;
	}
	template <>
	bool GetStringFromValue<std::vector<int>>(const std::vector<int> &val,std::string &s)
	{
		GetStringFromVector(val,s);
		return true;
	}
	template <>
	bool GetStringFromValue<std::vector<float>>(const std::vector<float> &val,std::string &s)
	{
		GetStringFromVector(val,s);
		return true;
	}
	template <>
	bool GetStringFromValue<std::vector<double>>(const std::vector<double> &val,std::string &s)
	{
		GetStringFromVector(val,s);
		return true;
	}

}