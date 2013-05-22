
#ifndef AI_ROAD_SEG_COMPONENT
#define AI_ROAD_SEG_COMPONENT

#include "Sim/GASS.h"
#include "Core/Utils/GASSFilePath.h"
#include "Plugins/Game/PlatformType.h"
#include "Plugins/Base/CoreMessages.h"
#include <set>

namespace GASS
{

	class RoadIntersectionComponent;
	typedef SPTR<RoadIntersectionComponent> RoadIntersectionComponentPtr;

	class RoadSegmentComponent :  public Reflection<RoadSegmentComponent,BaseSceneComponent> 
	{
		
	public:
		RoadSegmentComponent(void);
		~RoadSegmentComponent(void);
		static void RegisterReflection();
		virtual void OnInitialize();
		double GetDistanceToIntersection(const Vec3 &source_pos);
		RoadIntersectionComponentPtr GetNextIntersection(RoadIntersectionComponentPtr current);
		bool StartInIntersection(RoadIntersectionComponentPtr from);
		std::vector<Vec3> GetWaypointList(bool invert_dir);
		void SetStartNode(SceneObjectRef node);
		SceneObjectRef GetStartNode() const;
		void SetEndNode(SceneObjectRef node);
		SceneObjectRef GetEndNode() const;
		std::vector<Vec3> GenerateOffset(std::vector<Vec3> wps, Float offset);
		std::vector<Vec3> GetLane(int lane, bool upstream);
	private:
		void UpdateLanes();
		std::vector<Vec3> RoadSegmentComponent::GenerateLane(std::vector<Vec3> wps, bool upstream);
		void OnTransformationChanged(TransformationNotifyMessagePtr message);
		

		SceneObjectRef m_StartNode;
		SceneObjectRef m_EndNode;
		//ADD_ATTRIBUTE(Vec2,RandomVelocity)
		//ADD_ATTRIBUTE(bool,Enable)
		bool m_Initialized;
		void UpdateMesh();

		typedef std::vector<Vec3> Lane;
		std::vector<Lane> m_UpStreamLanes;
		std::vector<Lane> m_DownStreamLanes;
	
		//next instersection in right line
		//RoadIntersectionComponentPtr m_RightIntersection;
		//RoadIntersectionComponentPtr m_LeftIntersection;
	};

	typedef SPTR<RoadSegmentComponent> RoadSegmentComponentPtr;
}
#endif