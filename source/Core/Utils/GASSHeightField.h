#ifndef GASS_HEIGHTMAP_H
#define GASS_HEIGHTMAP_H

#include "Core/Common.h"
#include "Core/Math/GASSVector.h"
#include "Core/Math/GASSAABox.h"
#include "Core/Utils/GASSFloatArray16.h"

namespace GASS
{
	/**
		HeightField using 16-bit storage
	*/

	class GASSCoreExport HeightField
	{
	public:

		HeightField() = default;

		/**
			Constructor that allocate height data and define max/min values for the field.
			Note that min_bound.y should hold the lowest elevation value that can be store in the height field,
			and max_bound.y hold the highest elevation value to be stored.
			Note that 16-bit limitation have a max range of 65535.0f meters/units. Try to use as tight
			height range as possible to get best precision.
			If you don't know the range at allocation time you have to use some approximation because you can not change the
			range after construction. This is a performance reason, we don't want to do min/max check for each SetHeight call
		*/
		HeightField(const Vec3 &min_bound,const Vec3 &max_bound, unsigned int width_samples, unsigned int height_samples);
	
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
		void SetHeightAtLocation(Float x, Float z, float height)
		{
			const Float bounds_width = m_Max.x - m_Min.x;
			const Float bounds_height = m_Max.z - m_Min.z;
			const auto xindex = static_cast<unsigned int>(m_NumSamplesW * (x - m_Min.x) / bounds_width);
			const auto zindex = static_cast<unsigned int>(m_NumSamplesH * (z - m_Min.z) / bounds_height);
			if (xindex >= m_NumSamplesW || zindex >= m_NumSamplesH)
				return;
			m_Data.WriteValue(xindex + zindex*m_NumSamplesW, height);
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
		bool CheckLineOfSight(const Vec3& p1, const Vec3& p2, Vec3 &isec_pos) const;

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
		unsigned int m_NumSamplesW{ 0 };
		unsigned int m_NumSamplesH{0};
	};
	typedef GASS_SHARED_PTR<HeightField> HeightFieldPtr;
}
#endif
