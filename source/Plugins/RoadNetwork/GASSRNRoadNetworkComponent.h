
#ifndef GASS_RN_ROAD_NETWORK_COMPONENT
#define GASS_RN_ROAD_NETWORK_COMPONENT

#include "Sim/GASS.h"
#include "Sim/Messages/GASSPlatformMessages.h"
#include "Sim/Interface/GASSIGraphComponent.h"
#include "Sim/Interface/GASSINavigationComponent.h"
#include "RoadNetwork.h"

namespace tinyxml2
{
	class XMLElement;
}

namespace GASS
{
	class AIRoadLaneSectionComponent;

	typedef GASS_SHARED_PTR<AIRoadLaneSectionComponent> AIRoadLaneSectionComponentPtr;

	class RNRoadNetworkComponent :  public Reflection<RNRoadNetworkComponent,BaseSceneComponent> , public IGraphComponent, public INavigationComponent
	{
	public:
		RNRoadNetworkComponent(void);
		~RNRoadNetworkComponent(void);
		void OnInitialize();
		static void RegisterReflection();
		void OnPathfindToLocation(PathfindToPositionMessagePtr message);
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
		void _CreateEditableFromNetwork();
		void _CreateNetworkFromEditable();
		bool GetShowGraph() const;
		void SetShowGraph(bool value);
		//void Rebuild();
		//bool DebugSearchGraph();
		void SaveXML(tinyxml2::XMLElement * elem);
		void LoadXML(tinyxml2::XMLElement * elem);
		bool m_Edit;
		bool m_ShowGraph;
		RoadNetwork m_Network;
	};
	typedef GASS_SHARED_PTR<RNRoadNetworkComponent> RNRoadNetworkComponentPtr;

}
#endif