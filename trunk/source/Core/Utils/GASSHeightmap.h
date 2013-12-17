#ifndef GASS_HEIGHTMAP_H
#define GASS_HEIGHTMAP_H

#include "Core/Common.h"
#include "Core/Math/GASSVector.h"

namespace GASS
{
	class GASSCoreExport Heightmap  
	{
	public:
		Heightmap();
		Heightmap(const Vec3 &min_bound,const Vec3 &max_bound,int width, int height, float *data = NULL);
		virtual ~Heightmap();
		/**
			Get interpolated height at absolute location
		*/
		float GetInterpolatedHeight(Float x, Float z) const;
		/**
			Set height at absolute location, coordinate values are clamped
		*/
		void SetHeight(Float x, Float z, float height);
		void SetHeight(int x, int z, float height);
		void ImportRAWFile(const std::string &filename, float max_height, float min_height);
		void Save(const std::string &filename) const;
		void Load(const std::string &filename);
	private:
		void SetDimensions(const Vec3 &min,const Vec3 &max);
		float *m_Data;
		Vec3 m_Min;
		Vec3 m_Max;
		int m_Height;
		int m_Width;
	};
}
#endif 
