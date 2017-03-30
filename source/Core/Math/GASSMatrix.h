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

#ifndef GASS_MATRIX_H
#define GASS_MATRIX_H

#include <cassert>
#include "Core/Common.h"

#include "Core/Math/GASSMat4.h"


namespace GASS
{
/*	template<class Type> class TVec3;
	typedef TVec3<Float> Vec3;
	template<class Type> class TVec4;
	typedef TVec4<Float> Vec4;
	*/
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
	//class GASSCoreExport Mat4
	//{
	//public:

	//	static const Float EPSILON;

	//	/**
	//	* Contains the data of the matrix.
	//	*/
	//	union
	//	{
	//		Float m_Data[4][4];
	//		Float m_Data2[16];
	//	};

	//	Mat4( ) {}

	//	/**
	//		Setup transformation matrix by pos,rot and scale
	//	*/
	//	Mat4(const Vec3 &pos,const Quaternion &rot,const Vec3 &scale)
	//	{
	//		SetTransformation(pos,rot,scale);
	//	}

	//	/**
	//		Setup transformation matrix by pos,rot (Euler angles) and scale
	//	*/

	//	Mat4(const Vec3 &pos,const Vec3 &rot,const Vec3 &scale)
	//	{
	//		SetTransformation(pos,rot,scale);
	//	}

	//	Mat4(
	//		Float m00, Float m01, Float m02, Float m03,
	//		Float m10, Float m11, Float m12, Float m13,
	//		Float m20, Float m21, Float m22, Float m23,
	//		Float m30, Float m31, Float m32, Float m33 );


	//	inline Float* operator [] ( unsigned iRow )
	//	{
	//		assert( iRow < 4 );
	//		return m_Data[iRow];
	//	}

	//	inline const Float * operator [] ( unsigned iRow ) const
	//	{
	//		assert( iRow < 4 );
	//		return m_Data[iRow];
	//	}

	//	inline Mat4 Concatenate(const Mat4  &m2) const
	//	{
	//		Mat4 r;
	//		r.m_Data[0][0] = m_Data[0][0] * m2.m_Data[0][0] + m_Data[0][1] * m2.m_Data[1][0] + m_Data[0][2] * m2.m_Data[2][0] + m_Data[0][3] * m2.m_Data[3][0];
	//		r.m_Data[0][1] = m_Data[0][0] * m2.m_Data[0][1] + m_Data[0][1] * m2.m_Data[1][1] + m_Data[0][2] * m2.m_Data[2][1] + m_Data[0][3] * m2.m_Data[3][1];
	//		r.m_Data[0][2] = m_Data[0][0] * m2.m_Data[0][2] + m_Data[0][1] * m2.m_Data[1][2] + m_Data[0][2] * m2.m_Data[2][2] + m_Data[0][3] * m2.m_Data[3][2];
	//		r.m_Data[0][3] = m_Data[0][0] * m2.m_Data[0][3] + m_Data[0][1] * m2.m_Data[1][3] + m_Data[0][2] * m2.m_Data[2][3] + m_Data[0][3] * m2.m_Data[3][3];

	//		r.m_Data[1][0] = m_Data[1][0] * m2.m_Data[0][0] + m_Data[1][1] * m2.m_Data[1][0] + m_Data[1][2] * m2.m_Data[2][0] + m_Data[1][3] * m2.m_Data[3][0];
	//		r.m_Data[1][1] = m_Data[1][0] * m2.m_Data[0][1] + m_Data[1][1] * m2.m_Data[1][1] + m_Data[1][2] * m2.m_Data[2][1] + m_Data[1][3] * m2.m_Data[3][1];
	//		r.m_Data[1][2] = m_Data[1][0] * m2.m_Data[0][2] + m_Data[1][1] * m2.m_Data[1][2] + m_Data[1][2] * m2.m_Data[2][2] + m_Data[1][3] * m2.m_Data[3][2];
	//		r.m_Data[1][3] = m_Data[1][0] * m2.m_Data[0][3] + m_Data[1][1] * m2.m_Data[1][3] + m_Data[1][2] * m2.m_Data[2][3] + m_Data[1][3] * m2.m_Data[3][3];

	//		r.m_Data[2][0] = m_Data[2][0] * m2.m_Data[0][0] + m_Data[2][1] * m2.m_Data[1][0] + m_Data[2][2] * m2.m_Data[2][0] + m_Data[2][3] * m2.m_Data[3][0];
	//		r.m_Data[2][1] = m_Data[2][0] * m2.m_Data[0][1] + m_Data[2][1] * m2.m_Data[1][1] + m_Data[2][2] * m2.m_Data[2][1] + m_Data[2][3] * m2.m_Data[3][1];
	//		r.m_Data[2][2] = m_Data[2][0] * m2.m_Data[0][2] + m_Data[2][1] * m2.m_Data[1][2] + m_Data[2][2] * m2.m_Data[2][2] + m_Data[2][3] * m2.m_Data[3][2];
	//		r.m_Data[2][3] = m_Data[2][0] * m2.m_Data[0][3] + m_Data[2][1] * m2.m_Data[1][3] + m_Data[2][2] * m2.m_Data[2][3] + m_Data[2][3] * m2.m_Data[3][3];

	//		r.m_Data[3][0] = m_Data[3][0] * m2.m_Data[0][0] + m_Data[3][1] * m2.m_Data[1][0] + m_Data[3][2] * m2.m_Data[2][0] + m_Data[3][3] * m2.m_Data[3][0];
	//		r.m_Data[3][1] = m_Data[3][0] * m2.m_Data[0][1] + m_Data[3][1] * m2.m_Data[1][1] + m_Data[3][2] * m2.m_Data[2][1] + m_Data[3][3] * m2.m_Data[3][1];
	//		r.m_Data[3][2] = m_Data[3][0] * m2.m_Data[0][2] + m_Data[3][1] * m2.m_Data[1][2] + m_Data[3][2] * m2.m_Data[2][2] + m_Data[3][3] * m2.m_Data[3][2];
	//		r.m_Data[3][3] = m_Data[3][0] * m2.m_Data[0][3] + m_Data[3][1] * m2.m_Data[1][3] + m_Data[3][2] * m2.m_Data[2][3] + m_Data[3][3] * m2.m_Data[3][3];

	//		return r;
	//	}

	//	/** Matrix concatenation using '*'.
	//	*/
	//	inline Mat4 operator * ( const Mat4 &m2 ) const
	//	{
	//		return Concatenate( m2 );
	//	}

	//	Vec3 operator* ( const Vec3 &vec) const;
	//	Vec4 operator* ( const Vec4 &vec) const;

	//	/**
	//	* Set all matrix elements to zero.
	//	*/
	//	void Zero();

	//	void Rotate(Float h,Float p,Float r);

	//	/**
	//	* Set the rotation matrix for heading, pitch, roll.
	//	*/
	//	void RotateY(Float amount);
	//	void RotateX(Float amount);
	//	void RotateZ(Float amount);
	//	void Scale(Float sx,Float sy,Float sz);
	//	void RelScale(Vec3 scale);
	//	void Identity();
	//	Mat4 Transpose();
	//	void Translate(Float x,Float y,Float z);
	//	inline void RelTranslate(Float x,Float y,Float z){m_Data[0][3] += x;	m_Data[1][3] += y;m_Data[2][3] += z;};
	//	inline void SetTranslation(Float x,Float y,Float z){m_Data[0][3] = x; m_Data[1][3] = y; m_Data[2][3] = z;};
	//	Vec3 GetTranslation() const;
	//	void SetTransformation(const Vec3 &pos,const Vec3 &rot,const Vec3 &scale);
	//	void SetTransformation(const Vec3 &pos,const Quaternion &rot,const Vec3 &scale);
	//	Float Determinant() const;

	//	//TODO: document diff and select one method
	//	Mat4 Invert() const;
	//	Mat4 Invert2() const; 

	//	Mat4 GetRotation() const;
	//	Vec3 GetRotationRadians() const;
	//	Float GetEulerHeading() const;
	//	Float GetEulerPitch() const;
	//	Float GetEulerRoll() const;

	//	/**
	//	Get X axis in the rotation part of the matrix
	//	*/
	//	Vec3 GetXAxis() const;
	//	/**
	//	Get Y axis in the rotation part of the matrix
	//	*/
	//	Vec3 GetYAxis() const;

	//	/**
	//	Get Z axis in the rotation part of the matrix
	//	*/
	//	Vec3 GetZAxis() const;

	//	/**
	//	Set X axis in the rotation part of the matrix
	//	*/
	//	void SetXAxis(const Vec3 &dir);

	//	/**
	//	Set Y axis in the rotation part of the matrix
	//	*/
	//	void SetYAxis(const Vec3 &dir);
	//	/**
	//	Set Z axis in the rotation part of the matrix
	//	*/
	//	void SetZAxis(const Vec3 &dir);

	//};
}

#endif

