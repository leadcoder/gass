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

#ifndef FILEPATH_HH
#define FILEPATH_HH

#include "Core/Common.h"
#include <string>
#include <fstream>

namespace GASS
{
	/** \addtogroup GASSCore
	*  @{
	*/
	/** \addtogroup Utility
	*  @{
	*/

	/**
	*	Class used to handle file system paths with environment variables.
	*/

	class GASSCoreExport FilePath
	{
	public:
		FilePath(const std::string &path,bool expand = true);
		FilePath();
		~FilePath();
		/**
		* \brief Get path with expanded environment variables.
		* \return
		*/
		std::string GetFullPath() const;
		
		/**
		* \brief Get path as it is sent to SetPath, including environment variables.
		* \return
		*/
		std::string GetRawPath() const;
		/**
		* \brief Set function that both update FilePath raw path including eventual environment variables as well as expand complete.
		* \return
		*/
		void SetPath(const std::string &path);
		/**
		* \brief Get complete path without environment variables but not including file type extension.
		* \return
		*/
		std::string GetPathNoExtension() const;
		/**
		* \brief Get complete path without environment variables but not including file name.
		* \return
		*/
		std::string GetPathNoFile() const;
		/**
		* \brief Get file extension without dot.
		* \return
		*/
		std::string GetExtension() const;
		
		bool HasExtension() const;
		/**
		* \brief Get file name including extension.
		* \return
		*/
		std::string GetFilename() const;
		/**
		* \brief Get file name not including extension.
		* \return
		*/
		std::string GetStem() const;
		/**
		* \brief Get name of last folder in a path without trailing slash.
		* \return
		*/
		std::string GetLastFolder() const;
		
		static void GetFilesFromPath(std::vector<FilePath> &files, const FilePath &path, bool recursive = false, const std::vector<std::string> extenstion_filters = std::vector<std::string>());
		static void GetFoldersFromPath(std::vector<FilePath> &folders, const FilePath &path, bool recursive = false);
		bool Exist() const;
		bool IsDir() const;
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
		void _FixPath(std::string &path) const;
		std::string _ExpandEnvVariables(const std::string &inStr) const;
		std::string m_ExpandedPath;
		std::string m_RawPath;
	};
}

#endif // #ifndef FILEPATH_HH
