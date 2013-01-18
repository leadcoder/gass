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

#ifndef MISC_HH
#define MISC_HH

#include <string>
#include <algorithm>
#include "Core/Common.h"
#include "Core/Math/GASSVector.h"
class TiXmlElement;
#define READ_ULONG *(unsigned long*)ptr;	ptr += 4
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

	class GASSCoreExport Misc
	{
	public:
		Misc();
		virtual ~Misc();
		//xml helpers
		static std::string ReadString(TiXmlElement *xml_elem, const std::string &tag);
		static bool ReadBool(TiXmlElement *xml_elem, const std::string &tag);
		static Float ReadFloat(TiXmlElement *xml_elem, const std::string &tag);
		static int ReadInt(TiXmlElement *xml_elem, const std::string &tag);

		static std::string ReadStringAttribute(TiXmlElement *xml_elem, const std::string &attrib);

		static std::string LoadTextFile(const std::string &file_name);
		static std::string RemoveExtension(const std::string &file_name);
		static std::string RemoveQuotation(char* str);
		static std::string RemoveQuotation(const std::string &str);
		static std::string Replace(const std::string &str, const std::string &find, const std::string &replacement);
		static std::string GetExtension(const std::string &file_name);
		static int ReadOneLine(FILE *f,char *string);
		static std::string GetFilename(const std::string &path);
		static std::string RemoveFilename(const std::string &path);
		static bool FindTexture(std::string &name);
		inline static std::string ToLower(const std::string &str)
		{
			std::string new_str = str;
			std::transform(str.begin(), str.end(), new_str.begin(),::tolower);
			return new_str;
		}

		static std::string ToUpper(const std::string&str);
		static void GetVector(const char* str,Vec3 &vec);
		static int CompareNoCase(const std::string &str1, const std::string &str2)
		{
			return Misc::ToLower(str1).compare(ToLower(str2));
		}

		// Replaces windows specific _splitpath - TODO need to be tested!!!
		static void splitpath(char *name, char *drive, char *path, char *base, char *ext);

		static std::string Demangle(const std::string &name);
		static void GetFilesFromPath(std::vector<std::string> &files, const std::string &path,  bool recursive = false, bool full_path = false);
	};
}

#endif // #ifndef MISC_HH

