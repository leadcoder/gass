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
#include <osgDB/ReadFile> 



namespace GASS
{
	OSGManualMeshComponent::OSGManualMeshComponent() : m_GeomFlags(GEOMETRY_FLAG_UNKOWN),
		m_CastShadow(false)
	{

	}

	OSGManualMeshComponent::~OSGManualMeshComponent()
	{

	}

	void OSGManualMeshComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("ManualMeshComponent",new Creator<OSGManualMeshComponent, IComponent>);
		RegisterProperty<bool>("CastShadow", &GetCastShadow, &SetCastShadow,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Should this mesh cast shadows or not",PF_VISIBLE | PF_EDITABLE)));
		//RegisterProperty<GeometryFlags>("GeometryFlags", &OSGManualMeshComponent::GetGeometryFlags, &OSGManualMeshComponent::SetGeometryFlags);
	}

	void OSGManualMeshComponent::OnInitialize()
	{
		m_GFXSystem = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<OSGGraphicsSystem>();
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGManualMeshComponent::OnLocationLoaded,LocationLoadedMessage,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGManualMeshComponent::OnDataMessage,ManualMeshDataMessage,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGManualMeshComponent::OnClearMessage,ClearManualMeshMessage,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGManualMeshComponent::OnMaterialMessage,ReplaceMaterialMessage,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGManualMeshComponent::OnCollisionSettings,CollisionSettingsMessage ,0));

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

		OSGNodeData* node_data = new OSGNodeData(shared_from_this());
		m_GeoNode->setUserData(node_data);
		SetGeometryFlags(m_GeomFlags);
		BaseSceneComponent::OnInitialize();
		

	}

	void OSGManualMeshComponent::SetCastShadow(bool value)
	{
		m_CastShadow = value;
		if(m_CastShadow && m_GeoNode.valid())
			m_GeoNode->setNodeMask(NM_CAST_SHADOWS | m_GeoNode->getNodeMask());
		else if(m_GeoNode.valid())
		{
			m_GeoNode->setNodeMask(~NM_CAST_SHADOWS & m_GeoNode->getNodeMask());
		}
	}

	void OSGManualMeshComponent::SetGeometryFlags(GeometryFlags value)
	{
		m_GeomFlags = value;
		if(m_GeoNode.valid())
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
		GraphicsMeshPtr data = message->GetData();
		CreateMesh(data);
	}

	void OSGManualMeshComponent::OnClearMessage(ClearManualMeshMessagePtr message)
	{
		Clear();
	}

	void OSGManualMeshComponent::Clear()
	{
		//remove all drawables;
		for(size_t i = 0; i < m_OSGGeometries.size(); i++)
		{
			m_GeoNode->removeDrawable(m_OSGGeometries[i]);
		}
		m_OSGGeometries.clear();
	}

	void OSGManualMeshComponent::CreateSubMesh(GraphicsSubMeshPtr sm, osg::ref_ptr<osg::Geometry> geom )
	{
		
		/*if(data->Material != m_CurrentMaterial) //try loading material
		{
		m_CurrentMaterial = data->Material;

		const std::string osg_mat = data->Material + ".osg";
		ResourceManagerPtr rm = SimEngine::Get().GetResourceManager();
		if(rm->HasResource(osg_mat))
		{
		ResourceHandle res(osg_mat);
		osg::ref_ptr<osg::Group> material = (osg::Group*) osgDB::readNodeFile(res.GetResource()->Path().GetFullPath());
		osg::ref_ptr<osg::Node> node = material->getChild(0);
		osg::ref_ptr<osg::Drawable> drawable = node->asGeode()->getDrawable(0);
		osg::ref_ptr<osg::StateSet> state_set = drawable->getStateSet();
		m_OSGGeometry->setStateSet(state_set);
		//mat->setColorMode(osg::Material::ColorMode::DIFFUSE)
		}
		//osg::StateSet* state_copy =  static_cast<osg::StateSet*> (state_set->clone(osg::CopyOp::DEEP_COPY_STATESETS));
		//m_OSGGeometry->setStateSet(state_copy);
		}*/

		if(sm->MaterialName != "" && m_GFXSystem->HasMaterial(sm->MaterialName))
		{
			osg::ref_ptr<osg::StateSet> state_set = m_GFXSystem->GetStateSet(sm->MaterialName);
			geom->setStateSet(state_set);
		}
	
		osg::PrimitiveSet::Mode op;
		switch(sm->Type)
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

		osg::DrawElementsUInt* de = new osg::DrawElementsUInt(op);

		if(sm->IndexVector.size() > 0)
		{
			for(int i = 0; i < sm->IndexVector.size(); i++)
			{
				de->push_back(sm->IndexVector[i]);
			}
			geom->addPrimitiveSet(de);
		}
		else
		{
			geom->addPrimitiveSet(new osg::DrawArrays(op, 0, sm->PositionVector.size()));
		}

		osg::ref_ptr<osg::Vec3Array> positions = new osg::Vec3Array();
		for(size_t  i = 0; i < sm->PositionVector.size(); i++)
		{
			positions->push_back(OSGConvert::Get().ToOSG(sm->PositionVector[i]));
		}
		geom->setVertexArray(positions);

		if(sm->NormalVector.size() > 0)
		{
			osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
			for(size_t i = 0; i < sm->NormalVector.size(); i++)
			{
				normals->push_back(OSGConvert::Get().ToOSG(sm->NormalVector[i]));
			}
			geom->setNormalArray(normals);
			geom->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);
		}

		if(sm->ColorVector.size() > 0)
		{
			osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
			for(size_t i = 0; i < sm->ColorVector.size(); i++)
			{
				ColorRGBA color = sm->ColorVector[i];
				colors->push_back(osg::Vec4(color.r,color.g,color.b,color.a));
			}
			geom->setColorArray(colors);
			geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
		}

		for(size_t i = 0; i < sm->TexCoordsVector.size(); i++)
		{
			osg::ref_ptr<osg::Vec2Array> tex_coords = new osg::Vec2Array;
			for(size_t j = 0; j < sm->TexCoordsVector[i].size(); j++)
			{
				Vec4 texc = sm->TexCoordsVector[i][j];
				tex_coords->push_back(osg::Vec2(texc.x,texc.y));
			}
			geom->setTexCoordArray(i,tex_coords);
		}
		geom->dirtyBound();
	}

	void OSGManualMeshComponent::CreateMesh(GraphicsMeshPtr data)
	{
		Clear();
		for(size_t i = 0; i < data->SubMeshVector.size(); i++)
		{
			GraphicsSubMeshPtr sm = data->SubMeshVector[i];
			osg::ref_ptr<osg::Geometry> geom = new osg::Geometry();
			CreateSubMesh(sm,geom );
			m_OSGGeometries.push_back(geom);
			m_GeoNode->addDrawable(geom.get());
		}
		GetSceneObject()->PostMessage(MessagePtr(new GeometryChangedMessage(DYNAMIC_PTR_CAST<IGeometryComponent>(shared_from_this()))));
	}

	AABox OSGManualMeshComponent::GetBoundingBox() const
	{
		AABox comp_box;
		for(size_t i = 0; i < m_OSGGeometries.size(); i++)
		{
			osg::BoundingBox osg_box = m_OSGGeometries[i]->getBound();
			AABox box(OSGConvert::Get().ToGASS(osg_box._min),OSGConvert::Get().ToGASS(osg_box._max));
			comp_box.Union(box);
		}
		
		
		OSGLocationComponentPtr lc = GetSceneObject()->GetFirstComponentByClass<OSGLocationComponent>();
		if(lc)
		{
			Vec3 scale = OSGConvert::Get().ToGASS(lc->GetOSGNode()->getScale());
			comp_box.m_Max = comp_box.m_Max*scale;
			comp_box.m_Min = comp_box.m_Min*scale;
		}
		return comp_box;
	}

	Sphere OSGManualMeshComponent::GetBoundingSphere() const
	{
		Sphere sphere;
		//assert(m_MeshObject);
		sphere.m_Pos = Vec3(0,0,0);
		sphere.m_Radius = 0;
		for(size_t i = 0; i < m_OSGGeometries.size(); i++)
		{
			osg::BoundingSphere bsphere = m_OSGGeometries[i]->getBound();
			if(bsphere._radius > sphere.m_Radius)
				sphere.m_Radius = bsphere._radius;
		}
		

		OSGLocationComponentPtr lc = GetSceneObject()->GetFirstComponentByClass<OSGLocationComponent>();
		if(lc)
		{
			Vec3 scale = OSGConvert::Get().ToGASS(lc->GetOSGNode()->getScale());
			sphere.m_Radius *= Math::Max(scale.x,scale.y,scale.z);
		}
		return sphere;
	}


	void OSGManualMeshComponent::OnMaterialMessage(ReplaceMaterialMessagePtr message)
	{

		/*Vec4 diffuse = message->GetDiffuse();
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
		{
		nodess->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
		//after transparent bin
		nodess->setRenderBinDetails(11,"RenderBin");
		}

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
		}*/
	}
}
