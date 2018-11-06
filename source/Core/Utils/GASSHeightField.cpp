#include "GASSHeightField.h"
#include "GASSRawFile.h"
#include <fstream>


namespace GASS
{
	HeightField::HeightField(const Vec3 &min_bound,const Vec3 &max_bound, unsigned int width_samples, unsigned int height_samples) : m_Min(min_bound),
		m_Max(max_bound),
		m_NumSamplesW(width_samples),
		m_NumSamplesH(height_samples)
	{
		//maybe add some padding to min max to support new runtime height values?
		m_Data = FloatArray16(static_cast<float>(min_bound.y), static_cast<float>(max_bound.y));
		m_Data.Allocate(width_samples*height_samples);
	}

	#ifndef HM_LERP
		#define HM_LERP(a, b, t) (a + (b - a) * t)
	#endif

	float HeightField::GetInterpolatedHeight(Float x, Float z) const
	{
		const Float bounds_width = m_Max.x - m_Min.x;
		const Float bounds_height = m_Max.z - m_Min.z;

		const Float fxindex = m_NumSamplesW * (x - m_Min.x) / bounds_width;
		const Float fzindex = m_NumSamplesH * (z - m_Min.z) / bounds_height;
		//round?
		const auto xindex = static_cast<unsigned int>(fxindex);
		const auto zindex = static_cast<unsigned int>(fzindex);

		if (xindex >= m_NumSamplesW || zindex >= m_NumSamplesH)
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

		if( x1 >= m_NumSamplesW || z1 >= m_NumSamplesH )
			return 0.0;

		float h00, h01, h10, h11;
		h00 = m_Data.ReadValue(x0 + z0*m_NumSamplesW);
		h01 = m_Data.ReadValue(x1 + z0*m_NumSamplesW);
		h10 = m_Data.ReadValue(x0 + z1*m_NumSamplesW);
		h11 = m_Data.ReadValue(x1 + z1*m_NumSamplesW);

		Float tx, ty;
		tx = fxindex - x0;
		ty = fzindex - z0;
		const Float height = HM_LERP(HM_LERP(h00, h01, tx), HM_LERP(h10, h11, tx), ty);
		return static_cast<float>(height);
	}

	void HeightField::Save(const std::string &filename) const
	{
		std::ofstream ofs(filename.c_str(), std::ios::binary);
		Save(ofs);
		ofs.close();
	}

	void HeightField::Save(std::ofstream& ofs) const
	{
		ofs.write((char *) &m_Min, sizeof(Vec3));
		ofs.write((char *) &m_Max, sizeof(Vec3));
		ofs.write((char *) &m_NumSamplesW, sizeof(int));
		ofs.write((char *) &m_NumSamplesH, sizeof(int));

		const float min_range = m_Data.GetMinRange();
		const float max_range = m_Data.GetMaxRange();
		ofs.write((char *) &min_range, sizeof(float));
		ofs.write((char *) &max_range, sizeof(float));
		ofs.write((char *) &m_Data.Data[0], sizeof(unsigned short)*m_NumSamplesH*m_NumSamplesW);
	}

	void HeightField::Load(const std::string &filename)
	{
		m_NumSamplesW = 0;
		m_NumSamplesH = 0;
		std::ifstream fin(filename.c_str(), std::ios::binary);
		Load(fin);
		fin.close();
	}

	void HeightField::Load(std::ifstream &fin)
	{
		fin.read((char *) &m_Min, sizeof(Vec3));
		fin.read((char *) &m_Max, sizeof(Vec3));
		fin.read((char *) &m_NumSamplesW, sizeof(int));
		fin.read((char *) &m_NumSamplesH, sizeof(int));

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

		float min_range;
		float max_range;

		fin.read((char *) &min_range, sizeof(float));
		fin.read((char *) &max_range, sizeof(float));
		m_Data = FloatArray16(min_range,max_range);
		m_Data.Allocate(m_NumSamplesW * m_NumSamplesH);
		fin.read((char *) &(m_Data.Data[0]), sizeof(unsigned short)*m_NumSamplesW*m_NumSamplesH);
	}

	bool HeightField::CheckLineOfSight(const Vec3& p1, const Vec3& p2, Vec3 &isec_pos) const
	{
		const Vec3 ray = p2 - p1;
		Vec3 ray_2d = ray;
		ray_2d.y = 0;

		const double length_2d = ray_2d.Length();

		//get pixel spacing, assume square pixels
		double px_spacing = GetBoundingBox().GetSize().x/static_cast<double>(GetNumSamplesW());

		double stepsize = 1.0;

		if(length_2d < px_spacing) //check if we are ray cast in same pixel
		{
			//HACK; lock step size to 0.25 percent of image spacing
			stepsize = px_spacing*0.25;
		}
		else //use step sized based on px_spacing and ray length,
			stepsize = px_spacing/ length_2d;

		double s = 0.0;

		while( s < 1.0 )
		{
			Vec3 p = p1 + ray*s;
			const float h = GetInterpolatedHeight(p.x, p.z);

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
}
