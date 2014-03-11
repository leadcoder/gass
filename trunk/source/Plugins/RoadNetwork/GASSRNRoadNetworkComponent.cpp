#include <limits>
#include "GASSRNRoadNetworkComponent.h"
#include "GASSRNRoadNodeComponent.h"
#include "GASSRNRoadEdgeComponent.h"
#include "RoadEdge.h"
#include "RoadNode.h"
#include "Plugins/Base/CoreMessages.h"
#include "Plugins/Game/GameMessages.h"
#include "Sim/Interface/GASSIWaypointListComponent.h"
#include <tinyxml.h>

namespace GASS
{
	RNRoadNetworkComponent::RNRoadNetworkComponent(void) : m_Edit(true),
		m_ShowGraph(false),
		m_Optimize(true)
	{

	}	

	RNRoadNetworkComponent::~RNRoadNetworkComponent(void)
	{

	}

	void RNRoadNetworkComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("RNRoadNetworkComponent",new Creator<RNRoadNetworkComponent, IComponent>);
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("RNRoadNetworkComponent", OF_VISIBLE)));

		RegisterProperty<bool>("ShowGraph", &RNRoadNetworkComponent::GetShowGraph, &RNRoadNetworkComponent::SetShowGraph,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));

		RegisterProperty<bool>("Optimize", &RNRoadNetworkComponent::GetOptimize, &RNRoadNetworkComponent::SetOptimize,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));

		RegisterProperty<bool>("Edit", &RNRoadNetworkComponent::GetEdit, &RNRoadNetworkComponent::SetEdit,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));

		RegisterProperty<std::string>("NodeTemplate", &RNRoadNetworkComponent::GetNodeTemplate, &RNRoadNetworkComponent::SetNodeTemplate,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE )));
		RegisterProperty<std::string>("EdgeTemplate", &RNRoadNetworkComponent::GetEdgeTemplate, &RNRoadNetworkComponent::SetEdgeTemplate,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE )));
	}

	void RNRoadNetworkComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(RNRoadNetworkComponent::OnPathfindToLocation,PathfindToPositionMessage,0));
	}

	bool RNRoadNetworkComponent::GetShowGraph() const
	{
		return m_ShowGraph;
	}
	
	void RNRoadNetworkComponent::SetShowGraph(bool value) 
	{
		m_ShowGraph = value;
		if(GetSceneObject())
		{
			SceneObjectPtr debug = GetSceneObject()->GetChildByID("SEARCH_GRAPH");
			if(value)
			{
				std::vector<Vec3> pos_vec;
				
				for(size_t i = 0; i < m_Network.m_Edges.size();i++)
				{
					for(size_t j = 0; j < m_Network.m_Edges[i]->LLWaypoints.size();j++)
					{
						for(size_t k = 1; k < m_Network.m_Edges[i]->LLWaypoints[j].size();k++)
						{
							pos_vec.push_back(m_Network.m_Edges[i]->LLWaypoints[j].at(k-1));
							pos_vec.push_back(m_Network.m_Edges[i]->LLWaypoints[j].at(k));
						}
					}

					for(size_t j = 0; j < m_Network.m_Edges[i]->RLWaypoints.size();j++)
					{
						for(size_t k = 1; k < m_Network.m_Edges[i]->RLWaypoints[j].size();k++)
						{
							pos_vec.push_back(m_Network.m_Edges[i]->RLWaypoints[j].at(k-1));
							pos_vec.push_back(m_Network.m_Edges[i]->RLWaypoints[j].at(k));
						}
					}
					
					/*pos_vec.push_back(m_Network.m_Edges[i]->StartNode->Position);
					pos_vec.push_back(m_Network.m_Edges[i]->EndNode->Position);

					std::vector<Vec3> lane1 = m_Network.m_Edges[i]->LLWaypoints[0];

					pos_vec.push_back(lane1[0]);
					pos_vec.push_back(lane1[1]);

					std::vector<Vec3> lane2 = m_Network.m_Edges[i]->RLWaypoints[0];

					pos_vec.push_back(lane2[0]);
					pos_vec.push_back(lane2[1]);*/

				}
				if(debug)
				{
					GraphicsMeshPtr mesh_data(new GraphicsMesh());
					mesh_data->SubMeshVector.push_back(GraphicsSubMesh::GenerateLines(pos_vec, ColorRGBA(0,1,0,1), "WhiteTransparentNoLighting",false));
					
					debug->PostRequest(ManualMeshDataRequestPtr(new ManualMeshDataRequest(mesh_data)));
				}
			}
			else
			{
				if(debug)
				{
					debug->PostRequest(ClearManualMeshRequestPtr(new ClearManualMeshRequest()));
				}
			}
		}
	}

	bool RNRoadNetworkComponent::GetEdit() const
	{
		return m_Edit;
	}

	void RNRoadNetworkComponent::SetEdit(bool value)
	{
		m_Edit = value;
		if(GetSceneObject())
		{
			if(value)
			{
				_ExpandFromNetwork();
			}
			else
			{
				_RebuildNetwork();
				//remove all nodes and edges
				IComponentContainer::ComponentVector components;
				GetSceneObject()->GetComponentsByClass<RNRoadNodeComponent>(components);
				for(size_t i = 0 ;  i < components.size(); i++)
				{
					RNRoadNodeComponentPtr node_comp = DYNAMIC_PTR_CAST<RNRoadNodeComponent>(components[i]);
					GetSceneObject()->RemoveChildSceneObject(node_comp->GetSceneObject());
				}

				//Remove debug data
				SceneObjectPtr debug = GetSceneObject()->GetChildByID("EDIT_EDGES");
				if(debug)
				{
					debug->PostRequest(ClearManualMeshRequestPtr(new ClearManualMeshRequest()));
				}
				SetShowGraph(m_ShowGraph);
			}
		}
	}

	void RNRoadNetworkComponent::_ExpandFromNetwork()
	{
		//create all nodes and  edges!
		std::map<RoadNode*,GraphNodeComponentPtr> mapping;
		for(size_t i = 0; i < m_Network.m_Nodes.size();i++)
		{
			SceneObjectPtr node_obj = GetSceneObject()->GetScene()->LoadObjectFromTemplate(m_NodeTemplate,GetSceneObject());
			GASSAssert(node_obj,"Failed to create scene object in void RNRoadNetworkComponent::SetEdit");
			node_obj->SendImmediateRequest(WorldPositionRequestPtr(new WorldPositionRequest(m_Network.m_Nodes[i]->Position)));
			mapping[m_Network.m_Nodes[i]] = node_obj->GetFirstComponentByClass<IGraphNodeComponent>();
		}

		for(size_t i = 0; i < m_Network.m_Edges.size(); i++)
		{
			SceneObjectPtr edge_obj = GetSceneObject()->GetScene()->LoadObjectFromTemplate(m_EdgeTemplate,GetSceneObject());
			GASSAssert(edge_obj,"Failed to create scene object in void RNRoadNetworkComponent::SetEdit");
			RNRoadEdgeComponentPtr edge_comp = edge_obj->GetFirstComponentByClass<RNRoadEdgeComponent>();
			GASSAssert(edge_comp,"Failed to find IGraphEdgeComponent in RNRoadNetworkComponent::SetEdit");
			edge_comp->SetLaneWidth(m_Network.m_Edges[i]->LaneWidth);

			if(!m_Optimize)
			{
				if(m_Network.m_Edges[i]->Waypoints.size() > 2)
				{
					edge_comp->SetStartNode(mapping[m_Network.m_Edges[i]->StartNode]);
					mapping[m_Network.m_Edges[i]->StartNode]->AddEdge(edge_comp);
					for(size_t j = 1 ; j < m_Network.m_Edges[i]->Waypoints.size()-1; j++)
					{
						SceneObjectPtr node_obj = GetSceneObject()->GetScene()->LoadObjectFromTemplate(m_NodeTemplate,GetSceneObject());
						GASSAssert(node_obj,"Failed to create scene object in void RNRoadNetworkComponent::SetEdit");
						node_obj->SendImmediateRequest(WorldPositionRequestPtr(new WorldPositionRequest(m_Network.m_Edges[i]->Waypoints[j])));
						GraphNodeComponentPtr node_comp = node_obj->GetFirstComponentByClass<IGraphNodeComponent>();
						edge_comp->SetEndNode(node_comp);
						node_comp->AddEdge(edge_comp);

						edge_obj = GetSceneObject()->GetScene()->LoadObjectFromTemplate(m_EdgeTemplate,GetSceneObject());
						GASSAssert(edge_obj,"Failed to create scene object in void RNRoadNetworkComponent::SetEdit");
						edge_comp = edge_obj->GetFirstComponentByClass<RNRoadEdgeComponent>();
						GASSAssert(edge_comp,"Failed to find IGraphEdgeComponent in RNRoadNetworkComponent::SetEdit");
						edge_comp->SetStartNode(node_comp);
						node_comp->AddEdge(edge_comp);
						edge_comp->SetLaneWidth(m_Network.m_Edges[i]->LaneWidth);

					}
					edge_comp->SetEndNode(mapping[m_Network.m_Edges[i]->EndNode]);
					mapping[m_Network.m_Edges[i]->EndNode]->AddEdge(edge_comp);
				}
				else
				{
					edge_comp->SetStartNode(mapping[m_Network.m_Edges[i]->StartNode]);
					edge_comp->SetEndNode(mapping[m_Network.m_Edges[i]->EndNode]);
					mapping[m_Network.m_Edges[i]->StartNode]->AddEdge(edge_comp);
					mapping[m_Network.m_Edges[i]->EndNode]->AddEdge(edge_comp);
				}
			}
			else
			{
				edge_comp->SetStartNode(mapping[m_Network.m_Edges[i]->StartNode]);
				edge_comp->SetEndNode(mapping[m_Network.m_Edges[i]->EndNode]);
				edge_comp->SetWaypoints(m_Network.m_Edges[i]->Waypoints);
				mapping[m_Network.m_Edges[i]->StartNode]->AddEdge(edge_comp);
				mapping[m_Network.m_Edges[i]->EndNode]->AddEdge(edge_comp);
			}
		}
	}

	void RNRoadNetworkComponent::RebuildGraph()
	{
		if(!m_Edit)
			return;
		IComponentContainer::ComponentVector components;
		GetSceneObject()->GetComponentsByClass<RNRoadNodeComponent>(components);
		for(size_t i = 0 ;  i < components.size(); i++)
		{
			RNRoadNodeComponentPtr node_comp = DYNAMIC_PTR_CAST<RNRoadNodeComponent>(components[i]);
		}

		std::vector<Vec3> pos_vec;
		components.clear();
		GetSceneObject()->GetComponentsByClass<RNRoadEdgeComponent>(components);
		for(size_t i = 0 ;  i < components.size(); i++)
		{

			RNRoadEdgeComponentPtr edge_comp = DYNAMIC_PTR_CAST<RNRoadEdgeComponent>(components[i]);
			std::vector<Vec3> wps = edge_comp->GetWaypoints();
			for(size_t i = 1 ; i < wps.size(); i++) 
			{
				pos_vec.push_back(wps[i-1]);
				pos_vec.push_back(wps[i]);
			}
			/*RNRoadNodeComponentPtr start_node = DYNAMIC_PTR_CAST<RNRoadNodeComponent>(edge_comp->GetStartNode());
			RNRoadNodeComponentPtr end_node = DYNAMIC_PTR_CAST<RNRoadNodeComponent>(edge_comp->GetEndNode());
			if(start_node && end_node)
			{
				Vec3 start_pos  = start_node->GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->GetPosition();
				Vec3 end_pos  = end_node->GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->GetPosition();
				pos_vec.push_back(start_pos);
				pos_vec.push_back(end_pos);
			}*/
		}
		
		SceneObjectPtr debug = GetSceneObject()->GetChildByID("EDIT_EDGES");
		if(debug)
		{
			GraphicsMeshPtr mesh_data(new GraphicsMesh());
			mesh_data->SubMeshVector.push_back(GraphicsSubMesh::GenerateLines(pos_vec, ColorRGBA(1,0,0,1), "WhiteTransparentNoLighting",false));
			
			debug->PostRequest(ManualMeshDataRequestPtr(new ManualMeshDataRequest(mesh_data)));
		}
	}

	void RNRoadNetworkComponent::_RebuildNetwork()
	{
		m_Network.Clear();
		std::map<RNRoadNodeComponentPtr,RoadNode*> node_mapping;
		IComponentContainer::ComponentVector components;
		GetSceneObject()->GetComponentsByClass<RNRoadNodeComponent>(components);
		for(size_t i = 0 ;  i < components.size(); i++)
		{
			RNRoadNodeComponentPtr node_comp = DYNAMIC_PTR_CAST<RNRoadNodeComponent>(components[i]);
			Vec3 node_pos  = node_comp->GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->GetPosition();
			RoadNode* road_node = new RoadNode();
			road_node->Position = node_pos;
			node_mapping[node_comp] = road_node; 
			m_Network.m_Nodes.push_back(road_node);
			node_comp->GetEdges();
		}

		components.clear();
		GetSceneObject()->GetScene()->GetRootSceneObject()->GetComponentsByClass<RNRoadEdgeComponent>(components);
		for(size_t i = 0 ;  i < components.size(); i++)
		{
			RNRoadEdgeComponentPtr road_comp = DYNAMIC_PTR_CAST<RNRoadEdgeComponent>(components[i]);
			std::vector<Vec3> road_wps = road_comp->GetWaypoints();
			RoadNode* start_node;
			if(road_comp->GetStartNode())
			{
				RNRoadNodeComponentPtr start_rn = DYNAMIC_PTR_CAST<RNRoadNodeComponent>(road_comp->GetStartNode());
				start_node = node_mapping.find(start_rn)->second;
				//Vec3 start_pos  = start_rn->GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->GetPosition();
				//road_wps.push_back(start_pos);
			}
			RoadNode* end_node;
			if(road_comp->GetEndNode())
			{
				RNRoadNodeComponentPtr end_rn = DYNAMIC_PTR_CAST<RNRoadNodeComponent>(road_comp->GetEndNode());
				end_node = node_mapping.find(end_rn)->second;
				//Vec3 end_pos  = end_rn->GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->GetPosition();
				//road_wps.push_back(end_pos);
			}
		
			RoadEdge* edge = new RoadEdge();
			edge->Waypoints = road_wps;
			edge->Distance =  Math::GetPathLength(road_wps);
			edge->StartNode = start_node;
			edge->EndNode = end_node;
			edge->LaneWidth = road_comp->GetLaneWidth(); 
			start_node->Edges.push_back(edge);
			end_node->Edges.push_back(edge);
			m_Network.m_Edges.push_back(edge);
		}
		///remove extra nodes!
		if(m_Optimize)
			m_Network.ConvertNodesToWaypoint();
		m_Network.GenerateLanes();
	}

	/*void RNRoadNetworkComponent::GenerateGraph()
	{
		std::map<AIRoadIntersectionComponentPtr,RoadNode*> node_mapping;
		IComponentContainer::ComponentVector components;
		GetSceneObject()->GetScene()->GetRootSceneObject()->GetComponentsByClass<AIRoadIntersectionComponent>(components);
		for(size_t i = 0 ;  i < components.size(); i++)
		{
			AIRoadIntersectionComponentPtr inter_comp = DYNAMIC_PTR_CAST<AIRoadIntersectionComponent>(components[i]);
			Vec3 inter_pos = inter_comp->GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->GetWorldPosition();
			RoadNode* road_node = new RoadNode();
			road_node->Position = inter_pos;
			node_mapping[inter_comp] = road_node; 
			m_Nodes.push_back(road_node);
		}

		components.clear();
		GetSceneObject()->GetScene()->GetRootSceneObject()->GetComponentsByClass<AIRoadComponent>(components);
		for(size_t i = 0 ;  i < components.size(); i++)
		{
			AIRoadComponentPtr road_comp = DYNAMIC_PTR_CAST<AIRoadComponent>(components[i]);
			std::vector<Vec3> road_wps = road_comp->GetWaypointsObject()->GetFirstComponentByClass<IWaypointListComponent>()->GetWaypoints();

			RoadNode* start_node;
			if(road_comp->GetStartNode().IsValid())
			{
				AIRoadIntersectionComponentPtr start_inter = road_comp->GetStartNode()->GetFirstComponentByClass<AIRoadIntersectionComponent>();
				start_node = node_mapping.find(start_inter)->second;
			}
			else //Create end node
			{
				start_node = new RoadNode();
				start_node ->Position = road_wps.front();
				m_Nodes.push_back(start_node);
			}

			RoadNode* end_node;
			if(road_comp->GetEndNode().IsValid())
			{
				AIRoadIntersectionComponentPtr end_inter = road_comp->GetEndNode()->GetFirstComponentByClass<AIRoadIntersectionComponent>();
				end_node = node_mapping.find(end_inter)->second;
			}
			else
			{
				end_node = new RoadNode();
				end_node->Position = road_wps.back();
				m_Nodes.push_back(end_node);
			}

			RoadEdge* edge = new RoadEdge();
			edge->Waypoints = road_wps;
			edge->Distance =  Math::GetPathLength(road_wps);
			edge->StartNode = start_node;
			edge->EndNode = end_node;
			start_node->Edges.push_back(edge);
			end_node->Edges.push_back(edge);
			m_Edges.push_back(edge);
		}
	}*/

	void RNRoadNetworkComponent::OnPathfindToLocation(PathfindToPositionMessagePtr message)
	{
		Vec3 from = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->GetWorldPosition();
		std::vector<Vec3> path = m_Network.Search(from,message->GetPosition());

		SceneObjectPtr debug = GetSceneObject()->GetChildByID("DEBUG_SEARCH");
		if(debug)
		{
			GraphicsMeshPtr mesh_data(new GraphicsMesh());
			mesh_data->SubMeshVector.push_back(GraphicsSubMesh::GenerateLines(path, ColorRGBA(0,0,1,1), "WhiteTransparentNoLighting",true));
			
			debug->PostRequest(ManualMeshDataRequestPtr(new ManualMeshDataRequest(mesh_data)));
		}
	}

	bool RNRoadNetworkComponent::GetShortestPath(const Vec3 &from, const Vec3 &to, NavigationPath &path) const
	{
		path = m_Network.Search(from,to);
		return true;
	}

	void RNRoadNetworkComponent::SaveXML(TiXmlElement * elem)
	{
		if(m_Edit)
			_RebuildNetwork();
		m_Edit = false;
		m_ShowGraph = false;
		BaseSceneComponent::SaveXML(elem);

		TiXmlElement *  net_elem = elem->FirstChildElement("RNRoadNetworkComponent");
		m_Network.SaveXML(net_elem);
	
	}


	void RNRoadNetworkComponent::LoadXML(TiXmlElement * elem)
	{
		m_Network.LoadXML(elem);
		TiXmlElement *prop_elem = elem->FirstChildElement();
		while(prop_elem)
		{
			std::string prop_name = prop_elem->Value();
			if(prop_name != "RoadNetwork")
			{
				std::string prop_val = prop_elem->FirstAttribute()->Value();
				SetPropertyByString(prop_name,prop_val);
			}
			prop_elem  = prop_elem->NextSiblingElement();
		}
	}
}
