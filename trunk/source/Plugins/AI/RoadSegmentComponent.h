
#ifndef AI_TARGET__COMPONENT
#define AI_TARGET__COMPONENT

#include "Core/Utils/GASSFilePath.h"
#include "Sim/GASS.h"
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
	private:
		//ADD_ATTRIBUTE(Vec2,RandomDelay)
		//ADD_ATTRIBUTE(Vec2,RandomVelocity)
		//ADD_ATTRIBUTE(bool,Enable)
		bool m_Initialized;
		//next instersection in right line
		RoadIntersectionComponentPtr m_RightIntersection;
		RoadIntersectionComponentPtr m_LeftIntersection;
	};

	typedef SPTR<RoadSegmentComponent> RoadSegmentComponentPtr;
}
#endif