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
		GetSceneObject()->RegisterForMessage(OBJECT_RM_LOAD_GFX_COMPONENTS, TYPED_MESSAGE_FUNC(OSGLocationComponent::OnLoad,LoadGFXComponentsMessage),0);
		GetSceneObject()->RegisterForMessage(OBJECT_RM_POSITION, TYPED_MESSAGE_FUNC(OSGLocationComponent::OnPositionMessage,PositionMessage),0);
		GetSceneObject()->RegisterForMessage(OBJECT_RM_ROTATION, TYPED_MESSAGE_FUNC(OSGLocationComponent::OnRotationMessage,RotationMessage),0);
		GetSceneObject()->RegisterForMessage(OBJECT_RM_WORLD_POSITION, TYPED_MESSAGE_FUNC(OSGLocationComponent::OnWorldPositionMessage,WorldPositionMessage),0);
		GetSceneObject()->RegisterForMessage(OBJECT_RM_WORLD_ROTATION, TYPED_MESSAGE_FUNC(OSGLocationComponent::OnWorldRotationMessage,WorldRotationMessage),0);
		GetSceneObject()->RegisterForMessage(OBJECT_RM_VISIBILITY,  TYPED_MESSAGE_FUNC( OSGLocationComponent::OnVisibilityMessage,VisibilityMessage),0);
		
	}

	void OSGLocationComponent::OnLoad(LoadGFXComponentsMessagePtr message)
	{
		OSGGraphicsSceneManagerPtr  scene_man = boost::shared_dynamic_cast<OSGGraphicsSceneManager>(message->GetGFXSceneManager());
		//assert(m_GFXSceneManager);
		m_TransformNode = new osg::PositionAttitudeTransform();
		m_RotTransformNode = new osg::PositionAttitudeTransform();
		osg::ref_ptr<osg::PositionAttitudeTransform> root_node = scene_man->GetOSGRootNode();
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

		m_TransformNode->addChild(m_RotTransformNode.get());
		

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
			m_TransformNode->setPosition(osg::Vec3d(value.x,value.y,value.z));
			SendTransMessage();
		}
	}


	void OSGLocationComponent::OnWorldPositionMessage(WorldPositionMessagePtr message)
	{
		Vec3 value = message->GetPosition();
		SetWorldPosition(value);
		/*m_Pos = value;
		if(m_TransformNode.valid())
		{
			osg::Vec3d new_pos(value.x,value.y,value.z);
			osg::PositionAttitudeTransform* parent = dynamic_cast<osg::PositionAttitudeTransform*>(m_TransformNode->getParent(0));
			if(parent) 
			{
				osg::MatrixList mat_list = parent->getWorldMatrices();
				osg::Matrix final;
				final.identity();
				for(int i = 0 ;  i < mat_list.size() ; i++)
				{
					final = mat_list[i]*final;
				}
				osg::Matrix inv_trans = osg::Matrix::inverse(final);
				new_pos = new_pos*inv_trans;
			}
			m_TransformNode->setPosition(new_pos);
			SendTransMessage();
		}*/
	}

	
	void OSGLocationComponent::OnRotationMessage(RotationMessagePtr message)
	{
		Quaternion value = message->GetRotation();
		if(m_TransformNode.valid())
		{
			osg::Quat final = osg::Quat(-value.x,value.z,-value.y,value.w);
			m_TransformNode->setAttitude(final);
			SendTransMessage();
		}
	}

	void OSGLocationComponent::OnWorldRotationMessage(WorldRotationMessagePtr message)
	{
		Quaternion value = message->GetRotation();
		if(m_TransformNode.valid())
		{
			osg::Quat final = osg::Quat(-value.x,value.z,-value.y,value.w);
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

	void OSGLocationComponent::SendTransMessage()
	{
		Vec3 pos = GetWorldPosition();
		Vec3 scale = Vec3(1,1,1);//GetScale();
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
				osg::Matrix final;
				final.identity();
				for(int i = 0 ;  i < mat_list.size() ; i++)
				{
					final = mat_list[i]*final;
				}
				osg::Matrix inv_trans = osg::Matrix::inverse(final);
				new_pos = new_pos*inv_trans;
			}
			m_TransformNode->setPosition(new_pos);
			m_Pos.Set(new_pos.x(),new_pos.y(),new_pos.z());
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
				osg::Matrix final;
				final.identity();
				for(int i = 0 ;  i < mat_list.size() ; i++)
				{
					final = mat_list[i]*final;
				}
				osg::Vec3d osg_world_pos = m_TransformNode->getPosition()*final;
				Vec3 world_pos(osg_world_pos.x(),osg_world_pos.y(),osg_world_pos.z());
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
			Vec3 rot  = Math::Deg2Rad(m_Rot);
			m_TransformNode->setAttitude(osg::Quat(rot.x,osg::Vec3(0,0,1),
				rot.y,osg::Vec3(1,0,0),
				rot.z,osg::Vec3(0,1,0)));
			/*Quaternion q;
			Mat4 rot_mat;
			rot_mat.Identity();
			rot_mat.Rotate(rot.x,rot.y,rot.z);
			q.FromRotationMatrix(rot_mat);
			m_TransformNode->setAttitude(osg::Quat(-q.w,q.x,q.y,q.z));*/
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
			osg::Quat rot = m_TransformNode->getAttitude();
			q.x = rot.x();
			q.z = -rot.y();
			q.y = -rot.z();
			q.w = rot.w();
		}
		return q;
	}

	void OSGLocationComponent::SetWorldRotation(const Quaternion &value)
	{
		if(m_TransformNode.valid())
		{
			osg::Quat final = osg::Quat(-value.x,value.z,-value.y,value.w);
			
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
			q.x = rot.x();
			q.z = -rot.y();
			q.y = -rot.z();
			q.w = rot.w();
		}
		return q;
	}

	void OSGLocationComponent::operator()(osg::Node* node, osg::NodeVisitor* nv) 
     {  
          //std::cout<<"update callback - pre traverse"<<node<<std::endl; 
		 std::cout<<"update callback for:" << GetSceneObject()->GetName() <<std::endl; 
          traverse(node,nv); 
          //std::cout<<"update callback - post traverse"<<node<<std::endl; 
      } 

	void OSGLocationComponent::OnVisibilityMessage(VisibilityMessagePtr message)
	{
		bool visibility = message->GetValue();
		if(visibility) 
			m_TransformNode->setNodeMask(1);
		else m_TransformNode->setNodeMask(0);
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
					osg::ref_ptr<osg::PositionAttitudeTransform> root_node = scene_man->GetOSGRootNode();
					root_node->addChild(m_TransformNode);
				}
			}
			SetWorldPosition(world_pos);
			SetWorldRotation(world_rot);
		}
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
