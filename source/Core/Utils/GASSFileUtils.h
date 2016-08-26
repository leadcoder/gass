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

#ifndef FILE_UTILS_HH
#define FILE_UTILS_HH

#include "Core/Common.h"

namespace GASS
{
	/** \addtogroup GASSCore
	*  @{
	*/
	/** \addtogroup Utility
	*  @{
	*/

	/**
	Class used for file operations
	*/

	class GASSCoreExport FileUtils
	{
	public:
		FileUtils();
		virtual ~FileUtils();
		static std::string GetExtension(const std::string &file_name);
		static std::string GetFilename(const std::string &path);
		static std::string RemoveFilename(const std::string &path);
		static std::string RemoveExtension(const std::string &path);
		static bool FileExist(const std::string &file_name);
		static void GetFilesFromPath(std::vector<std::string> &files, const std::string &path,  bool recursive = false, bool full_path = false);
	};
}

#endif 

