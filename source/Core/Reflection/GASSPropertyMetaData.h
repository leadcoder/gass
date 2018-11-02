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

#ifndef GASS_PROPERTY_DATA_H
#define GASS_PROPERTY_DATA_H


#include "Core/Common.h"
#include <sstream>
#include <vector>

namespace GASS
{

	/** \addtogroup GASSCore
	*  @{
	*/
	/** \addtogroup Reflection
	*  @{
	*/
	/**
		Property flags indicating if the property mode
		that can be used by editor applications
	*/
	enum PropertyFlags 
	{
		PF_RESET  = 0,
		PF_VISIBLE  =   1 << 0,
		PF_EDITABLE =   1 << 1,
	};
	class BaseReflectionObject;
	typedef GASS_SHARED_PTR<BaseReflectionObject> BaseReflectionObjectPtr;

	inline PropertyFlags operator|(PropertyFlags a, PropertyFlags b)
	{return static_cast<PropertyFlags>(static_cast<int>(a) | static_cast<int>(b));}


	/**
		Interface for all property meta data
	*/
	class IPropertyMetaData
	{
	public:
		virtual ~IPropertyMetaData(){}
	};
	typedef GASS_SHARED_PTR<IPropertyMetaData> PropertyMetaDataPtr;


	/**
		Base implementation of the property meta data interface
		Property meta data can hold information about properties such as documentation, max	min values, 
		flags that indicate if it's read-only, visible etc. inside editor environment
		This base class is a convenience class that implements annotation and flags
	*/

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
	typedef GASS_SHARED_PTR<BasePropertyMetaData> BasePropertyMetaDataPtr;


	/**
		Meta data class that can be used for enumeration properties
	*/

	class EnumerationPropertyMetaData : public BasePropertyMetaData
	{
	public:
		EnumerationPropertyMetaData(const std::string &annotation, PropertyFlags flags, bool multi_select) : BasePropertyMetaData(annotation,flags), m_MultiSelect(multi_select)
		{

		}
		virtual std::vector<std::string> GetEnumeration(BaseReflectionObjectPtr object) const = 0;
		virtual bool GetMultiSelect() const {return m_MultiSelect;}
	private:
		bool m_MultiSelect;
	};
	typedef GASS_SHARED_PTR<EnumerationPropertyMetaData> EnumerationPropertyMetaDataPtr;
	
	/**
		Meta data class that can be used for static enumeration properties, 
		enumeration is known when property is registered
	*/

	class StaticEnumerationPropertyMetaData : public EnumerationPropertyMetaData
	{
	public:
		StaticEnumerationPropertyMetaData(const std::string &annotation, PropertyFlags flags,const std::vector<std::string> enumeration,bool multi_select = false): EnumerationPropertyMetaData(annotation,flags,multi_select) , 
			m_Enumeration(enumeration)
		{

		}
		std::vector<std::string> GetEnumeration(BaseReflectionObjectPtr object) const override {return m_Enumeration;}
	private:
		std::vector<std::string> m_Enumeration;
	};
	typedef GASS_SHARED_PTR<StaticEnumerationPropertyMetaData> StaticEnumerationPropertyMetaDataPtr;


	/**
		Meta data class that can be used for enumeration properties 
		that want to delegate the enumeration request to other class 
		
	*/
	typedef std::vector<std::string> EnumerationFunc();
	class EnumerationProxyPropertyMetaData : public EnumerationPropertyMetaData
	{
	public:
		EnumerationProxyPropertyMetaData(const std::string &annotation, PropertyFlags flags,EnumerationFunc *enumeration_func,bool multi_select = false): EnumerationPropertyMetaData(annotation,flags,multi_select) , 
			m_EnumFunc(enumeration_func)
		{

		}
		std::vector<std::string> GetEnumeration(BaseReflectionObjectPtr object) const override {return m_EnumFunc();}
	private:
		EnumerationFunc* m_EnumFunc;

	};
	typedef GASS_SHARED_PTR<EnumerationProxyPropertyMetaData > EnumerationProxyPropertyMetaDataPtr;


	/**
		Meta data class for floating point properties that want to support max min values
	*/

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
	typedef GASS_SHARED_PTR<FloatMaxMinPropertyMetaData> FloatMaxMinPropertyMetaDataPtr;


	/**
		Meta data class for integer properties that want to support max min values
	*/

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
	typedef GASS_SHARED_PTR<IntMaxMinPropertyMetaData> IntMaxMinPropertyMetaDataPtr;


	/**
		Meta data class for file path properties that want to support extention filters ect.
	*/

	class FilePathPropertyMetaData : public BasePropertyMetaData
	{
	public:
		enum FilePathEditType
		{
			IMPORT_FILE,
			EXPORT_FILE,
			PATH_SELECTION,
		};

		FilePathPropertyMetaData(const std::string &annotation, PropertyFlags flags, FilePathEditType type, const std::vector<std::string> &extensions): BasePropertyMetaData(annotation,flags) ,
			m_Type(type), m_Extensions(extensions)
		{
		}
		std::vector<std::string> GetExtensions() const {return m_Extensions;}
		FilePathEditType GetType() const {return m_Type;}
	private:
		FilePathEditType m_Type;
		std::vector<std::string> m_Extensions;
		
	};
	typedef GASS_SHARED_PTR<FilePathPropertyMetaData> FilePathPropertyMetaDataPtr;

}
#endif
