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

#include <cassert>
#include "Core/Common.h"

namespace GASS
{
	class Vec3;
	class Vec4;
	class Quaternion;

	class GASSCoreExport Mat3
	{
	public:
		Float m_Data[3][3];
	};

	/** \addtogroup GASSCore
	*  @{
	*/
	/** \addtogroup Math
	*  @{
	*/


	/**
	* Class representing a Float-matrix with 16 elements.
	*/
	class GASSCoreExport Mat4
	{
	public:

		static const Float EPSILON; 
		
		/**
		* Contains the data of the matrix.
		*/
		union
		{
			Float m_Data[4][4];
			Float m_Data2[16];
		};

		inline Float* operator [] ( unsigned iRow )
		{
			assert( iRow < 4 );
			return m_Data[iRow];
		}
		inline const Float *const operator [] ( unsigned iRow ) const
		{
			assert( iRow < 4 );
			return m_Data[iRow];
		}

		Mat4 operator* (const Mat4 &mat) const;
		Vec3 operator* ( const Vec3 &vec) const;
		Vec4 operator* ( const Vec4 &vec) const;

		/**
		* Set all matrix elements to zero.
		*/
		void Zero();
		
		void Rotate(Float h,Float p,Float r);
		
		/**
		* Set the rotation matrix for heading, pitch, roll.
		*/
		void RotateY(Float amount);
		void RotateX(Float amount);
		void RotateZ(Float amount);
		void Scale(Float sx,Float sy,Float sz);
		void RelScale(Vec3 scale);
		void Identity();
		Mat4 Transpose();
		void Translate(Float x,Float y,Float z);
		inline void RelTranslate(Float x,Float y,Float z){m_Data[3][0] += x;	m_Data[3][1] += y;m_Data[3][2] += z;};
		inline void SetTranslation(Float x,Float y,Float z){m_Data[3][0] = x; m_Data[3][1] = y; m_Data[3][2] = z;};
		void InverseRotateVect( Vec3 &vec);
		void InverseTranslateVect( Vec3 &vec);
		Vec3 GetTranslation() const;
		void SetTransformation(const Vec3 &pos,const Vec3 &rot,const Vec3 &scale);
		void SetTransformation(const Vec3 &pos,const Quaternion &rot,const Vec3 &scale);
		Float Determinant();
		Float Determinant(int row, int col);
		Mat4 Invert();
		Mat4 Invert2();
		Mat4 GetRotation() const;
		Vec3 GetRotationRadians() const;
		Float GetEulerHeading() const;
		Float GetEulerPitch() const;
		Float GetEulerRoll() const;
		Vec3 GetRightVector() const;
		Vec3 GetUpVector() const;
		Vec3 GetViewDirVector() const;
		void SetRightVector( const Vec3 &dir);
		void SetUpVector(const Vec3 &dir);
		void SetViewDirVector(const Vec3 &dir);
	};
}



