#include "GASSHeightmap.h"
#include "GASSRawFile.h"
#include <math.h>
#include <fstream>


namespace GASS
{
	
	Heightmap::Heightmap() : m_Data(NULL),
		m_Height(0),
		m_Width(0)
	{

	}

	Heightmap::Heightmap(const Vec3 &min_bound,const Vec3 &max_bound, unsigned int width, unsigned int height, HeightType *data) : m_Data(NULL),
		m_Min(min_bound),
		m_Max(max_bound),
		m_Width(width),
		m_Height(height)
	{
		if(data == NULL) // allocate?
			m_Data = new HeightType[width*height];
		else
			m_Data = data;
	}

	Heightmap::~Heightmap()
	{
		delete[] m_Data;
	}

	void Heightmap::SetDimensions(const Vec3 &min,const Vec3 &max)
	{
		m_Max = max;
		m_Min = min;
	}

	#ifndef HM_LERP
		#define HM_LERP(a, b, t) (a + (b - a) * t)
	#endif

	float Heightmap::GetInterpolatedHeight(Float x, Float z) const
	{
		Float bounds_width = m_Max.x - m_Min.x;
		Float bounds_height = m_Max.z - m_Min.z;

		Float fxindex = m_Width * (x - m_Min.x) / bounds_width;
		Float fzindex = m_Height * (z - m_Min.z) / bounds_height;
		//round?
		unsigned int xindex = fxindex;
		unsigned int zindex = fzindex;

		if (xindex < 0 || zindex < 0 || xindex >= m_Width || zindex >= m_Height)
			return 0.0f;

		//algo:
		//Find the 4 corner points of the quad that the cam is directly above
		//Find the cam's distance from these 4 points
		//Lerp or Bilerp the height value at camera point from above values

		//the x and y coords are all either int(x) or int(x)+1
		//use all 4 combos
		//x0 <= x <= x1
		//z0 <= z <= z1
		unsigned int x0, z0, x1, z1;
		x0 = xindex;
		x1 = xindex + 1;
		z0 = zindex;
		z1 = z0 + 1;

		//we are just slightly off the edge of the terrain if x1 or z1 is off the terrain
		if( x0 < 0.0 || z0 < 0.0 )
			return 0.0;

		if( x1 >= m_Width || z1 >= m_Height )
			return 0.0;

		float h00, h01, h10, h11;
		h00 = READ_HEIGHT(x0 + z0*m_Width);
		h01 = READ_HEIGHT(x1 + z0*m_Width);
		h10 = READ_HEIGHT(x0 + z1*m_Width);
		h11 = READ_HEIGHT(x1 + z1*m_Width);

		float tx, ty;
		tx = fxindex - x0;
		ty = fzindex - z0;
		float height = HM_LERP(HM_LERP(h00, h01, tx), HM_LERP(h10, h11, tx), ty);
		return height;
	}

	void Heightmap::SetHeight(Float x, Float z, float height)
	{
		Float bounds_width = m_Max.x - m_Min.x;
		Float bounds_height = m_Max.z - m_Min.z;

		unsigned int xindex = m_Width * (x - m_Min.x) / bounds_width;
		unsigned int zindex = m_Height * (z - m_Min.z) / bounds_height;
		if (xindex < 0 || zindex < 0 || xindex >= m_Width || zindex >= m_Height)
			return;
		WRITE_HEIGHT(height, xindex + zindex*m_Width);
	}

	void Heightmap::SetHeight(unsigned int x, unsigned int z, float height)
	{
		WRITE_HEIGHT(height, x + z * m_Width);
	}

	void Heightmap::SetHeight(unsigned int index, float height)
	{
		WRITE_HEIGHT(height,index);
	}

	float Heightmap::GetHeight(unsigned int index) const
	{
		return READ_HEIGHT(index);
	}

	float Heightmap::GetHeight(unsigned int x, unsigned int z) const
	{
		return READ_HEIGHT(x + z * m_Width);
	}

	void Heightmap::ImportRAWFile(const std::string &filename, float max_height, float min_height)
	{
		RawFile file;
		if(!file.Load(filename.c_str(), 16))
			GASS_EXCEPT(Exception::ERR_INVALIDPARAMS, "Failed to load heightmap:" + filename,"Heightmap::ImportRAWFile");
		m_Width = file.m_Width;
		m_Height = file.m_Height;
		delete[] m_Data;
		m_Data = new HeightType[file.m_Height*file.m_Width];

		for(unsigned int i = 0 ; i < file.m_Height; i++)
		{
			for(unsigned int j = 0 ; j < file.m_Width; j++)
			{
				unsigned int hm_index = (i * (file.m_Width) + j)*file.m_BPP/8;
				float height;
				if(file.m_BPP == 16)
				{
					unsigned short hm_value_16bit = *(unsigned short*) (&file.m_Data[hm_index]);
					height = (float)hm_value_16bit/65536.0;
				}
				else
				{
					unsigned char hm_value_8bit = *(unsigned char*) (&file.m_Data[hm_index]);
					height = (float)hm_value_8bit/256.0;
				}
				float h = min_height + height*(max_height - min_height);
				WRITE_HEIGHT(h, i*file.m_Width+j)
			}
		}
	}

	bool Heightmap::CheckLineOfSight(const Vec3& p1, const Vec3& p2, Vec3 &isec_pos)
	{   
		Vec3 ray = p2 - p1;
		Vec3 ray_2d = ray;
		ray_2d.y = 0;

		double length = ray.Length();
		double length_2d = ray_2d.Length();
		
		//Use pixel spacing for LOS step, assume square pixels
		double los_spacing = GetBoundingBox().GetSize().x/((double) GetWidth()); 

		double stepsize = 1.0;

		if(length_2d < los_spacing) //check if we are ray cast in same tile
		{
			stepsize = 0.25;
		}
		else
			stepsize = los_spacing / length_2d;
		
		double s = 0.0;

		while( s < 1.0 )
		{
			Vec3 p = p1 + ray*s;
			float h = GetInterpolatedHeight(p.x, p.z);
			
			if(h >= p.y)
			{
				isec_pos = p;
				isec_pos.y = h;
				return false;
			}
			s += stepsize;
		}
		return true;
	}

	void Heightmap::Save(const std::string &filename) const
	{
		std::ofstream ofs(filename.c_str(), std::ios::binary);
		ofs.write((char *) &m_Min, sizeof(Vec3));
		ofs.write((char *) &m_Max, sizeof(Vec3));
		ofs.write((char *) &m_Width, sizeof(int));
		ofs.write((char *) &m_Height, sizeof(int));
		ofs.write((char *) &m_Data[0], sizeof(HeightType)*m_Height*m_Width);
		ofs.close();
	}

	void Heightmap::Load(const std::string &filename)
	{
		delete[] m_Data;
		m_Width = 0;
		m_Height = 0;
		std::ifstream fin(filename.c_str(), std::ios::binary);
		fin.read((char *) &m_Min, sizeof(Vec3));
		fin.read((char *) &m_Max, sizeof(Vec3));
		fin.read((char *) &m_Width, sizeof(int));
		fin.read((char *) &m_Height, sizeof(int));

		//convert from float to 16bit
		/*
		m_Data = new HeightType[m_Width*m_Height];
		float* data = new float[m_Width*m_Height];
		fin.read((char *) &data[0], sizeof(float)*m_Width*m_Height);
		for(unsigned int i = 0; i < m_Width*m_Height; i++)
		{
			WRITE_HEIGHT(data[i],i);
		}
		delete[] data;*/

		m_Data = new HeightType[m_Width*m_Height];
		fin.read((char *) &m_Data[0], sizeof(HeightType)*m_Width*m_Height);
		fin.close();
	}

	AABox Heightmap::GetBoundingBox() const
	{
		return AABox(m_Min, m_Max);
	}

}
