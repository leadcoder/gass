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


#include "Core/Common.h"
#include "GASSMisc.h"
#include "GASSLogManager.h"
#include <algorithm>
#include <string.h>
#include <boost/filesystem.hpp>

#ifndef WIN32
#include <cxxabi.h>
#else
#include <Dbghelp.h>
#endif
namespace GASS
{
	Misc::Misc()
	{

	}

	Misc::~Misc()
	{

	}


	std::string Misc::RemoveQuotation(char* str)
	{
		std::string ret = str;
		ret = ret.substr(1,ret.length()-2);
		return ret;
	}

	std::string Misc::RemoveQuotation(const std::string &str)
	{
		std::string ret = str;
		ret = ret.substr(1,ret.length()-2);
		return ret;
	}


	std::string Misc::RemoveExtension(const std::string &file_name)
	{
		std::string ret;
		std::string::size_type pos;
		pos = file_name.find_last_of(".");
		ret = file_name.substr(0,pos);
		return ret;
	}

	std::string Misc::GetExtension(const std::string &file_name)
	{
		std::string ret, reversed_string;
		std::string::size_type pos  = file_name.find_last_of(".");

		if (pos == file_name.npos)
			return "";

		ret = file_name.substr(pos+1);

		return ret;
	}

	std::string Misc::GetFilename(const std::string &path)
	{
		std::string ret = path;
		
		std::string::size_type pos = ret.find("/",0);
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

	std::string Misc::RemoveFilename(const std::string &path)
	{
		std::string ret = path;

		
		std::string::size_type pos1 = ret.find_last_of("/",ret.size());
		std::string::size_type pos2 = ret.find_last_of("\\",ret.size());
		if(pos1 > pos2 && pos1 != std::string::npos)
		{
			ret = ret.substr(0,pos1+1);
		}
		else if(pos2  != std::string::npos)
		{
			ret = ret.substr(0,pos2+1);
		}
		return ret;
	}






	int Misc::ReadOneLine(FILE *f,char *string)
	{
		do
		{
			fgets(string, 255, f);
			if(feof(f)) return 0;
		} while ((string[0] == '/') || (string[0] == '\n'));
		int i = 0;
		for(; i < strlen(string); i++)
		{
			if(string[i] != '\t') break;
		}

		if(i == 0) return 1;
		for(int j = i; j < strlen(string)+1; j++)
		{
			string[j-i]	= string[j];
		}
		return 1;
	}


	bool Misc::FindTexture(std::string &name)
	{
		std::string ext,temp;

		std::string::size_type pos = name.find(".",0);

		if(pos != std::string::npos)
		{
			name = name.substr(0,pos);
		}
		std::string path = name;//FilePath::m_Texture;
		//path += name;

		FILE* fp;
		temp = path + ".tga";
		if(fp = fopen(temp.c_str(),"rb"))
		{
			fclose(fp);
			name += ".tga";
			return true;
		}

		temp = path + ".jpg";

		if(fp =fopen(temp.c_str(),"rb"))
		{
			fclose(fp);
			name += ".jpg";
			return true;
		}
		temp = path + ".bmp";

		if(fp = fopen(temp.c_str(),"rb"))
		{
			fclose(fp);
			name += ".bmp";
			return true;
		}

		temp = path + ".dds";
		if(fp = fopen(temp.c_str(),"rb"))
		{
			fclose(fp);
			name += ".dds";
			return true;
		}
		return false;
	}


	std::string Misc::ToUpper(const std::string&str)
	{
		std::string new_str = str;
		std::transform(str.begin(), str.end(), new_str.begin(),::toupper);
		return new_str;
	}


	std::string Misc::Replace(const std::string &str, const std::string &find, const std::string &replacement)
	{
		std::string::size_type  pos = 0;
		std::string::size_type  look_here = 0;
		std::string new_str = str;

		//if(find.find(replacement)) // what we are going to replace already exist replecement string -> infinite while
		while ((pos = new_str.find(find,look_here)) != std::string::npos)
		{
			new_str.replace(pos, find.size(), replacement);
			look_here = pos + replacement.size();
		}
		return new_str;
	}

	void Misc::GetVector(const char* str,Vec3 &vec)
	{
		char buffer[128];
		strcpy(buffer,str);
		char* tok = buffer;
		int i=0;
		vec.Set(0,0,0);

		while(tok[i] != '/' && tok[i] != ';')
		{
			if(tok[i] == NULL)
			{
				LogManager::getSingleton().stream() << "WARNING:Failed to parse vector " << tok;
				return;
			}
			i++;
		}

		tok[i] = NULL;
		vec.x = atof(tok);
		tok = &tok[++i];
		i=0;
		while(tok[i] != '/' && tok[i] != ';')
		{
			if(tok[i] == NULL)
			{
				//only vec2?
				vec.y = atof(tok);
				//LogManager::getSingleton().stream() << "WARNING:Failed to parse vector %s",tok);
				return;
			}
			i++;
		}
		tok[i] = NULL;
		vec.y = atof(tok);
		tok = &tok[++i];
		vec.z = atof(tok);
	}

	// Replaces windows specific _splitpath - TODO need to be tested!!!
	void Misc::splitpath(char *name, char *drive, char *path, char *base, char *ext)
	{
		char *s, *p;

		p = name;
		s = strchr(p, ':');
		if ( s != NULL ) {
			if (drive) {
				*s = '\0';
				strcpy(drive, p);
				*s = ':';
			}
			p = s+1;
			if (!p)
				return;
		} else if (drive)
			*drive = '\0';

		s = strrchr(p, '\\');
		if ( s != NULL) {
			if (path) {
				char c;

				c = *(s+1);
				*(s+1) = '\0';
				strcpy(path, p);
				*(s+1) = c;
			}
			p = s+1;
			if (!p)
				return;
		} else if (path)
			*path = '\0';

		s = strchr(p, '.');
		if ( s != NULL) {
			if (base) {
				*s = '\0';
				strcpy(base, p);
				*s = '.';
			}
			p = s+1;
			if (!p)
				return;
		} else if (base)
			*base = '\0';

		if (ext)
			strcpy(ext, p);
	}

	std::string Misc::Demangle(const std::string &name)
	{


#ifdef WIN32
		//substr(6) is used for removing "class_" when using vs,
		//Test UnDecorateSymbolName api functions
		//char out[256];
		//int len=0;
		//int flags=0;
		//UnDecorateSymbolName(name.c_str(),out,len,flags);
		//std::string ret = out;
		//remove "class " from name
		if(std::string::npos == name.find("class"))
			return name;
		std::string ret = name.substr(6);
#else
		int status;
		std::string ret = abi::__cxa_demangle(name.c_str(), 0, 0, &status);
#endif
		//remove namespace
		size_t pos = ret.find("::");
		if(pos != -1)
		{
			ret =  ret.substr(pos+2);
		}
		return ret;
	}


	void Misc::GetFilesFromPath(std::vector<std::string> &files, const std::string &path, bool recursive, bool full_path)
	{
		boost::filesystem::path boost_path(path); 
		if( boost::filesystem::exists(boost_path))  
		{
			boost::filesystem::directory_iterator end ;    
			for( boost::filesystem::directory_iterator iter(boost_path) ; iter != end ; ++iter )      
			{
				if (boost::filesystem::is_directory( *iter )  && recursive)      
				{   
					GetFilesFromPath(files,iter->path().string(), recursive,full_path);
				}
				else
				{
					const std::string exstension = iter->path().extension().generic_string();
					std::string filename; 
					if(full_path)
						filename = iter->path().string();
					else
						filename = iter->path().filename().generic_string();
					files.push_back(filename);
				}
			}
		}
	}
}
