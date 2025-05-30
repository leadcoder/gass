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

#include "OSGManualMeshComponent.h"

#include <memory>
#include "Modules/OSG/OSGGraphicsSceneManager.h"
#include "Modules/OSG/Components/OSGLocationComponent.h"
#include "Modules/OSG/OSGConvert.h"
#include "Modules/OSG/OSGNodeMasks.h"
#include "Modules/OSG/OSGNodeData.h"
#include "Modules/OSG/OSGGeometryRecorder.h"
#include "Modules/OSG/OSGMaterial.h"
#include "Core/Math/GASSMath.h"


namespace GASS
{
	OSGManualMeshComponent::OSGManualMeshComponent() : m_GeometryFlagsBinder(GEOMETRY_FLAG_UNKNOWN)
	{

	}

	OSGManualMeshComponent::~OSGManualMeshComponent()
	{

	}

	void OSGManualMeshComponent::RegisterReflection()
	{
		ComponentFactory::Get().Register<OSGManualMeshComponent>("ManualMeshComponent");
		GetClassRTTI()->SetMetaData(std::make_shared<ClassMetaData>("ManualMeshComponent", OF_VISIBLE));
		ADD_DEPENDENCY("OSGLocationComponent")
			RegisterGetSet("CastShadow", &OSGManualMeshComponent::GetCastShadow, &OSGManualMeshComponent::SetCastShadow, PF_VISIBLE | PF_EDITABLE, "Should this mesh cast shadows or not");

		RegisterGetSet("ReceiveShadow", &OSGManualMeshComponent::GetReceiveShadow, &OSGManualMeshComponent::SetReceiveShadow, PF_VISIBLE | PF_EDITABLE, "Should this mesh receive shadows or not");

		RegisterMember("GeometryFlags", &OSGManualMeshComponent::m_GeometryFlagsBinder, PF_VISIBLE | PF_EDITABLE | PF_MULTI_OPTIONS, "Geometry Flags");
	}

	void OSGManualMeshComponent::OnInitialize()
	{
		m_GFXSystem = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<OSGGraphicsSystem>();
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGManualMeshComponent::OnLocationLoaded, LocationLoadedEvent, 1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGManualMeshComponent::OnCollisionSettings, CollisionSettingsRequest, 0));

		m_GeoNode = new osg::Geode();

		osg::StateSet* ss = m_GeoNode->getOrCreateStateSet();
		auto* cull = new osg::CullFace();
		cull->setMode(osg::CullFace::BACK);
		ss->setAttributeAndModes(cull, osg::StateAttribute::ON);

		osg::ref_ptr<osg::Point> point(new osg::Point(8.0f));
		ss->setAttributeAndModes(point, osg::StateAttribute::ON);

		SetCastShadow(m_CastShadow);
		SetReceiveShadow(m_ReceiveShadow);

		auto* node_data = new OSGNodeData(shared_from_this());
		m_GeoNode->setUserData(node_data);
		SetGeometryFlags(GetGeometryFlags());

		Component::OnInitialize();
	}

	bool OSGManualMeshComponent::GetVisible() const
	{
		if (m_GeoNode)
			return m_GeoNode->getNodeMask() > 0;
		return false;
	}

	void OSGManualMeshComponent::SetVisible(bool value)
	{
		if (value)
		{
			m_GeoNode->setNodeMask(1);
			//restore flags
			SetCastShadow(m_CastShadow);
			SetReceiveShadow(m_ReceiveShadow);
			SetGeometryFlags(m_GeometryFlagsBinder.GetValue());
		}
		else
		{
			m_GeoNode->setNodeMask(0);
		}
	}

	void OSGManualMeshComponent::SetCastShadow(bool value)
	{
		m_CastShadow = value;
		if (m_CastShadow && m_GeoNode.valid())
			m_GeoNode->setNodeMask(NM_CAST_SHADOWS | m_GeoNode->getNodeMask());
		else if (m_GeoNode.valid())
		{
			m_GeoNode->setNodeMask(~NM_CAST_SHADOWS & m_GeoNode->getNodeMask());
		}
	}

	void OSGManualMeshComponent::SetReceiveShadow(bool value)
	{
		m_ReceiveShadow = value;
		if (m_GeoNode.valid())
		{
			if (m_ReceiveShadow)
				m_GeoNode->setNodeMask(NM_RECEIVE_SHADOWS | m_GeoNode->getNodeMask());
			else
				m_GeoNode->setNodeMask(~NM_RECEIVE_SHADOWS & m_GeoNode->getNodeMask());
			Material::SetReceiveShadows(m_GeoNode->getOrCreateStateSet(), value ? osg::StateAttribute::ON : osg::StateAttribute::OFF);
		}
	}

	void OSGManualMeshComponent::SetGeometryFlags(GeometryFlags value)
	{
		m_GeometryFlagsBinder.SetValue(value);
		if (m_GeoNode.valid())
		{
			OSGConvert::SetOSGNodeMask(value, m_GeoNode.get());
			//if(value & GEOMETRY_FLAG_TRANSPARENT_OBJECT)
			//	m_Collision = false;
		}
	}

	GeometryFlags OSGManualMeshComponent::GetGeometryFlags() const
	{
		return m_GeometryFlagsBinder.GetValue();
	}

	void OSGManualMeshComponent::OnCollisionSettings(CollisionSettingsRequestPtr message)
	{
		SetCollision(message->EnableCollision());
	}

	void OSGManualMeshComponent::SetCollision(bool value)
	{
		if (m_GeoNode && m_GeoNode->getNodeMask())
		{
			if (value)
				OSGConvert::SetOSGNodeMask(GetGeometryFlags(), m_GeoNode.get());
			else
				OSGConvert::SetOSGNodeMask(GEOMETRY_FLAG_TRANSPARENT_OBJECT, m_GeoNode.get());
		}
		m_Collision = value;
	}

	bool OSGManualMeshComponent::GetCollision() const
	{
		return m_Collision;
	}

	void OSGManualMeshComponent::OnLocationLoaded(LocationLoadedEventPtr /*message*/)
	{
		OSGLocationComponentPtr  lc = GetSceneObject()->GetFirstComponentByClass<OSGLocationComponent>();
		if (!lc)
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Failed to find location component: " + GetSceneObject()->GetName(), "OSGManualMeshComponent::OnLoad");

		lc->GetOSGNode()->addChild(m_GeoNode.get());
	}

	void OSGManualMeshComponent::SetMeshData(const GraphicsMesh& mesh)
	{
		if (!m_GeoNode)
			return;
		Clear();
		for (size_t i = 0; i < mesh.SubMeshVector.size(); i++)
		{
			GraphicsSubMeshPtr sm = mesh.SubMeshVector[i];
			osg::ref_ptr<osg::Geometry> geom = CreateSubMesh(sm);
			m_OSGGeometries.push_back(geom);
			m_GeoNode->addDrawable(geom.get());
		}
		GetSceneObject()->PostEvent(std::make_shared<GeometryChangedEvent>(GASS_DYNAMIC_PTR_CAST<IGeometryComponent>(shared_from_this())));
	}

	void OSGManualMeshComponent::Clear()
	{
		//remove all drawables;
		for (size_t i = 0; i < m_OSGGeometries.size(); i++)
		{
			m_GeoNode->removeDrawable(m_OSGGeometries[i]);
		}
		m_OSGGeometries.clear();
	}

	osg::ref_ptr<osg::Geometry>  OSGManualMeshComponent::CreateSubMesh(GraphicsSubMeshPtr sm)
	{
		osg::ref_ptr<osg::Geometry> geom = new osg::Geometry();
		auto material = new SimpleMaterial();

		if (sm->MaterialName != "" && m_GFXSystem && m_GFXSystem->HasMaterial(sm->MaterialName))
		{
			material->merge(*m_GFXSystem->GetStateSet(sm->MaterialName));
		}
		else if (sm->MaterialConfig)
		{
			material->merge(*Material::CreateFromConfig(sm->MaterialConfig.get()));
		}
		geom->setStateSet(material);

		osg::PrimitiveSet::Mode op;
		switch (sm->Type)
		{
		case LINE_LIST:
			op = osg::PrimitiveSet::LINES;
			break;
		case POINT_LIST:
			op = osg::PrimitiveSet::POINTS;
			break;
		case LINE_STRIP:
			op = osg::PrimitiveSet::LINE_STRIP;
			break;
		case TRIANGLE_FAN:
			op = osg::PrimitiveSet::TRIANGLE_FAN;
			break;
		case TRIANGLE_LIST:
			op = osg::PrimitiveSet::TRIANGLES;
			break;
		case TRIANGLE_STRIP:
			op = osg::PrimitiveSet::TRIANGLE_STRIP;
			break;
		default:
			op = osg::PrimitiveSet::TRIANGLES;
			break;
		}

		osg::ref_ptr<osg::DrawElementsUInt> de = new osg::DrawElementsUInt(op);

		if (sm->IndexVector.size() > 0)
		{
			for (size_t i = 0; i < sm->IndexVector.size(); i++)
			{
				de->push_back(sm->IndexVector[i]);
			}
			geom->addPrimitiveSet(de);
		}
		else
		{
			geom->addPrimitiveSet(new osg::DrawArrays(op, 0, static_cast<GLsizei>(sm->PositionVector.size())));
		}

		osg::ref_ptr<osg::Vec3Array> positions = new osg::Vec3Array();
		for (size_t i = 0; i < sm->PositionVector.size(); i++)
		{
			positions->push_back(OSGConvert::ToOSG(sm->PositionVector[i]));
		}
		geom->setVertexArray(positions);

		if (sm->NormalVector.size() > 0)
		{
			osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
			for (size_t i = 0; i < sm->NormalVector.size(); i++)
			{
				normals->push_back(OSGConvert::ToOSG(sm->NormalVector[i]));
			}
			geom->setNormalArray(normals);
			geom->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);
		}

		osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
		if (sm->ColorVector.size() > 0)
		{
			for (size_t i = 0; i < sm->ColorVector.size(); i++)
			{
				ColorRGBA color = sm->ColorVector[i];
				colors->push_back(OSGConvert::ToOSG(color));
			}
			geom->setColorArray(colors, osg::Array::BIND_PER_VERTEX);
		}
		else
		{
			colors->push_back(osg::Vec4f(1, 1, 1, 1));
			geom->setColorArray(colors, osg::Array::BIND_OVERALL);
		}

		for (size_t i = 0; i < sm->TexCoordsVector.size(); i++)
		{
			osg::ref_ptr<osg::Vec2Array> tex_coords = new osg::Vec2Array;
			for (size_t j = 0; j < sm->TexCoordsVector[i].size(); j++)
			{
				Vec4 texc = sm->TexCoordsVector[i][j];
				tex_coords->push_back(osg::Vec2(static_cast<float>(texc.x), static_cast<float>(texc.y)));
			}
			geom->setTexCoordArray(static_cast<unsigned int>(i), tex_coords);
		}
		geom->dirtyBound();
		return geom;
	}

	AABox OSGManualMeshComponent::GetBoundingBox() const
	{
		AABox comp_box;
		for (size_t i = 0; i < m_OSGGeometries.size(); i++)
		{

#ifdef OSG_VERSION_GREATER_OR_EQUAL
#if(OSG_VERSION_GREATER_OR_EQUAL(3,3,2))
			osg::BoundingBox osg_box = m_OSGGeometries[i]->getBoundingBox();
#else
			osg::BoundingBox osg_box = m_OSGGeometries[i]->getBound();
#endif
#else
			osg::BoundingBox osg_box = m_OSGGeometries[i]->getBound();
#endif
			Vec3 p1 = OSGConvert::ToGASS(osg_box._min);
			Vec3 p2 = OSGConvert::ToGASS(osg_box._max);
			comp_box.Union(p1);
			comp_box.Union(p2);
		}

		OSGLocationComponentPtr lc = GetSceneObject()->GetFirstComponentByClass<OSGLocationComponent>();
		if (lc)
		{
			Vec3 scale = OSGConvert::ToGASS(lc->GetOSGNode()->getScale());
			//scale should not flip
			scale.z = -scale.z;

			comp_box.Max = comp_box.Max * scale;
			comp_box.Min = comp_box.Min * scale;
		}
		return comp_box;
	}

	Sphere OSGManualMeshComponent::GetBoundingSphere() const
	{
		Sphere sphere;
		sphere.m_Pos = Vec3(0, 0, 0);
		sphere.m_Radius = 0;
		for (size_t i = 0; i < m_OSGGeometries.size(); i++)
		{
			osg::BoundingSphere bsphere = m_OSGGeometries[i]->getBound();
			if (bsphere._radius > sphere.m_Radius)
				sphere.m_Radius = bsphere._radius;
		}

		OSGLocationComponentPtr lc = GetSceneObject()->GetFirstComponentByClass<OSGLocationComponent>();
		if (lc)
		{
			Vec3 scale = OSGConvert::ToGASS(lc->GetOSGNode()->getScale());
			//scale should not flip
			scale.z = -scale.z;
			sphere.m_Radius *= static_cast<float>(std::max(std::max(scale.x, scale.y), scale.z));
		}
		return sphere;
	}

	GraphicsMesh OSGManualMeshComponent::GetMeshData() const
	{
		GraphicsMesh mesh_data;

		DrawableVisitor<TriangleRecorder> mv;
		m_GeoNode->accept(mv);

		GraphicsSubMeshPtr sub_mesh_data(new GraphicsSubMesh());

		//TODO: get materials and sub meshes!!

		sub_mesh_data->Type = TRIANGLE_LIST;
		mesh_data.SubMeshVector.push_back(sub_mesh_data);

		sub_mesh_data->PositionVector = mv.mFunctor.mVertices;
		sub_mesh_data->IndexVector = mv.mFunctor.mTriangles;

		return mesh_data;
	}


	void OSGManualMeshComponent::SetSubMeshMaterial(const std::string& material_name, int sub_mesh_index)
	{
		if (material_name != "" && m_GFXSystem && m_GFXSystem->HasMaterial(material_name))
		{
			auto material = new SimpleMaterial();
			material->merge(*m_GFXSystem->GetStateSet(material_name));
			if (sub_mesh_index >= 0)
			{
				if (sub_mesh_index < static_cast<int>(m_OSGGeometries.size()))
					m_OSGGeometries[sub_mesh_index]->setStateSet(material);
			}
			else
			{
				for (size_t i = 0; i < m_OSGGeometries.size(); i++)
				{
					m_OSGGeometries[i]->setStateSet(material);
				}
			}
		}
	}

	void OSGManualMeshComponent::SetSubMeshMaterial(IGfxMaterialConfig* config, int sub_mesh_index)
	{
		auto material = new SimpleMaterial();
		material->merge(*Material::CreateFromConfig(config));
		if (sub_mesh_index >= 0)
		{
			if (sub_mesh_index < static_cast<int>(m_OSGGeometries.size()))
				m_OSGGeometries[sub_mesh_index]->setStateSet(material);
		}
		else
		{
			for (size_t i = 0; i < m_OSGGeometries.size(); i++)
			{
				m_OSGGeometries[i]->setStateSet(material);
			}
		}

	}
}
