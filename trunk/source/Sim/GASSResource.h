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

#ifndef GASS_MESH_RESOURCE_H
#define GASS_MESH_RESOURCE_H

#include "Core/Common.h"
#include "Sim/Interface/GASSIResourceSystem.h"

namespace GASS
{
	class GASSExport Resource
	{
	public:
		Resource();
		Resource(const std::string &name);
		virtual ~Resource();
		std::string Name() const { return m_ResourceName;}
		void SetName(const std::string &name)  { m_ResourceName=name;}
		FilePath GetFilePath() const;
		bool Valid() const;
		//std::string operator () {return ResourceName;}
		friend std::ostream& operator << (std::ostream& os, const Resource& res)
		{
			os << res.Name();
			return os;
		}

		friend std::istream& operator >> (std::istream& os, Resource& res)
		{
			std::string name;
			os >> name;
			res.SetName(name);
			return os;
		}
	private:
		std::string m_ResourceName;
	};

	class GASSExport MeshResource: public Resource
	{
	public:
		MeshResource(){}
		MeshResource(const std::string &name) : Resource(name) {}
		virtual ~MeshResource() {}
		static std::vector<std::string> GetAllOptions(); 
		static bool IsMultiValue() {return false;}
	};
}
#endif 
