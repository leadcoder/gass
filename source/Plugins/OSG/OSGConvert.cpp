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

#include "OSGConvert.h"
#include "OSGNodeMasks.h"
#include "Sim/GASSGeometryFlags.h"
namespace GASS
{
	
	OSGConvert::OSGConvert()
	{
		
	}

	OSGConvert::~OSGConvert()
	{

	}

	osg::Vec3d OSGConvert::ToOSG(const Vec3 &v)
	{
		return osg::Vec3d(v.x,-v.z,v.y);
	}

	osg::Vec4 OSGConvert::ToOSG(const ColorRGBA &color)
	{
		return osg::Vec4(static_cast<float>(color.r), static_cast<float>(color.g), static_cast<float>(color.b), static_cast<float>(color.a));
	}

	Vec3 OSGConvert::ToGASS(const osg::Vec3 &v)
	{
		return Vec3(v.x(),v.z(),-v.y());
	}

	Vec3 OSGConvert::ToGASS(const osg::Vec3d &v)
	{
		return Vec3(v.x(),v.z(),-v.y());
	}

	Quaternion OSGConvert::ToGASS(const osg::Quat &value)
	{
		return Quaternion(-value.w(),-value.x(),-value.z(),value.y());
	}

	osg::Quat OSGConvert::ToOSG(const Quaternion &value)
	{
		return  osg::Quat(-value.x,value.z,-value.y,-value.w);
	}

	int OSGConvert::ToOSGNodeMask(GeometryFlags flag)
	{
		return flag << NM_USER_OFFSET;
	}

	void OSGConvert::SetOSGNodeMask(GeometryFlags flags, osg::Node* node)
	{
		int mask = ToOSGNodeMask(flags);
		int all_mask = ToOSGNodeMask(GEOMETRY_FLAG_ALL);
		//set geom bits to zero
		node->setNodeMask(~all_mask & node->getNodeMask());
		//set geom bitss
		node->setNodeMask(mask | node->getNodeMask());
	}
}



