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

#include "GASSFilePath.h"
#include "GASSLogManager.h"
#include "GASSStringUtils.h"
#include "GASSFilesystem.h"
#include <stdarg.h>
#include <stdlib.h>
#include <assert.h>

namespace GASS
{

	FilePath::FilePath(const std::string &path, bool expand)
	{
		m_RawPath  = path;
		_FixPath(m_RawPath);
		if(expand)
			m_ExpandedPath = _ExpandEnvVariables(m_RawPath);
		else
			m_ExpandedPath = m_RawPath;
	}

	void FilePath::_FixPath(std::string &path) const
	{
		//check if path start with \\\\, windows network path
		std::string::size_type  pos = path.find("\\\\");
		if(pos != std::string::npos && pos == 0)
		{
			path = StringUtils::Replace(path.substr(pos+2,path.length()), "\\", "/");
			path = StringUtils::Replace(path, "//", "/");
			path = "\\\\" + path;
		}
		else
		{
			path = StringUtils::Replace(path, "\\", "/");

			std::string::size_type pos2 = path.find("//");
			if (pos2 != std::string::npos)
			{
				if (pos2 == 0)
				{
					// replace leading duplicate front slashes in UNC path for it to work. How should this be handled more generic?
					path = StringUtils::Replace(path, "//", "\\\\");
				}
				else
				{
					// Removes duplicate front slashes if not first in path (UNC path).
					path = StringUtils::Replace(path, "//", "/");
				}
			}
		}
		//check if folder or file?
		GASS_FILESYSTEM::path gass_path(path);
		if(GASS_FILESYSTEM::exists(gass_path) && GASS_IS_DIRECTORY(gass_path))
		{
			if (!path.empty() && path[path.length()-1] != '/')
				path += '/';
		}
	}

	FilePath::FilePath() : m_ExpandedPath(""), m_RawPath("")
	{

	}

	FilePath::~FilePath()
	{

	}

	std::string FilePath::GetFullPath() const
	{
		return m_ExpandedPath;
	}

	std::string FilePath::GetRawPath() const
	{
		return m_RawPath;
	}

	void FilePath::SetPath(const std::string &path)
	{
		m_RawPath  = path;
		_FixPath(m_RawPath);
		m_ExpandedPath = _ExpandEnvVariables(m_RawPath);
	}

	std::string FilePath::_ExpandEnvVariables(const std::string &inStr) const
	{
		std::string curStr = inStr;
		std::string::size_type occurIndex1 = curStr.find("%");
		std::string varName = "";
		std::string replaceStr = "";
		if ( occurIndex1 != std::string::npos )
		{
			std::string::size_type endVarIndex = curStr.find("%", occurIndex1+1);
			if (endVarIndex == std::string::npos)
			{
				LogManager::getSingleton().stream() << "WARNING:FilePath::ExpandEnvVariables - Erroneous use of environment variable: " << curStr << "\nOnly one percent sign in string";
				assert(true);
			}
			else
			{
				varName = curStr.substr(occurIndex1+1, endVarIndex-occurIndex1-1);
				replaceStr = curStr.substr(occurIndex1, endVarIndex-occurIndex1+1);
				if (varName.length() > 0)
				{
					char* temp_str = getenv(varName.c_str());

					if (temp_str)
					{
						curStr.replace( occurIndex1, replaceStr.length(), std::string(temp_str));

					}
					else
					{
						LogManager::getSingleton().stream() << "WARNING:Failed to find env var: " << varName;
					}
				}
			}

		}
		std::string::size_type occurIndex2 = curStr.find("$");
		if (occurIndex2 != std::string::npos )
		{
			std::string::size_type startVarIndex = curStr.find("(");
			std::string::size_type endVarIndex = curStr.find(")");
			if (startVarIndex == std::string::npos || endVarIndex == std::string::npos)
			{
				LogManager::getSingleton().stream() << "WARNING:FilePath::ExpandEnvVariables - Erroneous use of environment variable: " << curStr << " Missing start or end parenthesis";
				assert(true);
			}
			else
			{
				varName = curStr.substr(startVarIndex+1, endVarIndex-startVarIndex-1);
				replaceStr = curStr.substr(occurIndex2, endVarIndex-occurIndex2+1);
				if (varName.length() > 0)
				{
					char* temp_str = getenv(varName.c_str());
					if (temp_str)
					{
						curStr.replace( occurIndex2, replaceStr.length(), std::string(temp_str) );
					}
					else
					{
						LogManager::getSingleton().stream() << "WARNING:Failed to find env var: " << varName;
					}
				}
			}
		}
		return curStr;
	}

	std::string FilePath::GetPathNoExtension() const
	{
		std::string ret;
		std::string::size_type pos = m_ExpandedPath.find_last_of(".");
		ret = m_ExpandedPath.substr(0,pos);
		return ret;
	}

	std::string FilePath::GetExtension() const
	{

		GASS_FILESYSTEM::path mypath(m_ExpandedPath);
		std::string ext = GASS_TO_GENERIC_STRING(mypath.extension());
		//remove dot
		if(ext != "")
			ext = ext.substr(1);
		return ext;
	}

	bool FilePath::HasExtension() const
	{
		GASS_FILESYSTEM::path mypath(m_ExpandedPath);
		return !mypath.extension().empty();
	}

	std::string FilePath::GetFilename() const
	{
		GASS_FILESYSTEM::path gass_path(m_ExpandedPath);
		return GASS_TO_GENERIC_STRING(gass_path.filename());
	}

	std::string FilePath::GetStem() const
	{
		GASS_FILESYSTEM::path gass_path(m_ExpandedPath);
		return GASS_TO_GENERIC_STRING(gass_path.stem());
	}

	std::string FilePath::GetLastFolder() const
	{
		std::string ret = m_ExpandedPath;
		std::string::size_type  pos = ret.find_last_of("/", ret.size());
		if (pos != std::string::npos)
		{
			ret = ret.substr(0, pos);
		}

		GASS_FILESYSTEM::path gass_path(ret);
		return GASS_TO_GENERIC_STRING(gass_path.filename()); 
	}

	std::string FilePath::GetPathNoFile() const
	{
		std::string ret = m_ExpandedPath;
		std::string::size_type  pos = ret.find_last_of("/",ret.size());
		if(pos != std::string::npos)
		{
			ret = ret.substr(0,pos+1);
		}
		return ret;
	}


	void FilePath::GetFoldersFromPath(std::vector<FilePath> &folders, const FilePath &path, bool recursive)
	{
		GASS_FILESYSTEM::path gass_path(path.GetFullPath());
		if( GASS_FILESYSTEM::exists(gass_path) && GASS_IS_DIRECTORY( gass_path))
		{
			GASS_FILESYSTEM::directory_iterator end;
			for( GASS_FILESYSTEM::directory_iterator iter(gass_path) ; iter != end ; ++iter )
			{
				if (GASS_IS_DIRECTORY( *iter ))
				{
					folders.push_back(FilePath(iter->path().string(),false));
					if(recursive)
						GetFoldersFromPath(folders,FilePath(iter->path().string(),false), recursive);
				}
			}
		}
	}

	bool FilePath::Exist() const
	{
		GASS_FILESYSTEM::path gass_path(GetFullPath());
		return GASS_FILESYSTEM::exists(gass_path);
	}

	bool FilePath::IsDir() const
	{
		GASS_FILESYSTEM::path path(GetFullPath());
		return GASS_FILESYSTEM::is_directory(path);
	}

	void FilePath::GetFilesFromPath(std::vector<FilePath> &files, const FilePath &path, bool recursive, const std::vector<std::string> extenstion_filters)
	{
		GASS_FILESYSTEM::path gass_path(path.GetFullPath());
		if( GASS_FILESYSTEM::exists(gass_path) && GASS_IS_DIRECTORY( gass_path))
		{
			GASS_FILESYSTEM::directory_iterator end;
			for( GASS_FILESYSTEM::directory_iterator iter(gass_path) ; iter != end ; ++iter )
			{
				if (GASS_IS_DIRECTORY( *iter )  && recursive)
				{
					GetFilesFromPath(files,FilePath(iter->path().string(),false), recursive,extenstion_filters);
				}
				else
				{
					if(extenstion_filters.size() > 0)
					{
						const std::string exstension = GASS_TO_GENERIC_STRING(iter->path().extension());

						bool find_ext = false;
						for(size_t i = 0; i < extenstion_filters.size(); i++)
						{
							if(StringUtils::ToLower(exstension) == extenstion_filters[i])
							{
								find_ext = true;
								break;
							}
						}
						if(find_ext)
							files.push_back(FilePath(iter->path().string(),false));
					}
					else
					{
						files.push_back(FilePath(iter->path().string(),false));
					}
				}
			}
		}
	}
}
