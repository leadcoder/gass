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

#ifndef FILEPATH_HH
#define FILEPATH_HH

#include "Core/Common.h"
#include "stdio.h"
#include <string>

namespace GASS
{


	/** \addtogroup GASSCore
	*  @{
	*/
	/** \addtogroup Utility
	*  @{
	*/

	/**
	*	Class used to handel filesystem paths with environment variables.
	*/

	class GASSCoreExport FilePath
	{
	public:
		FilePath(const std::string &path,bool expand = true);
		FilePath();
		~FilePath();
		std::string GetFullPath() const;
		std::string GetRawPath() const;
		void SetPath(const std::string &path);
		std::string GetPathNoExtension() const;
		std::string GetPathNoFile() const;
		std::string GetExtension() const;
		std::string GetFilename() const;
		static void GetFilesFromPath(std::vector<FilePath> &files, const FilePath &path, bool recursive = false, const std::vector<std::string> extenstion_filters = std::vector<std::string>());
		static void GetFoldersFromPath(std::vector<FilePath> &folders, const FilePath &path, bool recursive = false);

		friend std::ostream& operator << (std::ostream& os, const FilePath& path)
		{
			os.unsetf(std::ios::skipws);
			os << path.GetRawPath();
			return os;
		}

		friend std::istream& operator >> (std::istream& is, FilePath& path)
		{
			is.unsetf(std::ios::skipws);
			std::string line;
			std::getline (is,line);
			path.SetPath(line);
			return is;
		}
	private:
		std::string ExpandEnvVariables(const std::string &inStr);
		std::string m_ExpandedPath;
		std::string m_RawPath;
	};
}

#endif // #ifndef FILEPATH_HH
