
#ifndef GASS_RN_ROAD_NETWORK_COMPONENT
#define GASS_RN_ROAD_NETWORK_COMPONENT

#include "Sim/GASS.h"
#include "Sim/Messages/GASSPlatformMessages.h"
#include "Sim/Interface/GASSIGraphComponent.h"
#include "Sim/Interface/GASSIRoadNavigationComponent.h"
#include "RoadNetwork.h"

namespace tinyxml2
{
	class XMLElement;
}

namespace GASS
{
	class AIRoadLaneSectionComponent;

	using AIRoadLaneSectionComponentPtr = std::shared_ptr<AIRoadLaneSectionComponent>;

	class RNRoadNetworkComponent :  public Reflection<RNRoadNetworkComponent,BaseSceneComponent> , public IGraphComponent, public IRoadNavigationComponent
	{
	public:
		RNRoadNetworkComponent(void);
		~RNRoadNetworkComponent(void) override;
		void OnInitialize() override;
		static void RegisterReflection();
		void OnPathfindToLocation(PathfindToPositionMessagePtr message);
		bool GetEdit() const;
		void SetEdit(bool value);
		
		//IGraphComponent interface
		void RebuildGraph() override;
		std::string GetNodeTemplate() const override { return m_NodeTemplate; }
		std::string GetEdgeTemplate() const override { return m_EdgeTemplate; }

		//IRoadNavigationComponent
		bool GetShortestPath(const Vec3 &from, const Vec3 &to, NavigationPath &path) const override;
		bool GetShortestPathForPlatform(const PlatformType platform_type, const Vec3& from, const Vec3& to, NavigationPath& path) const override;
		bool GetClosestRoadPoint(const Vec3 &point, Vec3 &closest_point) const override;
	private:
		void CreateEditableFromNetwork();
		void CreateNetworkFromEditable();
		bool GetShowGraph() const;
		void SetShowGraph(bool value);
		//void Rebuild();
		//bool DebugSearchGraph();
		void SaveXML(tinyxml2::XMLElement * elem) override;
		void LoadXML(tinyxml2::XMLElement * elem) override;
		bool m_Edit{true};
		bool m_ShowGraph{false};
		bool m_Optimize{true};
		RoadNetwork m_Network;
		std::string m_NodeTemplate;
		std::string m_EdgeTemplate;
	};
	using RNRoadNetworkComponentPtr = std::shared_ptr<RNRoadNetworkComponent>;

}
#endif