
#ifndef DETECTION_DATA
#define DETECTION_DATA


#include "Sim/GASSCommon.h"
#include "Core/Math/GASSVector.h"
#include "Core/Math/GASSQuaternion.h"
#include "Sim/Interface/GASSIPlatformComponent.h"

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