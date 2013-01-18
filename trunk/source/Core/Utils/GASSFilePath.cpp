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

#include "GASSFilePath.h"
#include "GASSLogManager.h"
#include "GASSMisc.h"
#include <stdarg.h>
#include <stdlib.h>
#include <assert.h>



namespace GASS
{

	FilePath::FilePath(const std::string &path, bool expand)
	{
		m_RawPath  = path;
		if(expand)
			m_ExpandedPath = ExpandEnvVariables(path);
		else
			m_ExpandedPath = path; 
	}

	FilePath::FilePath()
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
		m_ExpandedPath = ExpandEnvVariables(path);
	}

	std::string FilePath::ExpandEnvVariables(const std::string &inStr)
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

		curStr = Misc::Replace(curStr, "\\", "/");
		curStr = Misc::Replace(curStr, "//", "/");
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
		boost::filesystem::path boost_path(m_ExpandedPath); 
		std::string ext = boost_path.extension().generic_string();
		//remove dot
		if(ext != "")
			ext = ext.substr(1);
		return ext;
	
		/*std::string ret, reversed_string;
		std::string::size_type pos = m_ExpandedPath.find_last_of(".");

		if (pos == m_ExpandedPath.npos)
			return "";

		ret = m_ExpandedPath.substr(pos+1);*/
		//return ret;
	}

	std::string FilePath::GetFilename() const
	{
		boost::filesystem::path boost_path(m_ExpandedPath); 
		return boost_path.filename().generic_string();

		/*std::string::size_type  pos = ret.find("/",0);
		while(pos != std::string::npos)
		{
			ret = ret.substr(pos+1);
			pos = ret.find("/",0);
		}
		pos = ret.find("\\",0);

		while(pos != std::string::npos)
		{
			ret = ret.substr(pos+1);
			pos = ret.find("\\",0);
		}
		return ret;*/
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
		boost::filesystem::path boost_path(path.GetFullPath()); 
		if( boost::filesystem::exists(boost_path) && boost::filesystem::is_directory( boost_path))  
		{
			boost::filesystem::directory_iterator end ;    
			for( boost::filesystem::directory_iterator iter(boost_path) ; iter != end ; ++iter )      
			{
				if (boost::filesystem::is_directory( *iter ))      
				{   
					folders.push_back(FilePath(iter->path().string(),false));
					if(recursive)
						GetFilesFromPath(folders,FilePath(iter->path().string(),false), recursive);
				}
			}
		}
	}


	void FilePath::GetFilesFromPath(std::vector<FilePath> &files, const FilePath &path, bool recursive, const std::vector<std::string> extenstion_filters)
	{
		boost::filesystem::path boost_path(path.GetFullPath()); 
		if( boost::filesystem::exists(boost_path) && boost::filesystem::is_directory( boost_path))  
		{
			boost::filesystem::directory_iterator end;
			for( boost::filesystem::directory_iterator iter(boost_path) ; iter != end ; ++iter )      
			{
				if (boost::filesystem::is_directory( *iter )  && recursive)      
				{   
					GetFilesFromPath(files,FilePath(iter->path().string(),false), recursive,extenstion_filters);
				}
				else
				{
					if(extenstion_filters.size() > 0)
					{
						const std::string exstension = iter->path().extension().generic_string();

						bool find_ext = false;
						for(size_t i = 0; i < extenstion_filters.size(); i++)
						{
							if(Misc::ToLower(exstension) == extenstion_filters[i])
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
