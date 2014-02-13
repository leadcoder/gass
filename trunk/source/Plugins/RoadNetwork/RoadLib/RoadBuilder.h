
#ifndef ROAD_BUILDER
#define ROAD_BUILDER

#include "Core/Utils/GASSFilePath.h"
#include "Core/Math/GASSMath.h"
#include "Core/Math/GASSVector.h"

namespace GASS
{
	class RoadNode;
	class RoadEdge;
	class RoadNetwork;

	class RoadBuilder
	{
	public:
		/* Helper class to represent a line */
		struct EdgeLine
		{
			Vec3 p1;
			Vec3 p2;
		};

		class EdgeConnection
		{
		public:
			EdgeConnection(RoadNode* node, RoadEdge* edge);
			~EdgeConnection();
			void ClipPaths(EdgeLine clip_left, EdgeLine clip_right);
			double GetAngle() const {return Angle;}
			EdgeLine GetLeftClip() const {return LeftClip;}
			EdgeLine GetRightClip() const {return RightClip;}
		private:
			RoadEdge* Edge;
			RoadNode* Node;
			double Angle;
			EdgeLine Center;
			EdgeLine LeftClip;
			EdgeLine RightClip;

			EdgeLine _LineOffset(Float offset, const EdgeLine &line);
			void _Init();
			std::vector<Vec3> GetLeftPath(size_t i) const;
			void SetLeftPath(const std::vector<Vec3> &path, size_t i);
			std::vector<Vec3> GetRightPath(size_t i) const;
			void SetRightPath(const std::vector<Vec3> &path, size_t i);
			std::vector<Vec3> _Clip(std::vector<Vec3> path,EdgeLine line);
		};
		static bool EdgeSort(const EdgeConnection &edge1, const EdgeConnection &edge2);
		void Extend(std::vector<Vec3> &waypoints, Float dist);
		void CreateLanes(const RoadNetwork &network);
	};
}
#endif