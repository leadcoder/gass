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

#ifndef LOG_HH
#define LOG_HH

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
	* Class used for printing messages to file.
	*/

	class GASSCoreExport FileLog
	{
	public:
		FileLog();
		FileLog(const std::string &file_name);
		virtual ~FileLog();
		bool Init(void);
		bool Shutdown(void);
		/**
		* \brief Use this function for printing error messages.
		*/
		static bool Error(const char* text, ...);
		/**
		* \brief Use this function for printing warning messages.
		*/
		static bool Warning(const char* text, ...);
		/**
		* \brief Use this function to print a message to file and get new line feed.
		*/
		static bool Print(const char* text, ...);

		/**
		* \brief Use this function to print a message to file.
		*/
		static bool LinePrint(char* text, ...);

		/**
		* \brief This function is used if the class GASSCoreExport is in non-singleton mode.
		*/
		bool Output(char* text, ...);
		static FileLog* GetInstance();
		std::string GetFileName(){return m_FileName;};
		void SetFileName(char* file_name){m_FileName = file_name;};
	private:
		std::string m_FileName;
		FILE* m_File;
		static FileLog *m_Instance;
	};

}
#endif // #ifndef LOG_HH

