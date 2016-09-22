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

#pragma once


namespace GASS
{
	/*
		Stream operator added for std::vector to support vector property serialization.
	*/
	template<typename T>
	std::ostream& operator << (std::ostream& os, const std::vector<T>& v)
	{
		for(size_t i = 0; i < v.size(); i++)
		{
			if(i > 0)
				os << " ";
			os << v[i];
		}
		return os;
	}

	template<typename T>
	std::istream& operator >> (std::istream& is, std::vector<T>& v)
	{
		T value;
		v.clear();
		while(is.peek() != EOF && is >> value)
		{
			v.push_back(value);
		}
		return is;
	}
}
