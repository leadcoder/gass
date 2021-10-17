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
#include "GASSLogger.h"
#include "GASSStringUtils.h"
#include "GASSFilesystem.h"
#include "GASSSystem.h"
#include <cstdarg>
#include <cassert>

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
		const std::string::size_type  pos = path.find("\\\\");
		if(pos != std::string::npos && pos == 0)
		{
			path = StringUtils::Replace(path.substr(pos+2,path.length()), "\\", "/");
			path = StringUtils::Replace(path, "//", "/");
			path = "\\\\" + path;
		}
		else
		{
			path = StringUtils::Replace(path, "\\", "/");

			const std::string::size_type pos2 = path.find("//");
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
		std::string cur_str = inStr;
		const std::string::size_type occur_index1 = cur_str.find("%");
		std::string var_name = "";
		std::string replace_str = "";
		if ( occur_index1 != std::string::npos )
		{
			const std::string::size_type end_var_index = cur_str.find("%", occur_index1+1);
			if (end_var_index == std::string::npos)
			{
				GASS_LOG(LWARNING) << "FilePath::ExpandEnvVariables - Erroneous use of environment variable: " << cur_str << "\nOnly one percent sign in string";
				assert(true);
			}
			else
			{
				var_name = cur_str.substr(occur_index1+1, end_var_index-occur_index1-1);
				replace_str = cur_str.substr(occur_index1, end_var_index-occur_index1+1);
				if (var_name.length() > 0)
				{
					const std::string var_value = System::GetEnvVar(var_name);
					if (var_value.empty())
					{
						GASS_LOG(LWARNING) << "Failed to find env var: " << var_name;
					}
					else
					{
						cur_str.replace(occur_index1, replace_str.length(), var_value);
					}
				}
			}

		}
		const std::string::size_type occur_index2 = cur_str.find("$");
		if (occur_index2 != std::string::npos )
		{
			const std::string::size_type start_var_index = cur_str.find("(");
			const std::string::size_type end_var_index = cur_str.find(")");
			if (start_var_index == std::string::npos || end_var_index == std::string::npos)
			{
				GASS_LOG(LWARNING) << "FilePath::ExpandEnvVariables - Erroneous use of environment variable: " << cur_str << " Missing start or end parenthesis";
				assert(true);
			}
			else
			{
				var_name = cur_str.substr(start_var_index+1, end_var_index-start_var_index-1);
				replace_str = cur_str.substr(occur_index2, end_var_index-occur_index2+1);
				if (var_name.length() > 0)
				{
					const std::string env_var_value = System::GetEnvVar(var_name.c_str());
					if (env_var_value.empty())
					{
						GASS_LOG(LWARNING) << "Failed to find env var: " << var_name;
					}
					else
					{
						cur_str.replace(occur_index2, replace_str.length(), env_var_value);
					}
				}
			}
		}
		return cur_str;
	}

	std::string FilePath::GetPathNoExtension() const
	{
		std::string ret;
		const std::string::size_type pos = m_ExpandedPath.find_last_of(".");
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
		const std::string::size_type  pos = ret.find_last_of("/", ret.size());
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
		const std::string::size_type  pos = ret.find_last_of("/",ret.size());
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
						for(const auto & extenstion_filter : extenstion_filters)
						{
							if(StringUtils::ToLower(exstension) == extenstion_filter)
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
