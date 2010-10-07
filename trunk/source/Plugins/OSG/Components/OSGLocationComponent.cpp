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

#include <boost/bind.hpp>
#include "Plugins/OSG/Components/OSGLocationComponent.h"
#include "Plugins/OSG/OSGGraphicsSceneManager.h"
#include "Plugins/OSG/OSGConvert.h"

#include "Core/Math/Quaternion.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/IMessage.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"


namespace GASS
{

	OSGLocationComponent::OSGLocationComponent() :
		m_Pos(0,0,0),
		m_Rot(0,0,0),
		//m_LastRot(0,0,0),
		//m_LastPos(0,0,0), 
		m_Scale(1,1,1),
		m_AttachToParent(false)
	{

	}

	OSGLocationComponent::~OSGLocationComponent()
	{
		//m_TransformNode.release();
		//m_RotTransformNode.release();

		if(m_TransformNode.valid() && m_TransformNode->getParent(0))
		{
			osg::Group* parent = m_TransformNode->getParent(0);
			parent->removeChild(m_TransformNode.get());
		}
	}

	void OSGLocationComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("LocationComponent",new Creator<OSGLocationComponent, IComponent>);
		RegisterProperty<Vec3>("Position", &GetPosition, &SetPosition);
		RegisterProperty<Vec3>("Rotation", &GetEulerRotation, &SetEulerRotation);
		RegisterProperty<bool>("AttachToParent", &GASS::OSGLocationComponent::GetAttachToParent, &GASS::OSGLocationComponent::SetAttachToParent);
	}

	void OSGLocationComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGLocationComponent::OnLoad,LoadGFXComponentsMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGLocationComponent::OnPositionMessage,PositionMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGLocationComponent::OnRotationMessage,RotationMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGLocationComponent::OnWorldPositionMessage,WorldPositionMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGLocationComponent::OnWorldRotationMessage,WorldRotationMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGLocationComponent::OnVisibilityMessage,VisibilityMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGLocationComponent::OnScaleMessage,ScaleMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGLocationComponent::OnParentChangedMessage,GASS::ParentChangedMessage,0));
	}

	void OSGLocationComponent::OnLoad(LoadGFXComponentsMessagePtr message)
	{
		OSGGraphicsSceneManagerPtr  scene_man = boost::shared_dynamic_cast<OSGGraphicsSceneManager>(message->GetGFXSceneManager());
		//assert(m_GFXSceneManager);
		m_TransformNode = new osg::PositionAttitudeTransform();
		std::string name = GetSceneObject()->GetName();
		m_TransformNode->setName(name);
		//m_RotTransformNode = new osg::PositionAttitudeTransform();
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
				//m_TransformNode->setAttitude(osg::Quat(Math::Deg2Rad(-90),osg::Vec3(1,0,0),
				//						     Math::Deg2Rad(180),osg::Vec3(0,1,0),
				//							 Math::Deg2Rad(0),osg::Vec3(0,0,1)));
			
		}

		//m_TransformNode->addChild(m_RotTransformNode.get());
		

		MessagePtr pos_msg(new PositionMessage(m_Pos));
		MessagePtr rot_msg(new RotationMessage(Quaternion(Math::Deg2Rad(m_Rot))));
	
		GetSceneObject()->PostMessage(pos_msg);
		GetSceneObject()->PostMessage(rot_msg);
	}

	void OSGLocationComponent::OnPositionMessage(PositionMessagePtr message)
	{
		Vec3 value = message->GetPosition();
		m_Pos = value;
		if(m_TransformNode.valid())
		{
			m_TransformNode->setPosition(ToOSGFromGASS(value));
			SendTransMessage();
		}
	}

	void OSGLocationComponent::OnWorldPositionMessage(WorldPositionMessagePtr message)
	{
		Vec3 value = message->GetPosition();
		SetWorldPosition(value);
	}
	
	void OSGLocationComponent::OnRotationMessage(RotationMessagePtr message)
	{
		Quaternion value = message->GetRotation();
		if(m_TransformNode.valid())
		{
			osg::Quat final = ToOSGFromGASS(value);//osg::Quat(-value.x,value.z,-value.y,value.w);
			//osg::Quat final = osg::Quat(-value.x,value.y,value.z,value.w);
			m_TransformNode->setAttitude(final);
			SendTransMessage();
		}
	}

	void OSGLocationComponent::OnWorldRotationMessage(WorldRotationMessagePtr message)
	{
		Quaternion value = message->GetRotation();
		if(m_TransformNode.valid())
		{
			osg::Quat final = ToOSGFromGASS(value);//osg::Quat(-value.x,value.z,-value.y,value.w);
			//osg::Quat final = osg::Quat(-value.x,value.y,value.z,value.w);
			m_TransformNode->setAttitude(final);
			SendTransMessage();
		}
	}

	void OSGLocationComponent::SetScale(const Vec3 &value)
	{
		m_Scale = value;
	}

	
	void OSGLocationComponent::SetPosition(const Vec3 &value)
	{
		m_Pos = value;
		if(m_TransformNode.valid())
		{
			MessagePtr pos_msg(new PositionMessage(value));
			GetSceneObject()->PostMessage(pos_msg);
		}
	}

	
	void OSGLocationComponent::OnScaleMessage(ScaleMessagePtr message)
	{
		m_Scale = message->GetScale();
		if(m_TransformNode.valid())
		{
			m_TransformNode->setScale( ToOSGFromGASS(m_Scale));
			SendTransMessage();
		}
	}

	void OSGLocationComponent::SendTransMessage()
	{
		Vec3 pos = GetWorldPosition();
		Vec3 scale = GetScale();
		Quaternion rot = GetWorldRotation();
		MessagePtr trans_msg(new TransformationNotifyMessage(pos,rot,scale));
		GetSceneObject()->PostMessage(trans_msg);
		//send for all child tranforms also?
		GASS::IComponentContainer::ComponentContainerIterator iter = GetSceneObject()->GetChildren();
		while(iter.hasMoreElements())
		{
			SceneObjectPtr obj = boost::shared_static_cast<SceneObject>(iter.getNext());
			OSGLocationComponentPtr c_location = obj->GetFirstComponent<OSGLocationComponent>();
			if(c_location)
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
			osg::Vec3d new_pos(value.x,value.y,value.z);
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
			m_Pos.Set(new_pos.x(),new_pos.y(),new_pos.z());
			SendTransMessage();
		}
	}

	Vec3 OSGLocationComponent::FromOSGToGASS(const osg::Vec3d &value) const 
	{
		return Vec3(value.x(),value.y(),value.z());
	}

	osg::Vec3d OSGLocationComponent::ToOSGFromGASS(const Vec3 &value) const
	{
		return osg::Vec3d(value.x,value.y,value.z);
	}

	Quaternion OSGLocationComponent::FromOSGToGASS(const osg::Quat &value) const
	{
		//return Quaternion(value.w(),value.x(),-value.z(),-value.y());
		return Quaternion(-value.w(),value.x(),value.y(),value.z());
	}

	osg::Quat OSGLocationComponent::ToOSGFromGASS(const Quaternion &value) const
	{
		//return osg::Quat(-value.x,value.z,-value.y,value.w);
		return osg::Quat(value.x,value.y,value.z,-value.w);
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
				Vec3 world_pos = FromOSGToGASS(osg_world_pos);
				return world_pos;
			}
		}
		return m_Pos;
	}

	void OSGLocationComponent::SetRotation(const Quaternion &value)
	{
		if(m_TransformNode.valid())
		{
			MessagePtr rot_msg(new RotationMessage(Quaternion(value)));
			GetSceneObject()->PostMessage(rot_msg);
		}
	}

	void OSGLocationComponent::SetEulerRotation(const Vec3 &value)
	{
		m_Rot = value;
		if(m_TransformNode.valid())
		{
			/*Vec3 rot = value;
			rot.x = value.z;
			rot.y = value.z;*/
			Vec3 rot = Math::Deg2Rad(value);
			MessagePtr rot_msg(new GASS::RotationMessage(Quaternion(rot)));
			GetSceneObject()->PostMessage(rot_msg);

			/*Vec3 rot  = Math::Deg2Rad(m_Rot);
			m_TransformNode->setAttitude(osg::Quat(rot.x,osg::Vec3(0,0,1),
				rot.y,osg::Vec3(1,0,0),
				rot.z,osg::Vec3(0,1,0)));*/
			
		}

	}

	Vec3 OSGLocationComponent::GetEulerRotation() const
	{
		return m_Rot;
	}
	
	Quaternion OSGLocationComponent::GetRotation() const
	{
		Quaternion q = Quaternion::IDENTITY;
		if(m_TransformNode.valid())
		{
			q = FromOSGToGASS(m_TransformNode->getAttitude());
			/*osg::Quat rot = m_TransformNode->getAttitude();
			q.x = rot.x();
			q.z = -rot.y();
			q.y = -rot.z();
			q.w = rot.w();*/
		}
		return q;
	}

	void OSGLocationComponent::SetWorldRotation(const Quaternion &value)
	{
		if(m_TransformNode.valid())
		{
			osg::Quat final = ToOSGFromGASS(value);

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
			q = FromOSGToGASS(rot);
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

	void OSGLocationComponent::OnVisibilityMessage(VisibilityMessagePtr message)
	{
		bool visibility = message->GetValue();
		if(visibility)  
			m_TransformNode->setNodeMask(1 | m_TransformNode->getNodeMask()); //TODO:Change this to keep old flags
		else m_TransformNode->setNodeMask(0 & m_TransformNode->getNodeMask());
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
				OSGGraphicsSceneManagerPtr scene_man(m_GFXSceneManager,boost::detail::sp_nothrow_tag());
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

	void OSGLocationComponent::OnParentChangedMessage(ParentChangedMessagePtr message)
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
		SceneObjectPtr scene_obj  = boost::shared_static_cast<SceneObject>(GetSceneObject()->GetParent());
		while(scene_obj && !parent_location)
		{
			parent_location  = scene_obj->GetFirstComponent<OSGLocationComponent>();
			if(parent_location)
				return parent_location;
			scene_obj = boost::shared_static_cast<SceneObject>(scene_obj->GetParent());
		}
		return parent_location;
	}
}
