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
	public:
		virtual ~IEnumBinder(){}
	};

	/*class IntEnumBinder : public IEnumBinder
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

		static std::vector<std::string> GetAllOptions(const BaseReflectionObject* object, const std::string &filter) 
		{
			(void) filter;
			std::vector<std::string> types;
			std::map<std::string ,int>::iterator iter =  m_Names.begin();
			while(iter != m_Names.end())
			{
				types.push_back(iter->first);
				++iter;
			}
			return types;
		}
		static bool IsMultiValue() {return false;}
	protected:
		void SetTypeFromName(const std::string &name) 
		{
			if(m_Names.find(name) != m_Names.end())
				m_Enum = m_Names[name];
			else
				GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Failed to find enum:" + name,"IntEnumBinder::SetValueFromName");
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
	};*/


	template< class ENUM,class CLASS>
	class MultiEnumBinder : public IEnumBinder
	{
	public:
		MultiEnumBinder(ENUM value) : m_Value(value)
		{
			CLASS::Register();
		}

		MultiEnumBinder() 
		{
			CLASS::Register();
		}

		virtual ~MultiEnumBinder(){}

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

		static std::vector<std::string> GetAllOptions(const BaseReflectionObject* object, const std::string &options) 
		{
			std::vector<std::string> types;
			std::map<std::string ,ENUM>::iterator iter =  m_NameToEnumMap.begin();
			while(iter != m_NameToEnumMap.end())
			{
				types.push_back(iter->first);
				++iter;
			}
			return types;
		}
		static bool IsMultiValue() {return true;}

		//virtual void Register() = 0;
	protected:
		void SetValueFromNames(const std::vector<std::string> &names) 
		{
			m_Value = static_cast<ENUM>(0);
			for(size_t i=0; i< names.size() ; i++)
			{
				m_Value = static_cast<ENUM>(static_cast<int>(m_Value) | static_cast<int>(m_NameToEnumMap[names[i]]));
			}
		}

		std::vector<std::string> GetNamesFromValue() const
		{
			std::vector<std::string> types;
			std::map<std::string ,ENUM>::iterator iter =  m_NameToEnumMap.begin();
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

		static void Bind(const std::string &name, ENUM type)
		{
			m_NameToEnumMap[name] = type;
			m_EnumToNameMap[type] = name;
		}

		ENUM m_Value;
		static std::map<std::string ,ENUM> m_NameToEnumMap;
		static std::map<ENUM,std::string > m_EnumToNameMap;
	};


	template< class ENUM,class CLASS>
	class SingleEnumBinder : public IEnumBinder
	{
	public:
		SingleEnumBinder(ENUM value) : m_Value(value)
		{
			CLASS::Register();
		}

		SingleEnumBinder() 
		{
			CLASS::Register();
		}

		virtual ~SingleEnumBinder(){}

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

		static std::vector<std::string> GetAllOptions(const BaseReflectionObject* object, const std::string &options) 
		{
			std::vector<std::string> types;
			std::map<std::string ,ENUM>::iterator iter =  m_NameToEnumMap.begin();
			while(iter != m_NameToEnumMap.end())
			{
				types.push_back(iter->first);
				++iter;
			}
			return types;
		}

		static std::vector<CLASS> GetAllEnums(const BaseReflectionObject* object) 
		{
			std::vector<CLASS> types;
			std::map<std::string ,ENUM>::iterator iter =  m_NameToEnumMap.begin();
			while(iter != m_NameToEnumMap.end())
			{
				types.push_back(iter->second);
				++iter;
			}
			return types;
		}

		static bool IsMultiValue() {return false;}


		static std::vector<CLASS> GetEnumeration() 
		{
			std::vector<CLASS> types;
			std::map<std::string ,ENUM>::iterator iter =  m_NameToEnumMap.begin();
			while(iter != m_NameToEnumMap.end())
			{
				types.push_back(iter->second);
				++iter;
			}
			return types;
		}

	protected:
		void SetValueFromName(const std::string &name) 
		{
			if(m_NameToEnumMap.find(name) != m_NameToEnumMap.end())
				m_Value = m_NameToEnumMap[name];
			else
			{
				const std::string class_name = typeid(CLASS).name();
				GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Failed to find enum:" + name + " In class" + class_name,"SingleEnumBinder::SetValueFromName");
			}
		}

		std::string GetNameFromValue() const
		{
			return m_EnumToNameMap[m_Value];
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
			return os;
		}

		static void Bind(const std::string &name, ENUM type)
		{
			m_NameToEnumMap[name] = type;
			m_EnumToNameMap[type] = name;
		}

		ENUM m_Value;
		static std::map<std::string ,ENUM> m_NameToEnumMap;
		static std::map<ENUM,std::string > m_EnumToNameMap;
	};

}

#define START_MULTI_ENUM_BINDER(ENUM,ENUM_BINDER) \
	class ENUM_BINDER : public MultiEnumBinder<ENUM,ENUM_BINDER>  \
	{								\
	public:							\
	ENUM_BINDER():MultiEnumBinder(){};			\
	ENUM_BINDER(ENUM value):MultiEnumBinder(value){};			\
	virtual ~ENUM_BINDER(){};	\
	static void Register()		\
	{							\

#define BIND_FLAG(ENUM) Bind(#ENUM, ENUM);


#define END_MULTI_ENUM_BINDER(ENUM,ENUM_BINDER) \
	}							\
	};								\
	template<> std::map<std::string ,ENUM>  MultiEnumBinder<ENUM,ENUM_BINDER>::m_NameToEnumMap;\
	template<> std::map<ENUM,std::string> MultiEnumBinder<ENUM,ENUM_BINDER>::m_EnumToNameMap;\




#define START_ENUM_BINDER(ENUM,ENUM_BINDER) \
	class ENUM_BINDER : public SingleEnumBinder<ENUM,ENUM_BINDER>  \
	{								\
	public:							\
	ENUM_BINDER():SingleEnumBinder(){};			\
	ENUM_BINDER(ENUM value):SingleEnumBinder(value){};			\
	virtual ~ENUM_BINDER(){};	\
	static void Register()		\
	{							\

#define BIND(ENUM) Bind(#ENUM, ENUM);


#define END_ENUM_BINDER(ENUM,ENUM_BINDER) \
	}							\
	};								\
	template<> std::map<std::string ,ENUM>  SingleEnumBinder<ENUM,ENUM_BINDER>::m_NameToEnumMap;\
	template<> std::map<ENUM,std::string> SingleEnumBinder<ENUM,ENUM_BINDER>::m_EnumToNameMap;\

