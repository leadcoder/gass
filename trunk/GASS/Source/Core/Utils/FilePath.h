/****************************************************************************
* This file is part of GASS.                                                *
* See http://sourceforge.net/projects/gass/                                 *
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

#pragma once

#include "Core/Common.h"
#include "stdio.h"
#include <string>

namespace GASS
{

/**\class FilePath
* \brief Class used to handel filesystem paths with environment variables.
*/
	class GASSCoreExport FilePath
	{
	public:
		FilePath(const std::string &path);
		FilePath();
		~FilePath();
		std::string GetPath() const;
		void SetPath(const std::string &path);
		std::string GetPathNoExtension() const;
		std::string GetPathNoFile() const;
		std::string GetExtension() const;
		std::string GetFilename() const;
	private:
		std::string ExpandEnvVariables(const std::string &inStr);
		std::string m_ExpandPath;
		std::string m_EnvPath;
	};
}

