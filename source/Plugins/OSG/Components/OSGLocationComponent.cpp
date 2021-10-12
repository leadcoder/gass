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

	OSGLocationComponent::OSGLocationComponent() : m_Position(0, 0, 0),
		m_EulerRotation(0, 0, 0),
		m_Scale(1, 1, 1),
		m_AttachToParent(false),
		m_NodeMask(0),
		m_ParentLocation(NULL)
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
		ComponentFactory::GetPtr()->Register<OSGLocationComponent>("LocationComponent");
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("Component used to handle object position, rotation and scale", OF_VISIBLE)));

		RegisterGetSet("Position", &GASS::OSGLocationComponent::GetPosition, &GASS::OSGLocationComponent::SetPosition, PF_VISIBLE | PF_EDITABLE,"Position relative to parent node");
		RegisterGetSet("Rotation", &GASS::OSGLocationComponent::GetEulerRotation, &GASS::OSGLocationComponent::SetEulerRotation, PF_VISIBLE | PF_EDITABLE,"Rotation relative to parent node, heading = Y-axis rotation, pitch = X-axis rotation, roll= Z-axis rotation [Degrees]");
		
		RegisterGetSet("Quaternion", &GASS::OSGLocationComponent::GetRotation, &GASS::OSGLocationComponent::SetRotation, PF_VISIBLE,"Rotation represented as Quaternion");

		RegisterGetSet("Scale", &GASS::OSGLocationComponent::GetScale, &GASS::OSGLocationComponent::SetScale, PF_VISIBLE | PF_EDITABLE,"Scale relative to parent node");
		RegisterGetSet("AttachToParent", &GASS::OSGLocationComponent::GetAttachToParent, &GASS::OSGLocationComponent::SetAttachToParent, PF_VISIBLE | PF_EDITABLE,"Should this location be relative to parent or not");

		//expose this component to script engine
		asIScriptEngine *engine = SimEngine::Get().GetScriptManager()->GetEngine();

		int r;
		r = engine->RegisterObjectType("LocationComponent", 0, asOBJ_REF | asOBJ_NOCOUNT); CheckASReturn(r);
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
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGLocationComponent::OnParentChangedMessage, ParentChangedEvent, 0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGLocationComponent::OnAttachToParent, AttachToParentRequest, 0));

		OSGGraphicsSceneManagerPtr  scene_man = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<OSGGraphicsSceneManager>();
		m_GFXSceneManager = scene_man;

		if (!m_TransformNode.valid()) //check if injected from outside
		{
			m_TransformNode = new osg::PositionAttitudeTransform();
			SetAttachToParent(m_AttachToParent);
		}
		else
		{
			const std::string name = GetSceneObject()->GetName();
			m_TransformNode->setName(name);
		}

		//Set m_TransformNode position
		SetPosition(m_Position);
		
		//Check if euler rotation provided
		const Quaternion rot = (m_EulerRotation != EulerRotation(0,0,0)) ? m_EulerRotation.GetQuaternion() : m_Rotation;

		//Set m_TransformNode rotation
		SetRotation(rot);
		
		//Set m_TransformNode scale
		SetScale(m_Scale);

		LocationComponentPtr location = GASS_DYNAMIC_PTR_CAST<ILocationComponent>(shared_from_this());
		GetSceneObject()->PostEvent(LocationLoadedEventPtr(new LocationLoadedEvent(location)));
	}

	void OSGLocationComponent::OnAttachToParent(AttachToParentRequestPtr message)
	{
		SetAttachToParent(message->GetAttachToParent());
	}

	Vec3 OSGLocationComponent::_WorldToLocal(const Vec3 &world_pos) const
	{
		if (m_AttachToParent && m_ParentLocation)
		{
			//update relative position
			const Vec3 parent_world_pos = m_ParentLocation->GetWorldPosition();
			const Quaternion parent_world_rot = m_ParentLocation->GetWorldRotation();
			const Mat4 parent_trans_mat(parent_world_rot, parent_world_pos);
			const Mat4 inv_parent_trans_mat = parent_trans_mat.GetInvert();
			return inv_parent_trans_mat*world_pos;
		}
		else
		{
			return world_pos;
		}
	}

	Quaternion OSGLocationComponent::_WorldToLocal(const Quaternion &world_rot) const
	{
		if (m_AttachToParent && m_ParentLocation)
		{
			const Quaternion parent_world_rot = m_ParentLocation->GetWorldRotation();
			const Quaternion inv_parent_world_rot = parent_world_rot.Inverse();
			return inv_parent_world_rot * world_rot;

			/*const Quaternion parent_world_rot = m_ParentLocation->GetWorldRotation();
			const Mat4 parent_trans_mat(parent_world_rot);
			const Mat4 inv_parent_trans_mat = parent_trans_mat.GetInvert();
			const Mat4 local_rot_mat = inv_parent_trans_mat*Mat4(world_rot);
			Quaternion local_rot;
			local_rot.FromRotationMatrix(local_rot_mat);
			return local_rot;*/
		}
		else
		{
			return world_rot;
		}
	}

	Quaternion OSGLocationComponent::_LocalToWorld(const Quaternion &local_rot) const
	{
		if (m_AttachToParent && m_ParentLocation)
		{
			const Quaternion parent_world_rot = m_ParentLocation->GetWorldRotation();
			return parent_world_rot * local_rot;

			/*const Quaternion parent_world_rot = m_ParentLocation->GetWorldRotation();
			const Mat4 parent_trans_mat(parent_world_rot);
			const Mat4 world_rot_mat = parent_trans_mat * Mat4(local_rot);
			Quaternion world_rot;
			world_rot.FromRotationMatrix(world_rot_mat);
			return world_rot;*/

		}
		else
		{
			return local_rot;
		}
	}

	Vec3 OSGLocationComponent::_LocalToWorld(const Vec3 &local_pos) const
	{
		if (m_AttachToParent && m_ParentLocation)
		{
			const Vec3 parent_world_pos = m_ParentLocation->GetWorldPosition();
			const Quaternion parent_world_rot = m_ParentLocation->GetWorldRotation();
			const Mat4 parent_trans_mat(parent_world_rot, parent_world_pos);
			return parent_trans_mat*local_pos;
		}
		else
		{
			return local_pos;
		}
	}
	
	void OSGLocationComponent::SetPosition(const Vec3 &value)
	{
		m_Position = value;
	
		if (m_TransformNode.valid())
		{
			m_WorldPosition = _LocalToWorld(m_Position);

			//update osg transform
			m_TransformNode->setPosition(OSGConvert::ToOSG(m_Position));

			_NotifyTransformationChange();

			//update children
			_OnPositionUpdateRecursive(GetSceneObject());
		}
	}
	
	Vec3 OSGLocationComponent::GetPosition() const
	{
		return m_Position;
	}

	void OSGLocationComponent::SetWorldPosition(const Vec3 &value)
	{
		m_WorldPosition = value;

		m_Position = _WorldToLocal(m_WorldPosition);

		if (m_TransformNode.valid())
			m_TransformNode->setPosition(OSGConvert::ToOSG(m_Position));

		_NotifyTransformationChange();

		//update children
		_OnPositionUpdateRecursive(GetSceneObject());
	}

	Vec3 OSGLocationComponent::GetWorldPosition() const
	{
		return m_WorldPosition;
	}

	void OSGLocationComponent::SetRotation(const Quaternion &value)
	{
		m_Rotation = value;
		if (m_TransformNode.valid())
		{
			m_WorldRotation = _LocalToWorld(m_Rotation);
			//update osg transform
			if (m_TransformNode.valid())
				m_TransformNode->setAttitude(OSGConvert::ToOSG(m_Rotation));

			_NotifyTransformationChange();

			//update children
			_OnRotationUpdateRecursive(GetSceneObject());
		}
	}

	Quaternion OSGLocationComponent::GetRotation() const
	{
		return m_Rotation;
	}

	void OSGLocationComponent::SetWorldRotation(const Quaternion &value)
	{
		m_WorldRotation = value;
		m_Rotation = _WorldToLocal(m_WorldRotation);

		if (m_TransformNode.valid())
			m_TransformNode->setAttitude(OSGConvert::ToOSG(m_Rotation));

		_NotifyTransformationChange();

		//update children
		_OnRotationUpdateRecursive(GetSceneObject());
	}

	Quaternion OSGLocationComponent::GetWorldRotation() const
	{
		return m_WorldRotation;
	}

	void OSGLocationComponent::SetEulerRotation(const EulerRotation &value)
	{
		m_EulerRotation = value;
		SetRotation(m_EulerRotation.GetQuaternion());
	}

	EulerRotation OSGLocationComponent::GetEulerRotation() const
	{
		return m_EulerRotation;
	}

	void OSGLocationComponent::SetScale(const Vec3 &value)
	{
		m_Scale = value;
		if (m_TransformNode.valid())
		{
			m_TransformNode->setScale(osg::Vec3d(m_Scale.x, m_Scale.y, m_Scale.z));
			_NotifyTransformationChange();
		}
	}

	Vec3 OSGLocationComponent::GetScale() const
	{ 
		return m_Scale; 
	}

	void OSGLocationComponent::_NotifyTransformationChange() const
	{
		GetSceneObject()->SendImmediateEvent(TransformationChangedEventPtr(new TransformationChangedEvent(m_WorldPosition, m_WorldRotation, m_Scale)));
	}

	void OSGLocationComponent::_OnParentPositionUpdated()
	{
		if (m_AttachToParent && _GetParentLocation())
		{
			const Vec3 parent_world_pos = _GetParentLocation()->GetWorldPosition();
			const Quaternion parent_world_rot = _GetParentLocation()->GetWorldRotation();
			//reflect new world position
			m_WorldPosition = Mat4(parent_world_rot, parent_world_pos) * GetPosition();
			_NotifyTransformationChange();
		}
	}

	void OSGLocationComponent::_OnPositionUpdateRecursive(SceneObjectPtr scene_object)
	{
		auto iter = scene_object->GetChildren();
		while (iter.hasMoreElements())
		{
			SceneObjectPtr child_obj = iter.getNext();
			OSGLocationComponentPtr location_comp = child_obj->GetFirstComponentByClass<OSGLocationComponent>();
			if (location_comp)
				location_comp->_OnParentPositionUpdated();
			_OnPositionUpdateRecursive(child_obj);
		}
	}

	void OSGLocationComponent::_OnParentRotationUpdated()
	{
		if (m_AttachToParent && _GetParentLocation())
		{
			const Vec3 parent_world_pos = _GetParentLocation()->GetWorldPosition();
			const Quaternion parent_world_rot = _GetParentLocation()->GetWorldRotation();
			
			//Calculate new world position
			m_WorldPosition = Mat4(parent_world_rot, parent_world_pos) * GetPosition();
			
			//Calculate new world rotation
			m_WorldRotation = parent_world_rot * GetRotation();
			
			_NotifyTransformationChange();
		}
	}

	void OSGLocationComponent::_OnRotationUpdateRecursive(SceneObjectPtr scene_object)
	{
		auto iter = scene_object->GetChildren();
		while (iter.hasMoreElements())
		{
			SceneObjectPtr child_obj = iter.getNext();
			OSGLocationComponentPtr location_comp = child_obj->GetFirstComponentByClass<OSGLocationComponent>();
			if (location_comp)
				location_comp->_OnParentRotationUpdated();
			_OnRotationUpdateRecursive(child_obj);
		}
	}

	void OSGLocationComponent::operator()(osg::Node* node, osg::NodeVisitor* nv)
	{
		traverse(node, nv);
	}

	void OSGLocationComponent::OnVisibilityMessage(LocationVisibilityRequestPtr message)
	{
		SetVisible(message->GetValue());
	}

	void OSGLocationComponent::SetVisible(bool value)
	{
		m_NodeMask = value ? ~0u : 0u;
		if (m_TransformNode)
			m_TransformNode->setNodeMask(m_NodeMask);
	}

	bool OSGLocationComponent::GetVisible() const
	{
		return m_NodeMask > 0;
	}

	osg::ref_ptr<osg::Group> OSGLocationComponent::_GetOSGRootGroup()
	{
		osg::ref_ptr<osg::Group> root_node;
		OSGGraphicsSceneManagerPtr scene_man = m_GFXSceneManager.lock();
		if (scene_man)
		{
			//HACK: do extra check to see if we should avoid shadow node...
			OSGManualMeshComponentPtr mm_comp = GetSceneObject()->GetFirstComponentByClass<OSGManualMeshComponent>();
			if (mm_comp && !mm_comp->GetCastShadow() && !mm_comp->GetReceiveShadow())
			{
				root_node = scene_man->GetOSGRootNode();
			}
			else
			{
				root_node = scene_man->GetOSGShadowRootNode();
			}
		}
		GASSAssert(root_node, "Failed to find osg root group in OSGLocationComponent::_GetOSGRootGroup()");
		return root_node;
	}

	bool OSGLocationComponent::HasParentLocation() const
	{
		return (m_ParentLocation != NULL);
	}

	void OSGLocationComponent::SetAttachToParent(bool value)
	{
		m_AttachToParent = value;
		if (m_TransformNode.valid())
		{
			bool was_attached = false;
			if (m_TransformNode->getNumParents() > 0 && m_TransformNode->getParent(0))
			{
				m_TransformNode->getParent(0)->removeChild(m_TransformNode);
				was_attached = true;
			}

			m_ParentLocation = _GetFirstParentLocation().get();
			if (value && m_ParentLocation)
			{
				m_ParentLocation->GetOSGNode()->addChild(m_TransformNode);
			}
			else //attach under osg root
			{
				m_ParentLocation = NULL;
				osg::ref_ptr<osg::Group> root_node = _GetOSGRootGroup();
				root_node->addChild(m_TransformNode);
			}
			
			if (was_attached)
			{
				//update new local location
				SetWorldPosition(m_WorldPosition);
				SetWorldRotation(m_WorldRotation);
			}
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

	OSGLocationComponentPtr OSGLocationComponent::_GetFirstParentLocation() const
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

	void OSGLocationComponent::OnPositionMessage(PositionRequestPtr message)
	{
		SetPosition(message->GetPosition());
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

	void OSGLocationComponent::OnRotationMessage(RotationRequestPtr message)
	{
		SetRotation(message->GetRotation());
	}

	void OSGLocationComponent::OnScaleMessage(ScaleRequestPtr message)
	{
		SetScale(message->GetScale());
	}
}