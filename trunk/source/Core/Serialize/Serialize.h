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
		virtual void IO(unsigned char &value)=0;
		virtual void IO(unsigned long &value)=0;
		virtual void IO(long &value)=0;
		virtual void IO(int &value)=0;
		virtual void IO(unsigned int &value)=0;
		virtual void IO(float &value)=0;
		virtual void IO(double &value)=0;
		virtual void IO(bool &value)=0;
		virtual void IO(Vec2 &value)=0;
		virtual void IO(Vec3 &value)=0;
		virtual void IO(Vec4 &value)=0;
		virtual void IO(Mat4 &value)=0;
		virtual void IO(std::string &str)=0;
		virtual void IO(std::vector<float> &value) = 0;
		virtual void IO(std::vector<double> &value) = 0;
		virtual void IO(std::vector<int> &value)  =0;
		virtual void IO(std::vector<std::string> &value) = 0;
		virtual void IO(FilePath &value) = 0;
		
		virtual bool Loading()=0;
	};

	class GASSCoreExport SerialSaver : virtual public ISerializer
	{

	protected:
		unsigned char *buffer;
		bool bHasOverflowed;
		unsigned long length;
		unsigned long bytesUsed;
	public:
		SerialSaver(unsigned char *buf, unsigned long size)
		{
			buffer=buf; length=size; bytesUsed=0; bHasOverflowed=false;
		}

		void IO(unsigned char &value)
		{
			if(bHasOverflowed)return; //stop writing when overflowed
			if(bytesUsed+1>length){bHasOverflowed=true; return; }
			*buffer=value;
			++buffer; ++bytesUsed;
		}

		void IO(bool &value)
		{
			if(bHasOverflowed)return; //stop writing when overflowed
			if(bytesUsed+1>length){bHasOverflowed=true; return; }
			*buffer=value;
			++buffer; ++bytesUsed;
		}
		void IO(unsigned long &value)
		{
			if(bHasOverflowed)return; //stop writing when overflowed
			if(bytesUsed+4>length){bHasOverflowed=true; return; }
			//SDLNet_Write32(value,buffer);
			*(unsigned long*) buffer = value;
			buffer+=4; bytesUsed+=4;
		}
		void IO(long &value)
		{
			if(bHasOverflowed)return; //stop writing when overflowed
			if(bytesUsed+4>length){bHasOverflowed=true; return; }

			*(long*) buffer = value;

			//SDLNet_Write32(value,buffer);
			buffer+=4; bytesUsed+=4;
		}
		void IO(int &value)
		{
			if(bHasOverflowed)return; //stop writing when overflowed
			if(bytesUsed+4>length){bHasOverflowed=true; return; }

			*(int*) buffer = value;

			//SDLNet_Write32(value,buffer);
			buffer+=4; bytesUsed+=4;
		}
		void IO(unsigned int &value)
		{
			if(bHasOverflowed)return; //stop writing when overflowed
			if(bytesUsed+4>length){bHasOverflowed=true; return; }

			*(int*) buffer = value;

			//SDLNet_Write32(value,buffer);
			buffer+=4; bytesUsed+=4;
		}
		void IO(float &value)
		{
			if(bHasOverflowed)return; //stop writing when overflowed
			if(bytesUsed+4>length){bHasOverflowed=true; return; }

			*(float*) buffer = value;

			//SDLNet_Write32(value,buffer);
			buffer+=4; bytesUsed+=4;
		}

		void IO(double &value)
		{
			if(bHasOverflowed)return; //stop writing when overflowed
			if(bytesUsed+8>length){bHasOverflowed=true; return; }

			*(double*) buffer = value;

			//SDLNet_Write32(value,buffer);
			buffer+=8; bytesUsed+=8;
		}

		void IO(Vec2 &value)
		{
			if(bHasOverflowed)return; //stop writing when overflowed
			IO(value.x);
			IO(value.y);
		}

		void IO(Vec3 &value)
		{
			if(bHasOverflowed)return; //stop writing when overflowed
			IO(value.x);
			IO(value.y);
			IO(value.z);
		}

		void IO(Vec4 &value)
		{
			if(bHasOverflowed)return; //stop writing when overflowed
			IO(value.x);
			IO(value.y);
			IO(value.z);
			IO(value.w);
		}

		void IO(Mat4 &value)
		{
			if(bHasOverflowed)return; //stop writing when overflowed
			int tot_size = 16*4;

			if(bytesUsed+tot_size>length){bHasOverflowed=true; return; }
			memcpy(buffer,value.m_Data,tot_size);
			buffer+=tot_size; bytesUsed+=tot_size;
		}

		void IO(std::string &str)
		{
			unsigned long l = (unsigned long) str.length();
			IO(l);
			if(bHasOverflowed)return;
			if(bytesUsed+l>length){bHasOverflowed=true; return; }
			memcpy(buffer,str.c_str(),l);
			buffer+=l; bytesUsed+=l;
		}

		void IO(std::vector<double> &value)
		{
			if(bHasOverflowed)return; //stop writing when overflowed
			for(int i = 0 ; i < value.size(); i++)
			{
				IO(value[i]);
			}
		}

		void IO(std::vector<float> &value)
		{
			if(bHasOverflowed)return; //stop writing when overflowed
			for(int i = 0 ; i < value.size(); i++)
			{
				IO(value[i]);
			}
		}

		void IO(std::vector<int> &value)
		{
			if(bHasOverflowed)return; //stop writing when overflowed
			for(int i = 0 ; i < value.size(); i++)
			{
				IO(value[i]);
			}
		}

		void IO(std::vector<std::string> &value)
		{
			if(bHasOverflowed)return; //stop writing when overflowed
			for(int i = 0 ; i < value.size(); i++)
			{
				IO(value[i]);
			}
		}

		void IO(FilePath &path)
		{
			IO(path.GetPath());
		}

		bool Loading(){return false;}

		bool hasOverflowed() { return bHasOverflowed; }
		long getFlow() { return length-bytesUsed; } //should be equal to 0 when we're done
	};

	class GASSCoreExport SerialLoader : virtual public ISerializer
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

		void IO(unsigned char &value)
		{
			if(bHasOverflowed)return; //stop writing when overflowed
			if(bytesUsed+1>length){bHasOverflowed=true; return; }
			value=*buffer;
			++buffer; ++bytesUsed;
		}

		void IO(bool &value)
		{
			if(bHasOverflowed)return; //stop writing when overflowed
			if(bytesUsed+1>length){bHasOverflowed=true; return; }
			value=*buffer;
			++buffer; ++bytesUsed;
		}

		void IO(unsigned long &value)
		{
			if(bHasOverflowed)return; //stop writing when overflowed
			if(bytesUsed+4 > length){bHasOverflowed=true; return; }
			value = *(unsigned long*) buffer;
			//value=SDLNet_Read32(buffer);
			buffer+=4; bytesUsed+=4;
		}
		void IO(long &value)
		{
			if(bHasOverflowed)return; //stop writing when overflowed
			if(bytesUsed+4>length){bHasOverflowed=true; return; }
			value = *(long*) buffer;
			//value=SDLNet_Read32(buffer);
			buffer+=4; bytesUsed+=4;
		}
		void IO(int &value)
		{
			if(bHasOverflowed)return; //stop writing when overflowed
			if(bytesUsed+4>length){bHasOverflowed=true; return; }
			value = *(int*) buffer;
			buffer+=4; bytesUsed+=4;
		}
		void IO(unsigned int &value)
		{
			if(bHasOverflowed)return; //stop writing when overflowed
			if(bytesUsed+4>length){bHasOverflowed=true; return; }
			value = *(unsigned int*) buffer;
			buffer+=4; bytesUsed+=4;
		}
		void IO(float &value)
		{
			if(bHasOverflowed)return; //stop writing when overflowed
			if(bytesUsed+4>length){bHasOverflowed=true; return; }
			value = *(float *) buffer;
			buffer+=4; bytesUsed+=4;
		}
		void IO(double &value)
		{
			if(bHasOverflowed)return; //stop writing when overflowed
			if(bytesUsed + 8> length ){bHasOverflowed=true; return; }
			value = *(double *) buffer;
			buffer+=8; bytesUsed+=8;
		}

		void IO(Vec2 &value)
		{
			if(bHasOverflowed)return; //stop writing when overflowed
			IO(value.x);
			IO(value.y);
		}

		void IO(Vec3 &value)
		{
			if(bHasOverflowed)return; //stop writing when overflowed
			IO(value.x);
			IO(value.y);
			IO(value.z);
		}

		void IO(Vec4 &value)
		{
			if(bHasOverflowed)return; //stop writing when overflowed
			IO(value.x);
			IO(value.y);
			IO(value.z);
			IO(value.w);
		}

		void IO(Mat4 &value)
		{
			if(bHasOverflowed)return; //stop writing when overflowed
			int tot_size = 16*4;
			if(bytesUsed+tot_size>length){bHasOverflowed=true; return; }
			memcpy(value.m_Data,buffer,tot_size);
			buffer+=tot_size; bytesUsed+=tot_size;
		}
		void IO(std::string &str)
		{
			unsigned long l;
			IO(l);
			if(bHasOverflowed)return;
			if(bytesUsed + l > length){bHasOverflowed=true; return; }
			char *szBuf=new char[l+1];
			szBuf[l]=0;
			memcpy(szBuf,buffer,l);
			str=szBuf;
			delete[] szBuf;
			buffer+=l; bytesUsed+=l;
		}

		void IO(std::vector<float> &value)
		{
			if(bHasOverflowed)return; //stop writing when overflowed
			for(int i = 0 ; i < value.size(); i++)
			{
				IO(value[i]);
			}
		}
		void IO(std::vector<double> &value)
		{
			if(bHasOverflowed)return; //stop writing when overflowed
			for(int i = 0 ; i < value.size(); i++)
			{
				IO(value[i]);
			}
		}

		void IO(std::vector<int> &value)
		{
			if(bHasOverflowed)return; //stop writing when overflowed
			for(int i = 0 ; i < value.size(); i++)
			{
				IO(value[i]);
			}
		}

		void IO(std::vector<std::string> &value)
		{
			if(bHasOverflowed)return; //stop writing when overflowed
			for(int i = 0 ; i < value.size(); i++)
			{
				IO(value[i]);
			}
		}

		void IO(FilePath &path)
		{
			IO(path.GetPath());
		}

		bool Loading(){return true;}

		bool hasOverflowed() { return bHasOverflowed; }
		long getFlow() { return length-bytesUsed; } //should be equal to 0 when we're done

	};

	class GASSCoreExport SerialSizer : virtual public ISerializer
	{

	protected:
		unsigned long length;
	public:
		SerialSizer() { length=0; }

		void IO(unsigned char &value)
		{ ++length; }

		void IO(bool &value)
		{ ++length; }

		void IO(unsigned long &value)
		{ length+=4; }

		void IO(long &value)
		{ length+=4; }

		void IO(int &value)
		{ length+=4; }

		void IO(unsigned int &value)
		{ length+=4; }

		void IO(float &value)
		{ length+=4; }

		void IO(double &value)
		{ length+=8; }

		void IO(Vec2 &value)
		{length+=4*2;}

		void IO(Vec3 &value)
		{length+=4*3;}

		void IO(Vec4 &value)
		{length+=4*4;}

		void IO(Mat4 &value)
		{length+=4*16;}

		void IO(std::string &str)
		{
			IO(length);
			length += (int) str.length();
		}


		void IO(std::vector<float> &value)
		{

			for(int i = 0 ; i < value.size(); i++)
			{
				IO(value[i]);
			}
		}
		void IO(std::vector<double> &value)
		{

			for(int i = 0 ; i < value.size(); i++)
			{
				IO(value[i]);
			}
		}

		void IO(std::vector<int> &value)
		{

			for(int i = 0 ; i < value.size(); i++)
			{
				IO(value[i]);
			}
		}

		void IO(std::vector<std::string> &value)
		{

			for(int i = 0 ; i < value.size(); i++)
			{
				IO(value[i]);
			}
		}

		void IO(FilePath &path)
		{
			IO(path.GetPath());
		}

		bool Loading(){return false;}

		unsigned long getLength() { return length; }
	};
}

#endif // #ifndef SERIALIZE_HH
