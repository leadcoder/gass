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

#ifndef StringUtils_HH
#define StringUtils_HH

#include "Core/Common.h"
#include "Core/Math/GASSVector.h"
#include "Core/Serialize/GASSSTDSerialize.h"

namespace GASS
{
	/** \addtogroup GASSCore
	*  @{
	*/
	/** \addtogroup Utility
	*  @{
	*/

	/**
	Class used for string operations
	*/

	class GASSCoreExport StringUtils
	{
	public:
		/**
		@brief Return first and last character in string (actually not checking if quotation characters).
		@param str String to work on.
		@return String without first and last character.
		*/
		static std::string RemoveQuotation(const char* str);
		/**
		@brief Return first and last character in string (actually not checking if quotation characters).
		@param str String to work on.
		@return String without first and last character.
		*/
		static std::string RemoveQuotation(const std::string &str);
		/**
		@brief Replace all occurances of find in str with replacement string.
		@param str String to look in.
		@param find String to search for in string str.
		@param replacement String to replace all occurrences of find with.
		@return String str but with all occurrences of find string replaced by replacement string.
		*/
		static std::string Replace(const std::string &str, const std::string &find, const std::string &replacement);

		/**
		@brief Return string in str with all characters converted to lower case.
		@param str String to work on.
		@return All lower case string.
		*/
		static std::string ToLower(const std::string &str);
		/**
		@brief Return string in str with all characters converted to upper case.
		@param str String to work on.
		@return All upper case string.
		*/
		static std::string ToUpper(const std::string&str);

		static int CompareNoCase(const std::string &str1, const std::string &str2)
		{
			return StringUtils::ToLower(str1).compare(ToLower(str2));
		}
		static std::string Demangle(const std::string &name);

		template <class TYPE>
		static bool GetValueFromString(TYPE &res, const std::string &s)
		{
			std::stringstream ss(s);
			ss >> res;
			return !ss.fail();
		}

		//Use specialized template to catch std::string
		template <>
		static bool GetValueFromString<std::string>(std::string &res, const std::string &s);

		//Use specialized template to catch bool
		template <>
		static bool GetValueFromString<bool>(bool &res, const std::string &s);

		//-------------------------Get string from value---------------------
		template <class TYPE>
		static bool GetStringFromValue(const TYPE &val, std::string &res)
		{
			std::stringstream sstream;

			//we prefer bool values as "true"/"false" in string
			sstream.setf(std::ios::boolalpha);

			sstream << val;
			res = sstream.str();
			return true;
		}

		//Use specialized template to catch float
		template <>
		static bool GetStringFromValue<float>(const float &val, std::string &res);

		//Use specialized template to catch double
		template <>
		static bool GetStringFromValue<double>(const double &val, std::string &res);

		//Use specialized template to catch float vector
		template <>
		static bool GetStringFromValue< std::vector<float> >(const std::vector<float> &val, std::string &res);

		//Use specialized template to catch double vector
		template <>
		static bool GetStringFromValue< std::vector<double> >(const std::vector<double> &val, std::string &res);
	};

	//Specialized template implementation to catch std::string
	template <>
	bool StringUtils::GetValueFromString<std::string>(std::string &res, const std::string &s)
	{
		res = s;
		return true;
	}

	//Specialized template implementation to catch bool
	template <>
	bool StringUtils::GetValueFromString<bool>(bool &res, const std::string &s)
	{
		std::stringstream ss;
		if (s.find("0") == std::string::npos && s.find("1") == std::string::npos)
			ss.setf(std::ios::boolalpha);
		ss << s;
		ss >> res;
		return !ss.fail();
	}

	template <>
	bool StringUtils::GetStringFromValue(const float &val, std::string &res)
	{
		std::stringstream ss;
		// When converting to float/double, clamp values to max precision data type (digits10) can guarantee + 1 value digit.
		ss << std::setprecision(std::numeric_limits<float>::digits10 + 1) << val;
		res = ss.str();
		return !ss.fail();
	}

	template <>
	bool StringUtils::GetStringFromValue(const double &val, std::string &res)
	{
		std::stringstream ss;
		ss << std::setprecision(std::numeric_limits<double>::digits10 + 1) << val;
		res = ss.str();
		return !ss.fail();
	}

	template <>
	bool StringUtils::GetStringFromValue(const std::vector<float> &val, std::string &res)
	{
		std::stringstream ss;
		ss << std::setprecision(std::numeric_limits<float>::digits10 + 1) << val;
		res = ss.str();
		return !ss.fail();
	}

	template <>
	bool StringUtils::GetStringFromValue(const std::vector<double> &val, std::string &res)
	{
		std::stringstream ss;
		ss << std::setprecision(std::numeric_limits<double>::digits10 + 1) << val;
		res = ss.str();
		return !ss.fail();
	}
}

#endif // #ifndef StringUtils_HH

