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

#include "Core/Utils/Log.h"
#include <stdarg.h>
#include <stdlib.h>
//#include "Core/Windows_PreReq.h"


namespace GASS
{
	FileLog::FileLog()
	{
		m_FileName = "FileLog.txt";
		Init();
	}

	FileLog::FileLog(const std::string &file_name)
	{
		m_FileName = file_name;
		Init();
	}


	FileLog::~FileLog()
	{
		Shutdown();
	}



	bool FileLog::Init(void)
	{
		//Clear the log contents
		if((m_File=fopen(m_FileName.c_str(), "wb"))==NULL)
			return false;

		//Close the file, and return a success!
		fclose(m_File);
		return true;
	}

	bool FileLog::Shutdown(void)
	{
		if(m_File)
			fclose(m_File);
		return true;
	}


	FileLog* FileLog::m_Instance = NULL;

	FileLog* FileLog::GetInstance()
	{
		if (m_Instance  == NULL) {
			m_Instance  = new FileLog();
		}
		return m_Instance ;
	}

	bool FileLog::Print(const char* text, ...)
	{
		va_list arg_list;
		va_start(arg_list, text);
		FileLog* log = FileLog::GetInstance();
		FILE* file;

		//Open the log file for append
		if((file = fopen(log->GetFileName().c_str(), "a+"))==NULL)
			return false;
		//Write the text and a newline
		vfprintf(file, text, arg_list);
		putc('\n', file);

		//Close the file
		fclose(file);
		va_end(arg_list);
		return true;
	}

	bool FileLog::Error(const char* text, ...)
	{
		va_list arg_list;
		va_start(arg_list, text);
		FileLog* log = FileLog::GetInstance();
		FILE* file;

		//Open the log file for append

		if((file = fopen(log->GetFileName().c_str(), "a+"))==NULL)
		{
			exit(0);
			return false;
		}
		//Write the text and a newline
		fprintf(file,"Error: ");
		vfprintf(file, text, arg_list);
		putc('\n', file);
		//Close the file
		fclose(file);

		char message[10000];
		vsprintf(message, text, arg_list);
#ifdef WIN32
		//	MessageBox(NULL,message,"Error",MB_OK);
#endif
		va_end(arg_list);

		exit(0);
		return true;
	}



	bool FileLog::Warning(const char* text, ...)
	{
		va_list arg_list;
		va_start(arg_list, text);
		FileLog* log = FileLog::GetInstance();
		FILE* file;

		//Open the log file for append
		if((file = fopen(log->GetFileName().c_str(), "a+"))==NULL)
			return false;
		//Write the text and a newline
		fprintf(file,"Warning: ");
		vfprintf(file, text, arg_list);
		putc('\n', file);
		//Close the file
		fclose(file);
		va_end(arg_list);
		return true;
	}



	bool FileLog::LinePrint(char* text, ...)
	{
		va_list arg_list;
		va_start(arg_list, text);
		FileLog* log = FileLog::GetInstance();
		FILE* file;

		//Open the log file for append
		if((file = fopen(log->GetFileName().c_str(), "a+"))==NULL)
			return false;
		//Write the text
		vfprintf(file, text, arg_list);
		//Close the file
		fclose(file);
		va_end(arg_list);
		return true;
	}

	bool FileLog::Output(char* text, ...)
	{
		va_list arg_list;

		//Initialize variable argument list
		va_start(arg_list, text);

		//Open the log file for append
		if((m_File = fopen(m_FileName.c_str(), "a+"))==NULL)
			return false;

		//Write the text and a newline
		vfprintf(m_File, text, arg_list);
		putc('\n', m_File);

		//Close the file
		fclose(m_File);
		va_end(arg_list);

		return true;
	}
}
