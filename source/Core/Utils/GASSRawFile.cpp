#include "GASSRawFile.h"
#include <math.h>
#include <cstdio>
// disable: "conversion from 'double' to 'int', possible loss of data

#ifdef _MSC_VER
    #pragma warning(push)
    #pragma warning (disable : 4244)
#endif


namespace GASS
{
	RawFile::RawFile() : m_Data(NULL),
		m_Height(0),
		m_Width(0),
		m_BPP(8)
	{

	}

	RawFile::~RawFile()
	{
		delete m_Data;
	}

	bool RawFile::Load(const std::string filename,int bpp)
	{
		m_Height = GetSize(filename, bpp);
		m_Width = m_Height;
		return Load(filename,m_Width, m_Height, bpp);
	}

	int RawFile::GetSize(const std::string filename, int bpp) const
	{
		FILE *fp=fopen(filename.c_str(),"rb");
		if(!fp)return false;

		fseek(fp,0,SEEK_END);
		unsigned long file_size=ftell(fp);
		fseek(fp,0,SEEK_SET);

		//only support square images
		fclose(fp);
		file_size = file_size / static_cast<unsigned long>((bpp / 8));
		int im_size = sqrt(static_cast<double>(file_size));
		return im_size;
	}

	bool RawFile::Load(const std::string filename,unsigned int width, unsigned int height, int bpp)
	{
		FILE *file = fopen(filename.c_str(), "rb");
		if(	!file)
		{
			return false;
		}

		m_Width  = width;
		m_Height = height;
		m_BPP = bpp;

		if(	m_Width	<=0	||
			m_Height	<=0)
		{
			fclose(file);
			return false;
		}

		unsigned int image_size = m_Width*m_Height*m_BPP/8;
		m_Data = new unsigned char[image_size];

		if(m_Data== NULL || fread(m_Data, 1, image_size, file) != image_size)
		{
			if(m_Data!=NULL)
				free(m_Data);

			fclose(file);
			return false;
		}
		fclose (file);
		return true;
	}
}
#ifdef _MSC_VER
    #pragma warning(pop)
#endif
