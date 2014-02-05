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
	AIRoadNetwork::AIRoadNetwork(void) : m_Edit(true)
	{

	}	

	AIRoadNetwork::~AIRoadNetwork(void)
	{

	}

	void AIRoadNetwork::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("AIRoadNetwork",new Creator<AIRoadNetwork, IComponent>);
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("AIRoadNetwork", OF_VISIBLE)));

		RegisterProperty<bool>("Build", &AIRoadNetwork::GetBuild, &AIRoadNetwork::SetBuild,
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

		/*std::vector<Vec3> pos_vec;
		for(size_t i = 0; i < m_Edges.size();i++)
		{
			pos_vec.push_back(m_Edges[i]->StartNode->Position);
			pos_vec.push_back(m_Edges[i]->EndNode->Position);
		}

		SceneObjectPtr debug = GetSceneObject()->GetChildByID("DEBUG_NODE");
		if(debug)
		{
			GraphicsMeshPtr mesh_data(new GraphicsMesh());
			mesh_data->SubMeshVector.push_back(GraphicsSubMesh::GenerateLines(pos_vec, ColorRGBA(1,0,0,1), "WhiteTransparentNoLighting",false));
			MessagePtr mesh_message(new ManualMeshDataMessage(mesh_data));
			debug->PostMessage(mesh_message);
		}*/
	}

	bool AIRoadNetwork::GetBuild() const
	{
		return false;
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

		//create all nodes and  edges!
		for(size_t i = 0; i < m_Network.m_Edges.size(); i++)
		{
			SceneObjectPtr edge_obj = GetSceneObject()->GetScene()->LoadObjectFromTemplate(m_EdgeTemplate,GetSceneObject());
			GASSAssert(edge_obj,"Failed to create scene object in void AIRoadNetwork::SetEdit");
			GraphEdgeComponentPtr edge_comp = edge_obj->GetFirstComponentByClass<IGraphEdgeComponent>();
			GASSAssert(edge_comp,"Failed to find IGraphEdgeComponent in AIRoadNetwork::SetEdit");
			edge_comp->SetStartNode(mapping[m_Network.m_Edges[i]->StartNode]);
			edge_comp->SetEndNode(mapping[m_Network.m_Edges[i]->EndNode]);
			mapping[m_Network.m_Edges[i]->StartNode]->AddEdge(edge_comp);
			mapping[m_Network.m_Edges[i]->EndNode]->AddEdge(edge_comp);
		}
	}

	void AIRoadNetwork::SetBuild(bool value) 
	{
		//if(GetSceneObject())
		//	RebuildNetwork();
		//GenerateGraph();
		//Rebuild();
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
			AIRoadNodeComponentPtr start_node = DYNAMIC_PTR_CAST<AIRoadNodeComponent>(edge_comp->GetStartNode());
			AIRoadNodeComponentPtr end_node = DYNAMIC_PTR_CAST<AIRoadNodeComponent>(edge_comp->GetEndNode());
			if(start_node && end_node)
			{
				Vec3 start_pos  = start_node->GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->GetPosition();
				Vec3 end_pos  = end_node->GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->GetPosition();
				pos_vec.push_back(start_pos);
				pos_vec.push_back(end_pos);
			}
		}
		
		SceneObjectPtr debug = GetSceneObject()->GetChildByID("DEBUG_NODE");
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
		}

		components.clear();
		GetSceneObject()->GetScene()->GetRootSceneObject()->GetComponentsByClass<AIRoadEdgeComponent>(components);
		for(size_t i = 0 ;  i < components.size(); i++)
		{
			AIRoadEdgeComponentPtr road_comp = DYNAMIC_PTR_CAST<AIRoadEdgeComponent>(components[i]);
			std::vector<Vec3> road_wps;// = road_comp->GetWaypointsObject()->GetFirstComponentByClass<IWaypointListComponent>()->GetWaypoints();

			RoadNode* start_node;
			if(road_comp->GetStartNode())
			{
				AIRoadNodeComponentPtr start_rn = DYNAMIC_PTR_CAST<AIRoadNodeComponent>(road_comp->GetStartNode());
				start_node = node_mapping.find(start_rn)->second;
				Vec3 start_pos  = start_rn->GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->GetPosition();
				road_wps.push_back(start_pos);
			}
			RoadNode* end_node;
			if(road_comp->GetEndNode())
			{
				AIRoadNodeComponentPtr end_rn = DYNAMIC_PTR_CAST<AIRoadNodeComponent>(road_comp->GetEndNode());
				end_node = node_mapping.find(end_rn)->second;
				Vec3 end_pos  = end_rn->GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->GetPosition();
				road_wps.push_back(end_pos);
			}
		
			RoadEdge* edge = new RoadEdge();
			edge->Waypoints = road_wps;
			edge->Distance =  Math::GetPathLength(road_wps);
			edge->StartNode = start_node;
			edge->EndNode = end_node;
			start_node->Edges.push_back(edge);
			end_node->Edges.push_back(edge);
			m_Network.m_Edges.push_back(edge);
		}
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

		SceneObjectPtr debug = GetSceneObject()->GetChildByID("DEBUG_NODE");
		if(debug)
		{
			GraphicsMeshPtr mesh_data(new GraphicsMesh());
			mesh_data->SubMeshVector.push_back(GraphicsSubMesh::GenerateLines(path, ColorRGBA(1,0,0,1), "WhiteTransparentNoLighting",true));
			MessagePtr mesh_message(new ManualMeshDataMessage(mesh_data));
			debug->PostMessage(mesh_message);
		}
	}

	/*std::vector<Vec3> AIRoadNetwork::Search(const Vec3 &from_point,const Vec3 &to_point)
	{
		std::vector<Vec3> path;
		path.push_back(from_point);
		RoadEdge* from_edge = GetCloesestEdge(from_point);
		RoadEdge* to_edge = GetCloesestEdge(to_point);

		if(from_edge && from_edge == to_edge) //no need to path find!
		{
			Vec3 start_point,end_point;
			int start_seg_index,end_seg_index;
			Math::GetClosestPointOnPath(from_point,from_edge->Waypoints, start_seg_index, start_point);
			Math::GetClosestPointOnPath(to_point,from_edge->Waypoints, end_seg_index, end_point);

			path.push_back(start_point);

			if(start_seg_index < end_seg_index)
			{
				for(int i = start_seg_index+1; i <= end_seg_index; i++)
				{
					path.push_back(from_edge->Waypoints[i]);
				}
			}
			else if(start_seg_index > end_seg_index)
			{
				for(int i = start_seg_index; i > end_seg_index; i--)
				{
					path.push_back(from_edge->Waypoints[i]);
				}
			}
			else //same segment
			{

			}
			path.push_back(end_point);
		}
		else if(from_edge && to_edge)
		{
			RoadNode* start_node = InsertNodeOnEdge(from_point,from_edge);
			RoadNode* end_node = InsertNodeOnEdge(to_point,to_edge);
			//do search!
			RoadNavigation nav;
			std::vector<RoadNode*> node_path;
			int ret = nav.GetPath(start_node, end_node, node_path);

			//std::vector<RoadNode*> edge_path;
			if(node_path.size() > 0)
			{
				for(size_t i = 0; i < node_path.size()-1; i++)
				{
					RoadNode* n1 = node_path[i];
					RoadNode* n2 = node_path[i+1];
					//Get edge!
					RoadEdge* edge = NULL;
					bool invert_dir = false;
					for(size_t j = 0; j < n1->Edges.size(); j++)
					{
						if(n1->Edges[j]->EndNode == n2 )
						{
							edge = n1->Edges[j];
							break;
						}

						if(n1->Edges[j]->StartNode == n2 )
						{
							invert_dir = true;
							edge = n1->Edges[j];
							break;
						}
					}
					if(edge)
					{
						//edge_path.push_back(edge);
						if(invert_dir)
						{
							for(size_t j = 0; j < edge->Waypoints.size(); j++)
							{
								path.push_back(edge->Waypoints[edge->Waypoints.size()-j-1]);
							}
						}
						else
						{
							for(size_t j = 0; j < edge->Waypoints.size(); j++)
							{
								path.push_back(edge->Waypoints[j]);
							}
						}

					}
				}
			}

			//remove temp nodes
			RemoveNode(start_node);
			RemoveNode(end_node);
			from_edge->Enabled = true;
			to_edge->Enabled = true;
		}
		path.push_back(to_point);
		return path;
	}

	void AIRoadNetwork::RemoveNode(RoadNode* node)
	{
		for(size_t i = 0; i < node->Edges.size(); i++)
		{
			std::vector<RoadEdge*>::iterator iter = node->Edges[i]->EndNode->Edges.begin();
			while(iter != node->Edges[i]->EndNode->Edges.end())
			{
				if((*iter)->StartNode == node)
				{
					iter = node->Edges[i]->EndNode->Edges.erase(iter);
				}
				else
					iter++;
			}
			delete node->Edges[i];
		}
		delete node;
	}

	RoadNode* AIRoadNetwork::InsertNodeOnEdge(const Vec3& point,RoadEdge* edge)
	{
		int seg_index;
		Vec3 target_point;

		Math::GetClosestPointOnPath(point,edge->Waypoints, seg_index, target_point);

		RoadEdge* edge1 = new RoadEdge();
		RoadEdge* edge2 = new RoadEdge();

		RoadNode* new_node = new RoadNode();
		new_node->Position = target_point;
		edge1->Waypoints.push_back(target_point);
		edge2->Waypoints.push_back(target_point);

		for(size_t i = 0;  i < seg_index+1; i++)
		{
			edge1->Waypoints.push_back(edge->Waypoints[seg_index - i]);
		}

		for(size_t i = seg_index+1;  i < edge->Waypoints.size(); i++)
		{
			edge2->Waypoints.push_back(edge->Waypoints[i]);
		}

		edge1->Distance =  Math::GetPathLength(edge1->Waypoints);
		edge1->StartNode = new_node;
		edge1->EndNode = edge->StartNode;
		new_node->Edges.push_back(edge1);
		edge->StartNode->Edges.push_back(edge1);

		edge2->Distance =  Math::GetPathLength(edge2->Waypoints);
		edge2->StartNode = new_node;
		edge2->EndNode = edge->EndNode;
		new_node->Edges.push_back(edge2);
		edge->EndNode->Edges.push_back(edge2);
		edge->Enabled = false;
		return new_node;
	}

	RoadEdge* AIRoadNetwork::GetCloesestEdge(const Vec3 &point)
	{
		int seg_index;
		RoadEdge* best_edge = NULL;
		Vec3 target_point;
		Vec3 best_target_point;
		Float min_dist  = FLT_MAX;//std::numeric_limits<Float>::max();
		for(size_t i = 0; i < m_Edges.size();i++)
		{
			if(Math::GetClosestPointOnPath(point,m_Edges[i]->Waypoints, seg_index, target_point))
			{
				Float dist = (target_point - point).Length();
				if(dist < min_dist )
				{
					min_dist = dist;
					best_target_point = target_point;
					best_edge = m_Edges[i];
				}
			}
		}
		return best_edge;
	}


	RoadNode* AIRoadNetwork::GetCloesestNode(const Vec3 &point)
	{
		RoadNode* best_node = NULL;
		Float min_dist  = FLT_MAX;//std::numeric_limits<Float>::max();
		for(size_t i = 0; i < m_Nodes.size();i++)
		{
			Float dist = (m_Nodes[i]->Position - point).Length();
			if(dist < min_dist )
			{
				min_dist = dist;
				best_node = m_Nodes[i];
			}
		}
		return best_node;
	}*/

	void AIRoadNetwork::SaveXML(TiXmlElement * elem)
	{
		if(m_Edit)
			_RebuildNetwork();
		m_Edit = false;
		BaseSceneComponent::SaveXML(elem);

		TiXmlElement *  net_elem = elem->FirstChildElement("AIRoadNetwork");
		m_Network.SaveXML(net_elem);
		/*TiXmlElement * nodes_elem = new TiXmlElement("Nodes");  
		net_elem->LinkEndChild( nodes_elem);  
		for(size_t i = 0; i < m_Nodes.size();i++)
		{
			int id = (int) (m_Nodes[i]);
			TiXmlElement *node_prop_elem = new TiXmlElement( "Node");
			nodes_elem->LinkEndChild( node_prop_elem);  
			node_prop_elem->SetAttribute("ID", id);
			std::string pos = m_Nodes[i]->Position.ToString(" ");
			node_prop_elem->SetAttribute("Position",pos.c_str());
		}
		TiXmlElement *edges_elem = new TiXmlElement( "Edges");
		net_elem->LinkEndChild( edges_elem);  
		for(size_t i = 0; i < m_Edges.size();i++)
		{
			TiXmlElement *edge_prop_elem = new TiXmlElement( "Edge");
			edges_elem->LinkEndChild( edge_prop_elem);  
			edge_prop_elem ->SetAttribute("Dir",(int) m_Edges[i]->Dir);
			edge_prop_elem ->SetAttribute("StartNode",(int) m_Edges[i]->StartNode);
			edge_prop_elem ->SetAttribute("EndNode",(int) m_Edges[i]->EndNode);
			edge_prop_elem ->SetAttribute("Distance",m_Edges[i]->Distance);
			TiXmlElement *wps_elem = new TiXmlElement( "Waypoints");
			edge_prop_elem->LinkEndChild( wps_elem);  
			for(size_t j = 0; j < m_Edges[i]->Waypoints.size(); j++)
			{
				TiXmlElement *wp_prop_elem = new TiXmlElement( "WP");
				wps_elem->LinkEndChild( wp_prop_elem);  
				std::string pos = m_Edges[i]->Waypoints[j].ToString(" ");
				wp_prop_elem->SetAttribute("Position",pos.c_str());
			}
		}*/
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
