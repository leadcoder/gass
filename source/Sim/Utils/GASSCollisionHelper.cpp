#include "GASSCollisionHelper.h"
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
		CollisionResult result;

		Vec3 ray_start = pos;
		ray_start.y = max_terrain_height;

		Vec3 ray_direction(0,-1,0);
		ray_direction = ray_direction*(max_terrain_height*2.0);
		
		CollisionSceneManagerPtr csm = scene->GetFirstSceneManagerByClass<ICollisionSceneManager>();
		csm->Raycast(ray_start,ray_direction,flags,result);

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

	bool CollisionHelper::GetGroundData(ScenePtr scene, const Vec3 &pos, Float vertical_ray_dist, GeometryFlags flags, Vec3 &ground_pos, Vec3 &normal)
	{
		CollisionResult result;
		const Vec3 ray_start = pos;
		//first check downwards
		///const Vec3 ray_direction(0, -vertical_ray_dist, 0);
		CollisionSceneManagerPtr csm = scene->GetFirstSceneManagerByClass<ICollisionSceneManager>();
		Vec3d up_vec(0,1,0);
		csm->GetUpVector(pos, up_vec);
		const Vec3 ray_direction = -vertical_ray_dist * up_vec;
		csm->Raycast(ray_start, ray_direction, flags, result);
		if (result.Coll)
		{
			ground_pos = result.CollPosition;
			normal = result.CollNormal;
		}
		else //check upwards
		{
			csm->Raycast(ray_start, -ray_direction, flags, result);
			if (result.Coll)
			{
				ground_pos = result.CollPosition;
				normal = result.CollNormal;
			}
		}

		return result.Coll;
	}
}