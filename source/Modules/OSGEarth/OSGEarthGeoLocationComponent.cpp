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

#include "OSGEarthCommonIncludes.h"
#include "OSGEarthGeoLocationComponent.h"
#include "OSGEarthGraphicsSceneManager.h"
#include "Modules/OSG/OSGConvert.h"
#include "Sim/GASSScriptManager.h"
#include "Core/Math/GASSMath.h"
#include <angelscript.h>

#include <memory>

namespace GASS
{
	OSGEarthGeoLocationComponent::OSGEarthGeoLocationComponent() : m_Pos(0, 0, 0),
		m_Rot(0, 0, 0),
		m_Scale(1, 1, 1)
		
	{

	}

	OSGEarthGeoLocationComponent::~OSGEarthGeoLocationComponent()
	{

	}

	void OSGEarthGeoLocationComponent::OnDelete()
	{
		if (m_TransformNode.valid() && m_TransformNode->getNumParents() > 0)
		{
			osg::Group* parent = m_TransformNode->getParent(0);
			parent->removeChild(m_TransformNode.get());
		}
	}


	void OSGEarthGeoLocationComponent::RegisterReflection()
	{
		ComponentFactory::Get().Register<OSGEarthGeoLocationComponent>("GeoLocationComponent");
		GetClassRTTI()->SetMetaData(std::make_shared<ClassMetaData>("Component used to handle object position, rotation and scale", OF_VISIBLE));

		RegisterGetSet("Position", &GASS::OSGEarthGeoLocationComponent::GetPosition, &GASS::OSGEarthGeoLocationComponent::SetPosition, PF_VISIBLE | PF_EDITABLE,"Position relative to parent node");
		RegisterGetSet("Rotation", &GASS::OSGEarthGeoLocationComponent::GetEulerRotation, &GASS::OSGEarthGeoLocationComponent::SetEulerRotation, PF_VISIBLE | PF_EDITABLE,"Rotation relative to parent node, x = heading, y=pitch, z=roll [Degrees]");

		RegisterGetSet("Quaternion", &GASS::OSGEarthGeoLocationComponent::GetRotation, &GASS::OSGEarthGeoLocationComponent::SetRotation, PF_VISIBLE,"Rotation represented as Quaternion");

		RegisterGetSet("Scale", &GASS::OSGEarthGeoLocationComponent::GetScale, &GASS::OSGEarthGeoLocationComponent::SetScale, PF_VISIBLE | PF_EDITABLE,"Scale relative to parent node");
		RegisterGetSet("AttachToParent", &GASS::OSGEarthGeoLocationComponent::GetAttachToParent, &GASS::OSGEarthGeoLocationComponent::SetAttachToParent, PF_VISIBLE | PF_EDITABLE,"Position relative to parent node");

		//expose this component to script engine
	/*	asIScriptEngine *engine = SimEngine::Get().GetScriptManager()->GetEngine();

		int r;
		r = engine->RegisterObjectType("LocationComponent", 0, asOBJ_REF | asOBJ_NOCOUNT); assert(r >= 0);


		r = engine->RegisterObjectBehaviour("Component", asBEHAVE_REF_CAST, "LocationComponent@ f()", asFUNCTION((refCast<Component, OSGEarthGeoLocationComponent>)), asCALL_CDECL_OBJLAST); assert(r >= 0);
		r = engine->RegisterObjectBehaviour("LocationComponent", asBEHAVE_IMPLICIT_REF_CAST, "Component@ f()", asFUNCTION((refCast<OSGEarthGeoLocationComponent, Component>)), asCALL_CDECL_OBJLAST); assert(r >= 0);

		r = engine->RegisterObjectMethod("LocationComponent", "string GetName() const", asMETHOD(Component, GetName), asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("LocationComponent", "void SetAttachToParent(bool) ", asMETHOD(OSGEarthGeoLocationComponent, SetAttachToParent), asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("LocationComponent", "bool GetAttachToParent() const", asMETHOD(OSGEarthGeoLocationComponent, GetAttachToParent), asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("LocationComponent", "Vec3 GetPosition() const", asMETHODPR(OSGEarthGeoLocationComponent, GetPosition, () const, Vec3), asCALL_THISCALL); assert(r >= 0);
	*/
	}

	void OSGEarthGeoLocationComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGEarthGeoLocationComponent::OnParentChanged, GASS::ParentChangedEvent, 0));
	
		auto scene_man = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<OSGEarthGraphicsSceneManager>();
		//assert(scene_man);nmbvcxh
		if (!m_TransformNode.valid())
		{
			m_GeoTransform = new osgEarth::GeoTransform();
			m_GeoTransform->setTerrain(scene_man->GetMap()->getTerrain());
			m_Map = scene_man->GetMap();
			m_TransformNode = new osg::PositionAttitudeTransform();
			m_GeoTransform->addChild(m_GeoTransform);
			osg::ref_ptr<osg::Group> root_node = scene_man->GetObjectGroup();
			m_GFXSceneManager = scene_man;
			
			if (m_AttachToParent)
			{
				OSGEarthGeoLocationComponentPtr parent = GetParentLocation();
				if (parent)
				{
					parent->GetOSGNode()->addChild(m_GeoTransform.get());
				}
				else
					root_node->addChild(m_GeoTransform.get());
			}
			else
			{
				root_node->addChild(m_GeoTransform.get());
			}
		}
		else
		{
			const std::string name = GetSceneObject()->GetName();
			m_TransformNode->setName(name);
		}

		//Set m_TransformNode position
		SetPosition(m_Pos);

		//Check if euler rotation provided
		const Quaternion rot = (m_Rot != EulerRotation(0, 0, 0)) ? m_Rot.GetQuaternion() : m_QRot;

		//Set m_TransformNode rotation
		SetRotation(rot);

		//Set m_TransformNode scale
		SetScale(m_Scale);

		LocationComponentPtr location = GASS_DYNAMIC_PTR_CAST<ILocationComponent>(shared_from_this());
		GetSceneObject()->PostEvent(std::make_shared<LocationLoadedEvent>(location));
	}

	bool OSGEarthGeoLocationComponent::HasParentLocation() const
	{
		bool value = m_AttachToParent;
		/*if (!_GetParentLocation() && m_AttachToParent) //if intialized and we have to check that we are top node 
		{
			value = false;
		}*/
		return value;
	}
	

	void OSGEarthGeoLocationComponent::SetScale(const Vec3 &value)
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

	void OSGEarthGeoLocationComponent::SetPosition(const Vec3 &value)
	{
		m_Pos = value;
		if (m_TransformNode.valid())
		{
			osgEarth::GeoPoint geo_pos;
			geo_pos.fromWorld(m_Map->getMapSRS(), OSGConvert::ToOSG(m_Pos));
			m_GeoTransform->setPosition(geo_pos);
			SendTransMessage();
		}
	}

	

	void OSGEarthGeoLocationComponent::SendTransMessage()
	{
		const Vec3 pos = GetWorldPosition();
		const Vec3 scale = GetScale();
		const Quaternion rot = GetWorldRotation();

		GetSceneObject()->PostEvent(std::make_shared<TransformationChangedEvent>(pos, rot, scale));
		//send for all child transforms also?
		auto iter = GetSceneObject()->GetChildren();
		while (iter.hasMoreElements())
		{
			SceneObjectPtr obj = iter.getNext();
			OSGEarthGeoLocationComponentPtr c_location = obj->GetFirstComponentByClass<OSGEarthGeoLocationComponent>();
			if (c_location && c_location->GetAttachToParent())
				c_location->SendTransMessage();
		}
	}

	Vec3 OSGEarthGeoLocationComponent::GetPosition() const
	{
		return m_Pos;
	}

	void OSGEarthGeoLocationComponent::SetWorldPosition(const Vec3 &value)
	{
		if (m_TransformNode.valid())
		{
			osg::Vec3d new_pos = OSGConvert::ToOSG(value);
			if (m_TransformNode->getNumParents() > 0)
			{
				auto* parent = dynamic_cast<osg::PositionAttitudeTransform*>(m_TransformNode->getParent(0));
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
			
			osgEarth::GeoPoint geo_pos;
			geo_pos.fromWorld(m_Map->getMapSRS(), new_pos);
			m_GeoTransform->setPosition(geo_pos);
			//m_TransformNode->setPosition(new_pos);
			m_Pos = OSGConvert::ToGASS(new_pos);
			SendTransMessage();
		}
	}

	Vec3 OSGEarthGeoLocationComponent::GetWorldPosition() const
	{
		Vec3 world_pos = m_Pos;
		if (m_TransformNode.valid())
		{
			osgEarth::GeoPoint geo_pos = m_GeoTransform->getPosition();
			osg::Vec3d wpos;
			geo_pos.toWorld(wpos);
			world_pos = OSGConvert::ToGASS(wpos);
			if (m_TransformNode->getNumParents() > 0)
			{
				auto* parent = dynamic_cast<osg::PositionAttitudeTransform*>(m_TransformNode->getParent(0));
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

	void OSGEarthGeoLocationComponent::SetRotation(const Quaternion &value)
	{
		m_QRot = value;
		if (m_TransformNode.valid())
		{
			const osg::Quat final = OSGConvert::ToOSG(value);
			m_TransformNode->setAttitude(final);
			SendTransMessage();
		}
	}

	void OSGEarthGeoLocationComponent::SetEulerRotation(const EulerRotation &value)
	{
		m_Rot = value;
		SetRotation(value.GetQuaternion());
	}

	EulerRotation OSGEarthGeoLocationComponent::GetEulerRotation() const
	{
		return m_Rot;
	}

	Quaternion OSGEarthGeoLocationComponent::GetRotation() const
	{
		Quaternion q = m_QRot;
		if (m_TransformNode.valid())
		{
			q = OSGConvert::ToGASS(m_TransformNode->getAttitude());
		}
		return q;
	}

	void OSGEarthGeoLocationComponent::SetWorldRotation(const Quaternion &value)
	{
		if (m_TransformNode.valid())
		{
			osg::Quat final = OSGConvert::ToOSG(value);

			if (m_TransformNode->getNumParents() > 0)
			{
				auto* parent = dynamic_cast<osg::PositionAttitudeTransform*>(m_TransformNode->getParent(0));
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
					inv_world_trans.get(inv_rot);
					final = final*inv_rot;
				}
			}
			m_TransformNode->setAttitude(final);
			SendTransMessage();
		}
	}

	Quaternion OSGEarthGeoLocationComponent::GetWorldRotation() const
	{
		Quaternion q = Quaternion::IDENTITY;

		if (m_TransformNode.valid())
		{
			osg::Quat rot = m_TransformNode->getAttitude();
			if (m_TransformNode->getNumParents() > 0)
			{
				auto* parent = dynamic_cast<osg::PositionAttitudeTransform*>(m_TransformNode->getParent(0));
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
					world_trans.get(parent_rot);
					rot = rot*parent_rot;
				}
			}
			q = OSGConvert::ToGASS(rot);
		}
		return q;
	}

	void OSGEarthGeoLocationComponent::operator()(osg::Node* node, osg::NodeVisitor* nv)
	{
		traverse(node, nv);
	}

	void OSGEarthGeoLocationComponent::SetVisible(bool value)
	{
		m_NodeMask = value ? ~0u : 0u;
		if (m_TransformNode)
			m_TransformNode->setNodeMask(m_NodeMask);
	}

	bool OSGEarthGeoLocationComponent::GetVisible() const
	{
		return m_NodeMask > 0;
	}

	void OSGEarthGeoLocationComponent::SetAttachToParent(bool value)
	{
		m_AttachToParent = value;
		if (m_TransformNode.valid())
		{
			Vec3 world_pos = GetWorldPosition();
			Quaternion world_rot = GetWorldRotation();
			if (m_TransformNode->getParent(0))
				m_TransformNode->getParent(0)->removeChild(m_TransformNode);

			OSGEarthGeoLocationComponentPtr parent = GetParentLocation();
			if (parent && value)
				parent->GetOSGNode()->addChild(m_TransformNode);
			else
			{
				OSGGraphicsSceneManagerPtr scene_man = m_GFXSceneManager.lock();
				if (scene_man)
				{
					auto root_node = scene_man->GetObjectGroup();
					root_node->addChild(m_TransformNode);
				}
			}
			SetWorldPosition(world_pos);
			SetWorldRotation(world_rot);
		}
	}

	void OSGEarthGeoLocationComponent::OnParentChanged(ParentChangedEventPtr /*message*/)
	{
		SetAttachToParent(GetAttachToParent());
	}

	bool OSGEarthGeoLocationComponent::GetAttachToParent() const
	{
		return m_AttachToParent;
	}

	OSGEarthGeoLocationComponentPtr OSGEarthGeoLocationComponent::GetParentLocation()
	{
		OSGEarthGeoLocationComponentPtr parent_location;
		auto scene_obj = GetSceneObject()->GetParent();
		while (scene_obj && !parent_location)
		{
			parent_location = scene_obj->GetFirstComponentByClass<OSGEarthGeoLocationComponent>();
			if (parent_location)
				return parent_location;
			scene_obj = scene_obj->GetParent();
		}
		return parent_location;
	}
}