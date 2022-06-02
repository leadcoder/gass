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

#include "Sim/GASSCommon.h"
#include "Core/Math/GASSVector.h"
#include "Core/Utils/GASSException.h"

namespace GASS
{
	class BaseReflectionObject;
	/** \addtogroup GASSSim
	*  @{
	*/
	/** \addtogroup Utilities
	*  @{
	*/

	/**
	Helper class enable enums to be
	used in reflection attribute registration
	*/

	class IEnumBinder
	{
		virtual std::vector<std::string> GetOptions() const = 0;
	};


	template< class ENUM,class CLASS>
	class MultiEnumBinder : public IEnumBinder
	{
	public:
		MultiEnumBinder(ENUM value) : m_Value(value)
		{

		}

		MultiEnumBinder()
		{

		}

		void SetValue(ENUM value)
		{
			m_Value = value;
		}

		ENUM GetValue() const
		{
			return m_Value;
		}

		bool operator== (const CLASS &v) const
		{
			return (m_Value == v.GetValue());
		}

		static std::vector<std::string> GetStringEnumeration()
		{
			std::vector<std::string> types;

			typename NameEnumMap::iterator iter =  m_NameToEnumMap.begin();
			while(iter != m_NameToEnumMap.end())
			{
				types.push_back(iter->first);
				++iter;
			}
			return types;
		}

		static std::vector<CLASS> GetEnumeration()
		{
			std::vector<CLASS> types;
			typename NameEnumMap::iterator iter = m_NameToEnumMap.begin();
			while (iter != m_NameToEnumMap.end())
			{
				types.push_back(iter->second);
				++iter;
			}
			return types;
		}

		std::vector<std::string> GetOptions() const override
		{
			return GetStringEnumeration();
		}

	protected:
		void SetValueFromNames(const std::vector<std::string> &names)
		{
			m_Value = static_cast<ENUM>(0);
			for(size_t i=0; i< names.size() ; i++)
			{
				if(names[i] != "" && m_NameToEnumMap.find(names[i]) != m_NameToEnumMap.end())
					m_Value = static_cast<ENUM>(static_cast<int>(m_Value) | static_cast<int>(m_NameToEnumMap[names[i]]));
				else
				{
					const std::string class_name = typeid(CLASS).name();
					GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Failed to find enum:" + names[i] + " In class" + class_name,"SingleEnumBinder::SetValueFromName");
				}
			}
		}

		std::vector<std::string> GetNamesFromValue() const
		{
			std::vector<std::string> types;
			typename NameEnumMap::iterator iter =  m_NameToEnumMap.begin();
			while(iter != m_NameToEnumMap.end())
			{
				if(iter->second & m_Value)
					types.push_back(iter->first);
				++iter;
			}
			return types;
		}



		friend std::ostream& operator << (std::ostream& os, const CLASS& enum_binder)
		{
			std::vector<std::string> names = enum_binder.GetNamesFromValue();
			for(size_t i = 0; i < names.size();i++)
			{
				if(i != 0)
					os << " ";
				os << names[i];
			}
			return os;
		}

		friend std::istream& operator >> (std::istream& os, CLASS& enum_binder)
		{
			std::string name;
			std::vector<std::string> values;
			while(os >> name)
			{
				values.push_back(name);
			}
			enum_binder.SetValueFromNames(values);
			return os;
		}

		ENUM m_Value;
		typedef std::map<std::string ,ENUM > NameEnumMap;
		static NameEnumMap m_NameToEnumMap;
	};
    template< class ENUM,class CLASS> std::map<std::string ,ENUM>  MultiEnumBinder<ENUM,CLASS>::m_NameToEnumMap = CLASS::InitMapping();

	template< class ENUM,class CLASS>
	class SingleEnumBinder : public IEnumBinder
	{
	public:
		SingleEnumBinder(ENUM value) : m_Value(value)
		{
		}

		SingleEnumBinder()
		{
		}

		void SetValue(ENUM value)
		{
			m_Value = value;
		}

		ENUM GetValue() const
		{
			return m_Value;
		}

		bool operator== (const CLASS &v) const
		{
			return (m_Value == v.GetValue());
		}

		
		static std::vector<CLASS> GetEnumeration()
		{
			std::vector<CLASS> types;
			typename NameEnumMap::iterator iter =  m_NameToEnumMap.begin();
			while(iter != m_NameToEnumMap.end())
			{
				types.push_back(iter->second);
				++iter;
			}
			return types;
		}

		std::vector<std::string> GetOptions() const override
		{
			return GetStringEnumeration();
		}

		static std::vector<std::string> GetStringEnumeration()
		{
			std::vector<std::string> types;
			typename NameEnumMap::iterator iter =  m_NameToEnumMap.begin();
			while(iter != m_NameToEnumMap.end())
			{
				types.push_back(iter->first);
				++iter;
			}
			return types;
		}

	protected:
		void SetValueFromName(const std::string &name)
		{
			if(name != "" && m_NameToEnumMap.find(name) != m_NameToEnumMap.end())
				m_Value = m_NameToEnumMap[name];
			else
			{
				const std::string class_name = typeid(CLASS).name();
				GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Failed to find enum:" + name + " In class" + class_name,"SingleEnumBinder::SetValueFromName");
			}
		}

		std::string GetNameFromValue() const
		{
			typename NameEnumMap::iterator iter = m_NameToEnumMap.begin();
			while (iter != m_NameToEnumMap.end())
			{
				if(m_Value == iter->second)
					return iter->first;
				iter++;
			}
			return "";
		}

		friend std::ostream& operator << (std::ostream& os, const CLASS& enum_binder)
		{
			std::string name = enum_binder.GetNameFromValue();
			os << name;
			return os;
		}

		friend std::istream& operator >> (std::istream& os, CLASS& enum_binder)
		{
			std::string name;
			if(os >> name)
				enum_binder.SetValueFromName(name);
			else
			{
				const std::string class_name = typeid(CLASS).name();
				GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Failed to parse enum value in class" + class_name,"SingleEnumBinder::SetValueFromName");
			}
			return os;
		}
		ENUM m_Value;
		typedef std::map<std::string ,ENUM > NameEnumMap;
		static NameEnumMap m_NameToEnumMap;
	};
	template<class ENUM, class CLASS> std::map<std::string, ENUM>  SingleEnumBinder<ENUM, CLASS>::m_NameToEnumMap = CLASS::InitMapping();
}

#define START_FLAG_ENUM_BINDER(ENUM,ENUM_BINDER) \
	class ENUM_BINDER : public GASS::MultiEnumBinder<ENUM,ENUM_BINDER>  \
	{								\
	public:							\
	ENUM_BINDER(): GASS::MultiEnumBinder<ENUM,ENUM_BINDER>(){};			\
	ENUM_BINDER(ENUM value): GASS::MultiEnumBinder<ENUM,ENUM_BINDER>(value){};			\
static NameEnumMap InitMapping()		\
	{							\
		NameEnumMap mapping; 

#define BIND_FLAG(ENUM) mapping[#ENUM] = ENUM;
#define BIND_FLAG2(NAME, ENUM) mapping[NAME] = ENUM;

#define END_FLAG_ENUM_BINDER(ENUM,ENUM_BINDER) \
		return mapping; \
	}							\
	};								\



#define START_ENUM_BINDER(ENUM,ENUM_BINDER) \
	class ENUM_BINDER : public GASS::SingleEnumBinder<ENUM, ENUM_BINDER>  \
	{								\
	public:							\
	ENUM_BINDER(): GASS::SingleEnumBinder<ENUM, ENUM_BINDER>(){};			\
	ENUM_BINDER(ENUM value): GASS::SingleEnumBinder<ENUM, ENUM_BINDER>(value){};			\
	static NameEnumMap InitMapping()		\
	{							\
		NameEnumMap mapping; 

#define BIND(ENUM) mapping[#ENUM] = ENUM;
#define BIND2(NAME, ENUM) mapping[NAME] = ENUM;

#define END_ENUM_BINDER(ENUM,ENUM_BINDER) \
		return mapping; \
	}							\
	}; \
	template class GASS::SingleEnumBinder<ENUM, ENUM_BINDER>;
