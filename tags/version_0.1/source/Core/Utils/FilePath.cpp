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

#include "Core/Utils/FilePath.h"
#include "Core/Utils/GASSLogManager.h"
#include "Core/Utils/Misc.h"
#include <stdarg.h>
#include <stdlib.h>
#include <assert.h>



namespace GASS
{

	FilePath::FilePath(const std::string &path)
	{
		m_EnvPath  = path;
		m_ExpandPath = ExpandEnvVariables(path);
	}

	FilePath::FilePath()
	{

	}


	FilePath::~FilePath()
	{

	}

	std::string FilePath::GetPath() const
	{
		return m_ExpandPath;
	}

	void FilePath::SetPath(const std::string &path)
	{
		m_EnvPath  = path;
		m_ExpandPath = ExpandEnvVariables(path);
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
						curStr = Misc::Replace(curStr, "\\", "/");
						curStr = Misc::Replace(curStr, "//", "/");
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
						curStr = Misc::Replace(curStr, "\\", "/");
						curStr = Misc::Replace(curStr, "//", "/");
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
		std::string::size_type pos = m_ExpandPath.find_last_of(".");
		ret = m_ExpandPath.substr(0,pos);
		return ret;
	}

	std::string FilePath::GetExtension() const
	{
		std::string ret, reversed_string;
		std::string::size_type pos = m_ExpandPath.find_last_of(".");

		if (pos == m_ExpandPath.npos)
			return "";

		ret = m_ExpandPath.substr(pos+1);

		return ret;
	}

	std::string FilePath::GetFilename() const
	{
		std::string ret = m_ExpandPath;

		std::string::size_type  pos = ret.find("/",0);
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
		return ret;
	}

	std::string FilePath::GetPathNoFile() const
	{
		std::string ret = m_ExpandPath;
		std::string::size_type  pos1 = ret.find_last_of("/",ret.size());
		std::string::size_type pos2 = ret.find_last_of("\\",ret.size());
		if(pos1 > pos2 && pos1 > -1)
		{
			ret = ret.substr(0,pos1+1);
		}
		else if(pos2 > -1)
		{
			ret = ret.substr(0,pos2+1);
		}
		return ret;
	}
}
