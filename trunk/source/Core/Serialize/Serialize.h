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

#ifndef SERIALIZE_HH
#define SERIALIZE_HH

#include "Core/Math/Vector.h"
#include "Core/Math/Matrix.h"
#include "Core/Utils/FilePath.h"
#include <cstring>

// This is a modified version of serialization code from
// the Enginuity article series (gamedev.net), Part V

namespace GASS
{
	class GASSCoreExport ISerializer
	{
	public:
		virtual ~ISerializer(){}
		virtual bool Loading()=0;
	};

	class GASSCoreExport SerialSaver : public ISerializer
	{

	protected:
		unsigned char *buffer;
		bool bHasOverflowed;
		unsigned long length;
		unsigned long bytesUsed;
	public:
		SerialSaver()
		{
			buffer=NULL; length=0; bytesUsed=0; bHasOverflowed=false;
		}

		SerialSaver(unsigned char *buf, unsigned long size)
		{
			buffer=buf; length=size; bytesUsed=0; bHasOverflowed=false;
		}

		template <class T>
		void IO(T &value)
		{
			if(buffer)
			{
				if(bHasOverflowed)return; //stop writing when overflowed
				int type_size = sizeof(T);
				if(bytesUsed+type_size > length){bHasOverflowed=true; return; }
				//SDLNet_Write32(value,buffer);
				*(T*) buffer = value;
				buffer+=type_size; bytesUsed+=type_size;
			}
			else //get size
			{
				int type_size = sizeof(T);
				length+=type_size;
			}
		}




		bool Loading(){return false;}

		bool hasOverflowed() { return bHasOverflowed; }
		long getFlow() { return length-bytesUsed; } //should be equal to 0 when we're done
		unsigned long getLength() { return length; }
	};

    //Use specialized template to catch std::string
    template <>
    void SerialSaver::IO<std::string>(std::string &value);
   /* {
		if(buffer)
		{
			unsigned long l = (unsigned long) value.length();
			IO<unsigned long>(l);
			if(bHasOverflowed)return;
			if(bytesUsed+l>length){bHasOverflowed=true; return; }
			memcpy(buffer,value.c_str(),l);
			buffer+=l; bytesUsed+=l;
		}
		else
		{
			int type_size = sizeof(unsigned long);
			length +=type_size;
			length += (int) value.length();
		}
	}*/

    template <>
    void SerialSaver::IO<FilePath>(FilePath &path);


	class GASSCoreExport SerialLoader : public ISerializer
	{

	protected:
		unsigned char *buffer;
		bool bHasOverflowed;
		unsigned long length;
		unsigned long bytesUsed;
	public:


		SerialLoader(unsigned char *buf, unsigned long size)
		{
			buffer=buf; length=size; bytesUsed=0; bHasOverflowed=false;
		}


		template <class T>
		void IO(T &value)
		{
			if(bHasOverflowed)return; //stop writing when overflowed
			int type_size = sizeof(T);
			if(bytesUsed+type_size > length){bHasOverflowed=true; return; }
			value = *(T*) buffer;
			buffer+=type_size; bytesUsed+=type_size;
		}



		bool Loading(){return true;}

		bool hasOverflowed() { return bHasOverflowed; }
		long getFlow() { return length-bytesUsed; } //should be equal to 0 when we're done

	};

	//Use specialized template to catch std::string
    template <>
    void SerialLoader::IO<std::string>(std::string &value);
    /*{
        unsigned long l;
		IO<unsigned long>(l);
		if(bHasOverflowed)return;
		if(bytesUsed + l > length){bHasOverflowed=true; return; }
		char *szBuf=new char[l+1];
		szBuf[l]=0;
		memcpy(szBuf,buffer,l);
		value=szBuf;
		delete[] szBuf;
		buffer+=l; bytesUsed+=l;
    }*/

    template <>
    void SerialLoader::IO<FilePath>(FilePath &path);
}

#endif // #ifndef SERIALIZE_HH
