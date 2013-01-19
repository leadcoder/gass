#pragma once

#include "Core/Math/GASSVector.h"

namespace GASS
{
	class SceneObject;
	typedef WPTR<SceneObject> SceneObjectWeakPtr;

	class SceneCursorInfo 
	{
	public:
		SceneCursorInfo(void): m_3DPos(0,0,0),m_RayDir(1,0,0), m_RayStart(0,0,0)//,m_ScreenPos(0,0)
		{

		}
		~SceneCursorInfo(void)
		{

		}
		SceneObjectWeakPtr m_ObjectUnderCursor;
		Vec3 m_3DPos;
		//Vec2 m_ScreenPos;
		Vec3 m_RayDir;
		Vec3 m_RayStart;
	};
}
