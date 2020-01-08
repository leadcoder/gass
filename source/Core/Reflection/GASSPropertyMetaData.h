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


#include "Core/Common.h"
#include <sstream>
#include <vector>

namespace GASS
{
	class BaseReflectionObject;
	typedef GASS_SHARED_PTR<BaseReflectionObject> BaseReflectionObjectPtr;
	
	/**
		Interface for all property meta data
		Property meta data can hold information about properties such as  max	min values,
		that can be used inside editor environment
	*/

	class IPropertyMetaData
	{
		GASS_DECLARE_CLASS_AS_INTERFACE(IPropertyMetaData)
	};
	typedef GASS_SHARED_PTR<IPropertyMetaData> PropertyMetaDataPtr;


	/**
		Meta data class for floating point properties that want to support max min values
	*/

	class FloatMaxMinPropertyMetaData : public IPropertyMetaData
	{
	public:
		FloatMaxMinPropertyMetaData(Float _max, Float _min ) :
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
	class IntMaxMinPropertyMetaData : public IPropertyMetaData
	{
	public:
		IntMaxMinPropertyMetaData(int _max, int _min ) : m_Max(_max),m_Min(_min)
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

	class FilePathPropertyMetaData : public IPropertyMetaData
	{
	public:
		enum FilePathEditType
		{
			IMPORT_FILE,
			EXPORT_FILE,
			PATH_SELECTION,
		};

		FilePathPropertyMetaData(FilePathEditType type, const std::vector<std::string> &extensions) :
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
