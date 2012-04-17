
#ifndef DETECTION_DATA
#define DETECTION_DATA


#include "Sim/Common.h"
#include "Core/Math/Vector.h"
#include "Core/Math/Quaternion.h"

namespace GASS
{
	struct DetectionData
	{
		std::string ID;
		PlatformType Type;
		Vec3 Pos;
		Vec3 Velocity;
		Quaternion Rotation;
	};

	typedef std::vector<DetectionData> DetectionVector;

}
#endif