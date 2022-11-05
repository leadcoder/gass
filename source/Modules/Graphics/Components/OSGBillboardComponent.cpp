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

#include <memory>

#include "Modules/Graphics/Components/OSGBillboardComponent.h"
#include "Modules/Graphics/Components/OSGLocationComponent.h"
#include "Modules/Graphics/OSGConvert.h"
#include "Modules/Graphics/OSGNodeMasks.h"
#include "Modules/Graphics/OSGNodeData.h"
#include "Modules/Graphics/OSGMaterial.h"
#include "Core/Math/GASSMath.h"


namespace GASS
{
	OSGBillboardComponent::OSGBillboardComponent() : 
		m_OSGBillboard (nullptr)
		
	{

	}

	OSGBillboardComponent::~OSGBillboardComponent()
	{

	}

	void OSGBillboardComponent::RegisterReflection()
	{
		ComponentFactory::Get().Register<OSGBillboardComponent>("BillboardComponent");
		ADD_DEPENDENCY("OSGLocationComponent")
		RegisterGetSet("Material", &OSGBillboardComponent::GetMaterial, &OSGBillboardComponent::SetMaterial);
		RegisterGetSet("CastShadow", &OSGBillboardComponent::GetCastShadow, &OSGBillboardComponent::SetCastShadow);
		RegisterGetSet("Height", &OSGBillboardComponent::GetHeight, &OSGBillboardComponent::SetHeight);
		RegisterGetSet("Width", &OSGBillboardComponent::GetWidth, &OSGBillboardComponent::SetWidth);
		RegisterGetSet("GeometryFlags", &OSGBillboardComponent::GetGeometryFlagsBinder, &OSGBillboardComponent::SetGeometryFlagsBinder, PF_VISIBLE | PF_EDITABLE | PF_MULTI_OPTIONS, "Geometry Flags");
	}

	void OSGBillboardComponent::SetGeometryFlagsBinder(GeometryFlagsBinder value)
	{
		SetGeometryFlags(value.GetValue());
	}
	GeometryFlagsBinder OSGBillboardComponent::GetGeometryFlagsBinder() const
	{
		return GeometryFlagsBinder(GetGeometryFlags());
	}

	void OSGBillboardComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGBillboardComponent::OnLocationLoaded,LocationLoadedEvent,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGBillboardComponent::OnCollisionSettings,CollisionSettingsRequest,0));
	}

	float OSGBillboardComponent::GetWidth() const
	{
		return m_Width;
	}

	void OSGBillboardComponent::SetWidth(float width)
	{
		SetSize(width,m_Height);
	}

	float OSGBillboardComponent::GetHeight() const
	{
		return m_Height;
	}

	void OSGBillboardComponent::SetHeight(float height)
	{
		SetSize(m_Width, height);
	}

	GeometryFlags OSGBillboardComponent::GetGeometryFlags() const
	{
		return m_GeomFlags;
	}

	void OSGBillboardComponent::SetGeometryFlags(GeometryFlags flags)
	{
		m_GeomFlags = flags;
		if(m_OSGBillboard)
			OSGConvert::SetOSGNodeMask(flags, m_OSGBillboard);
	}



	void OSGBillboardComponent::OnLocationLoaded(LocationLoadedEventPtr message)
	{
		ResourceManagerPtr rm = SimEngine::Get().GetResourceManager();
		const std::string  full_path = rm->GetFirstResourceByName(m_Material)->Path().GetFullPath();

		Vec3 up(0,0,m_Height);
		Vec3 east(m_Width,0,0);

		//make offset
		Vec3 corner = -east*0.5;
		auto* options = new osgDB::ReaderWriter::Options("dds_flip");

		m_OSGBillboard = new osg::Billboard();
		m_OSGBillboard->setMode(osg::Billboard::POINT_ROT_EYE);

		osg::ref_ptr<osg::Geometry> geom = CreateSquare(osg::Vec3(static_cast<float>(corner.x), static_cast<float>(corner.y), static_cast<float>(corner.z)),
														osg::Vec3(static_cast<float>(east.x), static_cast<float>(east.y), static_cast<float>(east.z)),
														osg::Vec3(static_cast<float>(up.x), static_cast<float>(up.y), static_cast<float>(up.z)),
														osgDB::readImageFile(full_path,options));

		
		m_OSGBillboard->addDrawable(geom);
		m_Geom = geom.get();

		m_OSGBillboard->setPosition(0,osg::Vec3(0,0,static_cast<float>(m_GroundOffset)));
		auto* node_data = new OSGNodeData(shared_from_this());
		m_OSGBillboard->setUserData(node_data);

		OSGLocationComponentPtr lc = GetSceneObject()->GetFirstComponentByClass<OSGLocationComponent>();
		lc->GetOSGNode()->addChild(m_OSGBillboard.get());

		auto material = new SimpleMaterial();
		m_OSGBillboard->setStateSet(material);
		//osg::ref_ptr<osg::StateSet> nodess (m_OSGBillboard->getOrCreateStateSet());
		Material::SetLighting(material, osg::StateAttribute::OFF);
	
		SetCastShadow(m_CastShadow);
		SetGeometryFlags(m_GeomFlags);
		GetSceneObject()->PostEvent(std::make_shared<GeometryChangedEvent>(GASS_DYNAMIC_PTR_CAST<IGeometryComponent>(shared_from_this())));
	}

	AABox OSGBillboardComponent::GetBoundingBox() const
	{
		if(m_Geom)
		{
			auto* coords = static_cast<osg::Vec3Array*> (m_Geom->getVertexArray());
			osg::Vec3 p_min = (*coords)[0];
			osg::Vec3 p_max = (*coords)[2];
			osg::Vec3 bb_size = p_max - p_min;
			Float max_size = std::max(bb_size.x() ,bb_size.z() )*0.5f;
			Float offset = bb_size.z() * 0.5f;
			AABox box(Vec3(-max_size,-max_size + offset,-max_size),Vec3(max_size,max_size+offset,max_size));
			box.Min.y += m_GroundOffset;
			box.Max.y += m_GroundOffset;
			return box;
		}
		return AABox();
	}

	Sphere OSGBillboardComponent::GetBoundingSphere() const
	{
		if(m_Geom)
		{
			return GetBoundingBox().GetBoundingSphere();
		}
		return Sphere();
	}

	void OSGBillboardComponent::GetMeshData(GraphicsMeshPtr mesh_data)
	{

	}

	//From OSG billboard example
	osg::ref_ptr<osg::Geometry> OSGBillboardComponent::CreateSquare(const osg::Vec3& corner,const osg::Vec3& width,const osg::Vec3& height, osg::Image* image)
	{
		// set up the Geometry.
		osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
		geom->setUseDisplayList(false);
		geom->setDataVariance(osg::Object::DYNAMIC);
		osg::ref_ptr<osg::Vec3Array> coords = new osg::Vec3Array(4);
		(*coords)[0] = corner;
		(*coords)[1] = corner+width;
		(*coords)[2] = corner+width+height;
		(*coords)[3] = corner+height;
		geom->setVertexArray(coords.get());

		osg::ref_ptr<osg::Vec3Array> norms = new osg::Vec3Array(1);
		(*norms)[0] = width^height;
		(*norms)[0].normalize();

		geom->setNormalArray(norms.get(),osg::Array::BIND_OVERALL);
	
		auto tcoords = new osg::Vec4Array(4);
		(*tcoords)[0].set(0.0f,0.0f, 0.0f, 0.0f);
		(*tcoords)[1].set(1.0f,0.0f, 0.0f, 0.0f);
		(*tcoords)[2].set(1.0f,1.0f, 0.0f, 0.0f);
		(*tcoords)[3].set(0.0f,1.0f, 0.0f, 0.0f);
		geom->setTexCoordArray(0,tcoords, osg::Array::BIND_PER_VERTEX);

		osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array(4);
		(*colors)[0].set(1.0f,1.0f,1.0f,1.0f);
		(*colors)[1].set(1.0f,1.0f,1.0f,1.0f);
		(*colors)[2].set(1.0f,1.0f,1.0f,1.0f);
		(*colors)[3].set(1.0f,1.0f,1.0f,1.0f);
		geom->setColorArray(colors, osg::Array::BIND_PER_VERTEX);

		osg::ref_ptr<osg::DrawArrays> arrays = new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,4);
		geom->addPrimitiveSet(arrays.get());
		if (image)
		{
			osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
			osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
			texture->setImage(image);

			osg::ref_ptr<osg::AlphaFunc> alpha_func = new osg::AlphaFunc;
			alpha_func->setFunction(osg::AlphaFunc::GEQUAL,0.05f);
			stateset->setAttributeAndModes( alpha_func.get(), osg::StateAttribute::ON );

			stateset->setTextureAttributeAndModes(0,texture.get(),osg::StateAttribute::ON);
			geom->setStateSet(stateset.get());
		}
		return geom;
	}

	void OSGBillboardComponent::SetCastShadow(bool value)
	{
		m_CastShadow = value;
		if(m_CastShadow && m_OSGBillboard.valid())
		{
			m_OSGBillboard->setNodeMask(NM_CAST_SHADOWS | m_OSGBillboard->getNodeMask());
			m_OSGBillboard->setNodeMask(NM_RECEIVE_SHADOWS | m_OSGBillboard->getNodeMask());
		}
		else if(m_OSGBillboard.valid())
		{
			m_OSGBillboard->setNodeMask(~NM_CAST_SHADOWS & m_OSGBillboard->getNodeMask());
			m_OSGBillboard->setNodeMask(~NM_RECEIVE_SHADOWS & m_OSGBillboard->getNodeMask());
		}
	}

	void OSGBillboardComponent::SetScale(float width, float height)
	{
		m_ScaleWidth = width;
		m_ScaleHeight = height;
		SetSize(m_Width, m_Height);
	}

	void OSGBillboardComponent::SetSize(float width,float height)
	{
		m_Width = width; 
		m_Height = width;
		if(m_OSGBillboard.valid())
		{
			auto* coords = static_cast<osg::Vec3Array*> (m_Geom->getVertexArray());

			osg::Vec3f osg_height(0.0f,0.0f, m_ScaleHeight*height);
			osg::Vec3f osg_width(m_ScaleWidth*width,0.0f,0.0f);
			osg::Vec3f osg_corner = -osg_width*0.5f;

			(*coords)[0] = osg_corner;
			(*coords)[1] = osg_corner+osg_width;
			(*coords)[2] = osg_corner+osg_width+osg_height;
			(*coords)[3] = osg_corner+osg_height;
			m_Geom->setVertexArray(coords);
			m_Geom->getVertexArray()->dirty();
		}
	}

	void OSGBillboardComponent::SetColor(const ColorRGBA &color)
	{
		if(m_Geom)
		{
			auto* colors = static_cast<osg::Vec4Array*> (m_Geom->getColorArray());
			osg::Vec4 osg_color = OSGConvert::ToOSG(color);
			(*colors)[0]= osg_color;
			(*colors)[1]= osg_color;
			(*colors)[2]= osg_color;
			(*colors)[3]= osg_color;
			m_Geom->setColorArray(colors);
			m_Geom->getColorArray()->dirty();
		}
	}

	void OSGBillboardComponent::OnCollisionSettings(CollisionSettingsRequestPtr message)
	{
		SetCollision(message->EnableCollision());
	}

	void OSGBillboardComponent::SetCollision(bool value)
	{
		if(m_OSGBillboard.valid() && m_OSGBillboard->getNodeMask())
		{
			if(value)
			{
				OSGConvert::SetOSGNodeMask(m_GeomFlags, m_OSGBillboard);
			}
			else
			{
				OSGConvert::SetOSGNodeMask(GEOMETRY_FLAG_TRANSPARENT_OBJECT, m_OSGBillboard);
			}
		}
		m_Collision = value;
	}

	bool OSGBillboardComponent::GetCollision() const
	{
		return m_Collision;
	}

	bool OSGBillboardComponent::GetVisible() const
	{
		if (m_OSGBillboard)
			return m_OSGBillboard->getNodeMask() > 0;
		return false;
	}

	void OSGBillboardComponent::SetVisible(bool value)
	{
		if(m_OSGBillboard)
		{
			if(value)
			{
				m_OSGBillboard->setNodeMask(1);
				SetGeometryFlags(m_GeomFlags);
				SetCastShadow(m_CastShadow);
			}
			else
			{
				m_OSGBillboard->setNodeMask(0);
			}
		}
	}
}