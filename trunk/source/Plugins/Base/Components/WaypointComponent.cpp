/****************************************************************************
*                                                                           *
* GASS                                                                *
* Copyright (C)2003 - 2005 Johan Hedstrom                                   *
* Email: GASS@gmail.com                                               *
* Web page: http://n00b.dyndns.org/GASS                               *
*                                                                           *
* GASS is only used with knowledge from the author. This software     *
* is not allowed to redistribute without permission from the author.        *
* For further license information, please turn to the product home page or  *
* contact author. Abuse against the GASS license is prohibited by law.*
*                                                                           *
*****************************************************************************/ 

#include <boost/bind.hpp>
#include "WaypointComponent.h"
#include "Core/Math/Quaternion.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/ComponentSystem/IComponent.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/IMessage.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Scenario/Scene/SceneManagerFactory.h"
#include "Sim/Scenario/Scene/SceneObjectManager.h"
#include "Sim/Components/Graphics/ILocationComponent.h"
#include "Sim/SimEngine.h"
#include "Sim/Systems/SimSystemManager.h"
#include "Sim/Components/Graphics/MeshData.h"


namespace GASS
{

	WaypointComponent::WaypointComponent() : m_TangentWeight(1.0), m_Initialized(false),  m_Tangent(0,0,0), m_CustomTangent(false)
	{

	}

	WaypointComponent::~WaypointComponent()
	{

	}

	void WaypointComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("WaypointComponent",new Creator<WaypointComponent, IComponent>);
		RegisterProperty<Float>("TangentWeight", &WaypointComponent::GetTangentWeight, &WaypointComponent::SetTangentWeight);
	}

	void WaypointComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(WaypointComponent::OnLoad,LoadCoreComponentsMessage,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(WaypointComponent::OnUnload,UnloadComponentsMessage,1));
		//GetSceneObject()->RegisterForMessage(REG_TMESS(WaypointComponent::OnMoved,TransformationNotifyMessage,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(WaypointComponent::OnMoved,PositionMessage,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(WaypointComponent::OnMoved,WorldPositionMessage,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(WaypointComponent::OnRotate,WorldRotationMessage,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(WaypointComponent::OnChangeName,SceneObjectNameMessage,0));
	}

	void WaypointComponent::OnMoved(MessagePtr message)
	{
		//notify parent
		int id = (int) this;
		if(id != message->GetSenderID())
		{
			NotifyUpdate();
		}
	}


	void WaypointComponent::OnTangentMoved(MessagePtr message)
	{
		//notify parent
		int id = (int) this;
		if(id != message->GetSenderID())
		{
			NotifyUpdate();
		}
		UpdateTangentLine();
	}

	void WaypointComponent::OnChangeName(GASS::SceneObjectNameMessagePtr message)
	{
		//notify parent
		//NotifyUpdate();
	}

	void WaypointComponent::OnUnload(UnloadComponentsMessagePtr message)
	{
		//notify parent
		NotifyUpdate();
		m_Initialized = false;
	}

	void WaypointComponent::OnLoad(LoadCoreComponentsMessagePtr message)
	{
		SceneObjectPtr tangent = GetSceneObject()->GetFirstChildByName("Tangent",false);
		if(tangent)
		{
			tangent->RegisterForMessage(REG_TMESS(WaypointComponent::OnTangentMoved,WorldPositionMessage,1));
			tangent->RegisterForMessage(REG_TMESS(WaypointComponent::OnTangentMoved,PositionMessage,1));
		}
		else
			std::cout << "Failed to find tangent in waypoint compoenent\n";
		//notify parent
		m_Initialized = true;
		NotifyUpdate();
	}

	void WaypointComponent::OnRotate(WorldRotationMessagePtr message)
	{
		//use custom tangent?
		int id = (int) this;
		if(id != message->GetSenderID())
		{
			NotifyUpdate();
		}
	}

	void WaypointComponent::NotifyUpdate()
	{
		if(m_Initialized)
			GetSceneObject()->GetParentSceneObject()->PostMessage(MessagePtr(new UpdateWaypointListMessage()));
	}

	
	Float WaypointComponent::GetTangentWeight()const
	{
		return m_TangentWeight;
	}

	void WaypointComponent::SetTangentWeight(Float value)
	{
		m_TangentWeight = value;
		NotifyUpdate();
	}

	void WaypointComponent::SetTangentLength(Float value)
	{
		m_TangentWeight = value;
	}

	void WaypointComponent::SetTangent(const Vec3 &tangent)
	{
		//m_Tangent = tangent;
		//if(!m_CustomTangent)
		{

			//rotate node
			Mat4 rot_mat;
			rot_mat.Identity();

			Vec3 up(0,1,0);
			
			Float l = tangent.Length();
			Vec3 dir = tangent*(1.0/l);
			
			Vec3 left = Math::Cross(dir,up);
			left.Normalize();
			up = Math::Cross(left,dir);
			up.Normalize();

			rot_mat.SetRightVector(-left);
			rot_mat.SetUpVector(up);
			rot_mat.SetViewDirVector(dir);

			Quaternion rot;
			rot.FromRotationMatrix(rot_mat);
			int id = (int) this;
			//GetSceneObject()->PostMessage(MessagePtr(new RotationMessage(rot,id)));

			
			//GetSceneObject()->GetFirstChildByClass<ILocationComponent>();
			//LocationComponentPtr t_location = GetSceneObject()->GetFirstChildByName("Tangent",false)->GetFirstComponentByClass<ILocationComponent>();
			Vec3 t_pos = tangent;
			SceneObjectPtr tangent = GetSceneObject()->GetFirstChildByName("Tangent",false);
			if(tangent)
				tangent->PostMessage(MessagePtr(new PositionMessage(t_pos*0.1,id)));
			else
				std::cout << "Failed to find tangent in waypoint compoenent\n";
			
		}
	}

	Vec3 WaypointComponent::GetTangent() const
	{

		LocationComponentPtr t_location = GetSceneObject()->GetFirstChildByName("Tangent",false)->GetFirstComponentByClass<ILocationComponent>(true);
		return t_location->GetPosition()*10;

		LocationComponentPtr wp_location = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>(true);
		Mat4 rot_mat;
		rot_mat.Identity();
		Quaternion rot = wp_location->GetRotation();
		rot.ToRotationMatrix(rot_mat);
		Vec3 tangent = rot_mat.GetViewDirVector();

		return tangent;
	}


	void WaypointComponent::UpdateTangentLine()
	{
		if(!m_Initialized)
			return;

		LocationComponentPtr t_location = GetSceneObject()->GetFirstChildByName("Tangent",false)->GetFirstComponentByClass<ILocationComponent>(true);
		Vec3 t_pos = t_location->GetPosition();

		//collect all children and update path
		//const double line_steps = 115;
		ManualMeshDataPtr mesh_data(new ManualMeshData());
		MeshVertex vertex;
		mesh_data->Material = "WhiteTransparentNoLighting";

		vertex.TexCoord.Set(0,0);
		vertex.Color = Vec4(1,1,1,1);
		mesh_data->Type = LINE_LIST;
		vertex.Pos = Vec3(0,0,0);
		mesh_data->VertexVector.push_back(vertex);

		vertex.Pos = t_pos;
		mesh_data->VertexVector.push_back(vertex);

		MessagePtr mesh_message(new ManualMeshDataMessage(mesh_data));
		GetSceneObject()->PostMessage(mesh_message);
		
	}
}
