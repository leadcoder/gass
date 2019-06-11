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

	namespace StringUtils
	{
	
		/**
		@brief Return first and last character in string (actually not checking if quotation characters).
		@param str String to work on.
		@return String without first and last character.
		*/
		GASSCoreExport std::string RemoveQuotation(const char* str);
		/**
		@brief Return first and last character in string (actually not checking if quotation characters).
		@param str String to work on.
		@return String without first and last character.
		*/
		GASSCoreExport std::string RemoveQuotation(const std::string &str);
		/**
		@brief Replace all occurances of find in str with replacement string.
		@param str String to look in.
		@param find String to search for in string str.
		@param replacement String to replace all occurrences of find with.
		@return String str but with all occurrences of find string replaced by replacement string.
		*/
		GASSCoreExport std::string Replace(const std::string &str, const std::string &find, const std::string &replacement);

		/**
		@brief Return string in str with all characters converted to lower case.
		@param str String to work on.
		@return All lower case string.
		*/
		GASSCoreExport std::string ToLower(const std::string &str);
		/**
		@brief Return string in str with all characters converted to upper case.
		@param str String to work on.
		@return All upper case string.
		*/
		GASSCoreExport std::string ToUpper(const std::string&str);

		GASSCoreExport int CompareNoCase(const std::string &str1, const std::string &str2);
		
		GASSCoreExport std::string Demangle(const std::string &name);

		template <class TYPE>
		bool GetValueFromString(TYPE &res, const std::string &s)
		{
			std::stringstream ss(s);
			ss >> res;
			return !ss.fail();
		}

		//Specialized template implementation to catch std::string
		template <>
		GASSCoreExport bool GetValueFromString<std::string>(std::string &res, const std::string &s);
		

		//Specialized template implementation to catch bool
		template <>
		GASSCoreExport bool GetValueFromString<bool>(bool &res, const std::string &s);
	
		//-------------------------Get string from value---------------------
		template <class TYPE>
		bool GetStringFromValue(const TYPE &val, std::string &res)
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
		GASSCoreExport bool GetStringFromValue<float>(const float &val, std::string &res);

		//Use specialized template to catch double
		template <>
		GASSCoreExport bool GetStringFromValue<double>(const double &val, std::string &res);

		//Use specialized template to catch float vector
		template <>
		GASSCoreExport bool GetStringFromValue< std::vector<float> >(const std::vector<float> &val, std::string &res);

		//Use specialized template to catch double vector
		template <>
		GASSCoreExport bool GetStringFromValue< std::vector<double> >(const std::vector<double> &val, std::string &res);
	}
}

#endif // #ifndef StringUtils_HH

