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
#include "Plugins/Ogre/Components/OgreLocationComponent.h"
#include <OgreSceneNode.h>
#include <OgreSceneManager.h>


#include "Core/Math/Quaternion.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/Message.h"
#include "Core/Utils/Log.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Plugins/Ogre/OgreGraphicsSceneManager.h"
#include "Plugins/Ogre/OgreConvert.h"


namespace GASS
{

	OgreLocationComponent::OgreLocationComponent() :
		m_AttachToParent(false),
		m_Pos(0,0,0),
		m_Rot(0,0,0),
		m_LastRot(0,0,0),
		m_LastPos(0,0,0), 
		m_Scale(1,1,1),
		m_OgreNode (NULL)
	{
	}

	OgreLocationComponent::~OgreLocationComponent()
	{
		
	}

	void OgreLocationComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("LocationComponent",new Creator<OgreLocationComponent, IComponent>);
		RegisterProperty<Vec3>("Position", &GetPosition, &SetPosition);
		RegisterProperty<Vec3>("Rotation", &GetEulerRotation, &SetEulerRotation);
		RegisterProperty<bool>("AttachToParent", &GetAttachToParent, &SetAttachToParent);
		//RegisterProperty<int>("InitPriority", &LocationComponent::GetInitPriority, &LocationComponent::SetInitPriority);
	}

	void OgreLocationComponent::OnCreate()
	{
		int obj_id = (int) this;
		MessageManager * mm = GetMessageManager();
		mm->RegisterForMessage(ScenarioScene::SM_MESSAGE_LOAD_GFX_COMPONENTS, obj_id,  boost::bind( &OgreLocationComponent::OnLoad, this, _1 ),0);
		
		mm->RegisterForMessage(ScenarioScene::OBJECT_MESSAGE_POSITION, obj_id,  boost::bind( &OgreLocationComponent::PositionMessage, this, _1 ),0);
		mm->RegisterForMessage(ScenarioScene::OBJECT_MESSAGE_ROTATION, obj_id,  boost::bind( &OgreLocationComponent::RotationMessage, this, _1 ),0);
		mm->RegisterForMessage(ScenarioScene::OBJECT_MESSAGE_VISIBILITY, obj_id,  boost::bind( &OgreLocationComponent::VisibilityMessage, this, _1 ),0);
	}

	void OgreLocationComponent::OnLoad(MessagePtr message)
	{
		OgreGraphicsSceneManager* ogsm = boost::any_cast<OgreGraphicsSceneManager*>(message->GetData("GraphicsSceneManager"));
		assert(ogsm);
		Ogre::SceneManager* sm = ogsm->GetSceneManger();

		static unsigned int obj_id = 0;
		obj_id++;
		std::stringstream ss;
		std::string name;
		ss << GetName() << obj_id;
		ss >> name;

		if(m_AttachToParent)
		{
			OgreLocationComponentPtr parent = GetParentLocation();
			if(parent)
			{	
				m_OgreNode = parent->GetOgreNode()->createChildSceneNode(name);
			}
			else
				m_OgreNode = sm->getRootSceneNode()->createChildSceneNode(name);

		}
		else
		{
			m_OgreNode = sm->getRootSceneNode()->createChildSceneNode(name);
		}
		
		int from_id = (int)this;
		MessagePtr pos_msg(new Message(ScenarioScene::OBJECT_MESSAGE_POSITION,from_id));
		pos_msg->SetData("Position",m_Pos);
		MessagePtr rot_msg(new Message(ScenarioScene::OBJECT_MESSAGE_ROTATION,from_id));
		rot_msg->SetData("Rotation",Quaternion(Math::Deg2Rad(m_Rot)));

		GetMessageManager()->SendGlobalMessage(pos_msg);
		GetMessageManager()->SendGlobalMessage(rot_msg);
		//std::cout << "Pos:" << m_Pos.x << " " << m_Pos.y << " " << m_Pos.z << std::endl;
	}

	void OgreLocationComponent::PositionMessage(MessagePtr message)
	{
		m_Pos = boost::any_cast<Vec3>(message->GetData("Position"));
		if(m_OgreNode)
		{
			m_OgreNode->setPosition(Convert::ToOgre(m_Pos));
		}
	}

	void OgreLocationComponent::RotationMessage(MessagePtr message)
	{
		SetRotation(boost::any_cast<Quaternion>(message->GetData("Rotation")));
	}

	void OgreLocationComponent::VisibilityMessage(MessagePtr message)
	{
		SetVisibility(boost::any_cast<bool>(message->GetData("Visibility")));
	}

	void OgreLocationComponent::SetVisibility(bool visibility)
	{
		if(m_OgreNode) m_OgreNode->setVisible(visibility);
	}

	void OgreLocationComponent::SetScale(const Vec3 &value)
	{
		m_Scale = value;
		if(m_OgreNode) m_OgreNode->setScale(Convert::ToOgre(value));
	}
	
	void OgreLocationComponent::SetPosition(const Vec3 &value)
	{
		//std::cout << "Pos:" << value.x << " " << value.y << " " << value.z << std::endl;
		if(m_OgreNode) //initialzed?
		{
			int from_id = (int)this;
			MessagePtr pos_msg(new Message(ScenarioScene::OBJECT_MESSAGE_POSITION,from_id));
			pos_msg->SetData("Position",value);
			GetMessageManager()->SendGlobalMessage(pos_msg);
		}
		m_Pos = value;
	}

	Vec3 OgreLocationComponent::GetPosition() const 
	{
		//if(m_OgreNode)
		//	return Convert::ToGASS(m_OgreNode->getPosition());
		//else return m_Pos;
		return m_Pos;
	}
	/*void OgreLocationComponent::SetEulerRotation(const Vec3 &value)
	{
		m_Rot = value;
		if(m_OgreNode)
		{
			Vec3 rot  = Math::Deg2Rad(m_Rot);
			m_OgreNode->resetOrientation();
			m_OgreNode->yaw(Ogre::Radian(rot.x));
			m_OgreNode->pitch(Ogre::Radian(rot.y));
			m_OgreNode->roll(Ogre::Radian(rot.z));

			// send rotation changed message
	
		}
	}*/


	void OgreLocationComponent::SetEulerRotation(const Vec3 &value)
	{
		//std::cout << "Pos:" << value.x << " " << value.y << " " << value.z << std::endl;
		if(m_OgreNode) //initialzed?
		{
			int from_id = (int)this;
			MessagePtr rot_msg(new Message(ScenarioScene::OBJECT_MESSAGE_ROTATION,from_id));
			rot_msg->SetData("Rotation",Quaternion(Math::Deg2Rad(value)));
			GetMessageManager()->SendGlobalMessage(rot_msg);
		}
		m_Rot = value;
	}

	Vec3 OgreLocationComponent::GetEulerRotation() const
	{
		return m_Rot;
	}

	void OgreLocationComponent::SetRotation(const Quaternion &value)
	{
		if(m_OgreNode)
		{
			m_OgreNode->setOrientation(Convert::ToOgre(value));
		}
	}

	Quaternion OgreLocationComponent::GetRotation() const
	{
		Quaternion q;
		if(m_OgreNode)
		{
			q = Convert::ToGASS(m_OgreNode->getOrientation());
		}
		return q;
	}

	OgreLocationComponentPtr OgreLocationComponent::GetParentLocation()
	{
		OgreLocationComponentPtr parent_location;
		SceneObjectPtr scene_obj  = boost::shared_static_cast<SceneObject>(GetSceneObject()->GetParent());
		while(scene_obj && !parent_location)
		{
			parent_location  = scene_obj->GetFirstComponent<OgreLocationComponent>();
			if(parent_location)
				return parent_location;
			scene_obj = boost::shared_static_cast<SceneObject>(scene_obj->GetParent());
		}
		return parent_location;

	}


	void OgreLocationComponent::SetAttachToParent(bool value)
	{
		m_AttachToParent = value;
	}
	bool OgreLocationComponent::GetAttachToParent() const
	{
		return m_AttachToParent;
	}

}
