#ifndef GASS_HEIGHTMAP_H
#define GASS_HEIGHTMAP_H

#include "Core/Common.h"
#include "Core/Math/GASSVector.h"
#include "Core/Math/GASSAABox.h"

#define USE_16BIT_DATA
#ifdef USE_16BIT_DATA //test to use 16-bit data
	#define MIN_HEIGHT -1000.0f
	#define MAX_HEIGHT 6000.0f
	#define MAX_USHORT 65536.0f
	typedef unsigned short HeightType;
	#define READ_HEIGHT(elem) MIN_HEIGHT + ((float)m_Data[elem]/MAX_USHORT) * (MAX_HEIGHT - MIN_HEIGHT);
	#define WRITE_HEIGHT(value, elem) m_Data[elem] = (HeightType) ((value - MIN_HEIGHT)/(MAX_HEIGHT - MIN_HEIGHT)*MAX_USHORT);
#else
	typedef float HeightType;
	#define READ_HEIGHT(elem) m_Data[elem];
	#define WRITE_HEIGHT(value, elem) m_Data[elem]= value;
#endif

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
		float ReadValue(int elem ) {return MinValue + ((float)Data[elem]/65536.0f)*(MaxValue - MinValue);}
		void WriteValue(int elem ,float value) {Data[elem] = (unsigned short) ((value - MinValue)/(MaxValue - MinValue)*65536.0f);}
	};
	
	struct FloatArray32
	{
		FloatArray32() : Data(NULL){}
		~FloatArray32() {delete[] Data;}
		void Allocate(int size) {delete[] Data; Data  = new float(size);}
		float* Data;
		float MinValue;
		float MaxValue;
		float ReadValue(int elem ) {return Data[elem];}
		void  WriteValue(int elem ,float value) {Data[elem] = value;}
	};

	//template<class HM_DATA_TYPE>
	class GASSCoreExport Heightmap
	{
	public:
		Heightmap();
		Heightmap(const Vec3 &min_bound,const Vec3 &max_bound,unsigned int width,unsigned  int height,HeightType *data =NULL);
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
			Get height at "pixel" index
		*/
		float GetHeight(unsigned int index) const;

		/**
			Set height at absolute location, coordinate values are clamped
		*/
		void SetHeight(Float x, Float z, float height);

		/**
			Set height pixel location, no bound check!
		*/
		void SetHeight(unsigned int x, unsigned int z, float height);

		/**
			Set height pixel index, no bound check!
		*/
		void SetHeight(unsigned int index, float height);

		void ImportRAWFile(const std::string &filename, float max_height, float min_height);
		void Save(const std::string &filename) const;
		void Load(const std::string &filename);
		unsigned int GetHeight() const {return m_Height;}
		unsigned int GetWidth() const {return m_Width;}
		//HeightType* GetData() const {return m_Data;}
		AABox GetBoundingBox() const;
		bool CheckLineOfSight(const Vec3& p1, const Vec3& p2, Vec3 &isec_pos);
	private:
		void SetDimensions(const Vec3 &min,const Vec3 &max);
		HeightType* m_Data;
		Vec3 m_Min;
		Vec3 m_Max;
		unsigned int m_Height;
		unsigned int m_Width;
	};
	typedef boost::shared_ptr<Heightmap> HeightmapPtr;
}
#endif 
