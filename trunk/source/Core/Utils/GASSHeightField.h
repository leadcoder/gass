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
		void Allocate(int size) {delete[] Data; Data  = new unsigned short[size];}
		float ReadValue(int elem ) const {return MinValue + ((float)Data[elem]/65536.0f)*(MaxValue - MinValue);} 
		void WriteValue(int elem ,float value) {Data[elem] = (unsigned short) ((value - MinValue)/(MaxValue - MinValue)*65536.0f);}
	};

	#define MIN_HEIGHT -1000.0f
	#define MAX_HEIGHT 6000.0f

	class GASSCoreExport HeightField
	{
	public:
		HeightField() : m_NumSamplesH(0),
			m_NumSamplesW(0)
		{
			m_Data.MaxValue = MAX_HEIGHT;
			m_Data.MinValue = MIN_HEIGHT;
		}

		HeightField(const Vec3 &min_bound,const Vec3 &max_bound, unsigned int width, unsigned int height) : m_Min(min_bound),
			m_Max(max_bound),
			m_NumSamplesW(width),
			m_NumSamplesH(height)
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
		float GetHeightAtSample(unsigned int x, unsigned int z) const
		{
			return m_Data.ReadValue(x + z * m_NumSamplesW);
		}

		/**
			Set height at absolute location, coordinate values are clamped
		*/
		void SetHeightAtSample(unsigned int index, float height)
		{
			m_Data.WriteValue(index,height);
		}

		/**
			Set height pixel index, no bound check!
		*/
		void SetHeightAtSample(unsigned int x, unsigned int z, float height)
		{
			m_Data.WriteValue(x + z * m_NumSamplesW, height);
		}

		/**
			Set height at world location
		*/
		void SetHeightAtSample(Float x, Float z, float height)
		{
			Float bounds_width = m_Max.x - m_Min.x;
			Float bounds_height = m_Max.z - m_Min.z;

			unsigned int xindex = m_NumSamplesW * (x - m_Min.x) / bounds_width;
			unsigned int zindex = m_NumSamplesH * (z - m_Min.z) / bounds_height;
			if (xindex < 0 || zindex < 0 || xindex >= m_NumSamplesW || zindex >= m_NumSamplesH)
				return;
			m_Data.WriteValue(height, xindex + zindex*m_NumSamplesW);
		}

		/**
			Get height at array index
		*/
		float GetHeightAtSample(unsigned int index) const
		{
			return m_Data.ReadValue(index);
		}

		/**
			Get bounding box for entire heightfield
		*/
		AABox GetBoundingBox() const
		{
			return AABox(m_Min, m_Max);
		}

		unsigned int GetNumSamplesH() const {return m_NumSamplesH;}
		unsigned int GetNumSamplesW() const {return m_NumSamplesW;}

		void Save(const std::string &filename) const;
		void Save(std::ofstream& ofs) const;
		void Load(const std::string &filename);
		void Load(std::ifstream &fin);

		/**
		Simple LOS check, just step along ray with image spacing distances and check if point is above ground
		*/
		bool CheckLineOfSight(const Vec3& p1, const Vec3& p2, Vec3 &isec_pos);

		/**
			Get raw data
		*/
		FloatArray16& GetFloatArray() {return m_Data;} 

		/**
			Get const raw data
		*/
		const FloatArray16& GetFloatArray() const {return m_Data;} 
	private:
		void SetDimensions(const Vec3 &min,const Vec3 &max)
		{
			m_Max = max;
			m_Min = min;
		}
		FloatArray16 m_Data;
		Vec3 m_Min;
		Vec3 m_Max;
		unsigned int m_NumSamplesH;
		unsigned int m_NumSamplesW;
	};
	typedef boost::shared_ptr<HeightField> HeightFieldPtr;
}
#endif 
