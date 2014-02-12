#include <limits>
#include "AIRoadNetwork.h"
#include "AISceneManager.h"
#include "AIRoadLaneSectionComponent.h"
#include "AIRoadLaneComponent.h"
#include "AIRoadNodeComponent.h"
#include "AIRoadEdgeComponent.h"
#include "AIRoadIntersectionComponent.h"
#include "Plugins/Base/CoreMessages.h"
#include "Plugins/Game/GameMessages.h"
#include "RoadIntersectionComponent.h"
#include "Sim/Interface/GASSIWaypointListComponent.h"
#include <algorithm>
#include <tinyxml.h>

namespace GASS
{
	AIRoadNetwork::AIRoadNetwork(void) : m_Edit(true),
		m_ShowGraph(false),
		m_Optimize(true)
	{

	}	

	AIRoadNetwork::~AIRoadNetwork(void)
	{

	}

	void AIRoadNetwork::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("AIRoadNetwork",new Creator<AIRoadNetwork, IComponent>);
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("AIRoadNetwork", OF_VISIBLE)));

		RegisterProperty<bool>("ShowGraph", &AIRoadNetwork::GetShowGraph, &AIRoadNetwork::SetShowGraph,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));

		RegisterProperty<bool>("Optimize", &AIRoadNetwork::GetOptimize, &AIRoadNetwork::SetOptimize,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));

		RegisterProperty<bool>("Edit", &AIRoadNetwork::GetEdit, &AIRoadNetwork::SetEdit,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));

		RegisterProperty<std::string>("NodeTemplate", &AIRoadNetwork::GetNodeTemplate, &AIRoadNetwork::SetNodeTemplate,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE )));
		RegisterProperty<std::string>("EdgeTemplate", &AIRoadNetwork::GetEdgeTemplate, &AIRoadNetwork::SetEdgeTemplate,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE )));
	}

	void AIRoadNetwork::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(AIRoadNetwork::OnPathfindToLocation,PathfindToPositionMessage,0));
	}

	bool AIRoadNetwork::GetShowGraph() const
	{
		return m_ShowGraph;
	}
	
	void AIRoadNetwork::SetShowGraph(bool value) 
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
					MessagePtr mesh_message(new ManualMeshDataMessage(mesh_data));
					debug->PostMessage(mesh_message);
				}
			}
			else
			{
				if(debug)
				{
					debug->PostMessage(MessagePtr(new ClearManualMeshMessage()));
				}
			}
		}
	}

	bool AIRoadNetwork::GetEdit() const
	{
		return m_Edit;
	}

	void AIRoadNetwork::SetEdit(bool value)
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
				GetSceneObject()->GetComponentsByClass<AIRoadNodeComponent>(components);
				for(size_t i = 0 ;  i < components.size(); i++)
				{
					AIRoadNodeComponentPtr node_comp = DYNAMIC_PTR_CAST<AIRoadNodeComponent>(components[i]);
					GetSceneObject()->RemoveChildSceneObject(node_comp->GetSceneObject());
				}

				//Remove debug data
				SceneObjectPtr debug = GetSceneObject()->GetChildByID("EDIT_EDGES");
				if(debug)
				{
					debug->PostMessage(MessagePtr(new ClearManualMeshMessage()));
				}
				SetShowGraph(m_ShowGraph);
			}
		}
	}

	void AIRoadNetwork::_ExpandFromNetwork()
	{
		//create all nodes and  edges!
		std::map<RoadNode*,GraphNodeComponentPtr> mapping;
		for(size_t i = 0; i < m_Network.m_Nodes.size();i++)
		{
			SceneObjectPtr node_obj = GetSceneObject()->GetScene()->LoadObjectFromTemplate(m_NodeTemplate,GetSceneObject());
			GASSAssert(node_obj,"Failed to create scene object in void AIRoadNetwork::SetEdit");
			node_obj->SendImmediate(MessagePtr(new GASS::WorldPositionMessage(m_Network.m_Nodes[i]->Position)));
			mapping[m_Network.m_Nodes[i]] = node_obj->GetFirstComponentByClass<IGraphNodeComponent>();
		}

		for(size_t i = 0; i < m_Network.m_Edges.size(); i++)
		{
			SceneObjectPtr edge_obj = GetSceneObject()->GetScene()->LoadObjectFromTemplate(m_EdgeTemplate,GetSceneObject());
			GASSAssert(edge_obj,"Failed to create scene object in void AIRoadNetwork::SetEdit");
			AIRoadEdgeComponentPtr edge_comp = edge_obj->GetFirstComponentByClass<AIRoadEdgeComponent>();
			GASSAssert(edge_comp,"Failed to find IGraphEdgeComponent in AIRoadNetwork::SetEdit");
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
						GASSAssert(node_obj,"Failed to create scene object in void AIRoadNetwork::SetEdit");
						node_obj->SendImmediate(MessagePtr(new GASS::WorldPositionMessage(m_Network.m_Edges[i]->Waypoints[j])));
						GraphNodeComponentPtr node_comp = node_obj->GetFirstComponentByClass<IGraphNodeComponent>();
						edge_comp->SetEndNode(node_comp);
						node_comp->AddEdge(edge_comp);

						edge_obj = GetSceneObject()->GetScene()->LoadObjectFromTemplate(m_EdgeTemplate,GetSceneObject());
						GASSAssert(edge_obj,"Failed to create scene object in void AIRoadNetwork::SetEdit");
						edge_comp = edge_obj->GetFirstComponentByClass<AIRoadEdgeComponent>();
						GASSAssert(edge_comp,"Failed to find IGraphEdgeComponent in AIRoadNetwork::SetEdit");
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

	void AIRoadNetwork::RebuildGraph()
	{
		if(!m_Edit)
			return;
		IComponentContainer::ComponentVector components;
		GetSceneObject()->GetComponentsByClass<AIRoadNodeComponent>(components);
		for(size_t i = 0 ;  i < components.size(); i++)
		{
			AIRoadNodeComponentPtr node_comp = DYNAMIC_PTR_CAST<AIRoadNodeComponent>(components[i]);
		}

		std::vector<Vec3> pos_vec;
		components.clear();
		GetSceneObject()->GetComponentsByClass<AIRoadEdgeComponent>(components);
		for(size_t i = 0 ;  i < components.size(); i++)
		{

			AIRoadEdgeComponentPtr edge_comp = DYNAMIC_PTR_CAST<AIRoadEdgeComponent>(components[i]);
			std::vector<Vec3> wps = edge_comp->GetWaypoints();
			for(size_t i = 1 ; i < wps.size(); i++) 
			{
				pos_vec.push_back(wps[i-1]);
				pos_vec.push_back(wps[i]);
			}
			/*AIRoadNodeComponentPtr start_node = DYNAMIC_PTR_CAST<AIRoadNodeComponent>(edge_comp->GetStartNode());
			AIRoadNodeComponentPtr end_node = DYNAMIC_PTR_CAST<AIRoadNodeComponent>(edge_comp->GetEndNode());
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
			MessagePtr mesh_message(new ManualMeshDataMessage(mesh_data));
			debug->PostMessage(mesh_message);
		}
	}

	void AIRoadNetwork::_RebuildNetwork()
	{
		m_Network.Clear();
		std::map<AIRoadNodeComponentPtr,RoadNode*> node_mapping;
		IComponentContainer::ComponentVector components;
		GetSceneObject()->GetComponentsByClass<AIRoadNodeComponent>(components);
		for(size_t i = 0 ;  i < components.size(); i++)
		{
			AIRoadNodeComponentPtr node_comp = DYNAMIC_PTR_CAST<AIRoadNodeComponent>(components[i]);
			Vec3 node_pos  = node_comp->GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->GetPosition();
			RoadNode* road_node = new RoadNode();
			road_node->Position = node_pos;
			node_mapping[node_comp] = road_node; 
			m_Network.m_Nodes.push_back(road_node);
			node_comp->GetEdges();
		}

		components.clear();
		GetSceneObject()->GetScene()->GetRootSceneObject()->GetComponentsByClass<AIRoadEdgeComponent>(components);
		for(size_t i = 0 ;  i < components.size(); i++)
		{
			AIRoadEdgeComponentPtr road_comp = DYNAMIC_PTR_CAST<AIRoadEdgeComponent>(components[i]);
			std::vector<Vec3> road_wps = road_comp->GetWaypoints();
			RoadNode* start_node;
			if(road_comp->GetStartNode())
			{
				AIRoadNodeComponentPtr start_rn = DYNAMIC_PTR_CAST<AIRoadNodeComponent>(road_comp->GetStartNode());
				start_node = node_mapping.find(start_rn)->second;
				//Vec3 start_pos  = start_rn->GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->GetPosition();
				//road_wps.push_back(start_pos);
			}
			RoadNode* end_node;
			if(road_comp->GetEndNode())
			{
				AIRoadNodeComponentPtr end_rn = DYNAMIC_PTR_CAST<AIRoadNodeComponent>(road_comp->GetEndNode());
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

	/*void AIRoadNetwork::GenerateGraph()
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

	void AIRoadNetwork::OnPathfindToLocation(PathfindToPositionMessagePtr message)
	{
		Vec3 from = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->GetWorldPosition();
		std::vector<Vec3> path = m_Network.Search(from,message->GetPosition());

		SceneObjectPtr debug = GetSceneObject()->GetChildByID("DEBUG_SEARCH");
		if(debug)
		{
			GraphicsMeshPtr mesh_data(new GraphicsMesh());
			mesh_data->SubMeshVector.push_back(GraphicsSubMesh::GenerateLines(path, ColorRGBA(0,0,1,1), "WhiteTransparentNoLighting",true));
			MessagePtr mesh_message(new ManualMeshDataMessage(mesh_data));
			debug->PostMessage(mesh_message);
		}
	}

	bool AIRoadNetwork::GetShortestPath(const Vec3 &from, const Vec3 &to, NavigationPath &path) const
	{
		path = m_Network.Search(from,to);
		return true;
	}

	void AIRoadNetwork::SaveXML(TiXmlElement * elem)
	{
		if(m_Edit)
			_RebuildNetwork();
		m_Edit = false;
		m_ShowGraph = false;
		BaseSceneComponent::SaveXML(elem);

		TiXmlElement *  net_elem = elem->FirstChildElement("AIRoadNetwork");
		m_Network.SaveXML(net_elem);
	
	}


	void AIRoadNetwork::LoadXML(TiXmlElement * elem)
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
