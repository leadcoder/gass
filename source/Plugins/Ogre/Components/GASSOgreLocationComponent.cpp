/****************************************************************************
* This file is part of GASS.                                                *
* See https://github.com/leadcoder/gass                                     *
*                                                                           *
* Copyright (c) 2008-2016 GASS team. See Contributors.txt for details.      *
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

#include "Plugins/Ogre/Components/GASSOgreLocationComponent.h"
#include "Core/Math/GASSQuaternion.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Math/GASSMath.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSimEngine.h"
#include "Plugins/Ogre/GASSOgreGraphicsSceneManager.h"
#include "Plugins/Ogre/GASSOgreConvert.h"
#include "Sim/GASSScriptManager.h"
#include <angelscript.h>

namespace GASS
{
	OgreLocationComponent::OgreLocationComponent() : m_AttachToParent(false),
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
		ComponentFactory::GetPtr()->Register("LocationComponent",new Creator<OgreLocationComponent, Component>);
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

		asIScriptEngine *engine = SimEngine::Get().GetScriptManager()->GetEngine();
		int r;
		r = engine->RegisterObjectType("LocationComponent", 0, asOBJ_REF | asOBJ_NOCOUNT); assert( r >= 0 );

		r = engine->RegisterObjectBehaviour("BaseSceneComponent", asBEHAVE_REF_CAST, "LocationComponent@ f()", asFUNCTION((refCast<BaseSceneComponent,OgreLocationComponent>)), asCALL_CDECL_OBJLAST); assert( r >= 0 );
		r = engine->RegisterObjectBehaviour("LocationComponent", asBEHAVE_IMPLICIT_REF_CAST, "BaseSceneComponent@ f()", asFUNCTION((refCast<OgreLocationComponent,BaseSceneComponent>)), asCALL_CDECL_OBJLAST); assert( r >= 0 );

		r = engine->RegisterObjectMethod("LocationComponent", "string GetName() const", asMETHOD(Component, GetName), asCALL_THISCALL);assert(r >= 0);
		r = engine->RegisterObjectMethod("LocationComponent", "void SetAttachToParent(bool) ", asMETHOD(OgreLocationComponent, SetAttachToParent), asCALL_THISCALL);assert(r >= 0);
		r = engine->RegisterObjectMethod("LocationComponent", "bool GetAttachToParent() const", asMETHOD(OgreLocationComponent, GetAttachToParent), asCALL_THISCALL);assert(r >= 0);
		r = engine->RegisterObjectMethod("LocationComponent", "Vec3 GetPosition() const", asMETHOD(OgreLocationComponent, GetPosition), asCALL_THISCALL);assert(r >= 0);
		r = engine->RegisterObjectMethod("LocationComponent", "void SetPosition(const Vec3 &in)", asMETHOD(OgreLocationComponent, SetPosition), asCALL_THISCALL);assert(r >= 0);
		r = engine->RegisterObjectMethod("LocationComponent", "Vec3 GetWorldPosition() const", asMETHOD(OgreLocationComponent, GetWorldPosition), asCALL_THISCALL);assert(r >= 0);
		r = engine->RegisterObjectMethod("LocationComponent", "void SetWorldPosition(const Vec3 &in)", asMETHOD(OgreLocationComponent, SetWorldPosition), asCALL_THISCALL);assert(r >= 0);
		r = engine->RegisterObjectMethod("LocationComponent", "Quaternion GetRotation() const", asMETHOD(OgreLocationComponent, GetRotation), asCALL_THISCALL);assert(r >= 0);
		r = engine->RegisterObjectMethod("LocationComponent", "void SetRotation(const Quaternion &in)", asMETHOD(OgreLocationComponent, SetRotation), asCALL_THISCALL);assert(r >= 0);
		r = engine->RegisterObjectMethod("LocationComponent", "Quaternion GetWorldRotation() const", asMETHOD(OgreLocationComponent, GetWorldRotation), asCALL_THISCALL);assert(r >= 0);
		r = engine->RegisterObjectMethod("LocationComponent", "void SetWorldRotation(const Quaternion &in)", asMETHOD(OgreLocationComponent, SetWorldRotation), asCALL_THISCALL);assert(r >= 0);
	}

	void OgreLocationComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreLocationComponent::PositionRequest,GASS::PositionRequest,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreLocationComponent::OnScaleMessage,GASS::ScaleRequest,0));

		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreLocationComponent::RotationRequest,GASS::RotationRequest,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreLocationComponent::WorldPositionRequest,GASS::WorldPositionRequest ,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreLocationComponent::WorldRotationRequest,GASS::WorldRotationRequest ,0));

		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreLocationComponent::ParentChangedEvent,GASS::ParentChangedEvent,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreLocationComponent::VisibilityRequest,GASS::LocationVisibilityRequest ,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreLocationComponent::BoundingInfoRequest, GASS::BoundingInfoRequest ,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreLocationComponent::OnUpdateEulerAngles, GASS::UpdateEulerAnglesRequest,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OgreLocationComponent::OnAttachToParent,GASS::AttachToParentRequest,0));

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
		Ogre::Any any_this(this);
		m_OgreNode->setUserAny(any_this);

		PositionRequestPtr pos_msg(new GASS::PositionRequest(m_Pos));
		RotationRequestPtr rot_msg;
		if(m_Rot != Vec3(0,0,0))
			rot_msg =RotationRequestPtr(new GASS::RotationRequest(Quaternion(Math::Deg2Rad(m_Rot))));
		else //use
			rot_msg = RotationRequestPtr(new GASS::RotationRequest(m_QRot));

		LocationComponentPtr location = GASS_DYNAMIC_PTR_CAST<ILocationComponent>( shared_from_this());
		GetSceneObject()->PostEvent(LocationLoadedEventPtr(new LocationLoadedEvent(location)));

		GetSceneObject()->PostRequest(pos_msg);
		GetSceneObject()->PostRequest(rot_msg);
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


	void OgreLocationComponent::OnAttachToParent(AttachToParentRequestPtr message)
	{
		SetAttachToParent(message->GetAttachToParent());
	}

	void OgreLocationComponent::ParentChangedEvent(ParentChangedEventPtr message)
	{
		SetAttachToParent(m_AttachToParent);
	}

	void OgreLocationComponent::BoundingInfoRequest(BoundingInfoRequestPtr message)
	{
		if(m_OgreNode)
		{
			m_OgreNode->showBoundingBox(message->ShowBoundingBox());
		}
	}

	void OgreLocationComponent::PositionRequest(PositionRequestPtr message)
	{
		m_Pos = message->GetPosition();
		if(m_OgreNode)
		{
			m_OgreNode->setPosition(OgreConvert::ToOgre(m_Pos));
		}
	}

	void OgreLocationComponent::OnScaleMessage(ScaleRequestPtr message)
	{
		SetScale(message->GetScale());
	}

	void OgreLocationComponent::WorldPositionRequest(WorldPositionRequestPtr message)
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

	void OgreLocationComponent::WorldRotationRequest(WorldRotationRequestPtr message)
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

	void OgreLocationComponent::RotationRequest(RotationRequestPtr message)
	{
		SetRotation(message->GetRotation());
	}

	void OgreLocationComponent::VisibilityRequest(LocationVisibilityRequestPtr message)
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
		if(m_OgreNode) //initialized?
		{
			GetSceneObject()->PostRequest(PositionRequestPtr(new GASS::PositionRequest(value)));
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
		if(m_OgreNode) //initialized?
		{
			GetSceneObject()->PostRequest(RotationRequestPtr(new GASS::RotationRequest(Quaternion(Math::Deg2Rad(value)))));
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

	void OgreLocationComponent::OnUpdateEulerAngles(UpdateEulerAnglesRequestPtr message)
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
		SceneObjectPtr scene_obj  = GASS_STATIC_PTR_CAST<SceneObject>(GetSceneObject()->GetParent());
		while(scene_obj && !parent_location)
		{
			parent_location  = scene_obj->GetFirstComponentByClass<OgreLocationComponent>();
			if(parent_location)
				return parent_location;
			scene_obj = GASS_STATIC_PTR_CAST<SceneObject>(scene_obj->GetParent());
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

			//should we preserve old world position and orientation???
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

	void OgreLocationComponent::nodeUpdated(const Ogre::Node* /*node*/)
	{
		//send transformation message
		Vec3 pos = OgreConvert::ToGASS(m_OgreNode->_getDerivedPosition());
		Vec3 scale = OgreConvert::ToGASS(m_OgreNode->_getDerivedScale());
		Quaternion rot = OgreConvert::ToGASS(m_OgreNode->_getDerivedOrientation());
		GetSceneObject()->PostEvent(TransformationChangedEventPtr(new TransformationChangedEvent(pos,rot,scale)));
	}
}