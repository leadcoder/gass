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
#include "Core/Utils/Misc.h"
//#include "FilePath.h"
#include "Core/Utils/Log.h"
#include <algorithm>
#include <string.h>

#ifndef WIN32
#include <cxxabi.h>
#endif
namespace GASS
{
Misc::Misc()
{

}

Misc::~Misc()
{

}

#define MAX_BYTES 100000

std::string Misc::LoadTextFile(const std::string &file_name)
{
	FILE* fp;
	std::string text = "";
	fp = fopen(file_name.c_str(),"r");
	if(fp == NULL) return text;

	int num_bytes = 0;
	bool end_of_file = false;
	char buffer[MAX_BYTES];
	while(!feof(fp))
	{
		if(num_bytes >= MAX_BYTES) Log::Error("Failed to load textfile:%s",file_name.c_str());
		fread(&buffer[num_bytes++] ,sizeof(char),1,fp);
	}
	buffer[num_bytes-1] = NULL;

	fclose(fp);

	text = buffer;
	return text;
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
	int pos = 0;
	pos = file_name.find_last_of(".");
	ret = file_name.substr(0,pos);
	return ret;
}

std::string Misc::GetExtension(const std::string &file_name)
{
	std::string ret, reversed_string;
	int pos = file_name.find_last_of(".");

	if (pos == file_name.npos)
		return "";

	ret = file_name.substr(pos+1);

	return ret;
}

std::string Misc::GetFilename(const std::string &path)
{
	std::string ret = path;
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

std::string Misc::RemoveFilename(const std::string &path)
{
	std::string ret = path;
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

	int pos = name.find(".",0);

	if(pos != -1)
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
	int pos = 0;
	int look_here = 0;
	std::string new_str = str;

	//if(find.find(replacement)) // what we are going to replace already exist replecement string -> infinite while
	while ((pos = new_str.find(find,look_here)) != -1)
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
			Log::Warning("Failed to parse vector %s",tok);
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
			//Log::Warning("Failed to parse vector %s",tok);
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
        //TODO: use UnDecorateSymbolName api functions instead?
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



}
