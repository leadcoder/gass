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



#include "OSGManualMeshComponent.h"
#include "Plugins/OSG/OSGGraphicsSceneManager.h"
#include "Plugins/OSG/Components/OSGLocationComponent.h"
#include "Plugins/OSG/OSGConvert.h"
#include "Plugins/OSG/OSGNodeMasks.h"
#include "Plugins/OSG/OSGNodeData.h"

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/LineWidth>
#include <osg/Depth>
#include <osg/Point>
#include <osg/StateAttribute>
#include <osg/Material>
#include <osg/BlendFunc>
#include <osg/CullFace>



namespace GASS
{
	OSGManualMeshComponent::OSGManualMeshComponent() : m_GeomFlags(GEOMETRY_FLAG_UNKOWN)
	{

	}

	OSGManualMeshComponent::~OSGManualMeshComponent()
	{

	}

	void OSGManualMeshComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("ManualMeshComponent",new Creator<OSGManualMeshComponent, IComponent>);
		//RegisterProperty<GeometryFlags>("GeometryFlags", &OSGManualMeshComponent::GetGeometryFlags, &OSGManualMeshComponent::SetGeometryFlags);
	}

	void OSGManualMeshComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGManualMeshComponent::OnLocationLoaded,LocationLoadedMessage,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGManualMeshComponent::OnDataMessage,ManualMeshDataMessage,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGManualMeshComponent::OnClearMessage,ClearManualMeshMessage,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGManualMeshComponent::OnMaterialMessage,MaterialMessage,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGManualMeshComponent::OnCollisionSettings,CollisionSettingsMessage ,0));


		m_OSGGeometry = new osg::Geometry();
		m_GeoNode = new osg::Geode();

		osg::StateSet *ss = m_GeoNode->getOrCreateStateSet();
		//ss->setMode(GL_LIGHTING,osg::StateAttribute::OFF); 
		osg::CullFace* cull = new osg::CullFace();
		cull->setMode(osg::CullFace::BACK);
		ss->setAttributeAndModes(cull, osg::StateAttribute::ON);


		osg::ref_ptr<osg::Point> point (new osg::Point( 8.0f ));
		ss->setAttributeAndModes(point, osg::StateAttribute::ON); 

		m_GeoNode->setNodeMask(NM_CAST_SHADOWS | m_GeoNode->getNodeMask());
		m_GeoNode->setNodeMask(NM_RECEIVE_SHADOWS | m_GeoNode->getNodeMask());
		
		m_GeoNode->addDrawable(m_OSGGeometry.get());

		OSGNodeData* node_data = new OSGNodeData(shared_from_this());
		m_GeoNode->setUserData(node_data);
		SetGeometryFlags(m_GeomFlags);


		BaseSceneComponent::OnInitialize();
	}

	void OSGManualMeshComponent::SetGeometryFlags(GeometryFlags value)
	{
		m_GeomFlags = value;
		if(m_OSGGeometry.valid())
		{
			OSGConvert::Get().SetOSGNodeMask(value,m_GeoNode.get());
		}
	}

	GeometryFlags OSGManualMeshComponent::GetGeometryFlags() const
	{
		return m_GeomFlags;
	}

	void OSGManualMeshComponent::OnCollisionSettings(CollisionSettingsMessagePtr message)
	{
		if(m_GeoNode)
		{
			if(message->EnableCollision())
				OSGConvert::Get().SetOSGNodeMask(m_GeomFlags,m_GeoNode.get());
			else
				OSGConvert::Get().SetOSGNodeMask(GEOMETRY_FLAG_TRANSPARENT_OBJECT,m_GeoNode.get());
		}
	}

	void OSGManualMeshComponent::OnLocationLoaded(LocationLoadedMessagePtr message)
	{

		OSGLocationComponentPtr  lc = GetSceneObject()->GetFirstComponentByClass<OSGLocationComponent>();
		if(!lc)
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Failed to find location component: " + GetSceneObject()->GetName(),"OSGManualMeshComponent::OnLoad");

		lc->GetOSGNode()->addChild(m_GeoNode.get());
		
	}

	void OSGManualMeshComponent::OnDataMessage(ManualMeshDataMessagePtr message)
	{
		ManualMeshDataPtr data = message->GetData();
		CreateMesh(data);
	}


	void OSGManualMeshComponent::OnClearMessage(ClearManualMeshMessagePtr message)
	{
		Clear();
	}

	void OSGManualMeshComponent::Clear()
	{
		if(m_OSGGeometry == NULL)
			return;

		osg::Vec3Array* vertices = static_cast<osg::Vec3Array*>( m_OSGGeometry->getVertexArray());
		osg::Vec4Array* colors = static_cast<osg::Vec4Array*>( m_OSGGeometry->getColorArray());
		osg::Vec3Array* normals = static_cast<osg::Vec3Array*>( m_OSGGeometry->getNormalArray());
		if(vertices)
			vertices->clear();
		if(colors)
			colors->clear();

		if(normals)
			normals->clear();
		


		m_OSGGeometry->setVertexArray(vertices);
		m_OSGGeometry->setColorArray(colors);


		if(m_OSGGeometry->getNumPrimitiveSets() > 0)
			m_OSGGeometry->removePrimitiveSet(0);

		m_OSGGeometry->dirtyBound();
		
	}

	void OSGManualMeshComponent::CreateMesh(ManualMeshDataPtr data)
	{
		if(m_OSGGeometry == NULL)
			return;
		osg::PrimitiveSet::Mode op;

		switch(data->Type)
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
		}

		osg::Vec3Array* vertices = static_cast<osg::Vec3Array*>( m_OSGGeometry->getVertexArray());
		osg::Vec4Array* colors = static_cast<osg::Vec4Array*>( m_OSGGeometry->getColorArray());
		osg::Vec3Array* normals = static_cast<osg::Vec3Array*>( m_OSGGeometry->getNormalArray());
		
		if(vertices)
			vertices->clear();
		else
			vertices = new osg::Vec3Array();
		
		if(colors)
			colors->clear();
		else
			colors = new osg::Vec4Array;

		if(normals)
			normals->clear();
		else
			normals = new osg::Vec3Array;

		

		vertices->resize(data->VertexVector.size());
		colors->resize(data->VertexVector.size());
		normals->resize(data->VertexVector.size());

//CopyOp::SHALLOW_COPY

		

		if(data->IndexVector.size() > 0)
		{
			osg::DrawElementsUInt* de = new osg::DrawElementsUInt(op);
			for(int i = 0; i < data->IndexVector.size(); i++)
			{
				de->push_back(data->IndexVector[i]);
			}
			if(m_OSGGeometry->getNumPrimitiveSets() > 0)
				m_OSGGeometry->setPrimitiveSet(0,de);
			else
				m_OSGGeometry->addPrimitiveSet(de);
		}
		else
		{
			if(m_OSGGeometry->getNumPrimitiveSets() > 0)
				m_OSGGeometry->setPrimitiveSet(0,new osg::DrawArrays(op, 0, data->VertexVector.size()));
			else 
				m_OSGGeometry->addPrimitiveSet(new osg::DrawArrays(op, 0, data->VertexVector.size()));
		}

		osg::Vec3Array::iterator vitr = vertices->begin();
		osg::Vec4Array::iterator citr = colors->begin();
		osg::Vec3Array::iterator nitr = normals->begin();

		for(int i = 0; i < data->VertexVector.size(); i++)
		{
			Vec3 pos = data->VertexVector[i].Pos;
			Vec3 normal = data->VertexVector[i].Normal;
			Vec2 tex_coord  = data->VertexVector[i].TexCoord;
			Vec4 color  = data->VertexVector[i].Color;

			osg::Vec3 o_pos = OSGConvert::Get().ToOSG(pos); 
			osg::Vec3 o_normal = OSGConvert::Get().ToOSG(normal); 

			(vitr++)->set(o_pos.x(), o_pos.y(), o_pos.z());
			(citr++)->set(color.x, color.y, color.z,color.w);
			(nitr++)->set(o_normal.x(), o_normal.y(), o_normal.z());
		}

		m_OSGGeometry->setVertexArray(vertices);
		m_OSGGeometry->setColorArray(colors);

		m_OSGGeometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
		m_OSGGeometry->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);
		GetSceneObject()->PostMessage(MessagePtr(new GeometryChangedMessage(DYNAMIC_PTR_CAST<IGeometryComponent>(shared_from_this()))));
		
		m_OSGGeometry->dirtyBound();
	}


	AABox OSGManualMeshComponent::GetBoundingBox() const
	{
		
		osg::BoundingBox osg_box = m_OSGGeometry->getBound();
		AABox box(OSGConvert::Get().ToGASS(osg_box._min),
				OSGConvert::Get().ToGASS(osg_box._max));

		OSGLocationComponentPtr lc = GetSceneObject()->GetFirstComponentByClass<OSGLocationComponent>();
		if(lc)
		{
			Vec3 scale = OSGConvert::Get().ToGASS(lc->GetOSGNode()->getScale());
			box.m_Max = box.m_Max*scale;
			box.m_Min = box.m_Min*scale;
		}
		return box;
	}

	Sphere OSGManualMeshComponent::GetBoundingSphere() const
	{
		Sphere sphere;
		//assert(m_MeshObject);
		sphere.m_Pos = Vec3(0,0,0);

		osg::BoundingSphere bsphere = m_OSGGeometry->getBound();
		sphere.m_Radius = bsphere._radius;

		OSGLocationComponentPtr lc = GetSceneObject()->GetFirstComponentByClass<OSGLocationComponent>();
		if(lc)
		{
			Vec3 scale = OSGConvert::Get().ToGASS(lc->GetOSGNode()->getScale());
			sphere.m_Radius *= Math::Max(scale.x,scale.y,scale.z);
		}
		return sphere;
	}


	void OSGManualMeshComponent::OnMaterialMessage(MaterialMessagePtr message)
	{
		Vec4 diffuse = message->GetDiffuse();
		Vec3 ambient = message->GetAmbient();
		Vec3 specular = message->GetSpecular();
		Vec3 si = message->GetSelfIllumination();

		osg::ref_ptr<osg::Material> mat (new osg::Material);
		
		if( diffuse.x >= 0)
			mat->setDiffuse(osg::Material::FRONT_AND_BACK,osg::Vec4(diffuse.x,diffuse.y,diffuse.z,diffuse.w));
		if( ambient.x >= 0)
			mat->setAmbient(osg::Material::FRONT_AND_BACK,osg::Vec4(ambient.x,ambient.y,ambient.z,1));
		if( specular.x >= 0)
			mat->setSpecular(osg::Material::FRONT_AND_BACK,osg::Vec4(specular.x,specular.y,specular.z,1));
		if( message->GetShininess() >= 0)
			mat->setShininess(osg::Material::FRONT_AND_BACK,message->GetShininess());
		if( si.x >= 0)
			mat->setEmission(osg::Material::FRONT_AND_BACK,osg::Vec4(si.x,si.y,si.z,1));
		
		osg::ref_ptr<osg::StateSet> nodess (m_OSGGeometry->getOrCreateStateSet());
		nodess->setAttribute(mat.get());

		if(message->GetDepthTest())
			nodess->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
		else
			nodess->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
		
		nodess->setAttributeAndModes( mat.get() , osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
        // Turn on blending
		if(diffuse.w < 1.0)
		{
			osg::ref_ptr<osg::BlendFunc> bf (new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA,  osg::BlendFunc::ONE_MINUS_SRC_ALPHA ));
			nodess->setAttributeAndModes(bf);

			// Enable blending, select transparent bin.
			nodess->setMode( GL_BLEND, osg::StateAttribute::ON );
			nodess->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );

			// Enable depth test so that an opaque polygon will occlude a transparent one behind it.
			nodess->setMode( GL_DEPTH_TEST, osg::StateAttribute::ON );

			// Conversely, disable writing to depth buffer so that
			// a transparent polygon will allow polygons behind it to shine through.
			// OSG renders transparent polygons after opaque ones.
			osg::ref_ptr<osg::Depth> depth (new osg::Depth);
			depth->setWriteMask( false );
			nodess->setAttributeAndModes( depth, osg::StateAttribute::ON );
		}
	}
}
