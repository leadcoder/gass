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
#include "Core/Utils/Log.h"
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
		char * replaceVal = "";
		if ( occurIndex1 != std::string::npos )
		{
			std::string::size_type endVarIndex = curStr.find("%", occurIndex1+1);
			if (endVarIndex == std::string::npos)
			{
				Log::Warning("FilePath::ExpandEnvVariables - Erroneous use of environment variable: %s\nOnly one percent sign in string", curStr.c_str());
				assert(true);
			}
			else
			{
				varName = curStr.substr(occurIndex1+1, endVarIndex-occurIndex1-1);
				replaceStr = curStr.substr(occurIndex1, endVarIndex-occurIndex1+1);
				if (varName.length() > 0)
				{
					replaceVal = getenv(varName.c_str());
					if (replaceVal)
					{
						curStr.replace( occurIndex1, replaceStr.length(), replaceVal );
					}
					else
					{
						Log::Warning("Failed to find env var: %s",varName.c_str());
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
				Log::Warning("FilePath::ExpandEnvVariables - Erroneous use of environment variable: %s\nMissing start or end parenthesis", curStr.c_str());
				assert(true);
			}
			else
			{
				varName = curStr.substr(startVarIndex+1, endVarIndex-startVarIndex-1);
				replaceStr = curStr.substr(occurIndex2, endVarIndex-occurIndex2+1);
				if (varName.length() > 0)
				{
					replaceVal = getenv(varName.c_str());
					if (replaceVal)
					{
						curStr.replace( occurIndex2, replaceStr.length(), replaceVal );
					}
				}
			}
		}

		return curStr;
	}




	std::string FilePath::GetPathNoExtension() const
	{
		std::string ret;
		int pos = 0;
		pos = m_ExpandPath.find_last_of(".");
		ret = m_ExpandPath.substr(0,pos);
		return ret;
	}

	std::string FilePath::GetExtension() const
	{
		std::string ret, reversed_string;
		int pos = m_ExpandPath.find_last_of(".");

		if (pos == m_ExpandPath.npos)
			return "";

		ret = m_ExpandPath.substr(pos+1);

		return ret;
	}

	std::string FilePath::GetFilename() const 
	{
		std::string ret = m_ExpandPath;
		int pos = 0;

		pos = ret.find("/",0);
		while(pos >= 0)
		{
			ret = ret.substr(pos+1);
			pos = ret.find("/",0);
		}
		pos = ret.find("\\",0);

		while(pos >= 0)
		{
			ret = ret.substr(pos+1);
			pos = ret.find("\\",0);
		}
		return ret;
	}

	std::string FilePath::GetPathNoFile() const
	{
		std::string ret = m_ExpandPath;
		int pos1 = 0;
		int pos2 = 0;

		pos1 = ret.find_last_of("/",ret.size());
		pos2 = ret.find_last_of("\\",ret.size());
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
