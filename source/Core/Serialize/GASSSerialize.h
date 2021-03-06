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

#ifndef GASS_SERIALIZE_H
#define GASS_SERIALIZE_H

#include "Core/Math/GASSVector.h"
#include "Core/Utils/GASSFilePath.h"

// This is a modified version of serialization code from
// the Enginuity article series (gamedev.net), Part V

namespace GASS
{
	/** \addtogroup GASSCore
	*  @{
	*/
	/** \addtogroup Serialize
	*  @{
	*/

	/**
		Common Interface for all binary serialization operations
	*/

	class GASSCoreExport ISerializer
	{
		GASS_DECLARE_CLASS_AS_INTERFACE(ISerializer)
	public:
		virtual bool Loading()=0;
	};

	/**
		Class used to save binaray data
	*/

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
			buffer=nullptr; length=0; bytesUsed=0; bHasOverflowed=false;
		}

		SerialSaver(unsigned char *buf, unsigned long size)
		{
			buffer=buf; length=size; bytesUsed=0; bHasOverflowed=false;
		}

		template <class T>
		void IO(const T &value)
		{
			if(buffer)
			{
				if(bHasOverflowed)return; //stop writing when overflowed
				int type_size = sizeof(T);
				if(bytesUsed+type_size > length){bHasOverflowed=true; return; }
				*(T*) buffer = value;
				buffer+=type_size; bytesUsed+=type_size;
			}
			else //get size
			{
				const int type_size = sizeof(T);
				length+=type_size;
			}
		}




		bool Loading() override{return false;}

		bool hasOverflowed() const { return bHasOverflowed; }
		long getFlow() const { return length-bytesUsed; } //should be equal to 0 when we're done
		unsigned long getLength() const { return length; }
	};

    //Use specialized template to catch std::string
    template <>
    GASSCoreExport void SerialSaver::IO<std::string>(const std::string &value);
  
    template <>
    GASSCoreExport void SerialSaver::IO<FilePath>(const FilePath &path);



	/**
		Class used to load binaray data
	*/

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



		bool Loading() override{return true;}

		bool hasOverflowed() const { return bHasOverflowed; }
		long getFlow() const { return length-bytesUsed; } //should be equal to 0 when we're done

	};

	//Use specialized template to catch std::string
    template <>
    GASSCoreExport void SerialLoader::IO<std::string>(std::string &value);
    
    template <>
    GASSCoreExport void SerialLoader::IO<FilePath>(FilePath &path);
}

#endif // #ifndef SERIALIZE_HH
