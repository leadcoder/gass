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
#include "Core/MessageSystem/Message.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"


namespace GASS
{

	OSGLocationComponent::OSGLocationComponent() 
	{
		m_Pos.Set(0,0,0);
		m_Rot.Set(0,0,0);
		m_LastRot.Set(0,0,0);
		m_LastPos.Set(0,0,0); 
		m_Scale.Set(1,1,1);
	}

	OSGLocationComponent::~OSGLocationComponent()
	{
		
	}

	void OSGLocationComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("LocationComponent",new Creator<OSGLocationComponent, IComponent>);
		RegisterProperty<Vec3>("Position", &GetPosition, &SetPosition);
		RegisterProperty<Vec3>("Rotation", &GetEulerRotation, &SetEulerRotation);
	}

	void OSGLocationComponent::OnCreate()
	{
		int obj_id = (int) this;
		MessageManager * mm = GetMessageManager();
		mm->RegisterForMessage(ScenarioScene::SM_MESSAGE_LOAD_GFX_COMPONENTS, obj_id,  boost::bind( &OSGLocationComponent::OnLoad, this, _1 ),0);
		mm->RegisterForMessage(ScenarioScene::OBJECT_MESSAGE_POSITION, obj_id,  boost::bind( &OSGLocationComponent::PositionChanged, this, _1 ),0);
		mm->RegisterForMessage(ScenarioScene::OBJECT_MESSAGE_ROTATION, obj_id,  boost::bind( &OSGLocationComponent::RotationChanged, this, _1 ),0);
		
	}

	void OSGLocationComponent::OnLoad(MessagePtr message)
	{
		OSGGraphicsSceneManager* osg_sm = boost::any_cast<OSGGraphicsSceneManager*>(message->GetData("GraphicsSceneManager"));
		assert(osg_sm);
		osg::ref_ptr<osg::PositionAttitudeTransform> root_node = osg_sm->GetOSGRootNode();
		m_TransformNode = new osg::PositionAttitudeTransform();

		//m_TransformNode->setAttitude(osg_sm->GetSceneTransformatation()->getAttitude());
		m_RotTransformNode = new osg::PositionAttitudeTransform();
		if(root_node.valid())
		{
			root_node->addChild(m_TransformNode.get());
			m_TransformNode->addChild(m_RotTransformNode.get());
			//m_TransformNode->setAttitude(osg::Quat(Math::Deg2Rad(-90),osg::Vec3(1,0,0),
			//						     Math::Deg2Rad(180),osg::Vec3(0,1,0),
			//							 Math::Deg2Rad(0),osg::Vec3(0,0,1)));
		}
		int from_id = (int)this;

		boost::shared_ptr<Message> pos_msg(new Message(ScenarioScene::OBJECT_MESSAGE_POSITION,from_id));
		pos_msg->SetData("Position",m_Pos);
		boost::shared_ptr<Message> rot_msg(new Message(ScenarioScene::OBJECT_MESSAGE_ROTATION,from_id));
		rot_msg->SetData("Rotation",Quaternion(Math::Deg2Rad(m_Rot)));

		GetMessageManager()->SendGlobalMessage(pos_msg);
		GetMessageManager()->SendGlobalMessage(rot_msg);
	}

	void OSGLocationComponent::PositionChanged(MessagePtr message)
	{
		SetPosition(boost::any_cast<Vec3>(message->GetData("Position")));
	}

	void OSGLocationComponent::RotationChanged(MessagePtr message)
	{
		SetRotation(boost::any_cast<Quaternion>(message->GetData("Rotation")));
	}

	void OSGLocationComponent::SetScale(const Vec3 &value)
	{
		m_Scale = value;
	}

	void OSGLocationComponent::SetPosition(const Vec3 &value)
	{
		if(m_TransformNode.valid())
			m_TransformNode->setPosition(osg::Vec3(value.x,value.y,value.z));
		m_Pos = value;
	//	std::cout << "Pos " << m_Pos.x << " " << m_Pos.y << " " << m_Pos.z << std::endl;
	}

	Vec3 OSGLocationComponent::GetPosition() const 
	{
		return m_Pos;
	}

	void OSGLocationComponent::SetRotation(const Quaternion &value)
	{
		if(m_TransformNode.valid())
		{
			//Quaternion q(value.y,value.z,-value.w,-value.x);
		//	Quaternion trans;
		//	trans.FromEulerAngles(Vec3(Math::Deg2Rad(180),0,0));
			//q = value*trans;
			//osg::Quat final = osg::Quat(q.x,-q.z,-q.y,q.w);
		//	osg::Quat final = osg::Quat(value.x,-value.z,-value.y,value.w);
			osg::Quat final = osg::Quat(-value.x,value.z,-value.y,value.w);
			
			m_TransformNode->setAttitude(final);
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
		Quaternion q;
		osg::Quat rot = m_TransformNode->getAttitude();
		q.x = rot.x();
		q.z = -rot.y();
		q.y = -rot.z();
		q.w = rot.w();
		return q;
	}
}
