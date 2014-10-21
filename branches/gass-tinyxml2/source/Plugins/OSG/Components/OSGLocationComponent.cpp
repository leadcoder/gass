/****************************************************************************
* This file is part of GASS.                                                *
* See http://sourceforge.net/projects/gass/                                 *
*                                                                           *
* Copyright (c) 2008-2009 GASS team. See Contributors.txt for details.      *
*                                                                           *
* GASS is free software: you can redistribute it and/or modify              *
* it under the terms of the GNU Lesser General Public License as published  *
* by the Free Software Foundation, either version 3 of the License, or      *
* (at your option) any later version.                                       *
*                                                                           *
* GASS is distributed in the hope that it will be useful,                   *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU Lesser General Public License for more details.                       *
*                                                                           *
* You should have received a copy of the GNU Lesser General Public License  *
* along with GASS. If not, see <http://www.gnu.org/licenses/>.              *
*****************************************************************************/

#include "Plugins/OSG/Components/OSGLocationComponent.h"
#include "Plugins/OSG/OSGGraphicsSceneManager.h"
#include "Plugins/OSG/OSGConvert.h"
#include "Plugins/OSG/OSGNodeMasks.h"
#include "Sim/GASSScriptManager.h"
#include <angelscript.h>

namespace GASS
{

	OSGLocationComponent::OSGLocationComponent() :
		m_Pos(0,0,0),
		m_Rot(0,0,0),
		m_Scale(1,1,1),
		m_AttachToParent(false),
		m_NodeMask(0)
	{
	}

	OSGLocationComponent::~OSGLocationComponent()
	{
		
		if(m_TransformNode.valid() && m_TransformNode->getParent(0))
		{
			osg::Group* parent = m_TransformNode->getParent(0);
			parent->removeChild(m_TransformNode.get());
		}
	}

	void OSGLocationComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("LocationComponent",new Creator<OSGLocationComponent, Component>);
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("Component used to handle object position, rotation and scale", OF_VISIBLE)));

		RegisterProperty<Vec3>("Position", &GASS::OSGLocationComponent::GetPosition, &GASS::OSGLocationComponent::SetPosition,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Postion relative to parent node",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<Vec3>("Rotation", &GASS::OSGLocationComponent::GetEulerRotation, &GASS::OSGLocationComponent::SetEulerRotation,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Rotation relative to parent node, x = heading, y=pitch, z=roll [Degrees]",PF_VISIBLE | PF_EDITABLE)));

		RegisterProperty<Quaternion>("Quaternion", &GASS::OSGLocationComponent::GetRotation, &GASS::OSGLocationComponent::SetRotation,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Rotation represented as Quaternion",PF_VISIBLE)));

		RegisterProperty<Vec3>("Scale", &GASS::OSGLocationComponent::GetScale, &GASS::OSGLocationComponent::SetScale,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Scale relative to parent node",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<bool>("AttachToParent", &GASS::OSGLocationComponent::GetAttachToParent, &GASS::OSGLocationComponent::SetAttachToParent,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Postion relative to parent node",PF_VISIBLE | PF_EDITABLE)));

		//expose this component to script engine
		asIScriptEngine *engine = SimEngine::Get().GetScriptManager()->GetEngine();

		int r;
		//r = engine->RegisterObjectType("ILocationComponent", 0, asOBJ_REF | asOBJ_NOCOUNT); assert( r >= 0 );
		r = engine->RegisterObjectType("LocationComponent", 0, asOBJ_REF | asOBJ_NOCOUNT); assert( r >= 0 );

		
		/*r = engine->RegisterObjectBehaviour("ILocationComponent", asBEHAVE_REF_CAST, "LocationComponent@ f()", asFUNCTION((refCast<ILocationComponent,OSGLocationComponent>)), asCALL_CDECL_OBJLAST); assert( r >= 0 );
		r = engine->RegisterObjectBehaviour("LocationComponent", asBEHAVE_IMPLICIT_REF_CAST, "ILocationComponent@ f()", asFUNCTION((refCast<OSGLocationComponent,ILocationComponent>)), asCALL_CDECL_OBJLAST); assert( r >= 0 );
		r = engine->RegisterObjectMethod("ILocationComponent", "Vec3 GetPosition() const", asMETHODPR(ILocationComponent, GetPosition,() const, Vec3), asCALL_THISCALL);assert(r >= 0);*/

		

		r = engine->RegisterObjectBehaviour("BaseSceneComponent", asBEHAVE_REF_CAST, "LocationComponent@ f()", asFUNCTION((refCast<BaseSceneComponent,OSGLocationComponent>)), asCALL_CDECL_OBJLAST); assert( r >= 0 );
		r = engine->RegisterObjectBehaviour("LocationComponent", asBEHAVE_IMPLICIT_REF_CAST, "BaseSceneComponent@ f()", asFUNCTION((refCast<OSGLocationComponent,BaseSceneComponent>)), asCALL_CDECL_OBJLAST); assert( r >= 0 );

		r = engine->RegisterObjectMethod("LocationComponent", "string GetName() const", asMETHOD(Component, GetName), asCALL_THISCALL);assert(r >= 0);
		r = engine->RegisterObjectMethod("LocationComponent", "void SetAttachToParent(bool) ", asMETHOD(OSGLocationComponent, SetAttachToParent), asCALL_THISCALL);assert(r >= 0);
		r = engine->RegisterObjectMethod("LocationComponent", "bool GetAttachToParent() const", asMETHOD(OSGLocationComponent, GetAttachToParent), asCALL_THISCALL);assert(r >= 0);
		r = engine->RegisterObjectMethod("LocationComponent", "Vec3 GetPosition() const", asMETHODPR(OSGLocationComponent, GetPosition,() const, Vec3), asCALL_THISCALL);assert(r >= 0);
		//r = engine->RegisterObjectMethod("LocationComponent", "void SetPosition(const Vec3 &in) ", asMETHOD(OSGLocationComponent, SetPosition), asCALL_THISCALL);assert(r >= 0);
		//r = engine->RegisterObjectMethod("LocationComponent", "Vec3 GetPosition() const", 
		//	asSMethodPtr<sizeof(void (GASS::OSGLocationComponent::*)())>::Convert((void (GASS::OSGLocationComponent::*)())(&GASS::OSGLocationComponent::GetPosition)),
		//	asCALL_THISCALL);assert(r >= 0);
		
		
	}

	void OSGLocationComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGLocationComponent::OnPositionMessage,PositionRequest,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGLocationComponent::OnRotationMessage,RotationRequest,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGLocationComponent::OnWorldPositionRequest,WorldPositionRequest,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGLocationComponent::OnWorldRotationMessage,WorldRotationRequest,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGLocationComponent::OnVisibilityMessage,VisibilityRequest,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGLocationComponent::OnScaleMessage,ScaleRequest,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGLocationComponent::OnParentChangedMessage,GASS::ParentChangedEvent,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGLocationComponent::OnAttachToParent,GASS::AttachToParentRequest,0));

		OSGGraphicsSceneManagerPtr  scene_man = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<OSGGraphicsSceneManager>();
		//assert(m_GFXSceneManager);
		if(!m_TransformNode.valid())
		{
			m_TransformNode = new osg::PositionAttitudeTransform();
			osg::ref_ptr<osg::Group> root_node = scene_man->GetOSGShadowRootNode();
			m_GFXSceneManager = scene_man;

			if(m_AttachToParent)
			{
				OSGLocationComponentPtr parent = GetParentLocation();
				if(parent)
				{
					parent->GetOSGNode()->addChild(m_TransformNode.get());
				}
				else
					root_node->addChild(m_TransformNode.get());
			}
			else
			{
				root_node->addChild(m_TransformNode.get());
			}
		}
		else
		{
			std::string name = GetSceneObject()->GetName();
			m_TransformNode->setName(name);
		}
		LocationComponentPtr location = DYNAMIC_PTR_CAST<ILocationComponent>( shared_from_this());
		GetSceneObject()->PostEvent(LocationLoadedEventPtr(new LocationLoadedEvent(location)));

		PositionRequestPtr pos_msg(new PositionRequest(m_Pos));
		RotationRequestPtr rot_msg;//(new RotationRequest(Quaternion(Math::Deg2Rad(m_Rot))));

		if(m_Rot != Vec3(0,0,0))
			rot_msg =RotationRequestPtr(new GASS::RotationRequest(Quaternion(Math::Deg2Rad(m_Rot))));
		else //use 
			rot_msg = RotationRequestPtr(new GASS::RotationRequest(m_QRot));

		GetSceneObject()->PostRequest(pos_msg);
		GetSceneObject()->PostRequest(rot_msg);
		GetSceneObject()->PostRequest(ScaleRequestPtr(new ScaleRequest(m_Scale)));
	}


	void OSGLocationComponent::OnAttachToParent(AttachToParentRequestPtr message)
	{
		SetAttachToParent(message->GetAttachToParent());
	}

	
	void OSGLocationComponent::OnPositionMessage(PositionRequestPtr message)
	{
		Vec3 value = message->GetPosition();
		m_Pos = value;
		if(m_TransformNode.valid())
		{
			m_TransformNode->setPosition(OSGConvert::Get().ToOSG(value));
			SendTransMessage();
		}
	}

	void OSGLocationComponent::OnWorldPositionRequest(WorldPositionRequestPtr message)
	{
		Vec3 value = message->GetPosition();
		SetWorldPosition(value);
	}
	
	void OSGLocationComponent::OnRotationMessage(RotationRequestPtr message)
	{
		Quaternion value = message->GetRotation();
		if(m_TransformNode.valid())
		{
			osg::Quat final = OSGConvert::Get().ToOSG(value);
			m_TransformNode->setAttitude(final);
			SendTransMessage();
		}
	}

	void OSGLocationComponent::OnWorldRotationMessage(WorldRotationRequestPtr message)
	{
		Quaternion value = message->GetRotation();
		if(m_TransformNode.valid())
		{
			//osg::Quat final = OSGConvert::Get().ToOSG(value);
			//m_TransformNode->setAttitude(final);
			//SendTransMessage();
			SetWorldRotation(value);
		}
	}

	void OSGLocationComponent::SetScale(const Vec3 &value)
	{
		m_Scale = value;
		if(m_TransformNode.valid())
		{
			osg::Vec3d scale = OSGConvert::Get().ToOSG(m_Scale);
			double y = fabs(scale.y());
			double z = fabs(scale.z());
			scale.set(scale.x(),y,z);
			m_TransformNode->setScale(scale);
		}
	}

	
	void OSGLocationComponent::SetPosition(const Vec3 &value)
	{
		m_Pos = value;
		if(m_TransformNode.valid())
		{
			GetSceneObject()->PostRequest(PositionRequestPtr(new PositionRequest(value)));
		}
	}

	
	void OSGLocationComponent::OnScaleMessage(ScaleRequestPtr message)
	{
		m_Scale = message->GetScale();
		if(m_TransformNode.valid())
		{
			osg::Vec3d scale = OSGConvert::Get().ToOSG(m_Scale);
			double y = fabs(scale.y());
			double z = fabs(scale.z());
			scale.set(scale.x(),y,z);

			m_TransformNode->setScale( scale);
			SendTransMessage();
		}
	}

	void OSGLocationComponent::SendTransMessage()
	{
		Vec3 pos = GetWorldPosition();
		Vec3 scale = GetScale();
		Quaternion rot = GetWorldRotation();
		
		GetSceneObject()->PostEvent(TransformationChangedEventPtr(new TransformationChangedEvent(pos,rot,scale)));
		//send for all child tranforms also?
		GASS::ComponentContainer::ComponentContainerIterator iter = GetSceneObject()->GetChildren();
		while(iter.hasMoreElements())
		{
			SceneObjectPtr obj = STATIC_PTR_CAST<SceneObject>(iter.getNext());
			OSGLocationComponentPtr c_location = obj->GetFirstComponentByClass<OSGLocationComponent>();
			if(c_location && c_location->GetAttachToParent())
				c_location->SendTransMessage();
		}
	}

	Vec3 OSGLocationComponent::GetPosition() const 
	{
		return m_Pos;
	}

	void OSGLocationComponent::SetWorldPosition(const Vec3 &value)
	{
		if(m_TransformNode.valid())
		{
			osg::Vec3d new_pos =  OSGConvert::Get().ToOSG(value);
			osg::PositionAttitudeTransform* parent = dynamic_cast<osg::PositionAttitudeTransform*>(m_TransformNode->getParent(0));
			if(parent) 
			{
				osg::MatrixList mat_list = parent->getWorldMatrices();
				osg::Matrix world_trans;
				world_trans.identity();
				if(mat_list.size() > 0)
				{
					world_trans = mat_list[0];
				}
				osg::Matrix inv_world_trans = osg::Matrix::inverse(world_trans);
				new_pos = new_pos*inv_world_trans;
			}
			m_TransformNode->setPosition(new_pos);
			m_Pos = OSGConvert::Get().ToGASS(new_pos);
			SendTransMessage();
		}
	}

	
	Vec3 OSGLocationComponent::GetWorldPosition() const 
	{
		if(m_TransformNode.valid())
		{
			osg::PositionAttitudeTransform* parent = dynamic_cast<osg::PositionAttitudeTransform*>(m_TransformNode->getParent(0));
			if(parent) 
			{
				osg::MatrixList mat_list = parent->getWorldMatrices();
				osg::Matrix world_trans;
				world_trans.identity();
				if(mat_list.size() > 0)
				{
					world_trans = mat_list[0];
				}
				osg::Vec3d osg_world_pos = m_TransformNode->getPosition()*world_trans;
				Vec3 world_pos = OSGConvert::Get().ToGASS(osg_world_pos);
				return world_pos;
			}
		}
		return m_Pos;
	}

	void OSGLocationComponent::SetRotation(const Quaternion &value)
	{
		m_QRot = value;
		if(m_TransformNode.valid())
		{
			GetSceneObject()->PostRequest(RotationRequestPtr(new RotationRequest(Quaternion(value))));
		}
	}

	void OSGLocationComponent::SetEulerRotation(const Vec3 &value)
	{
		m_Rot = value;
		if(m_TransformNode.valid())
		{
			Vec3 rot = Math::Deg2Rad(value);
			GetSceneObject()->PostRequest(RotationRequestPtr(new GASS::RotationRequest(Quaternion(rot))));
		}
	}

	Vec3 OSGLocationComponent::GetEulerRotation() const
	{
		return m_Rot;
	}
	
	Quaternion OSGLocationComponent::GetRotation() const
	{
		Quaternion q = m_QRot;
		if(m_TransformNode.valid())
		{
			q = OSGConvert::Get().ToGASS(m_TransformNode->getAttitude());
		}
		return q;
	}

	void OSGLocationComponent::SetWorldRotation(const Quaternion &value)
	{
		if(m_TransformNode.valid())
		{
			osg::Quat final = OSGConvert::Get().ToOSG(value);

			osg::PositionAttitudeTransform* parent = dynamic_cast<osg::PositionAttitudeTransform*>(m_TransformNode->getParent(0));
			if(parent) 
			{
				osg::MatrixList mat_list = parent->getWorldMatrices();
				osg::Matrix world_trans;
				world_trans.identity();
				if(mat_list.size() > 0)
				{
					world_trans = mat_list[0];
				}
				osg::Matrix inv_world_trans = osg::Matrix::inverse(world_trans);

				osg::Quat inv_rot;
				inv_world_trans.get(inv_rot);
				final = final*inv_rot;
			}
			m_TransformNode->setAttitude(final);
			SendTransMessage();
		}
	}

	Quaternion OSGLocationComponent::GetWorldRotation() const
	{
		Quaternion q = Quaternion::IDENTITY;
		
		if(m_TransformNode.valid())
		{
			osg::Quat rot = m_TransformNode->getAttitude();
			
			osg::PositionAttitudeTransform* parent = dynamic_cast<osg::PositionAttitudeTransform*>(m_TransformNode->getParent(0));
			if(parent) 
			{
				osg::MatrixList mat_list = parent->getWorldMatrices();
				osg::Matrix world_trans;
				world_trans.identity();
				if(mat_list.size() > 0)
				{
					world_trans = mat_list[0];
				}
				//osg::Matrix inv_world_trans = osg::Matrix::inverse(world_trans);
				osg::Quat parent_rot;
				world_trans.get(parent_rot);
				rot = rot*parent_rot;
			}
			q = OSGConvert::Get().ToGASS(rot);
		}
		return q;
	}

	void OSGLocationComponent::operator()(osg::Node* node, osg::NodeVisitor* nv) 
     {  
          //std::cout<<"update callback - pre traverse"<<node<<std::endl; 
		 //std::cout<<"update callback for:" << GetSceneObject()->GetName() <<std::endl; 
          traverse(node,nv); 
          //std::cout<<"update callback - post traverse"<<node<<std::endl; 
      } 

	void OSGLocationComponent::OnVisibilityMessage(VisibilityRequestPtr message)
	{
		bool visibility = message->GetValue();
		if(visibility)
		{
			//if(m_NodeMask)
			//	m_TransformNode->setNodeMask(m_NodeMask);
			//m_TransformNode->setNodeMask(NM_VISIBLE | m_TransformNode->getNodeMask());
			m_TransformNode->setNodeMask(~0);
		}
		else
		{
			//m_NodeMask = m_TransformNode->getNodeMask();
			m_TransformNode->setNodeMask(0);
			//m_TransformNode->setNodeMask(~NM_VISIBLE & m_TransformNode->getNodeMask());
		}
	}

	void OSGLocationComponent::SetAttachToParent(bool value)
	{
		m_AttachToParent = value;
		if(m_TransformNode.valid())
		{
			Vec3 world_pos = GetWorldPosition();
			Quaternion world_rot = GetWorldRotation();
			if(m_TransformNode->getParent(0))
				m_TransformNode->getParent(0)->removeChild(m_TransformNode);

			OSGLocationComponentPtr parent = GetParentLocation();
			if(parent && value)
				parent->GetOSGNode()->addChild(m_TransformNode);
			else
			{
				OSGGraphicsSceneManagerPtr scene_man(m_GFXSceneManager,NO_THROW);
				if(scene_man)
				{
					osg::ref_ptr<osg::Group> root_node = scene_man->GetOSGShadowRootNode();
					root_node->addChild(m_TransformNode);
				}
			}
			SetWorldPosition(world_pos);
			SetWorldRotation(world_rot);
		}
	}

	void OSGLocationComponent::OnParentChangedMessage(ParentChangedEventPtr message)
	{
		SetAttachToParent(GetAttachToParent());
	}

	bool OSGLocationComponent::GetAttachToParent() const
	{
		return m_AttachToParent;
	}


	OSGLocationComponentPtr OSGLocationComponent::GetParentLocation()
	{
		OSGLocationComponentPtr parent_location;
		SceneObjectPtr scene_obj  = STATIC_PTR_CAST<SceneObject>(GetSceneObject()->GetParent());
		while(scene_obj && !parent_location)
		{
			parent_location  = scene_obj->GetFirstComponentByClass<OSGLocationComponent>();
			if(parent_location)
				return parent_location;
			scene_obj = STATIC_PTR_CAST<SceneObject>(scene_obj->GetParent());
		}
		return parent_location;
	}
}
