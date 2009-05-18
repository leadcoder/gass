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

#include <OgreVector3.h>
#include <OgreVector4.h>
#include <OgreMatrix4.h>
#include <OgreQuaternion.h>
#include <OgreSphere.h>
#include <OgreAxisAlignedBox.h>
#include <iostream>

#include "Core/Math/Matrix.h"
#include "Core/Math/Vector.h"
#include "Core/Math/Quaternion.h"
#include "Core/Math/Sphere.h"
#include "Core/Math/AABox.h"

namespace GASS
{
	class Convert
	{
	public:
		static inline Ogre::Vector3				ToOgre(const Vec3 &v) {return Ogre::Vector3(v.x,v.y,v.z);}
		static inline Ogre::Vector4				ToOgre(const Vec4 &v) {return Ogre::Vector4(v.x,v.y,v.z,v.w);}
		static inline Ogre::Matrix4				ToOgre(const Mat4 &mat){Ogre::Matrix4(mat[0][0],mat[0][1],mat[0][2],mat[0][3],	mat[1][0],mat[1][1],mat[1][2],mat[1][3],	mat[2][0],mat[2][1],mat[2][2],mat[2][3],	mat[3][0],mat[3][1],mat[3][2],mat[3][3]);}
		static inline Ogre::AxisAlignedBox		ToOgre(const AABox &b) {return Ogre::AxisAlignedBox(ToOgre(b.m_Min),ToOgre(b.m_Max));}
		static inline Ogre::Quaternion			ToOgre(const Quaternion &q) {return Ogre::Quaternion(-q.w,q.x,q.y,q.z);}
		static inline Ogre::Sphere				ToOgre(const Sphere &s) {return Ogre::Sphere(ToOgre(s.m_Pos),s.m_Radius);}

		static inline Vec3						ToGASS(const Ogre::Vector3 &v) {return Vec3(v.x,v.y,v.z);}
		static inline Vec4						ToGASS(const Ogre::Vector4 &v) {return Vec4(v.x,v.y,v.z,v.w);}
		static inline AABox						ToGASS(const Ogre::AxisAlignedBox &b) {return AABox(ToGASS(b.getMinimum()),ToGASS(b.getMaximum()));}
		static inline Quaternion				ToGASS(const Ogre::Quaternion &q) {return Quaternion(-q.w,q.x,q.y,q.z);}
		static inline Sphere					ToGASS(const Ogre::Sphere &s){return Sphere(ToGASS(s.getCenter()),s.getRadius());}
	};
}


