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

#pragma once

#include <iostream>
#include "Sim/GASS.h"
#include "Plugins/OSG/OSGCommon.h"

namespace GASS
{
	class OSGConvert
	{
	public:
		OSGConvert();
		virtual ~OSGConvert();
		static osg::Vec3d ToOSG(const Vec3 &v); 
		static Vec3 ToGASS(const osg::Vec3 &v); 
		static Vec3 ToGASS(const osg::Vec3d &v); 
		static osg::Vec4 ToOSG(const ColorRGBA &color);
		static osg::Quat ToOSG(const Quaternion &value);
		static Quaternion ToGASS(const osg::Quat &value);
		static int ToOSGNodeMask(GeometryFlags flag);
		static void SetOSGNodeMask(GeometryFlags flags, osg::Node* node);
	};
}