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

#include "Sim/GASSCommon.h"
#include "Core/Math/GASSMath.h"
#include "Core/Math/GASSVector.h"
#include "Core/Math/GASSQuaternion.h"

namespace GASS
{
	/**
		In GASSSim we have a defined coordinate system (Y is always up-axis) hence we can define
		euler rotations as heading,pitch,roll where heading is rotation around Y-axis, pitch X-Axis and roll Z-Axis.
		This class converts to and from HPR to axis rotations that are used in GASSCore where no up-axis is defined. 
		Note: All angles are defined in degrees
	*/
	class EulerRotation
	{
	public:
		EulerRotation() : Heading(0), Pitch(0), Roll(0) {}
		EulerRotation(Float h, Float p, Float r) : Heading(h), Pitch(p), Roll(r) {}
		
		Float Heading; //in Degrees
		Float Pitch; //in Degrees
		Float Roll; //in Degrees

		//conversion functions
		Vec3 GetAxisRotation() const
		{
			return Vec3::Deg2Rad(Vec3(Pitch, Heading, Roll));
		}

		Quaternion GetQuaternion() const
		{
			return Quaternion::CreateFromEulerYXZ(GetAxisRotation());
		}
	
		static EulerRotation FromQuaternion(const Quaternion &rot)
		{
			Mat4 rot_mat = rot.GetRotationMatrix();
			Vec3 euler_axis;
			rot_mat.ToEulerAnglesYXZ(euler_axis);
			return FromAxisRotation(euler_axis);
		}

		static EulerRotation FromAxisRotation(const Vec3 &axis_rot)
		{
			return EulerRotation(Math::Rad2Deg(axis_rot.y), Math::Rad2Deg(axis_rot.x), Math::Rad2Deg(axis_rot.z));
		}

		friend std::ostream& operator << (std::ostream& os, const EulerRotation& rot)
		{
			os << rot.Heading << " " << rot.Pitch << " " << rot.Roll;
			return os;
		}

		friend std::istream& operator >> (std::istream& is, EulerRotation& rot)
		{
			if (!(is >> rot.Heading >> rot.Pitch >> rot.Roll))
			{
				GASS_EXCEPT(Exception::ERR_INVALIDPARAMS, "Failed to parse EulerRotation", " std::istream >>");
			}
			return is;
		}
	};
}
