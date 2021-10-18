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
		bool Load(const std::string &filename,unsigned int width, unsigned int height, int bpp);
		bool Load(const std::string &filename,int bpp);
		int GetSize(const std::string &filename, int bpp) const;
		unsigned char *m_Data{nullptr};
		unsigned int m_Width{0};
		unsigned int m_Height{0};
		int m_BPP{8};
	};
}
#endif 
