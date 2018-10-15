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

		//Set values
		SetPosition(m_Pos);
		Quaternion rot;
		//Check if rotation provided?
		if (m_EulerRot.Heading != 0 &&
			m_EulerRot.Pitch != 0 &&
			m_EulerRot.Roll != 0)
			rot = m_EulerRot.GetQuaternion();
		else
			rot = m_QRot;
		SetRotation(rot);

		LocationComponentPtr location = GASS_DYNAMIC_PTR_CAST<ILocationComponent>(shared_from_this());
		GetSceneObject()->PostEvent(LocationLoadedEventPtr(new LocationLoadedEvent(location)));
		//GetSceneObject()->PostRequest(ScaleRequestPtr(new ScaleRequest(m_Scale)));
	}

	void OSGLocationComponent::OnAttachToParent(AttachToParentRequestPtr message)
	{
		SetAttachToParent(message->GetAttachToParent());
	}

	Vec3 OSGLocationComponent::_WorldToLocal(const Vec3 &world_pos) const
	{
		if (!m_AttachToParent)
		{
			return world_pos;
		}
		else
		{
			//update relative position
			OSGLocationComponentPtr parent = _GetParentLocation();
			if (parent)
			{
				const Vec3 parent_world_pos = parent->GetWorldPosition();
				const Quaternion parent_world_rot = parent->GetWorldRotation();
				GASS::Mat4 parent_trans_mat(parent_world_rot, parent_world_pos);
				GASS::Mat4 inv_parent_trans_mat = parent_trans_mat.GetInvert();
				GASS::Mat4 trans_mat(GetWorldRotation(), world_pos);
				return inv_parent_trans_mat*world_pos;
			}
			else
			{
				return world_pos;
			}
		}
	}

	Quaternion OSGLocationComponent::_WorldToLocal(const Quaternion &world_rot) const
	{
		if (!m_AttachToParent)
		{
			return world_rot;
		}
		else
		{
			//update relative position
			OSGLocationComponentPtr parent = _GetParentLocation();
			if (parent)
			{
				//const Vec3 parent_world_pos = parent->GetWorldPosition();
				const Quaternion parent_world_rot = parent->GetWorldRotation();
				const Quaternion inv_parent_world_rot = parent_world_rot.Inverse();
				return inv_parent_world_rot*world_rot;
			}
			else
			{
				return world_rot;
			}
		}
	}

	Quaternion OSGLocationComponent::_LocalToWorld(const Quaternion &local_rot) const
	{
		OSGLocationComponentPtr parent = _GetParentLocation();
		if (!m_AttachToParent)
		{
			return local_rot;
		}
		else
		{
			if (parent)
			{
				const Quaternion parent_world_rot = parent->GetWorldRotation();
				return parent_world_rot*local_rot;
			}
			else
			{
				return local_rot;
			}
		}
	}

	Vec3 OSGLocationComponent::_LocalToWorld(const Vec3 &local_pos) const
	{
		OSGLocationComponentPtr parent = _GetParentLocation();
		if (!m_AttachToParent)
		{
			return local_pos;
		}
		else
		{
			//update world position
			if (parent)
			{
				const Vec3 parent_world_pos = parent->GetWorldPosition();
				const Quaternion parent_world_rot = parent->GetWorldRotation();
				GASS::Mat4 parent_trans_mat(parent_world_rot, parent_world_pos);
				return parent_trans_mat*local_pos;
			}
			else
			{
				return local_pos;
			}
		}
	}
	

	void OSGLocationComponent::OnWorldPositionRequest(WorldPositionRequestPtr message)
	{
		SetWorldPosition(message->GetPosition());
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
			m_WorldPosition = _LocalToWorld(m_Pos);

			//update osg transform
			m_TransformNode->setPosition(OSGConvert::ToOSG(m_Pos));

			_NotifyTransformationChange();

			//update children
			_OnPositionUpdate(GetSceneObject());
		}
	}

	void OSGLocationComponent::OnPositionMessage(PositionRequestPtr message)
	{
		SetPosition(message->GetPosition());
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
			_NotifyTransformationChange();
		}
	}
	
	Vec3 OSGLocationComponent::GetPosition() const
	{
		return m_Pos;
	}

	void OSGLocationComponent::_NotifyTransformationChange() const
	{
		const Vec3 scale = GetScale();
		const Quaternion rot = GetWorldRotation();
		//GetSceneObject()->PostEvent(TransformationChangedEventPtr(new TransformationChangedEvent(m_WorldPosition, rot, scale)));
		GetSceneObject()->SendImmediateEvent(TransformationChangedEventPtr(new TransformationChangedEvent(m_WorldPosition, rot, scale)));
	}

	void OSGLocationComponent::_OnParentPositionUpdated(OSGLocationComponentPtr parent_location)
	{
		if (m_AttachToParent)
		{
			const Vec3 parent_world_pos = parent_location->GetWorldPosition();
			const Quaternion parent_world_rot = parent_location->GetWorldRotation();
			//reflect new world position
			m_WorldPosition = GASS::Mat4(parent_world_rot, parent_world_pos) * GetPosition();
			_NotifyTransformationChange();
		}
	}

	void OSGLocationComponent::_OnPositionUpdate(SceneObjectPtr scene_object, OSGLocationComponentPtr parent_location)
	{
		OSGLocationComponentPtr current_location = scene_object->GetFirstComponentByClass<OSGLocationComponent>();

		if (current_location)
		{
			if (parent_location)
				current_location->_OnParentPositionUpdated(parent_location);
			//update parent location component
			parent_location = current_location;
		}
		
		//Update ALL children
		GASS::ComponentContainer::ComponentContainerIterator iter = scene_object->GetChildren();
		while (iter.hasMoreElements())
		{
			SceneObjectPtr obj = GASS_STATIC_PTR_CAST<SceneObject>(iter.getNext());
			_OnPositionUpdate(obj, parent_location);
		}
	}

	void OSGLocationComponent::_OnParentRotationUpdated(OSGLocationComponentPtr parent_location)
	{
		if (m_AttachToParent)
		{
			const Vec3 parent_world_pos = parent_location->GetWorldPosition();
			const Quaternion parent_world_rot = parent_location->GetWorldRotation();
			
			//reflect new world position
			m_WorldPosition = GASS::Mat4(parent_world_rot, parent_world_pos) * GetPosition();
			
			//reflect new world rotation
			m_WorldRotation = parent_world_rot * GetRotation();
			
			_NotifyTransformationChange();
		}
	}

	void OSGLocationComponent::_OnRotationUpdate(SceneObjectPtr scene_object, OSGLocationComponentPtr parent_location)
	{
		OSGLocationComponentPtr current_location = scene_object->GetFirstComponentByClass<OSGLocationComponent>();

		if (current_location)
		{
			if (parent_location)
				current_location->_OnParentRotationUpdated(parent_location);
			//update parent location component
			parent_location = current_location;
		}

		//Update ALL children
		GASS::ComponentContainer::ComponentContainerIterator iter = scene_object->GetChildren();
		while (iter.hasMoreElements())
		{
			SceneObjectPtr obj = GASS_STATIC_PTR_CAST<SceneObject>(iter.getNext());
			_OnRotationUpdate(obj, parent_location);
		}
	}
	
	void OSGLocationComponent::SetWorldPosition(const Vec3 &value)
	{
		m_WorldPosition = value;

		m_Pos = _WorldToLocal(m_WorldPosition);

		if (m_TransformNode.valid())
			m_TransformNode->setPosition(OSGConvert::ToOSG(m_Pos));

		_NotifyTransformationChange();

		//update children
		_OnPositionUpdate(GetSceneObject());
	}

	Vec3 OSGLocationComponent::GetWorldPosition() const
	{
		return m_WorldPosition;
	}

	void OSGLocationComponent::OnRotationMessage(RotationRequestPtr message)
	{
		SetRotation(message->GetRotation());
	}

	void OSGLocationComponent::SetRotation(const Quaternion &value)
	{
		m_QRot = value;
		if (m_TransformNode.valid())
		{
			m_WorldRotation = _LocalToWorld(m_QRot);
			//update osg transform
			if (m_TransformNode.valid())
				m_TransformNode->setAttitude(OSGConvert::ToOSG(m_QRot));

			_NotifyTransformationChange();

			//update children
			_OnRotationUpdate(GetSceneObject());
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
		return m_QRot;
	}

	void OSGLocationComponent::SetWorldRotation(const Quaternion &value)
	{
		m_WorldRotation = value;
		m_QRot = _WorldToLocal(m_WorldRotation);

		if (m_TransformNode.valid())
			m_TransformNode->setAttitude(OSGConvert::ToOSG(m_QRot));

		_NotifyTransformationChange();

		//update children
		_OnRotationUpdate(GetSceneObject());
	}

	Quaternion OSGLocationComponent::GetWorldRotation() const
	{
		return m_WorldRotation;
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

	OSGLocationComponentPtr OSGLocationComponent::_GetParentLocation() const 
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