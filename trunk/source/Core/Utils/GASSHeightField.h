#ifndef GASS_HEIGHTMAP_H
#define GASS_HEIGHTMAP_H

#include "Core/Common.h"
#include "Core/Math/GASSVector.h"
#include "Core/Math/GASSAABox.h"

namespace GASS
{
	struct FloatArray16
	{
		FloatArray16() : Data(NULL), MinValue(-1000), MaxValue(6000){}
		~FloatArray16() {delete[] Data;}
		unsigned short* Data;
		float MinValue;
		float MaxValue;
		void Allocate(int size) {delete[] Data; Data  = new unsigned short(size);}
		float ReadValue(int elem ) const {return MinValue + ((float)Data[elem]/65536.0f)*(MaxValue - MinValue);} 
		void WriteValue(int elem ,float value) {Data[elem] = (unsigned short) ((value - MinValue)/(MaxValue - MinValue)*65536.0f);}
	};
	#define MIN_HEIGHT -1000.0f
	#define MAX_HEIGHT 6000.0f

	class GASSCoreExport HeightField
	{
	public:
		HeightField() : m_Height(0),
			m_Width(0)
		{
			m_Data.MaxValue = MAX_HEIGHT;
			m_Data.MinValue = MIN_HEIGHT;
		}

		HeightField(const Vec3 &min_bound,const Vec3 &max_bound, unsigned int width, unsigned int height) : m_Min(min_bound),
			m_Max(max_bound),
			m_Width(width),
			m_Height(height)
		{
			m_Data.MaxValue = MAX_HEIGHT;
			m_Data.MinValue = MIN_HEIGHT;
			m_Data.Allocate(width*height);
		}
		virtual ~HeightField(){};

		/**
			Get interpolated height at absolute location
		*/
		float GetInterpolatedHeight(Float x, Float z) const;

		
		/**
			Get height at "pixel" index
		*/
		float GetHeight(unsigned int x, unsigned int z) const
		{
			return m_Data.ReadValue(x + z * m_Width);
		}

		/**
			Set height at absolute location, coordinate values are clamped
		*/
		void SetHeight(unsigned int index, float height)
		{
			m_Data.WriteValue(height,index);
		}

		/**
			Set height pixel index, no bound check!
		*/
		void SetHeight(unsigned int x, unsigned int z, float height)
		{
			m_Data.WriteValue(height, x + z * m_Width);
		}

		void SetHeight(Float x, Float z, float height)
		{
			Float bounds_width = m_Max.x - m_Min.x;
			Float bounds_height = m_Max.z - m_Min.z;

			unsigned int xindex = m_Width * (x - m_Min.x) / bounds_width;
			unsigned int zindex = m_Height * (z - m_Min.z) / bounds_height;
			if (xindex < 0 || zindex < 0 || xindex >= m_Width || zindex >= m_Height)
				return;
			m_Data.WriteValue(height, xindex + zindex*m_Width);
		}

		float GetHeight(unsigned int index) const
		{
			return m_Data.ReadValue(index);
		}

		AABox GetBoundingBox() const
		{
			return AABox(m_Min, m_Max);
		}

	private:
		void SetDimensions(const Vec3 &min,const Vec3 &max)
		{
			m_Max = max;
			m_Min = min;
		}
	
		FloatArray16 m_Data;
		Vec3 m_Min;
		Vec3 m_Max;
		unsigned int m_Height;
		unsigned int m_Width;
	};
	typedef boost::shared_ptr<HeightField> HeightFieldPtr;
}
#endif 
