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

namespace tinyxml2
{
	class XMLElement;
}
/*#define READ_ULONG *(unsigned long*)ptr;	ptr += 4
#define READ_LONG *(long*)ptr;	ptr += 4
#define READ_FLOAT *(float*)ptr;	ptr += 4
#define READ_SHORT *(short*)ptr;	ptr += 2
#define READ_USHORT *(unsigned short*)ptr;	ptr += 2
#define READ_CHAR *(char*)ptr;	ptr++
#define READ_UCHAR *(unsigned char*)ptr;	ptr++

// Constants replacing windows specific _MAX_DRIVE, _MAX_DIR etc
const int GASS_MAX_DRIVE = 3;
const int GASS_MAX_DIR	= 256;
const int GASS_MAX_FNAME	= 256;
const int GASS_MAX_EXT	= 256;
*/
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
		StringUtils();
		virtual ~StringUtils();
		/**
		@brief Return first and last character in string (actually not checking if quotation characters).
		@param str String to work on.
		@return String without first and last character.
		*/
		static std::string RemoveQuotation(char* str);
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


	};
}

#endif // #ifndef StringUtils_HH

