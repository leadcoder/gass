#ifndef GASS_RAW_FILE_H
#define GASS_RAW_FILE_H

#include "Core/Common.h"

namespace GASS
{
	class GASSCoreExport RawFile  
	{
	public:
		RawFile();
		virtual ~RawFile();
		bool Load(const std::string filename,unsigned int width, unsigned int height, int bpp);
		bool Load(const std::string filename,int bpp);
		int GetSize(const std::string filename, int bpp);
		unsigned char *m_Data;
		unsigned int m_Width;
		unsigned int m_Height;
		int m_BPP;
	};
}
#endif 
