#ifndef GASS_HEIGHTMAP_H
#define GASS_HEIGHTMAP_H

#include "Core/Common.h"
#include "Core/Math/GASSVector.h"
#include "Core/Math/GASSAABox.h"
#ifdef USE_16BIT_HM
	#define MIN_HEIGHT -1000.0f
	#define MAX_HEIGHT 8000.0f
	typedef unsigned short HeightType;
	#define READ_HEIGHT(elem) MIN_HEIGHT + ((float)m_Data[elem]/65536.0f)*(MAX_HEIGHT - MIN_HEIGHT);
	#define WRITE_HEIGHT(value, elem) m_Data[elem] = (HeightType) ((value - MIN_HEIGHT)/(MAX_HEIGHT - MIN_HEIGHT)*65536.0f);
#else
	typedef float HeightType;
	#define READ_HEIGHT(elem) m_Data[elem];
	#define WRITE_HEIGHT(value, elem) m_Data[elem]= value;
#endif
namespace GASS
{
	class GASSCoreExport Heightmap  
	{
	public:
		Heightmap();
		Heightmap(const Vec3 &min_bound,const Vec3 &max_bound,unsigned int width,unsigned  int height, HeightType *data = NULL);
		virtual ~Heightmap();
		/**
			Get interpolated height at absolute location
		*/
		float GetInterpolatedHeight(Float x, Float z) const;

		/**
			Get height at "pixel"
		*/
		float GetHeight(unsigned int x, unsigned int z) const;
		/**
			Set height at absolute location, coordinate values are clamped
		*/
		void SetHeight(Float x, Float z, float height);
		void SetHeight(unsigned int x, unsigned int z, float height);
		void ImportRAWFile(const std::string &filename, float max_height, float min_height);
		void Save(const std::string &filename) const;
		void Load(const std::string &filename);
		unsigned int GetHeight() const {return m_Height;}
		unsigned int GetWidth() const {return m_Width;}
		HeightType* GetData() const {return m_Data;}
		AABox GetBoundingBox() const;
		bool CheckLineOfSight(const Vec3& p1, const Vec3& p2);
	private:
		void SetDimensions(const Vec3 &min,const Vec3 &max);
		HeightType *m_Data;
		Vec3 m_Min;
		Vec3 m_Max;
		unsigned int m_Height;
		unsigned int m_Width;
	};
}
#endif 
