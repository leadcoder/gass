#pragma once
#include "Sim/GASSCommon.h"
#include "Core/Math/GASSVector.h"
#include "Core/Math/GASSRay.h"

namespace GASS
{
	class SceneObject;
	typedef GASS_WEAK_PTR<SceneObject> SceneObjectWeakPtr;

	class SceneCursorInfo 
	{
	public:
		SceneCursorInfo(void): m_3DPos(0,0,0),m_Ray(Vec3(1,0,0), Vec3(0,0,0))
		{

		}
		~SceneCursorInfo(void)
		{

		}
		SceneObjectWeakPtr m_ObjectUnderCursor;
		Vec3 m_3DPos;
		Ray m_Ray;
	};
}
