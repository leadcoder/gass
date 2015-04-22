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
#pragma once

#include "Sim/GASS.h"
#include "OSGConvert.h"
#include <osg/Referenced>
#include <osg/LineWidth>
#include <osg/Point>
namespace GASS
{
	class OSGDebugDraw: public osg::Referenced
	{
	public:	
		OSGDebugDraw() : m_Geode(new osg::Geode())
			,m_Geometry(new osg::Geometry())
			,m_Lines(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, 0))
			,m_Points(new osg::DrawArrays(osg::PrimitiveSet::POINTS, 0, 0))
			,m_Offset(0,0,0)
		{
			m_Geode->setName("Debug Lines");
			
			osg::StateSet* ss = m_Geode->getOrCreateStateSet();
			osg::LineWidth* linewidth = new osg::LineWidth(); 
			linewidth->setWidth(2); 
			ss->setAttributeAndModes(linewidth, osg::StateAttribute::ON); 
			ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);    
			ss->setAttribute(new osg::Point(10.0f), osg::StateAttribute::ON);
			ss->setRenderingHint(osg::StateSet::DEFAULT_BIN);
			ss->setMode(GL_BLEND,osg::StateAttribute::OVERRIDE |
				osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF);

			m_Geometry->setVertexArray(new osg::Vec3Array());
			m_Geometry->setUseDisplayList(false);
			
			m_Geode->addDrawable(m_Geometry.get());
			m_Geometry->addPrimitiveSet(m_Points.get());
			m_Geometry->addPrimitiveSet(m_Lines.get());
		}
		virtual ~OSGDebugDraw(){}

		void DrawLine(const Vec3 &start_pos,const Vec3 &end_pos, const ColorRGBA &start_color, const ColorRGBA &end_color)
		{
			osg::Vec3 start = OSGConvert::Get().ToOSG(start_pos); 
			osg::Vec3 end = OSGConvert::Get().ToOSG(end_pos); 

			osg::Vec3Array* arr = static_cast<osg::Vec3Array*>(m_Geometry->getVertexArray());
			arr->push_back(start);
			arr->push_back(end);

			// hack around OSG bug:
			// OSG seems to delete the color array when I add a geometry with zero vertices.
			// So check now if no color array exists and add one if necessary.
			osg::Vec4Array* colors;
			if(m_Geometry->getColorArray() == NULL)
			{
				colors = new osg::Vec4Array();
				m_Geometry->setColorArray(colors);
				m_Geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
			}
			else
			{
				colors = static_cast<osg::Vec4Array*>(m_Geometry->getColorArray());
			}
			
			colors->push_back(osg::Vec4(start_color.r, start_color.g, start_color.b, start_color.a));
			colors->push_back(osg::Vec4(end_color.r, end_color.g, end_color.b, end_color.a));

			m_Lines->setCount(m_Lines->getCount() + 2);
			m_Geometry->dirtyBound();
		}


		void Clear()
		{
			// clear local geometry
			static_cast<osg::Vec3Array*>(m_Geometry->getVertexArray())->clear();
			if(m_Geometry->getColorArray() != NULL)
			{
				static_cast<osg::Vec4Array*>(m_Geometry->getColorArray())->clear();
			}

			m_Lines->setCount(0);
			m_Points->setCount(0);
			m_Geometry->dirtyBound();

			// now remove all drawables that were added to the geodes from outside
			for(unsigned int i = 0; i < m_Geode->getNumDrawables(); ++i)
			{
				if(m_Geode->getDrawable(i) != m_Geometry)
				{
					m_Geode->removeDrawables(i, 1);
				}
			}

			for(unsigned int i = 0; i < m_Geode->getNumDrawables(); ++i)
			{
				if(m_Geode->getDrawable(i) != m_Geometry)
				{
					m_Geode->removeDrawables(i, 1);
				}
			}
		}
		osg::ref_ptr<osg::Node> GetNode() const {return m_Geode;}
	private:
		osg::ref_ptr<osg::Geode> m_Geode;
		osg::ref_ptr<osg::Geometry> m_Geometry;
		osg::ref_ptr<osg::DrawArrays> m_Lines;
		osg::ref_ptr<osg::DrawArrays> m_Points;
		Vec3 m_Offset;
	};
}