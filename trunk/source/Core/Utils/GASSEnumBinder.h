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

#include "Sim/GASSCommon.h"
#include "Core/Math/GASSVector.h"

namespace GASS
{

	/** \addtogroup GASSSim
	*  @{
	*/
	/** \addtogroup Utilities
	*  @{
	*/

	/**
		Helper class enable enums to be 
		used in reflection attribute registration
		TODO: Move this to GASSCore
	*/

	class IEnumBinder
	{
	public:
		virtual ~IEnumBinder(){}
		//virtual std::vector<std::string> GetAllOptions() = 0;
	};

	class IntEnumBinder : public IEnumBinder
	{
	public:
		IntEnumBinder(int type) : m_Enum(type)
		{
			Register();
		}

		IntEnumBinder() 
		{
			Register();
		}

		virtual ~IntEnumBinder(){}

		void Set(int value) 
		{
			m_Enum = value;
		}

		int Get() const 
		{
			return m_Enum;
		}

		bool operator== (const IntEnumBinder &v) const
		{
			return (m_Enum == v.Get());
		}

		static std::vector<std::string> GetAllOptions() 
		{
			std::vector<std::string> types;
			std::map<std::string ,int>::iterator iter =  m_Names.begin();
			while(iter != m_Names.end())
			{
				types.push_back(iter->first);
				++iter;
			}
			return types;
		}

		//virtual void Register() = 0;
	protected:
		void SetTypeFromName(const std::string &name) 
		{
			m_Enum = m_Names[name];
		}

		std::string GetName() const
		{
			return m_Enums[m_Enum];	
		}

		

		friend std::ostream& operator << (std::ostream& os, const IntEnumBinder& value)
		{
			std::string name (value.GetName());
			os << name;
			return os;
		}

		friend std::istream& operator >> (std::istream& os, IntEnumBinder& value)
		{
			std::string name;
			os >> name;
			value.SetTypeFromName(name);
			return os;
		}

		static void Bind(const std::string &name, int value)
		{
			m_Names[name] = value;
			m_Enums[value] = name;
		}

		static void Register()		
		{						

		}		

		int m_Enum;
		static std::map<std::string ,int> m_Names;
		static std::map<int,std::string > m_Enums;
	};


	template< class ENUM,class CLASS>
	class EnumBinder : public IEnumBinder
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

		static std::vector<std::string> GetAllOptions() 
		{
			std::vector<std::string> types;
			std::map<std::string ,ENUM>::iterator iter =  m_Names.begin();
			while(iter != m_Names.end())
			{
				types.push_back(iter->first);
				++iter;
			}
			return types;
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

#define START_ENUM_BINDER(ENUM,ENUM_BINDER) \
	class ENUM_BINDER : public EnumBinder<ENUM,ENUM_BINDER>  \
	{								\
	public:							\
		ENUM_BINDER():EnumBinder(){};			\
		ENUM_BINDER(ENUM type):EnumBinder(type){};			\
		virtual ~ENUM_BINDER(){};	\
		static void Register()		\
		{							\

#define BIND(ENUM) Bind(#ENUM, ENUM);


#define END_ENUM_BINDER(ENUM,ENUM_BINDER) \
		}							\
	};								\
	template<> std::map<std::string ,ENUM>  EnumBinder<ENUM,ENUM_BINDER>::m_Names;\
	template<> std::map<ENUM,std::string> EnumBinder<ENUM,ENUM_BINDER>::m_Types;\

}
