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

/*
This class is based on the Game Programming Gems 5 article
"Using Templates for Reflection in C++" by Dominic Filion.
*/


#ifndef GASS_PROPERTY_DATA_H
#define GASS_PROPERTY_DATA_H


#include "Core/Common.h"
#include <assert.h>
#include <sstream>
#include <vector>

namespace GASS
{
	enum PropertyFlags 
	{
		PF_RESET  = 0,
		PF_VISIBLE  =   1 << 0,
		PF_EDITABLE =   1 << 1,
	};


	inline PropertyFlags operator|(PropertyFlags a, PropertyFlags b)
	{return static_cast<PropertyFlags>(static_cast<int>(a) | static_cast<int>(b));}

	class IPropertyMetaData
	{
	public:
		virtual ~IPropertyMetaData(){}
	};
	typedef SPTR<IPropertyMetaData> PropertyMetaDataPtr;


	class BasePropertyMetaData : public IPropertyMetaData
	{
		
	public:
		BasePropertyMetaData(const std::string &annotation, PropertyFlags flags): m_Annotation(annotation), m_Flags(flags) {}
		std::string GetAnnotation() const {return m_Annotation;}
		void SetAnnotation(const std::string &value) {m_Annotation= value;}
		PropertyFlags GetFlags() const {return m_Flags;}
		void SetFlags(PropertyFlags value) {m_Flags= value;}
	private:
		std::string m_Annotation;
		PropertyFlags m_Flags;
	};
	typedef SPTR<BasePropertyMetaData> BasePropertyMetaDataPtr;


	class EnumerationPropertyMetaData : public BasePropertyMetaData
	{
	public:
		EnumerationPropertyMetaData(const std::string &annotation, PropertyFlags flags, bool multi_select) : BasePropertyMetaData(annotation,flags), m_MultiSelect(multi_select)
		{

		}
		virtual std::vector<std::string> GetEnumeration() const = 0;
		virtual bool GetMultiSelect() const {return m_MultiSelect;}
	private:
		bool m_MultiSelect;
	};
	typedef SPTR<EnumerationPropertyMetaData> EnumerationPropertyMetaDataPtr;
	

	class StaticEnumerationPropertyMetaData : public EnumerationPropertyMetaData
	{
	public:
		StaticEnumerationPropertyMetaData(const std::string &annotation, PropertyFlags flags,const std::vector<std::string> enumeration,bool multi_select = false): EnumerationPropertyMetaData(annotation,flags,multi_select) , 
			m_Enumeration(enumeration)
		{

		}
		virtual std::vector<std::string> GetEnumeration() const {return m_Enumeration;}
	private:
		std::vector<std::string> m_Enumeration;
	};
	typedef SPTR<StaticEnumerationPropertyMetaData> StaticEnumerationPropertyMetaDataPtr;


	typedef std::vector<std::string> EnumerationFunc(void);
	class EnumerationProxyPropertyMetaData : public EnumerationPropertyMetaData
	{
	public:
		EnumerationProxyPropertyMetaData(const std::string &annotation, PropertyFlags flags,EnumerationFunc *enumeration_func,bool multi_select = false): EnumerationPropertyMetaData(annotation,flags,multi_select) , 
			m_EnumFunc(enumeration_func),
			m_MultiSelect(multi_select)
		{

		}
		virtual std::vector<std::string> GetEnumeration() const {return m_EnumFunc();}
		bool GetMultiSelect() const {return m_MultiSelect;}
	private:
		bool m_MultiSelect;
		EnumerationFunc* m_EnumFunc;

	};
	typedef SPTR<EnumerationProxyPropertyMetaData > EnumerationProxyPropertyMetaDataPtr;


	class FloatMaxMinPropertyMetaData : public BasePropertyMetaData
	{
	public:
		FloatMaxMinPropertyMetaData(const std::string &annotation, PropertyFlags flags,Float _max, Float _min ) : BasePropertyMetaData(annotation,flags) ,
			m_Max(_max),m_Min(_min)
		{
		}
		virtual Float GetMin() const {return m_Min;}
		virtual Float GetMax() const {return m_Max;}
	private:
		Float m_Max;
		Float m_Min;
	};
	typedef SPTR<FloatMaxMinPropertyMetaData> FloatMaxMinPropertyMetaDataPtr;

	class IntMaxMinPropertyMetaData : public BasePropertyMetaData
	{
	public:
		IntMaxMinPropertyMetaData(const std::string &annotation, PropertyFlags flags,int _max, int _min ): BasePropertyMetaData(annotation,flags) ,
			m_Max(_max),m_Min(_min)
		{
		}
		virtual int GetMin() const {return m_Min;}
		virtual int GetMax() const {return m_Max;}
	private:
		int m_Max;
		int m_Min;
	};
	typedef SPTR<IntMaxMinPropertyMetaData> IntMaxMinPropertyMetaDataPtr;
}
#endif
