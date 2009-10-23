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
#include "OSGLineComponent.h"
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
#include "OSGLocationComponent.h"


#include <osg/Geode>
#include <osg/Geometry>
#include <osg/LineWidth>

namespace GASS
{
	OSGLineComponent::OSGLineComponent(): m_TexScale(0.1,1),
		m_HeightOffset (0),
		m_MaterialName("BaseWhiteNoLighting"),
		m_Type("line_list")
	{


	}

	OSGLineComponent::~OSGLineComponent()
	{

	}

	void OSGLineComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("LineComponent",new Creator<OSGLineComponent, IComponent>);
		RegisterProperty<std::string>("MaterialName", &OSGLineComponent::GetMaterialName, &OSGLineComponent::SetMaterialName);
		RegisterProperty<std::string>("Type", &OSGLineComponent::GetType, &OSGLineComponent::SetType);
		RegisterProperty<float>("HeightOffset", &OSGLineComponent::GetHeightOffset, &OSGLineComponent::SetHeightOffset);
	}

	void OSGLineComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(OBJECT_RM_LOAD_GFX_COMPONENTS, TYPED_MESSAGE_FUNC(OSGLineComponent::OnLoad,LoadGFXComponentsMessage),1);
	}

	void OSGLineComponent::OnLoad(LoadGFXComponentsMessagePtr message)
	{
		OSGGraphicsSceneManager* osgsm = static_cast<OSGGraphicsSceneManager*>(message->GetGFXSceneManager());
		assert(osgsm);

		m_OSGGeometry = new osg::Geometry();
		m_GeoNode = new osg::Geode();
		
		osg::StateSet *ss = m_GeoNode->getOrCreateStateSet();
		osg::ref_ptr<osg::LineWidth> linewidth = new osg::LineWidth(); 
		
		linewidth->setWidth(2); 
		ss->setAttributeAndModes(linewidth.get(),osg::StateAttribute::ON); 
		ss->setMode(GL_LIGHTING,osg::StateAttribute::OFF); 


		OSGLocationComponent * lc = GetSceneObject()->GetFirstComponent<OSGLocationComponent>().get();
		
		m_GeoNode->addDrawable(m_OSGGeometry.get());
		lc->GetOSGNode()->addChild(m_GeoNode.get());


		osg::ref_ptr<osg::Vec3dArray> vertices = new osg::Vec3dArray();
		osg::ref_ptr<osg::Vec4Array> colors= new osg::Vec4Array;
		
		m_OSGGeometry->setVertexArray(vertices.get());
		m_OSGGeometry->setColorArray(colors.get());

		osg::ref_ptr<osg::DrawArrays> drawable = new osg::DrawArrays();
		m_OSGGeometry->addPrimitiveSet(drawable.get());

		if(m_ControlPointList != "")
		{
			Vec3 new_pos;
			std::string cpl  = m_ControlPointList;
			int pos = cpl.find("#");
			while(pos != -1)
			{
				std::string pos_post = cpl.substr(0,pos);
				Misc::GetVector(pos_post.c_str(),new_pos);
				LineData data;
				data.pos = new_pos;
				data.color = Vec3(1,1,1);
				m_ControlPoints.push_back(data);
				cpl = cpl.substr(pos+1);
				pos = cpl.find("#");

			}
			Misc::GetVector(cpl.c_str(),new_pos);
			LineData data;
			data.pos = new_pos;
			data.color = Vec3(1,1,1);
			m_ControlPoints.push_back(data);

			BuildLineFromControlPoints();
		}
	}

	void OSGLineComponent::Clear()
	{
		m_ControlPoints.clear();
		//	if(m_LineObject)
		//		m_LineObject->clear();
		if(m_OSGGeometry == NULL)
			return;

		osg::Vec3dArray* vertices = static_cast<osg::Vec3dArray*>( m_OSGGeometry->getVertexArray());
		osg::Vec4Array* colors = static_cast<osg::Vec4Array*>( m_OSGGeometry->getColorArray());
		if(vertices)
			vertices->clear();
		if(colors)
			colors->clear();
	}


	void OSGLineComponent::UpdateLineFromControlPoints()
	{
		// pass the created vertex array to the points geometry object.

		if(m_OSGGeometry == NULL)
			return;
		osg::Vec3dArray* vertices = static_cast<osg::Vec3dArray*>( m_OSGGeometry->getVertexArray());
		osg::Vec4Array* colors = static_cast<osg::Vec4Array*>( m_OSGGeometry->getColorArray());
		if(vertices)
			vertices->clear();
		
		if(colors)
		{
			colors->clear();
		}
		

		if(m_ControlPoints.size() > 0)
		{
			vertices->resize(m_ControlPoints.size());
			colors->resize(m_ControlPoints.size());

			osg::Vec3dArray::iterator vitr = vertices->begin();
			osg::Vec4Array::iterator citr = colors->begin();

			float tex_coord = 0;
			for(int i = 0; i < m_ControlPoints.size(); i++)
			{
				Vec3 pos = m_ControlPoints[i].pos;
				tex_coord  = pos.FastLength();

				(vitr++)->set(pos.x, pos.y, pos.z);
				(citr++)->set(m_ControlPoints[i].color.x, m_ControlPoints[i].color.y, m_ControlPoints[i].color.z,1);
			}

			m_OSGGeometry->setVertexArray(vertices);
			m_OSGGeometry->setColorArray(colors);

			//m_Geometry->setColorBinding(osg::Geometry::BIND_OVERALL);
		}
	}

	void OSGLineComponent::BuildLineFromControlPoints()
	{

		if(m_OSGGeometry == NULL)
			return;
		osg::PrimitiveSet::Mode op;

		if(m_Type == "line_list")
		{
			op = osg::PrimitiveSet::LINES;
		}
		else if(m_Type == "point_list")
		{
			op = osg::PrimitiveSet::POINTS;
		}
		else if(m_Type == "line_strip")
		{
			op = osg::PrimitiveSet::LINE_STRIP;
		}

		osg::Vec3dArray* vertices = static_cast<osg::Vec3dArray*>( m_OSGGeometry->getVertexArray());
		osg::Vec4Array* colors = static_cast<osg::Vec4Array*>( m_OSGGeometry->getColorArray());
		if(vertices)
			vertices->clear();
		if(colors)
		{
			colors->clear();
		}
		

		if(m_ControlPoints.size() > 0)
		{
			vertices->resize(m_ControlPoints.size());
			colors->resize(m_ControlPoints.size());

			//osg::ref_ptr<osg::DrawArrays> drawable = new osg::DrawArrays(op, 0, m_ControlPoints.size());

		
			OSGLocationComponent * lc = GetSceneObject()->GetFirstComponent<OSGLocationComponent>().get();
		
			
			osg::DrawArrays* drawable = static_cast<osg::DrawArrays*>(m_OSGGeometry->getPrimitiveSet(0));//drawable.get());
			drawable->set(op, 0, m_ControlPoints.size());

			osg::Vec3dArray::iterator vitr = vertices->begin();

			osg::Vec4Array::iterator citr = colors->begin();

			
			Vec3 start_pos = m_ControlPoints[0].pos;
			lc->SetPosition(start_pos);

			float tex_coord = 0;
			for(int i = 0; i < m_ControlPoints.size(); i++)
			{
				Vec3 pos = m_ControlPoints[i].pos - start_pos;
				tex_coord  = pos.FastLength();

				(vitr++)->set(pos.x, pos.y, pos.z);
				(citr++)->set(m_ControlPoints[i].color.x, m_ControlPoints[i].color.y, m_ControlPoints[i].color.z,1);
			}

			m_OSGGeometry->setVertexArray(vertices);
			m_OSGGeometry->setColorArray(colors);
			//m_Geometry->setColorBinding(osg::Geometry::BIND_OVERALL);
		}
	}

	void OSGLineComponent::Add(const std::vector<LineData> &line_seg)
	{
		for(unsigned int i = 0 ; i < line_seg.size();i++)
		{
			m_ControlPoints.push_back(line_seg[i]);
		}
		BuildLineFromControlPoints();
	}


	AABox OSGLineComponent::GetBoundingBox() const
	{
		AABox box;
		return box;
	}

	Sphere OSGLineComponent::GetBoundingSphere() const
	{
		Sphere sphere;
		return sphere;
	}

}
