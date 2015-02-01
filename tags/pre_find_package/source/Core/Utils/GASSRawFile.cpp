#include "GASSRawFile.h"
#include <math.h>

namespace GASS
{
	RawFile::RawFile() : m_Data(NULL),
		m_Height(0),
		m_Width(0)
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

	int RawFile::GetSize(const std::string filename, int bpp)
	{
		FILE *fp=fopen(filename.c_str(),"rb");
		if(!fp)return false;

		fseek(fp,0,SEEK_END);
		unsigned long file_size=ftell(fp);
		fseek(fp,0,SEEK_SET);

		fclose(fp);
		
		file_size = sqrt((long double)file_size / (bpp/8));
		return file_size;
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
