#ifndef GASS_FLOAT_ARRAY_16_H
#define GASS_FLOAT_ARRAY_16_H

#include "Core/Common.h"

namespace GASS
{
	struct FloatArray16
	{
	public:
		/**
			Constructor that take float range used by this array
		*/
		FloatArray16(float min_value = -1000,float max_value = 6000) : Data(NULL), RangeMin(min_value), RangeMax(max_value){}
		~FloatArray16() {delete[] Data;}

		/**
			Pointer to data (16-bit)
		*/
		unsigned short* Data;

		/**
			Allocate array

		*/
		void Allocate(int size) {delete[] Data; Data  = new unsigned short[size];}

		/**
			Read 16-bit value and return uncompressed float value
			Note: No allocation check present, performance reason
		*/
		float ReadValue(int elem ) const {return RangeMin + (static_cast<float>(Data[elem])/65535.0f)*(RangeMax - RangeMin);} 

		/**
			Write float value by converting to 16-bit using value range,
			Note: No allocation check present, performance reason  
		*/
		void WriteValue(int elem , float value) {Data[elem] = static_cast<unsigned short>((value - RangeMin)/(RangeMax - RangeMin)*65535.0f);}

		float GetMinRange() const {return RangeMin;}
		float GetMaxRange() const {return RangeMax;}
	private:
		/** Hold min value supported by this array, used for range compression*/
		float RangeMin; 

		/** Hold max value supported by this array, used for range compression*/
		float RangeMax;
	};
}
#endif 
