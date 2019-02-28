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

#ifndef GASS_RESOURCE_HANDLE_H
#define GASS_RESOURCE_HANDLE_H

#include "Core/Common.h"
#include "Sim/GASSResource.h"

namespace GASS
{
	class GASSExport ResourceHandle
	{
	public:
		ResourceHandle();
		ResourceHandle(const std::string &name);
		std::string Name() const { return m_ResourceName;}
		void SetName(const std::string &name) { m_ResourceName = name;}
		FileResourcePtr GetResource() const;
		bool Valid() const {return (m_ResourceName != "");}
		friend std::ostream& operator << (std::ostream& os, const ResourceHandle& res)
		{
			os.unsetf(std::ios::skipws);
			os << res.Name();
			return os;
		}
		friend std::istream& operator >> (std::istream& is, ResourceHandle& res)
		{
			is.unsetf(std::ios::skipws);
			std::string name;
			std::getline (is,name);
			res.SetName(name);
			return is;
		}
	private:
		std::string m_ResourceName;
	};
}
#endif 
