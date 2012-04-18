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

#pragma once

#include <iostream>

#include "Sim/GASS.h"
#include <osg/Projection>

namespace GASS
{
	class OSGConvert : public Singleton<OSGConvert>
	{
	public:
		OSGConvert();
		virtual ~OSGConvert();
		static OSGConvert* GetPtr();
		static OSGConvert& Get();
		osg::Vec3d ToOSG(const Vec3 &v) const; 
		Vec3 ToGASS(const osg::Vec3 &v) const; 
		Vec3 ToGASS(const osg::Vec3d &v) const; 
		osg::Quat ToOSG(const Quaternion &value) const;
		Quaternion ToGASS(const osg::Quat &value) const;
		Mat4 m_Tranform;
		Mat4 m_InvTranform;
		bool m_FlipYZ;
	};
}


