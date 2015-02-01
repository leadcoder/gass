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

#include "Plugins/OSG/Components/OSGBillboardComponent.h"
#include <osg/Billboard>
#include <osgDB/ReadFile>
#include <osg/Texture2D>
#include <osg/AlphaFunc>

#include "Plugins/OSG/OSGGraphicsSceneManager.h"
#include "Plugins/OSG/OSGGraphicsSystem.h"

#include "Plugins/OSG/Components/OSGLocationComponent.h"
#include "Plugins/OSG/OSGConvert.h"
#include "Plugins/OSG/OSGNodeMasks.h"
#include "Plugins/OSG/OSGNodeData.h"
#include <osg/Material>
#include <osg/BlendFunc>


namespace GASS
{
	OSGBillboardComponent::OSGBillboardComponent() : m_CastShadow(false),
		m_OSGBillboard (NULL),
		m_Width(1.0f),
		m_Height(1.0f),
		m_GroundOffset(0.5),
		m_GeomFlags(GEOMETRY_FLAG_UNKNOWN)
	{

	}	

	OSGBillboardComponent::~OSGBillboardComponent()
	{
		
	}

	void OSGBillboardComponent::RegisterReflection()
	{
		GASS::ComponentFactory::GetPtr()->Register("BillboardComponent",new GASS::Creator<OSGBillboardComponent, Component>);
		RegisterProperty<std::string>("Material", &GetMaterial, &SetMaterial);
		RegisterProperty<bool>("CastShadow", &GetCastShadow, &SetCastShadow);
		RegisterProperty<float>("Height", &GASS::OSGBillboardComponent::GetHeight, &GASS::OSGBillboardComponent::SetHeight);
		RegisterProperty<float>("Width", &GASS::OSGBillboardComponent::GetWidth, &GASS::OSGBillboardComponent::SetWidth);

		RegisterProperty<GeometryFlagsBinder>("GeometryFlags", &OSGBillboardComponent::GetGeometryFlagsBinder, &OSGBillboardComponent::SetGeometryFlagsBinder,
			EnumerationProxyPropertyMetaDataPtr(new EnumerationProxyPropertyMetaData("Geometry Flags",PF_VISIBLE,&GeometryFlagsBinder::GetStringEnumeration, true)));
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
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGBillboardComponent::OnGeometryScale,GeometryScaleRequest,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGBillboardComponent::OnCollisionSettings,CollisionSettingsRequest,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGBillboardComponent::OnSetColorMessage,BillboardColorRequest,0));
	}

	float OSGBillboardComponent::GetWidth() const 
	{
		return m_Width;
	}

	void OSGBillboardComponent::SetWidth(float width)
	{
		m_Width = width;
		UpdateSize(m_Width,m_Height);
	}

	float OSGBillboardComponent::GetHeight() const
	{
		return m_Height;
	}

	void OSGBillboardComponent::SetHeight(float height)
	{
		m_Height = height;
		UpdateSize(m_Width,m_Height);
	}

	GeometryFlags OSGBillboardComponent::GetGeometryFlags() const
	{
		return m_GeomFlags;
	}

	void OSGBillboardComponent::SetGeometryFlags(GeometryFlags flags)
	{
		m_GeomFlags = flags;
		if(m_OSGBillboard)
			OSGConvert::Get().SetOSGNodeMask(flags, m_OSGBillboard);
	}

	void OSGBillboardComponent::OnLocationLoaded(LocationLoadedEventPtr message)
	{
		
		ResourceManagerPtr rm = SimEngine::Get().GetResourceManager();
		const std::string  full_path = rm->GetFirstResourceByName(m_Material)->Path().GetFullPath();

		Vec3 up(0,0,m_Height);
		Vec3 east(m_Width,0,0);
		
		//make offset
		Vec3 corner = -east*0.5;
		osgDB::ReaderWriter::Options* options = new osgDB::ReaderWriter::Options("dds_flip");

		m_OSGBillboard = new osg::Billboard();
		m_OSGBillboard->setMode(osg::Billboard::POINT_ROT_EYE);


		osg::ref_ptr<osg::Geometry> geom = CreateSquare(osg::Vec3(corner.x,corner.y,corner.z),
														osg::Vec3(east.x,east.y,east.z),
														osg::Vec3(up.x,up.y,up.z),
														osgDB::readImageFile(full_path,options));
														
		m_OSGBillboard->addDrawable(geom);
		m_Geom = geom.get();

		m_OSGBillboard->setPosition(0,osg::Vec3(0,0,m_GroundOffset));
		OSGNodeData* node_data = new OSGNodeData(shared_from_this());
		m_OSGBillboard->setUserData(node_data);
	
		OSGLocationComponentPtr lc = GetSceneObject()->GetFirstComponentByClass<OSGLocationComponent>();
		lc->GetOSGNode()->addChild(m_OSGBillboard.get());

		osg::ref_ptr<osg::StateSet> nodess (m_OSGBillboard->getOrCreateStateSet());
		nodess->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
		SetCastShadow(m_CastShadow);
		SetGeometryFlags(m_GeomFlags);
		GetSceneObject()->PostEvent(GeometryChangedEventPtr(new GeometryChangedEvent(DYNAMIC_PTR_CAST<IGeometryComponent>(shared_from_this()))));
	}

	AABox OSGBillboardComponent::GetBoundingBox() const
	{
		if(m_Geom)
		{
			osg::Vec3Array* coords = static_cast<osg::Vec3Array*> (m_Geom->getVertexArray());
			osg::Vec3 p_min = (*coords)[0];
			osg::Vec3 p_max = (*coords)[2];
			osg::Vec3 bb_size = p_max - p_min;
			Float max_size = Math::Max(bb_size.x() ,bb_size.z() )*0.5f;
			Float offset = bb_size.z() * 0.5f;
			AABox box(Vec3(-max_size,-max_size + offset,-max_size),Vec3(max_size,max_size+offset,max_size));
			box.m_Min.y += m_GroundOffset;
			box.m_Max.y += m_GroundOffset;
			return box;
		}
		return AABox();
	}

	Sphere OSGBillboardComponent::GetBoundingSphere() const
	{
		if(m_Geom)
		{
			return GetBoundingBox().GetBoundingSphere();
			/*osg::Vec3Array* coords = static_cast<osg::Vec3Array*> (m_Geom->getVertexArray());
			osg::Vec3 p_min = (*coords)[0];
			osg::Vec3 p_max = (*coords)[2];
			osg::Vec3 bb_size = p_max - p_min;
			Float max_size = Math::Max(bb_size.x() ,bb_size.z() )*0.5f;
			Sphere sphere;
			sphere.m_Pos = Vec3(0,m_GroundOffset,0);
			sphere.m_Radius = max_size;
			return sphere;*/
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
		osg::ref_ptr<osg::Vec3Array> coords = new osg::Vec3Array(4);
		(*coords)[0] = corner;
		(*coords)[1] = corner+width;
		(*coords)[2] = corner+width+height;
		(*coords)[3] = corner+height;
		geom->setVertexArray(coords.get());

		osg::ref_ptr<osg::Vec3Array> norms = new osg::Vec3Array(1);
		(*norms)[0] = width^height;
		(*norms)[0].normalize();

		geom->setNormalArray(norms.get());
		geom->setNormalBinding(osg::Geometry::BIND_OVERALL);

		osg::ref_ptr<osg::Vec2Array> tcoords = new osg::Vec2Array(4);
		(*tcoords)[0].set(0.0f,0.0f);
		(*tcoords)[1].set(1.0f,0.0f);
		(*tcoords)[2].set(1.0f,1.0f);
		(*tcoords)[3].set(0.0f,1.0f);
		geom->setTexCoordArray(0,tcoords.get());

		osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array(4);
		(*colors)[0].set(1.0f,1.0f,1.0f,1.0f);
		(*colors)[1].set(1.0f,1.0f,1.0f,1.0f);
		(*colors)[2].set(1.0f,1.0f,1.0f,1.0f);
		(*colors)[3].set(1.0f,1.0f,1.0f,1.0f);
		geom->setColorArray(colors);
		geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);


		osg::ref_ptr<osg::DrawArrays> arrays = new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,4);
		geom->addPrimitiveSet(arrays.get());
		if (image)
		{
			osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
			osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
			texture->setImage(image);

			osg::ref_ptr<osg::AlphaFunc> alphaFunc = new osg::AlphaFunc;
			alphaFunc->setFunction(osg::AlphaFunc::GEQUAL,0.05f);
			stateset->setAttributeAndModes( alphaFunc.get(), osg::StateAttribute::ON );
			
			stateset->setTextureAttributeAndModes(0,texture.get(),osg::StateAttribute::ON);
			geom->setStateSet(stateset.get());
		}
		return geom;
	}

	void OSGBillboardComponent::SetCastShadow(bool value)
	{
		m_CastShadow = value;
		if(m_CastShadow && m_OSGBillboard.valid())
			m_OSGBillboard->setNodeMask(NM_CAST_SHADOWS | m_OSGBillboard->getNodeMask());
		else if(m_OSGBillboard.valid())
		{
			m_OSGBillboard->setNodeMask(~NM_CAST_SHADOWS & m_OSGBillboard->getNodeMask());
		}
	}

	void OSGBillboardComponent::OnGeometryScale(GeometryScaleRequestPtr message)
	{
		const Vec3 scale = message->GetScale();
		UpdateSize(m_Width*scale.x,m_Height*scale.y);
	}

	void OSGBillboardComponent::UpdateSize(float width,float height)
	{
		if(m_OSGBillboard.valid())
		{
			osg::Vec3Array* coords = static_cast<osg::Vec3Array*> (m_Geom->getVertexArray());

			osg::Vec3 height(0,0,height);
			osg::Vec3 width(width,0,0);
			osg::Vec3 corner = -width*0.5;
			

			(*coords)[0] = corner;
			(*coords)[1] = corner+width;
			(*coords)[2] = corner+width+height;
			(*coords)[3] = corner+height;
			m_Geom->setVertexArray(coords);
		}
	}


	void OSGBillboardComponent::OnSetColorMessage(BillboardColorRequestPtr message)
	{
		/*if(m_OSGBillboard.valid())
		{
		
		osg::ref_ptr<osg::Material> mat (new osg::Material);
		mat->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(color.r,color.g,color.b,color.a));
		mat->setAmbient(osg::Material::FRONT_AND_BACK,osg::Vec4(color.r,color.g,color.b,color.a));
		osg::ref_ptr<osg::StateSet> nodess (m_OSGBillboard->getOrCreateStateSet());
		nodess->setAttribute(mat.get());
		nodess->setAttributeAndModes( mat.get() , osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
		}*/

		if(m_Geom)
		{
			const ColorRGBA color = message->GetColor();
			osg::Vec4Array* colors = static_cast<osg::Vec4Array*> (m_Geom->getColorArray());
			//osg::Vec4Array* colors = dynamic_cast<osg::Vec4Array*>(m_Geom->getColorArray());
			(*colors)[0].set(color.r,color.g,color.b,color.a);
			(*colors)[1].set(color.r,color.g,color.b,color.a);
			(*colors)[2].set(color.r,color.g,color.b,color.a);
			(*colors)[3].set(color.r,color.g,color.b,color.a);
			m_Geom->setColorArray(colors);
			
		}
	}

	void OSGBillboardComponent::OnCollisionSettings(CollisionSettingsRequestPtr message)
	{
		if(m_OSGBillboard.valid())
		{
			if(message->EnableCollision())
			{
 				OSGConvert::Get().SetOSGNodeMask(m_GeomFlags, m_OSGBillboard);
			}
			else
			{
				OSGConvert::Get().SetOSGNodeMask(GEOMETRY_FLAG_TRANSPARENT_OBJECT, m_OSGBillboard);
			}
		}
	}
}
