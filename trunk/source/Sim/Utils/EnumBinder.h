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

#pragma once

#include "Sim/Common.h"
#include "Core/Math/Vector.h"

namespace GASS
{
	template< class ENUM,class CLASS>
	class EnumBinder
	{
	public:
		EnumBinder(ENUM type) : m_Type(type)
		{
			CLASS::Register();
		}

		EnumBinder() 
		{
			CLASS::Register();
		}

		virtual ~EnumBinder(){}

		void Set(ENUM value) 
		{
			m_Type = value;
		}

		ENUM Get() const 
		{
			return m_Type;
		}

		bool operator== (const CLASS &v) const
		{
			return (m_Type == v.Get());
		}

		//virtual void Register() = 0;
	protected:
		void SetTypeFromName(const std::string &name) 
		{
			m_Type = m_Names[name];
		}

		std::string GetName() const
		{
			return m_Types[m_Type];	
		}

		static std::vector<std::string> GetAllNames() 
		{
			std::vector<std::string> types;
			std::map<std::string ,ENUM>::iterator iter =  m_Names.begin();
			while(iter != m_Names.end())
			{
				types.push_back(iter->first);
				iter++;
			}
			return types;
		}

		friend std::ostream& operator << (std::ostream& os, const CLASS& cat)
		{
			std::string name (cat.GetName());
			os << name;
			return os;
		}

		friend std::istream& operator >> (std::istream& os, CLASS& cat)
		{
			std::string name;
			os >> name;
			cat.SetTypeFromName(name);
			return os;
		}

		static void Bind(const std::string &name, ENUM type)
		{
			m_Names[name] = type;
			m_Types[type] = name;
		}

		ENUM m_Type;
		static std::map<std::string ,ENUM> m_Names;
		static std::map<ENUM,std::string > m_Types;
	};
}
