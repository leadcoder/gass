#include "GASSHeightField.h"
#include "GASSRawFile.h"
#include <math.h>
#include <fstream>


namespace GASS
{
	#ifndef HM_LERP
		#define HM_LERP(a, b, t) (a + (b - a) * t)
	#endif

	float HeightField::GetInterpolatedHeight(Float x, Float z) const
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
		h00 = m_Data.ReadValue(x0 + z0*m_Width);
		h01 = m_Data.ReadValue(x1 + z0*m_Width);
		h10 = m_Data.ReadValue(x0 + z1*m_Width);
		h11 = m_Data.ReadValue(x1 + z1*m_Width);

		float tx, ty;
		tx = fxindex - x0;
		ty = fzindex - z0;
		float height = HM_LERP(HM_LERP(h00, h01, tx), HM_LERP(h10, h11, tx), ty);
		return height;
	}
}
