#include "GASSCollisionHelper.h"
#include "Sim/GASSBaseSceneManager.h"
#include "Sim/Interface/GASSICollisionSceneManager.h"

namespace GASS
{

	CollisionHelper::CollisionHelper()
	{

	}
	
	CollisionHelper::~CollisionHelper()
	{

	}

	Float CollisionHelper::GetHeightAtPosition(ScenePtr scene, const Vec3 &pos, GeometryFlags flags, bool absolute )
	{
		static const Float max_terrain_height = 2000000;
		CollisionRequest request;
		CollisionResult result;

		Vec3 ray_start = pos;
		ray_start.y = max_terrain_height;

		Vec3 ray_direction(0,-1,0);
		ray_direction = ray_direction*(max_terrain_height*2.0);

		request.LineStart = ray_start;
		request.LineEnd = ray_start + ray_direction;
		request.Type = COL_LINE_VERTICAL;
		request.ReturnFirstCollisionPoint = false;
		request.CollisionBits = flags;
		result.Coll = false;

		CollisionSceneManagerPtr csm = scene->GetFirstSceneManagerByClass<ICollisionSceneManager>();
		csm->Force(request,result);

		if(result.Coll)
		{
			Vec3 col_pos;
			if(absolute)
				col_pos  = result.CollPosition;
			else
				col_pos = pos - result.CollPosition;
			
			return col_pos.y;
		}
		return 0;
	}
}