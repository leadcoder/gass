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

#include "Plugins/OSG/Components/OSGLocationComponent.h"
#include "Plugins/OSG/Components/OSGManualMeshComponent.h"
#include "Plugins/OSG/OSGGraphicsSceneManager.h"
#include "Plugins/OSG/OSGConvert.h"
#include "Sim/GASSScriptManager.h"
#include "Core/Math/GASSMath.h"
#include <angelscript.h>

namespace GASS
{
	void CheckASReturn(int ret_val, const std::string &error_message = "")
	{
		if (ret_val < 0)
			GASS_EXCEPT(Exception::ERR_INTERNAL_ERROR, "Failed register:" + error_message, "OSGLocationComponent::RegisterReflection");
	}

	OSGLocationComponent::OSGLocationComponent() : m_Pos(0, 0, 0),
		m_EulerRot(0, 0, 0),
		m_Scale(1, 1, 1),
		m_AttachToParent(false),
		m_NodeMask(0)
	{

	}

	OSGLocationComponent::~OSGLocationComponent()
	{

	}

	void OSGLocationComponent::OnDelete()
	{
		if (m_TransformNode.valid() && m_TransformNode->getNumParents() > 0)
		{
			osg::Group* parent = m_TransformNode->getParent(0);
			parent->removeChild(m_TransformNode.get());
		}
	}


	void OSGLocationComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("LocationComponent", new Creator<OSGLocationComponent, Component>);
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("Component used to handle object position, rotation and scale", OF_VISIBLE)));

		RegisterProperty<Vec3>("Position", &GASS::OSGLocationComponent::GetPosition, &GASS::OSGLocationComponent::SetPosition,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Position relative to parent node", PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<EulerRotation>("Rotation", &GASS::OSGLocationComponent::GetEulerRotation, &GASS::OSGLocationComponent::SetEulerRotation,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Rotation relative to parent node, heading = Y-axis rotation, pitch = X-axis rotation, roll= Z-axis rotation [Degrees]", PF_VISIBLE | PF_EDITABLE)));
		
		RegisterProperty<Quaternion>("Quaternion", &GASS::OSGLocationComponent::GetRotation, &GASS::OSGLocationComponent::SetRotation,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Rotation represented as Quaternion", PF_VISIBLE)));

		RegisterProperty<Vec3>("Scale", &GASS::OSGLocationComponent::GetScale, &GASS::OSGLocationComponent::SetScale,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Scale relative to parent node", PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<bool>("AttachToParent", &GASS::OSGLocationComponent::GetAttachToParent, &GASS::OSGLocationComponent::SetAttachToParent,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Should this location be relative to parent or not", PF_VISIBLE | PF_EDITABLE)));

		//expose this component to script engine
		asIScriptEngine *engine = SimEngine::Get().GetScriptManager()->GetEngine();

		int r;
		//r = engine->RegisterObjectType("ILocationComponent", 0, asOBJ_REF | asOBJ_NOCOUNT); CheckASReturn(r);
		r = engine->RegisterObjectType("LocationComponent", 0, asOBJ_REF | asOBJ_NOCOUNT); CheckASReturn(r);


		/*r = engine->RegisterObjectBehaviour("ILocationComponent", asBEHAVE_REF_CAST, "LocationComponent@ f()", asFUNCTION((refCast<ILocationComponent,OSGLocationComponent>)), asCALL_CDECL_OBJLAST); CheckASReturn(r);
		r = engine->RegisterObjectBehaviour("LocationComponent", asBEHAVE_IMPLICIT_REF_CAST, "ILocationComponent@ f()", asFUNCTION((refCast<OSGLocationComponent,ILocationComponent>)), asCALL_CDECL_OBJLAST); CheckASReturn(r);
		r = engine->RegisterObjectMethod("ILocationComponent", "Vec3 GetPosition() const", asMETHODPR(ILocationComponent, GetPosition,() const, Vec3), asCALL_THISCALL);CheckASReturn(r);*/

		r = engine->RegisterObjectBehaviour("BaseSceneComponent", asBEHAVE_REF_CAST, "LocationComponent@ f()", asFUNCTION((refCast<BaseSceneComponent, OSGLocationComponent>)), asCALL_CDECL_OBJLAST); CheckASReturn(r);
		r = engine->RegisterObjectBehaviour("LocationComponent", asBEHAVE_IMPLICIT_REF_CAST, "BaseSceneComponent@ f()", asFUNCTION((refCast<OSGLocationComponent, BaseSceneComponent>)), asCALL_CDECL_OBJLAST); CheckASReturn(r);

		r = engine->RegisterObjectMethod("LocationComponent", "string GetName() const", asMETHOD(Component, GetName), asCALL_THISCALL); CheckASReturn(r);
		r = engine->RegisterObjectMethod("LocationComponent", "void SetAttachToParent(bool) ", asMETHOD(OSGLocationComponent, SetAttachToParent), asCALL_THISCALL); CheckASReturn(r);
		r = engine->RegisterObjectMethod("LocationComponent", "bool GetAttachToParent() const", asMETHOD(OSGLocationComponent, GetAttachToParent), asCALL_THISCALL); CheckASReturn(r);
		r = engine->RegisterObjectMethod("LocationComponent", "Vec3 GetPosition() const", asMETHODPR(OSGLocationComponent, GetPosition, () const, Vec3), asCALL_THISCALL); CheckASReturn(r);
		//r = engine->RegisterObjectMethod("LocationComponent", "void SetPosition(const Vec3 &in) ", asMETHOD(OSGLocationComponent, SetPosition), asCALL_THISCALL);CheckASReturn(r);
		//r = engine->RegisterObjectMethod("LocationComponent", "Vec3 GetPosition() const", 
		//	asSMethodPtr<sizeof(void (GASS::OSGLocationComponent::*)())>::Convert((void (GASS::OSGLocationComponent::*)())(&GASS::OSGLocationComponent::GetPosition)),
		//	asCALL_THISCALL);CheckASReturn(r);
	}

	void OSGLocationComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGLocationComponent::OnPositionMessage, PositionRequest, 0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGLocationComponent::OnRotationMessage, RotationRequest, 0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGLocationComponent::OnWorldPositionRequest, WorldPositionRequest, 0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGLocationComponent::OnWorldRotationMessage, WorldRotationRequest, 0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGLocationComponent::OnVisibilityMessage, LocationVisibilityRequest, 0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGLocationComponent::OnScaleMessage, ScaleRequest, 0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGLocationComponent::OnParentChangedMessage, GASS::ParentChangedEvent, 0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGLocationComponent::OnAttachToParent, GASS::AttachToParentRequest, 0));

		OSGGraphicsSceneManagerPtr  scene_man = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<OSGGraphicsSceneManager>();
		//assert(scene_man);
		if (!m_TransformNode.valid())
		{
			m_TransformNode = new osg::PositionAttitudeTransform();
			
			osg::ref_ptr<osg::Group> root_node = scene_man->GetOSGShadowRootNode();
			m_GFXSceneManager = scene_man;

			if (m_AttachToParent)
			{
				OSGLocationComponentPtr parent = _GetParentLocation();
				if (parent)
				{
					parent->GetOSGNode()->addChild(m_TransformNode.get());
				}
				else
					root_node->addChild(m_TransformNode.get());
			}
			else
			{
				//HACK: do extra check to see if we should avoid shadow node...
				OSGManualMeshComponentPtr mm_comp = GetSceneObject()->GetFirstComponentByClass<OSGManualMeshComponent>();
				if (mm_comp && !mm_comp->GetCastShadow() && !mm_comp->GetReceiveShadow())
				{
					scene_man->GetOSGRootNode()->addChild(m_TransformNode.get());
				}
				else
					root_node->addChild(m_TransformNode.get());
			}
		}
		else
		{
			const std::string name = GetSceneObject()->GetName();
			m_TransformNode->setName(name);
		}
		LocationComponentPtr location = GASS_DYNAMIC_PTR_CAST<ILocationComponent>(shared_from_this());
		GetSceneObject()->PostEvent(LocationLoadedEventPtr(new LocationLoadedEvent(location)));

		PositionRequestPtr pos_msg(new PositionRequest(m_Pos));
		RotationRequestPtr rot_msg;

		//Check if rotation provided?
		if (m_EulerRot.Heading != 0 && 
			m_EulerRot.Pitch != 0 && 
			m_EulerRot.Roll != 0)
			rot_msg = RotationRequestPtr(new GASS::RotationRequest(m_EulerRot.GetQuaternion()));
		else
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
		m_Pos = message->GetPosition();
		if (m_TransformNode.valid())
		{
			m_TransformNode->setPosition(OSGConvert::ToOSG(m_Pos));
			_SendTransMessage();
		}
	}

	void OSGLocationComponent::OnWorldPositionRequest(WorldPositionRequestPtr message)
	{
		SetWorldPosition(message->GetPosition());
	}

	void OSGLocationComponent::OnRotationMessage(RotationRequestPtr message)
	{
		if (m_TransformNode.valid())
		{
			const osg::Quat final = OSGConvert::ToOSG(message->GetRotation());
			m_TransformNode->setAttitude(final);
			_SendTransMessage();
		}
	}

	void OSGLocationComponent::OnWorldRotationMessage(WorldRotationRequestPtr message)
	{
		if (m_TransformNode.valid())
		{
			SetWorldRotation(message->GetRotation());
		}
	}

	void OSGLocationComponent::SetScale(const Vec3 &value)
	{
		m_Scale = value;
		if (m_TransformNode.valid())
		{
			osg::Vec3d scale = OSGConvert::ToOSG(m_Scale);

			//Why? scale should not be negative
			double y = fabs(scale.y());
			double z = fabs(scale.z());
			scale.set(scale.x(), y, z);
			m_TransformNode->setScale(scale);
		}
	}

	void OSGLocationComponent::SetPosition(const Vec3 &value)
	{
		m_Pos = value;
		if (m_TransformNode.valid())
		{
			GetSceneObject()->PostRequest(PositionRequestPtr(new PositionRequest(value)));
		}
	}

	void OSGLocationComponent::OnScaleMessage(ScaleRequestPtr message)
	{
		m_Scale = message->GetScale();
		if (m_TransformNode.valid())
		{
			osg::Vec3d scale = OSGConvert::ToOSG(m_Scale);
			double y = fabs(scale.y());
			double z = fabs(scale.z());
			scale.set(scale.x(), y, z);
			m_TransformNode->setScale(scale);
			_SendTransMessage();
		}
	}

	void OSGLocationComponent::_SendTransMessage()
	{
		const Vec3 pos = GetWorldPosition();
		const Vec3 scale = GetScale();
		const Quaternion rot = GetWorldRotation();

		GetSceneObject()->PostEvent(TransformationChangedEventPtr(new TransformationChangedEvent(pos, rot, scale)));
		//send for all child transforms also?
		GASS::ComponentContainer::ComponentContainerIterator iter = GetSceneObject()->GetChildren();
		while (iter.hasMoreElements())
		{
			SceneObjectPtr obj = GASS_STATIC_PTR_CAST<SceneObject>(iter.getNext());
			OSGLocationComponentPtr c_location = obj->GetFirstComponentByClass<OSGLocationComponent>();
			if (c_location && c_location->GetAttachToParent())
				c_location->_SendTransMessage();
		}
	}

	Vec3 OSGLocationComponent::GetPosition() const
	{
		return m_Pos;
	}

	void OSGLocationComponent::SetWorldPosition(const Vec3 &value)
	{
		if (m_TransformNode.valid())
		{
			osg::Vec3d new_pos = OSGConvert::ToOSG(value);
			if (m_TransformNode->getNumParents() > 0)
			{
				osg::PositionAttitudeTransform* parent = dynamic_cast<osg::PositionAttitudeTransform*>(m_TransformNode->getParent(0));
				if (parent)
				{
					osg::MatrixList mat_list = parent->getWorldMatrices();
					osg::Matrix world_trans;
					world_trans.identity();
					if (mat_list.size() > 0)
					{
						world_trans = mat_list[0];
					}
					osg::Matrix inv_world_trans = osg::Matrix::inverse(world_trans);
					new_pos = new_pos*inv_world_trans;
				}
			}
			m_TransformNode->setPosition(new_pos);
			m_Pos = OSGConvert::ToGASS(new_pos);
			_SendTransMessage();
		}
	}

	Vec3 OSGLocationComponent::GetWorldPosition() const
	{
		Vec3 world_pos = m_Pos;
		if (m_TransformNode.valid())
		{
			world_pos = OSGConvert::ToGASS(m_TransformNode->getPosition());
			if (m_TransformNode->getNumParents() > 0)
			{
				osg::PositionAttitudeTransform* parent = dynamic_cast<osg::PositionAttitudeTransform*>(m_TransformNode->getParent(0));
				if (parent)
				{
					osg::MatrixList mat_list = parent->getWorldMatrices();
					osg::Matrix world_trans;
					world_trans.identity();
					if (mat_list.size() > 0)
					{
						world_trans = mat_list[0];
					}
					osg::Vec3d osg_world_pos = m_TransformNode->getPosition()*world_trans;
					world_pos = OSGConvert::ToGASS(osg_world_pos);
				}
			}
		}
		return world_pos;
	}

	void OSGLocationComponent::SetRotation(const Quaternion &value)
	{
		m_QRot = value;
		if (m_TransformNode.valid())
		{
			GetSceneObject()->PostRequest(RotationRequestPtr(new RotationRequest(Quaternion(value))));
		}
	}

	void OSGLocationComponent::SetEulerRotation(const EulerRotation &value)
	{
		m_EulerRot = value;
		if (m_TransformNode.valid())
		{
			GetSceneObject()->PostRequest(RotationRequestPtr(new GASS::RotationRequest(m_EulerRot.GetQuaternion())));
		}
	}

	EulerRotation OSGLocationComponent::GetEulerRotation() const
	{
		return m_EulerRot;
	}

	Quaternion OSGLocationComponent::GetRotation() const
	{
		Quaternion q = m_QRot;
		if (m_TransformNode.valid())
		{
			q = OSGConvert::ToGASS(m_TransformNode->getAttitude());
		}
		return q;
	}

	void OSGLocationComponent::SetWorldRotation(const Quaternion &value)
	{
		if (m_TransformNode.valid())
		{
			osg::Quat final = OSGConvert::ToOSG(value);

			if (m_TransformNode->getNumParents() > 0)
			{
				osg::PositionAttitudeTransform* parent = dynamic_cast<osg::PositionAttitudeTransform*>(m_TransformNode->getParent(0));
				if (parent)
				{
					osg::MatrixList mat_list = parent->getWorldMatrices();
					osg::Matrix world_trans;
					world_trans.identity();
					if (mat_list.size() > 0)
					{
						world_trans = mat_list[0];
					}
					osg::Matrix inv_world_trans = osg::Matrix::inverse(world_trans);

					osg::Quat inv_rot;
					inv_rot = inv_world_trans.getRotate();
					final = final*inv_rot;
				}
			}
			m_TransformNode->setAttitude(final);
			_SendTransMessage();
		}
	}

	Quaternion OSGLocationComponent::GetWorldRotation() const
	{
		Quaternion q = Quaternion::IDENTITY;

		if (m_TransformNode.valid())
		{
			osg::Quat rot = m_TransformNode->getAttitude();
			if (m_TransformNode->getNumParents() > 0)
			{
				osg::PositionAttitudeTransform* parent = dynamic_cast<osg::PositionAttitudeTransform*>(m_TransformNode->getParent(0));
				if (parent)
				{
					osg::MatrixList mat_list = parent->getWorldMatrices();
					osg::Matrix world_trans;
					world_trans.identity();
					if (mat_list.size() > 0)
					{
						world_trans = mat_list[0];
					}

					osg::Quat parent_rot;
					parent_rot = world_trans.getRotate();
					rot = rot*parent_rot;
				}
			}
			q = OSGConvert::ToGASS(rot);
		}
		return q;
	}

	void OSGLocationComponent::operator()(osg::Node* node, osg::NodeVisitor* nv)
	{
		traverse(node, nv);
	}

	void OSGLocationComponent::OnVisibilityMessage(LocationVisibilityRequestPtr message)
	{
		bool visibility = message->GetValue();
		if (visibility)
		{
			//m_TransformNode->setNodeMask(NM_VISIBLE | m_TransformNode->getNodeMask());
			m_TransformNode->setNodeMask(~0u);
		}
		else
		{
			m_TransformNode->setNodeMask(0u);
			//m_TransformNode->setNodeMask(~NM_VISIBLE & m_TransformNode->getNodeMask());
		}
	}

	void OSGLocationComponent::SetAttachToParent(bool value)
	{
		m_AttachToParent = value;
		if (m_TransformNode.valid())
		{
			Vec3 world_pos = GetWorldPosition();
			Quaternion world_rot = GetWorldRotation();
			if (m_TransformNode->getParent(0))
				m_TransformNode->getParent(0)->removeChild(m_TransformNode);

			OSGLocationComponentPtr parent = _GetParentLocation();
			if (parent && value)
				parent->GetOSGNode()->addChild(m_TransformNode);
			else
			{
				OSGGraphicsSceneManagerPtr scene_man = m_GFXSceneManager.lock();
				if (scene_man)
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

	OSGLocationComponentPtr OSGLocationComponent::_GetParentLocation()
	{
		OSGLocationComponentPtr parent_location;
		SceneObjectPtr scene_obj = GASS_STATIC_PTR_CAST<SceneObject>(GetSceneObject()->GetParent());
		while (scene_obj && !parent_location)
		{
			parent_location = scene_obj->GetFirstComponentByClass<OSGLocationComponent>();
			if (parent_location)
				return parent_location;
			scene_obj = GASS_STATIC_PTR_CAST<SceneObject>(scene_obj->GetParent());
		}
		return parent_location;
	}
}