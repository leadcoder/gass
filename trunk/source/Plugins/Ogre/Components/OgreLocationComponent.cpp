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

#include "Plugins/Ogre/Components/OgreLocationComponent.h"
#include <OgreSceneNode.h>
#include <OgreSceneManager.h>


#include "Core/Math/GASSQuaternion.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Utils/GASSLogManager.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"
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
		m_OgreNode (NULL),
		m_QRot()
	{
	}

	OgreLocationComponent::~OgreLocationComponent()
	{

	}

	void OgreLocationComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("LocationComponent",new Creator<OgreLocationComponent, IComponent>);
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("Component used to handle object position, rotation and scale", OF_VISIBLE)));
		
		RegisterProperty<Vec3>("Position", &GASS::OgreLocationComponent::GetPosition, &GASS::OgreLocationComponent::SetPosition,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Postion relative to parent node",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<Vec3>("Rotation", &GASS::OgreLocationComponent::GetEulerRotation, &GASS::OgreLocationComponent::SetEulerRotation,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Rotation relative to parent node, x = heading, y=pitch, z=roll [Degrees]",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<Quaternion>("Quaternion", &GASS::OgreLocationComponent::GetRotation, &GASS::OgreLocationComponent::SetRotation,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Rotation represented as Quaternion",PF_VISIBLE)));
		RegisterProperty<Vec3>("Scale", &GASS::OgreLocationComponent::GetScale, &GASS::OgreLocationComponent::SetScale,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Scale relative to parent node",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<bool>("AttachToParent", &GASS::OgreLocationComponent::GetAttachToParent, &GASS::OgreLocationComponent::SetAttachToParent,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("If true this node is attached to first ancestor SceneObject that has a location component",PF_VISIBLE | PF_EDITABLE)));
	}

	void OgreLocationComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreLocationComponent::PositionMessage,GASS::PositionMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreLocationComponent::OnScaleMessage,GASS::ScaleMessage,0));
		
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreLocationComponent::RotationMessage,GASS::RotationMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreLocationComponent::WorldPositionMessage,GASS::WorldPositionMessage ,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreLocationComponent::WorldRotationMessage,GASS::WorldRotationMessage ,0));

		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreLocationComponent::ParentChangedMessage,GASS::ParentChangedMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreLocationComponent::VisibilityMessage,GASS::VisibilityMessage ,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreLocationComponent::BoundingInfoMessage, GASS::BoundingInfoMessage ,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreLocationComponent::OnUpdateEulerAngles, GASS::UpdateEulerAnglesMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreLocationComponent::OnAttachToParent,GASS::AttachToParentMessage,0));
	
		OgreGraphicsSceneManagerPtr ogsm =  GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<OgreGraphicsSceneManager>();
		assert(ogsm);
		Ogre::SceneManager* sm = ogsm->GetOgreSceneManager();

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
		m_OgreNode->setUserAny(Ogre::Any(this));

		MessagePtr pos_msg(new GASS::PositionMessage(m_Pos));
		MessagePtr rot_msg;
		if(m_Rot != Vec3(0,0,0))
			rot_msg =MessagePtr(new GASS::RotationMessage(Quaternion(Math::Deg2Rad(m_Rot))));
		else //use 
			rot_msg = MessagePtr(new GASS::RotationMessage(m_QRot));

		LocationComponentPtr location = DYNAMIC_PTR_CAST<ILocationComponent>( shared_from_this());
		GetSceneObject()->PostMessage(MessagePtr(new LocationLoadedMessage(location)));
		
		
		GetSceneObject()->PostMessage(pos_msg);
		GetSceneObject()->PostMessage(rot_msg);

		//std::cout << "Pos:" << m_Pos.x << " " << m_Pos.y << " " << m_Pos.z << std::endl;
	}

	void OgreLocationComponent::OnDelete()
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


	void OgreLocationComponent::OnAttachToParent(AttachToParentMessagePtr message)
	{
		SetAttachToParent(message->GetAttachToParent());
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
			m_OgreNode->setPosition(OgreConvert::ToOgre(m_Pos));
		}
	}

	void OgreLocationComponent::OnScaleMessage(ScaleMessagePtr message)
	{
		SetScale(message->GetScale());
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
			Ogre::Vector3 opos = OgreConvert::ToOgre(pos);
			Ogre::Node* op = m_OgreNode->getParent();
			if(op) //check that we dont have sm root node?
			{
				Ogre::Matrix4 trans = op->_getFullTransform();
				Ogre::Matrix4 inv_trans = trans.inverse();
				opos = inv_trans*opos;
			}
			m_OgreNode->setPosition(opos);
			m_Pos = OgreConvert::ToGASS(opos);
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
			Ogre::Quaternion orot = OgreConvert::ToOgre(rot);
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
			m_QRot = OgreConvert::ToGASS(orot);
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
		if(m_OgreNode) m_OgreNode->setScale(OgreConvert::ToOgre(value));
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
			pos = OgreConvert::ToGASS(m_OgreNode->_getDerivedPosition());
		}
		return pos;
	}


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
		m_QRot = value;
		if(m_OgreNode)
		{
			m_OgreNode->setOrientation(OgreConvert::ToOgre(value));
		}
	}

	Quaternion OgreLocationComponent::GetRotation() const
	{
		return m_QRot;
		/*Quaternion q;
		if(m_OgreNode)
		{
			q = OgreConvert::ToGASS(m_OgreNode->getOrientation());
		}
		return q;*/
	}

	Quaternion OgreLocationComponent::GetWorldRotation() const
	{
		Quaternion q;
		if(m_OgreNode)
		{
			q = OgreConvert::ToGASS(m_OgreNode->_getDerivedOrientation());
		}
		return q;
	}

	void OgreLocationComponent::OnUpdateEulerAngles(UpdateEulerAnglesMessagePtr message)
	{
		Quaternion q;
		if(m_OgreNode)
		{
			q = OgreConvert::ToGASS(m_OgreNode->getOrientation());
			Mat4 rot_mat;
			q.ToRotationMatrix(rot_mat);
			m_Rot.x = Math::Rad2Deg(rot_mat.GetEulerHeading());
			m_Rot.y = Math::Rad2Deg(rot_mat.GetEulerPitch());
			m_Rot.z = Math::Rad2Deg(rot_mat.GetEulerRoll());
		}
	}

	OgreLocationComponentPtr OgreLocationComponent::GetParentLocation()
	{
		OgreLocationComponentPtr parent_location;
		SceneObjectPtr scene_obj  = STATIC_PTR_CAST<SceneObject>(GetSceneObject()->GetParent());
		while(scene_obj && !parent_location)
		{
			parent_location  = scene_obj->GetFirstComponentByClass<OgreLocationComponent>();
			if(parent_location)
				return parent_location;
			scene_obj = STATIC_PTR_CAST<SceneObject>(scene_obj->GetParent());
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
			GetSceneObject()->GetScene()->PostMessage(SceneMessagePtr(new SceneObjectChangedParentEvent(GetSceneObject())));
		}
	}

	bool OgreLocationComponent::GetAttachToParent() const
	{
		return m_AttachToParent;
	}

	void OgreLocationComponent::nodeUpdated(const Ogre::Node* node)
	{
		//send transformation message
		Vec3 pos = OgreConvert::ToGASS(m_OgreNode->_getDerivedPosition());
		Vec3 scale = OgreConvert::ToGASS(m_OgreNode->_getDerivedScale());
		Quaternion rot = OgreConvert::ToGASS(m_OgreNode->_getDerivedOrientation());

		MessagePtr trans_msg(new TransformationNotifyMessage(pos,rot,scale));
		GetSceneObject()->PostMessage(trans_msg);

	/*	MessagePtr debug_msg(new Message(SimSystemManager::SYSTEM_RM_DEBUG_PRINT,from_id));
		debug_msg->SetData("Text",GetSceneObject()->GetName());
		SimEngine::Get().GetSimSystemManager()->SendImmediate(debug_msg);*/

	}
}
