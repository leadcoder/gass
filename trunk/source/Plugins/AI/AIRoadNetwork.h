
#ifndef AI_ROAD_NETWORK_COMPONENT
#define AI_ROAD_NETWORK_COMPONENT

#include "Sim/GASS.h"
#include "Core/Utils/GASSFilePath.h"
#include "Plugins/Game/PlatformType.h"
#include "Plugins/Base/CoreMessages.h"
#include "Plugins/AI/AIRoadLaneComponent.h"
#include <tbb/spin_mutex.h>
#include <set>
#include <tbb/atomic.h>
//#include "Plugins/AI/micropather.h"
#include "Plugins/Game/GameMessages.h"
#include "Sim/Interface/GASSIGraphComponent.h"
#include "Sim/Interface/GASSINavigationComponent.h"
#include "RoadNetwork.h"
class TiXmlElement;

namespace GASS
{
	class AIRoadLaneSectionComponent;

	typedef SPTR<AIRoadLaneSectionComponent> AIRoadLaneSectionComponentPtr;

	class AIRoadNetwork :  public Reflection<AIRoadNetwork,BaseSceneComponent> , public IGraphComponent, public INavigationComponent
	{
	public:
		AIRoadNetwork(void);
		~AIRoadNetwork(void);
		void OnInitialize();
		static void RegisterReflection();
		void OnPathfindToLocation(PathfindToPositionMessagePtr message);
		//std::vector<Vec3> Search(const Vec3 &from_point,const Vec3 &to_point) const;
		bool GetEdit() const;
		void SetEdit(bool value);
		ADD_PROPERTY(std::string,NodeTemplate)
		ADD_PROPERTY(std::string,EdgeTemplate)
		ADD_PROPERTY(bool,Optimize)
		//IGraphComponent interface
		virtual void RebuildGraph();
		//INavigationComponent
		virtual bool GetShortestPath(const Vec3 &from, const Vec3 &to, NavigationPath &path) const;
	private:
		void _ExpandFromNetwork();
		void _RebuildNetwork();
		bool GetShowGraph() const;
		void SetShowGraph(bool value);
		void Rebuild();
		bool DebugSearchGraph();
		void SaveXML(TiXmlElement * elem);
		void LoadXML(TiXmlElement * elem);
		bool m_Edit;
		bool m_ShowGraph;
		RoadNetwork m_Network;
	};
	typedef SPTR<AIRoadNetwork> AIRoadNetworkPtr;

}
#endif