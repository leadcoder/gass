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


#include <boost/bind.hpp>

#include "OSGManualMeshComponent.h"
#include "Core/Math/Vector.h"
#include "Core/Math/Quaternion.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/ComponentSystem/IComponent.h"

#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/IMessage.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"

#include "Plugins/OSG/OSGGraphicsSceneManager.h"
#include "Plugins/OSG/OSGConvert.h"
#include "Plugins/OSG/Components/OSGLocationComponent.h"

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/LineWidth>


namespace GASS
{
	OSGManualMeshComponent::OSGManualMeshComponent()
	{

	}

	OSGManualMeshComponent::~OSGManualMeshComponent()
	{

	}

	void OSGManualMeshComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("ManualMeshComponent",new Creator<OSGManualMeshComponent, IComponent>);
	}

	void OSGManualMeshComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGManualMeshComponent::OnLoad,LoadGFXComponentsMessage,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGManualMeshComponent::OnDataMessage,ManualMeshDataMessage,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGManualMeshComponent::OnClearMessage,ClearManualMeshMessage,1));
	}

	void OSGManualMeshComponent::OnLoad(LoadGFXComponentsMessagePtr message)
	{
//		OSGGraphicsSceneManager* osgsm = static_cast<OSGGraphicsSceneManager*>(message->GetGFXSceneManager());
//		assert(osgsm);

		m_OSGGeometry = new osg::Geometry();
		m_GeoNode = new osg::Geode();

		osg::StateSet *ss = m_GeoNode->getOrCreateStateSet();
		
		ss->setMode(GL_LIGHTING,osg::StateAttribute::OFF); 
		

		OSGLocationComponent * lc = GetSceneObject()->GetFirstComponent<OSGLocationComponent>().get();

		m_GeoNode->addDrawable(m_OSGGeometry.get());
		lc->GetOSGNode()->addChild(m_GeoNode.get());

		osg::ref_ptr<osg::Vec3dArray> vertices = new osg::Vec3dArray();
		osg::ref_ptr<osg::Vec4Array> colors= new osg::Vec4Array;

		m_OSGGeometry->setVertexArray(vertices.get());
		m_OSGGeometry->setColorArray(colors.get());

		//m_DrawArrays = new osg::DrawArrays();
		//m_DrawElements = new osg::DrawElementsUInt():
		//m_OSGGeometry->addPrimitiveSet(m_DrawArrays);

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

		osg::Vec3dArray* vertices = static_cast<osg::Vec3dArray*>( m_OSGGeometry->getVertexArray());
		osg::Vec4Array* colors = static_cast<osg::Vec4Array*>( m_OSGGeometry->getColorArray());
		if(vertices)
			vertices->clear();
		if(colors)
			colors->clear();
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

		osg::Vec3dArray* vertices = static_cast<osg::Vec3dArray*>( m_OSGGeometry->getVertexArray());
		osg::Vec4Array* colors = static_cast<osg::Vec4Array*>( m_OSGGeometry->getColorArray());
		
		if(vertices)
			vertices->clear();
		if(colors)
		{
			colors->clear();
		}

		vertices->resize(data->VertexVector.size());
		colors->resize(data->VertexVector.size());


		if(data->IndexVector.size() > 0)
		{
			if(m_OSGGeometry->getNumPrimitiveSets() > 0)
				m_OSGGeometry->setPrimitiveSet(0,new osg::DrawElementsUInt(op,data->IndexVector.size(),&data->IndexVector[0]));
			else 
				m_OSGGeometry->addPrimitiveSet(new osg::DrawElementsUInt(op,data->IndexVector.size(),&data->IndexVector[0]));
			
		}
		else
		{
			if(m_OSGGeometry->getNumPrimitiveSets() > 0)
				m_OSGGeometry->setPrimitiveSet(0,new osg::DrawArrays(op, 0, data->VertexVector.size()));
			else 
				m_OSGGeometry->addPrimitiveSet(new osg::DrawArrays(op, 0, data->VertexVector.size()));
		}

		osg::Vec3dArray::iterator vitr = vertices->begin();
		osg::Vec4Array::iterator citr = colors->begin();

		//m_MeshObject->begin(data->Material, op);
		for(int i = 0; i < data->VertexVector.size(); i++)
		{
			Vec3 pos = data->VertexVector[i].Pos;
			Vec2 tex_coord  = data->VertexVector[i].TexCoord;
			Vec4 color  = data->VertexVector[i].Color;

			(vitr++)->set(pos.x, pos.y, pos.z);
			(citr++)->set(color.x, color.y, color.z,color.w);

			/*m_MeshObject->position(pos.x, pos.y, pos.z);
			m_MeshObject->textureCoord(tex_coord.x,tex_coord.y);
			Ogre::ColourValue col;
			col.r = color.x;
			col.g = color.y;
			col.b = color.z;
			col.a = color.w;
			m_MeshObject->colour(col);*/
		}

		
		m_OSGGeometry->setVertexArray(vertices);
		m_OSGGeometry->setColorArray(colors);

		m_OSGGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);
	}


	AABox OSGManualMeshComponent::GetBoundingBox() const
	{
		AABox box;
		return box;
		//assert(m_MeshObject);
		//return Convert::ToGASS(m_MeshObject->getBoundingBox());

	}

	Sphere OSGManualMeshComponent::GetBoundingSphere() const
	{
		Sphere sphere;
		//assert(m_MeshObject);
		sphere.m_Pos = Vec3(0,0,0);

		osg::BoundingSphere bsphere = m_OSGGeometry->getBound();
		sphere.m_Radius = bsphere._radius;

		return sphere;
	}

}
