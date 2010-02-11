/****************************************************************************
* This file is part of GASS.                                                *
* See http://code.google.com/p/gass/                                 *
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
#include "Core/MessageSystem/IMessage.h"
#include "Core/Utils/Log.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/SimEngine.h"
#include "Sim/Systems/SimSystemManager.h"
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

		if(m_OgreNode)
		{
			m_OgreNode->setListener(NULL);
			Ogre::SceneNode* parent = m_OgreNode->getParentSceneNode();
			if(parent)
			{
				parent->removeChild(m_OgreNode);
			}
			else
			{
				Ogre::SceneManager* sm = m_OgreNode->getCreator();
				sm->destroySceneNode(m_OgreNode->getName());
			}

		}

	}

	void OgreLocationComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("LocationComponent",new Creator<OgreLocationComponent, IComponent>);
		RegisterProperty<Vec3>("Position", &GASS::OgreLocationComponent::GetPosition, &GASS::OgreLocationComponent::SetPosition);
		RegisterProperty<Vec3>("Rotation", &GASS::OgreLocationComponent::GetEulerRotation, &GASS::OgreLocationComponent::SetEulerRotation);
		RegisterProperty<Vec3>("Scale", &GASS::OgreLocationComponent::GetScale, &GASS::OgreLocationComponent::SetScale);
		RegisterProperty<bool>("AttachToParent", &GASS::OgreLocationComponent::GetAttachToParent, &GASS::OgreLocationComponent::SetAttachToParent);

		//RegisterProperty<int>("InitPriority", &LocationComponent::GetInitPriority, &LocationComponent::SetInitPriority);
	}

	void OgreLocationComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreLocationComponent::OnLoad,GASS::LoadGFXComponentsMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreLocationComponent::PositionMessage,GASS::PositionMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreLocationComponent::RotationMessage,GASS::RotationMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreLocationComponent::WorldPositionMessage,GASS::WorldPositionMessage ,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreLocationComponent::WorldRotationMessage,GASS::WorldRotationMessage ,0));

		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreLocationComponent::ParentChangedMessage,GASS::ParentChangedMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreLocationComponent::VisibilityMessage,GASS::VisibilityMessage ,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreLocationComponent::BoundingInfoMessage, GASS::BoundingInfoMessage ,0));
	}

	void OgreLocationComponent::OnLoad(LoadGFXComponentsMessagePtr message)
	{
		OgreGraphicsSceneManagerPtr ogsm = boost::shared_static_cast<OgreGraphicsSceneManager>(message->GetGFXSceneManager());
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

		//update scale
		SetScale(m_Scale);

		m_OgreNode->setListener(this);

		MessagePtr pos_msg(new GASS::PositionMessage(m_Pos));
		MessagePtr rot_msg(new GASS::RotationMessage(Quaternion(Math::Deg2Rad(m_Rot))));
		GetSceneObject()->PostMessage(pos_msg);
		GetSceneObject()->PostMessage(rot_msg);
		
		//std::cout << "Pos:" << m_Pos.x << " " << m_Pos.y << " " << m_Pos.z << std::endl;
	}

	void OgreLocationComponent::ParentChangedMessage(ParentChangedMessagePtr message)
	{
		SetAttachToParent(m_AttachToParent);
	}

	void OgreLocationComponent::BoundingInfoMessage(BoundingInfoMessagePtr message)
	{
		if(m_OgreNode)
		{
			m_OgreNode->showBoundingBox(message->ShowBoundingBox());
		}
	}

	void OgreLocationComponent::PositionMessage(PositionMessagePtr message)
	{
		m_Pos = message->GetPosition();
		if(m_OgreNode)
		{
			m_OgreNode->setPosition(Convert::ToOgre(m_Pos));
		}
	}

	void OgreLocationComponent::WorldPositionMessage(WorldPositionMessagePtr message)
	{
		Vec3 pos = message->GetPosition();
		SetWorldPosition(pos);
	}

	void OgreLocationComponent::SetWorldPosition(const Vec3 &pos)
	{
		if(m_OgreNode)
		{
			Ogre::Vector3 opos = Convert::ToOgre(pos);
			Ogre::Node* op = m_OgreNode->getParent();
			if(op) //check that we dont have sm root node?
			{
				Ogre::Matrix4 trans = op->_getFullTransform();
				Ogre::Matrix4 inv_trans = trans.inverse();
				opos = inv_trans*opos;
			}
			m_OgreNode->setPosition(opos);
			m_Pos = Convert::ToGASS(opos);
		}
	}

	void OgreLocationComponent::WorldRotationMessage(WorldRotationMessagePtr message)
	{
		Quaternion q = message->GetRotation();
		SetWorldRotation(q);
	}

	void OgreLocationComponent::SetWorldRotation(const Quaternion &rot)
	{
		if(m_OgreNode)
		{
			Ogre::Quaternion orot = Convert::ToOgre(rot);
			Ogre::Node* op = m_OgreNode->getParent();
			if(op) //check that we dont have sm root node?
			{
				Ogre::Matrix4 trans = op->_getFullTransform();
				Ogre::Matrix4 inv_trans = trans.inverse();
				Ogre::Matrix3 rot_mat;
				orot.ToRotationMatrix(rot_mat);
				Ogre::Matrix3 tmp;
				inv_trans.extract3x3Matrix(tmp);
				rot_mat = tmp*rot_mat;
				orot.FromRotationMatrix(rot_mat);
			}
			m_OgreNode->setOrientation(orot);
		}
	}


	void OgreLocationComponent::RotationMessage(RotationMessagePtr message)
	{
		SetRotation(message->GetRotation());
	}

	void OgreLocationComponent::VisibilityMessage(VisibilityMessagePtr message)
	{
		SetVisibility(message->GetValue());
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
		
			MessagePtr pos_msg(new GASS::PositionMessage(value));
			GetSceneObject()->PostMessage(pos_msg);
		}
		m_Pos = value;
	}

	Vec3 OgreLocationComponent::GetPosition() const
	{
		return m_Pos;
	}

	Vec3 OgreLocationComponent::GetWorldPosition() const
	{
		Vec3 pos = m_Pos;
		if(m_OgreNode)
		{
			pos = Convert::ToGASS(m_OgreNode->_getDerivedPosition());
		}
		return pos;
	}



	//nodeUpdated
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
			MessagePtr rot_msg(new GASS::RotationMessage(Quaternion(Math::Deg2Rad(value))));
			GetSceneObject()->PostMessage(rot_msg);
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

	Quaternion OgreLocationComponent::GetWorldRotation() const
	{
		Quaternion q;
		if(m_OgreNode)
		{
			q = Convert::ToGASS(m_OgreNode->_getDerivedOrientation());
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
		if(m_OgreNode)
		{
			Vec3 world_pos = GetWorldPosition();
			Quaternion world_rot = GetWorldRotation();
			if(m_OgreNode->getParent())
				m_OgreNode->getParent()->removeChild(m_OgreNode);

			OgreLocationComponentPtr parent = GetParentLocation();
			if(parent && value)
				parent->GetOgreNode()->addChild(m_OgreNode);
			else
				m_OgreNode->getCreator()->getRootSceneNode()->addChild(m_OgreNode );

			//should we preserve old world position and orientenation???
			SetWorldPosition(world_pos);
			SetWorldRotation(world_rot);
			m_OgreNode->needUpdate();
		}
	}

	bool OgreLocationComponent::GetAttachToParent() const
	{
		return m_AttachToParent;
	}

	void OgreLocationComponent::nodeUpdated(const Ogre::Node* node)
	{
		//send transformation message
		Vec3 pos = Convert::ToGASS(m_OgreNode->_getDerivedPosition());
		Vec3 scale = Convert::ToGASS(m_OgreNode->_getDerivedScale());
		Quaternion rot = Convert::ToGASS(m_OgreNode->_getDerivedOrientation());

		MessagePtr trans_msg(new TransformationNotifyMessage(pos,rot,scale));
		GetSceneObject()->PostMessage(trans_msg);

	/*	MessagePtr debug_msg(new Message(SimSystemManager::SYSTEM_RM_DEBUG_PRINT,from_id));
		debug_msg->SetData("Text",GetSceneObject()->GetName());
		SimEngine::Get().GetSimSystemManager()->SendImmediate(debug_msg);*/

	}
}
