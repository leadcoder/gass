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

#include <cassert>
#include "Core/Common.h"
#include "Core/Math/GASSMath.h"

namespace GASS
{
	template<class TYPE> class TVec3;
	template<class TYPE> class TVec4;
	template<class TYPE> class TQuaternion;

	/** \addtogroup GASSCore
	*  @{
	*/
	/** \addtogroup Math
	*  @{
	*/

	/**
	Class representing a OpenGL-style collum major transformation matrix.
	in a right handed coordinate system defined as:
	   +Y
	   |
	   |
	   |______ +X
	  /
	 /
	+Z
	
	About euler rotation:
	Postive euler rotations are defined clockwise when looking in positive axis-direction,
	for instance, a 90deg rotation around Y axis will put +Z at +X, and  +X at -Z in figure above

	Matrix is collum major with following indexing
	Single index:
	m0  m1  m2  m3
	m4  m5  m6  m7
	m8  m9  m10 m11
	m12 m13 m14 m15

	Row and collum index:
	m00  m01  m02  m03
	m10  m11  m12  m13
	m20  m21  m22  m23
	m30  m31  m32  m33

	Where m03,m13,m23 hold the position (x,y,z)
	And:
	m00 m10 m20
	m10 m11 m12
	m20 m21 m22

	is 3x3 rotation matrix defined by three ortogonal axis, where
	m00, m10, m20 is the X-axis
	m01, m11, m21 is the Y-axis
	m02, m12, m22 is the Z-axis
	*/

	template<class TYPE>
	class TMat4
	{
	public:

		static const TYPE EPSILON;

		/**
		* Contains the data of the matrix.
		*/
		union
		{
			TYPE m_Data[4][4]; //Legacy, to be removed
			TYPE m_Data2[16]; //Legacy, to be removed
			TYPE E4x4[4][4]; //Matrix elements in 4 by 4 array
			TYPE E16[16]; //Matrix elements in 16 array
		};

		TMat4() {}

		/**
		Setup full transformation matrix (Scale, Rotate and Translate),
		This transformation matrix will first apply scale followed
		by a rotation and  last the translation is applied.
		@param pos Translation part of the transformation
		@param rot Rotation represented as a quaternion
		@param scale Scale in each axis
		*/
		TMat4(const TVec3<TYPE> &pos, const TQuaternion<TYPE> &rot, const TVec3<TYPE> &scale)
		{
			SetTransformation(pos, rot, scale);
		}

		/**
		Setup full transformation matrix: Scale, Rotate (Euler angles)  and Translate.
		This transformation matrix will first apply scale followed
		by a rotation and  last the translation is applied.
		@param pos Translation part of the transformation
		@param rot Rotation represented as a euler angles in radians)
		@param scale Scale in each axis
		*/
		TMat4(const TVec3<TYPE> &pos, const TVec3<TYPE> &rot, const TVec3<TYPE> &scale)
		{
			SetTransformation(pos, rot, scale);
		}

		inline TMat4(
			TYPE m00, TYPE m01, TYPE m02, TYPE m03,
			TYPE m10, TYPE m11, TYPE m12, TYPE m13,
			TYPE m20, TYPE m21, TYPE m22, TYPE m23,
			TYPE m30, TYPE m31, TYPE m32, TYPE m33);

		inline TYPE* operator [] (unsigned iRow)
		{
			assert(iRow < 4);
			return E4x4[iRow];
		}

		inline const TYPE * operator [] (unsigned iRow) const
		{
			assert(iRow < 4);
			return E4x4[iRow];
		}

		inline TMat4 Concatenate(const TMat4  &m2) const;

		/** Matrix concatenation using '*'.
		*/
		inline TMat4 operator * (const TMat4 &m2) const
		{
			return Concatenate(m2);
		}

		/**
		Vector transformation using '*'.
		Transforms the given point by the matrix, projecting the result back into w = 1.
		This means that the initial w is considered to be 1.0, and then all the tree elements
		of the resulting vector are divided by the resulting w.
		*/
		inline TVec3<TYPE> operator* (const TVec3<TYPE> &vec) const;

		inline TVec4<TYPE> operator* (const TVec4<TYPE> &vec) const;

		inline bool operator== (const TMat4 &m2) const;

		/**
		* Set all matrix elements to zero.
		*/
		inline void Zero();

		/**
		Set the rotation matrix for heading, pitch, roll.
		@param h Heading in in radians
		@param p Pitch in in radians
		@param r Roll in in radians
		*/
		inline void Rotate(TYPE h, TYPE p, TYPE r);

		/**
		* Create Y rotation matrix.
		*/
		inline void RotateY(TYPE amount);

		/**
		* Create X rotation matrix.
		*/
		inline void RotateX(TYPE amount);

		/**
		* Create Z rotation matrix.
		*/
		inline void RotateZ(TYPE amount);
		inline void Scale(TYPE sx, TYPE sy, TYPE sz);
		inline void RelScale(TVec3<TYPE> scale);
		inline void Identity();
		inline TMat4 Transpose();
		inline void Translate(TYPE x, TYPE y, TYPE z);
		inline void RelTranslate(TYPE x, TYPE y, TYPE z) { E4x4[0][3] += x;	E4x4[1][3] += y; E4x4[2][3] += z; };
		inline void SetTranslation(TYPE x, TYPE y, TYPE z) { E4x4[0][3] = x; E4x4[1][3] = y; E4x4[2][3] = z; };
		inline TVec3<TYPE> GetTranslation() const;
		inline void SetTransformation(const TVec3<TYPE> &pos, const TVec3<TYPE> &rot, const TVec3<TYPE> &scale);
		inline void SetTransformation(const TVec3<TYPE> &pos, const TQuaternion<TYPE> &rot, const TVec3<TYPE> &scale);
		inline TYPE Determinant() const;

		//TODO: document diff and select one method
		inline TMat4 Invert() const;
		//inline TMat4 Invert2() const;

		inline TMat4 GetRotation() const;
		inline TVec3<TYPE> GetRotationRadians() const;
		inline TYPE GetEulerHeading() const;
		inline TYPE GetEulerPitch() const;
		inline TYPE GetEulerRoll() const;

		/**
		Get X axis in the rotation part of the matrix
		*/
		inline TVec3<TYPE> GetXAxis() const;
		/**
		Get Y axis in the rotation part of the matrix
		*/
		inline TVec3<TYPE> GetYAxis() const;

		/**
		Get Z axis in the rotation part of the matrix
		*/
		inline TVec3<TYPE> GetZAxis() const;

		/**
		Set X axis in the rotation part of the matrix
		*/
		inline void SetXAxis(const TVec3<TYPE> &dir);

		/**
		Set Y axis in the rotation part of the matrix
		*/
		inline void SetYAxis(const TVec3<TYPE> &dir);
		/**
		Set Z axis in the rotation part of the matrix
		*/
		inline void SetZAxis(const TVec3<TYPE> &dir);


		inline bool _Equal(TYPE v1, TYPE v2, TYPE tolerance = std::numeric_limits<TYPE>::epsilon()) const
		{
			return (abs(v1 - v2) < tolerance);
		}

		inline bool Equal(const TMat4 &m, TYPE tolerance = std::numeric_limits<TYPE>::epsilon()) const
		{
			for (int i = 0; i < 16; i++)
			{
				if (!_Equal(E16[i], m.E16[i], tolerance))
					return false;
			}
			return true;
		}

		//TODO: why use this order
		inline friend TVec4<TYPE> operator* (TVec4<TYPE> vec, const TMat4<TYPE> &mat)
		{
			TVec4<TYPE> ret;
			ret.x = vec.x*mat.E4x4[0][0] + vec.y*mat.E4x4[1][0] + vec.z*mat.E4x4[2][0] + vec.w*mat.E4x4[3][0];
			ret.y = vec.x*mat.E4x4[0][1] + vec.y*mat.E4x4[1][1] + vec.z*mat.E4x4[2][1] + vec.w*mat.E4x4[3][1];
			ret.z = vec.x*mat.E4x4[0][2] + vec.y*mat.E4x4[1][2] + vec.z*mat.E4x4[2][2] + vec.w*mat.E4x4[3][2];
			ret.w = vec.x*mat.E4x4[0][3] + vec.y*mat.E4x4[1][3] + vec.z*mat.E4x4[2][3] + vec.w*mat.E4x4[3][3];
			return ret;
		}
	};

	typedef TMat4<Float> Mat4;

	template <class TYPE> const TYPE TMat4<TYPE>::EPSILON = 0.0001f;


	template<class TYPE>
	TMat4<TYPE>::TMat4(
		TYPE m00, TYPE m01, TYPE m02, TYPE m03,
		TYPE m10, TYPE m11, TYPE m12, TYPE m13,
		TYPE m20, TYPE m21, TYPE m22, TYPE m23,
		TYPE m30, TYPE m31, TYPE m32, TYPE m33)
	{
		E4x4[0][0] = m00;
		E4x4[0][1] = m01;
		E4x4[0][2] = m02;
		E4x4[0][3] = m03;
		E4x4[1][0] = m10;
		E4x4[1][1] = m11;
		E4x4[1][2] = m12;
		E4x4[1][3] = m13;
		E4x4[2][0] = m20;
		E4x4[2][1] = m21;
		E4x4[2][2] = m22;
		E4x4[2][3] = m23;
		E4x4[3][0] = m30;
		E4x4[3][1] = m31;
		E4x4[3][2] = m32;
		E4x4[3][3] = m33;
	}

	template<class TYPE>
	TMat4<TYPE>  TMat4<TYPE>::Concatenate(const TMat4<TYPE>  &m2) const
	{
		TMat4<TYPE> r;
		r.E4x4[0][0] = E4x4[0][0] * m2.E4x4[0][0] + E4x4[0][1] * m2.E4x4[1][0] + E4x4[0][2] * m2.E4x4[2][0] + E4x4[0][3] * m2.E4x4[3][0];
		r.E4x4[0][1] = E4x4[0][0] * m2.E4x4[0][1] + E4x4[0][1] * m2.E4x4[1][1] + E4x4[0][2] * m2.E4x4[2][1] + E4x4[0][3] * m2.E4x4[3][1];
		r.E4x4[0][2] = E4x4[0][0] * m2.E4x4[0][2] + E4x4[0][1] * m2.E4x4[1][2] + E4x4[0][2] * m2.E4x4[2][2] + E4x4[0][3] * m2.E4x4[3][2];
		r.E4x4[0][3] = E4x4[0][0] * m2.E4x4[0][3] + E4x4[0][1] * m2.E4x4[1][3] + E4x4[0][2] * m2.E4x4[2][3] + E4x4[0][3] * m2.E4x4[3][3];

		r.E4x4[1][0] = E4x4[1][0] * m2.E4x4[0][0] + E4x4[1][1] * m2.E4x4[1][0] + E4x4[1][2] * m2.E4x4[2][0] + E4x4[1][3] * m2.E4x4[3][0];
		r.E4x4[1][1] = E4x4[1][0] * m2.E4x4[0][1] + E4x4[1][1] * m2.E4x4[1][1] + E4x4[1][2] * m2.E4x4[2][1] + E4x4[1][3] * m2.E4x4[3][1];
		r.E4x4[1][2] = E4x4[1][0] * m2.E4x4[0][2] + E4x4[1][1] * m2.E4x4[1][2] + E4x4[1][2] * m2.E4x4[2][2] + E4x4[1][3] * m2.E4x4[3][2];
		r.E4x4[1][3] = E4x4[1][0] * m2.E4x4[0][3] + E4x4[1][1] * m2.E4x4[1][3] + E4x4[1][2] * m2.E4x4[2][3] + E4x4[1][3] * m2.E4x4[3][3];

		r.E4x4[2][0] = E4x4[2][0] * m2.E4x4[0][0] + E4x4[2][1] * m2.E4x4[1][0] + E4x4[2][2] * m2.E4x4[2][0] + E4x4[2][3] * m2.E4x4[3][0];
		r.E4x4[2][1] = E4x4[2][0] * m2.E4x4[0][1] + E4x4[2][1] * m2.E4x4[1][1] + E4x4[2][2] * m2.E4x4[2][1] + E4x4[2][3] * m2.E4x4[3][1];
		r.E4x4[2][2] = E4x4[2][0] * m2.E4x4[0][2] + E4x4[2][1] * m2.E4x4[1][2] + E4x4[2][2] * m2.E4x4[2][2] + E4x4[2][3] * m2.E4x4[3][2];
		r.E4x4[2][3] = E4x4[2][0] * m2.E4x4[0][3] + E4x4[2][1] * m2.E4x4[1][3] + E4x4[2][2] * m2.E4x4[2][3] + E4x4[2][3] * m2.E4x4[3][3];

		r.E4x4[3][0] = E4x4[3][0] * m2.E4x4[0][0] + E4x4[3][1] * m2.E4x4[1][0] + E4x4[3][2] * m2.E4x4[2][0] + E4x4[3][3] * m2.E4x4[3][0];
		r.E4x4[3][1] = E4x4[3][0] * m2.E4x4[0][1] + E4x4[3][1] * m2.E4x4[1][1] + E4x4[3][2] * m2.E4x4[2][1] + E4x4[3][3] * m2.E4x4[3][1];
		r.E4x4[3][2] = E4x4[3][0] * m2.E4x4[0][2] + E4x4[3][1] * m2.E4x4[1][2] + E4x4[3][2] * m2.E4x4[2][2] + E4x4[3][3] * m2.E4x4[3][2];
		r.E4x4[3][3] = E4x4[3][0] * m2.E4x4[0][3] + E4x4[3][1] * m2.E4x4[1][3] + E4x4[3][2] * m2.E4x4[2][3] + E4x4[3][3] * m2.E4x4[3][3];

		return r;
	}

	template<class TYPE>
	TVec3<TYPE> TMat4<TYPE>::operator * (const TVec3<TYPE> &vec) const
	{
		TVec3<TYPE> res;
		TYPE d;
		res.x = E4x4[0][0] * vec.x +
			E4x4[0][1] * vec.y +
			E4x4[0][2] * vec.z +
			E4x4[0][3];

		res.y = E4x4[1][0] * vec.x +
			E4x4[1][1] * vec.y +
			E4x4[1][2] * vec.z +
			E4x4[1][3];

		res.z = E4x4[2][0] * vec.x +
			E4x4[2][1] * vec.y +
			E4x4[2][2] * vec.z +
			E4x4[2][3];

		d = E4x4[3][0] * vec.x +
			E4x4[3][1] * vec.y +
			E4x4[3][2] * vec.z +
			E4x4[3][3];

		res.x /= d;
		res.y /= d;
		res.z /= d;

		return res;
	}

	template<class TYPE>
	TVec4<TYPE> TMat4<TYPE>::operator * (const TVec4<TYPE> &vec) const
	{
		TVec4<TYPE> res;
		//first row
		res.x = E4x4[0][0] * vec.x +
			E4x4[0][1] * vec.y +
			E4x4[0][2] * vec.z +
			E4x4[0][3] * vec.w;

		//second row
		res.y = E4x4[1][0] * vec.x +
			E4x4[1][1] * vec.y +
			E4x4[1][2] * vec.z +
			E4x4[1][3] * vec.w;

		//third row
		res.z = E4x4[2][0] * vec.x +
			E4x4[2][1] * vec.y +
			E4x4[2][2] * vec.z +
			E4x4[2][3] * vec.w;

		res.w = E4x4[3][0] * vec.x +
			E4x4[3][1] * vec.y +
			E4x4[3][2] * vec.z +
			E4x4[3][3] * vec.w;

		return res;
	}

	template<class TYPE>
	bool TMat4<TYPE>::operator== (const TMat4<TYPE> &m2) const
	{
		for (int i = 0; i < 16; i++)
		{
			if (E16[i] != m2.E16[i])
				return false;
		}
		return true;
	}

	template<class TYPE>
	void TMat4<TYPE>::Zero()
	{
		E4x4[0][0] = 0;
		E4x4[0][1] = 0;
		E4x4[0][2] = 0;
		E4x4[0][3] = 0;
		E4x4[1][0] = 0;
		E4x4[1][1] = 0;
		E4x4[1][2] = 0;
		E4x4[1][3] = 0;
		E4x4[2][0] = 0;
		E4x4[2][1] = 0;
		E4x4[2][2] = 0;
		E4x4[2][3] = 0;
		E4x4[3][0] = 0;
		E4x4[3][1] = 0;
		E4x4[3][2] = 0;
		E4x4[3][3] = 0;
	}

	template<class TYPE>
	void TMat4<TYPE>::Identity()
	{
		E4x4[0][0] = 1;
		E4x4[0][1] = 0;
		E4x4[0][2] = 0;
		E4x4[0][3] = 0;
		E4x4[1][0] = 0;
		E4x4[1][1] = 1;
		E4x4[1][2] = 0;
		E4x4[1][3] = 0;
		E4x4[2][0] = 0;
		E4x4[2][1] = 0;
		E4x4[2][2] = 1;
		E4x4[2][3] = 0;
		E4x4[3][0] = 0;
		E4x4[3][1] = 0;
		E4x4[3][2] = 0;
		E4x4[3][3] = 1;
	}

	template<class TYPE>
	TMat4<TYPE> TMat4<TYPE>::Transpose()
	{
		TMat4<TYPE> ret;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				ret.E4x4[j][i] = E4x4[i][j];
			}
		}
		return ret;
	}

	template<class TYPE>
	void TMat4<TYPE>::Translate(TYPE x, TYPE y, TYPE z)
	{
		Identity();
		E4x4[0][3] = x;
		E4x4[1][3] = y;
		E4x4[2][3] = z;
	}

	template<class TYPE>
	void TMat4<TYPE>::Scale(TYPE sx, TYPE sy, TYPE sz)
	{
		Identity();
		E4x4[0][0] = sx;
		E4x4[1][1] = sy;
		E4x4[2][2] = sz;
	}

	template<class TYPE>
	void TMat4<TYPE>::Rotate(TYPE h, TYPE p, TYPE r)
	{
		TYPE cp = cos(p);
		TYPE sp = sin(p);
		TYPE ch = cos(h);
		TYPE sh = sin(h);
		TYPE cr = cos(r);
		TYPE sr = sin(r);

		E4x4[0][0] = (cr*ch + sr*sp*sh);
		E4x4[0][1] = (-ch*sr + sh*sp*ch);
		E4x4[0][2] = (sh*cp);

		E4x4[1][0] = cp*sr;
		E4x4[1][1] = cp*cr;
		E4x4[1][2] = -sp;

		E4x4[2][0] = -sh*cr + ch*sp*sr;
		E4x4[2][1] = sh*sr + ch*sp*cr;
		E4x4[2][2] = ch*cp;

		/*E4x4[0][0] = (cr*ch + sr*sp*sh);
		E4x4[0][1] = (sr*cp);
		E4x4[0][2] = (-cr*sh + sr*sp*ch);

		E4x4[1][0] = (-sr*ch + cr*sp*sh);
		E4x4[1][1] = (cr*cp);
		E4x4[1][2] = (sr*sh + cr*sp*ch);

		E4x4[2][0] = (cp*sh);
		E4x4[2][1] = (-sp);
		E4x4[2][2] = (cp*ch);*/
	}

	template<class TYPE>
	void TMat4<TYPE>::SetTransformation(const TVec3<TYPE> &pos, const TVec3<TYPE> &rot, const TVec3<TYPE> &scale)
	{

		TMat4<TYPE> mat_scale;
		mat_scale.Identity();
		mat_scale.Scale(scale.x, scale.y, scale.z);

		TMat4<TYPE> mat_pos;
		mat_pos.Identity();
		mat_pos.SetTranslation(pos.x, pos.y, pos.z);

		TMat4<TYPE> mat_rot;
		mat_rot.Identity();
		mat_rot.Rotate(rot.x, rot.y, rot.z);
		//TMat4<TYPE> final_trans = mat_scale*mat_pos*mat_rot;
		TMat4<TYPE> final_trans = mat_pos*mat_rot*mat_scale;
		*this = final_trans;
		/*Identity();
		E4x4[0][3] = pos.x;
		E4x4[1][3] = pos.y;
		E4x4[2][3] = pos.z;

		Rotate(rot.x, rot.y, rot.z);

		E4x4[3][0] = 0;
		E4x4[3][1] = 0;
		E4x4[3][2] = 0;

		//Scale,
		//we should only scale diagonal!
		E4x4[0][0] *= scale.x;
		E4x4[0][1] *= scale.x;
		E4x4[0][2] *= scale.x;
		E4x4[0][3] *= scale.x;

		E4x4[1][0] *= scale.y;
		E4x4[1][1] *= scale.y;
		E4x4[1][2] *= scale.y;
		E4x4[1][3] *= scale.y;

		E4x4[2][0] *= scale.z;
		E4x4[2][1] *= scale.z;
		E4x4[2][2] *= scale.z;
		E4x4[2][3] *= scale.z;*/
	}

	template<class TYPE>
	void TMat4<TYPE>::SetTransformation(const TVec3<TYPE> &pos, const TQuaternion<TYPE> &rot, const TVec3<TYPE> &scale)
	{

		TMat4<TYPE> mat_scale;
		mat_scale.Identity();
		mat_scale.Scale(scale.x, scale.y, scale.z);

		TMat4<TYPE> mat_pos;
		mat_pos.Identity();
		mat_pos.SetTranslation(pos.x, pos.y, pos.z);

		TMat4<TYPE> mat_rot;
		mat_rot.Identity();
		rot.ToRotationMatrix(mat_rot);

		//TMat4<TYPE> final_trans = mat_scale*mat_pos*mat_rot;
		TMat4<TYPE> final_trans = mat_pos*mat_rot*mat_scale;
		*this = final_trans;

		/*Identity();
		rot.ToRotationMatrix(*this);

		E4x4[0][3] = pos.x;
		E4x4[1][3] = pos.y;
		E4x4[2][3] = pos.z;

		E4x4[3][0] = 0;
		E4x4[3][1] = 0;
		E4x4[3][2] = 0;

		//Scale
		E4x4[0][0] *= scale.x;
		E4x4[1][0] *= scale.x;
		E4x4[2][0] *= scale.x;

		E4x4[0][1] *= scale.y;
		E4x4[1][1] *= scale.y;
		E4x4[2][1] *= scale.y;

		E4x4[0][2] *= scale.z;
		E4x4[1][2] *= scale.z;
		E4x4[2][2] *= scale.z;
		*/
	}


	template<class TYPE>
	void TMat4<TYPE>::RotateY(TYPE amount)
	{
		TYPE ch = cos(amount);
		TYPE sh = sin(amount);

		E4x4[0][0] = ch;
		E4x4[0][1] = 0;
		E4x4[0][2] = sh;
		E4x4[0][3] = 0;

		E4x4[1][0] = 0;
		E4x4[1][1] = 1;
		E4x4[1][2] = 0;
		E4x4[1][3] = 0;

		E4x4[2][0] = -sh;
		E4x4[2][1] = 0;
		E4x4[2][2] = ch;
		E4x4[2][3] = 0;

		E4x4[3][0] = 0;
		E4x4[3][1] = 0;
		E4x4[3][2] = 0;
		E4x4[3][3] = 1;
	}

	template<class TYPE>
	void TMat4<TYPE>::RotateX(TYPE amount)
	{
		TYPE cp = cos(amount);
		TYPE sp = sin(amount);

		E4x4[0][0] = 1;
		E4x4[0][1] = 0;
		E4x4[0][2] = 0;
		E4x4[0][3] = 0;

		E4x4[1][0] = 0;
		E4x4[1][1] = cp;
		E4x4[1][2] = -sp;
		E4x4[1][3] = 0;

		E4x4[2][0] = 0;
		E4x4[2][1] = sp;
		E4x4[2][2] = cp;
		E4x4[2][3] = 0;

		E4x4[3][0] = 0;
		E4x4[3][1] = 0;
		E4x4[3][2] = 0;
		E4x4[3][3] = 1;
	}

	template<class TYPE>
	void TMat4<TYPE>::RotateZ(TYPE amount)
	{
		TYPE cr = cos(amount);
		TYPE sr = sin(amount);

		E4x4[0][0] = cr;
		E4x4[0][1] = -sr;
		E4x4[0][2] = 0;
		E4x4[0][3] = 0;

		E4x4[1][0] = sr;
		E4x4[1][1] = cr;
		E4x4[1][2] = 0;
		E4x4[1][3] = 0;

		E4x4[2][0] = 0;
		E4x4[2][1] = 0;
		E4x4[2][2] = 1;
		E4x4[2][3] = 0;

		E4x4[3][0] = 0;
		E4x4[3][1] = 0;
		E4x4[3][2] = 0;
		E4x4[3][3] = 1;
	}

	template<class TYPE>
	void TMat4<TYPE>::RelScale(TVec3<TYPE> scale)
	{
		E4x4[0][0] *= scale.x;
		E4x4[0][1] *= scale.y;
		E4x4[0][2] *= scale.x;

		E4x4[1][0] *= scale.x;
		E4x4[1][1] *= scale.y;
		E4x4[1][2] *= scale.z;

		E4x4[2][0] *= scale.x;
		E4x4[2][1] *= scale.y;
		E4x4[2][2] *= scale.z;
	}

	template<class TYPE>
	TVec3<TYPE> TMat4<TYPE>::GetTranslation()  const
	{
		TVec3<TYPE> ret;
		ret.x = E4x4[0][3];
		ret.y = E4x4[1][3];
		ret.z = E4x4[2][3];
		return ret;
	}

	template<class TYPE>
	TMat4<TYPE> TMat4<TYPE>::GetRotation() const
	{

		TMat4<TYPE> ret;
		ret = *this;
		ret.E4x4[0][3] = 0;
		ret.E4x4[1][3] = 0;
		ret.E4x4[2][3] = 0;
		return ret;
	}

	template<class TYPE>
	TYPE TMat4<TYPE>::Determinant() const
	{
		TYPE fCofactor00 = E4x4[1][1] * E4x4[2][2] -
			E4x4[1][2] * E4x4[2][1];
		TYPE fCofactor10 = E4x4[1][2] * E4x4[2][0] -
			E4x4[1][0] * E4x4[2][2];
		TYPE fCofactor20 = E4x4[1][0] * E4x4[2][1] -
			E4x4[1][1] * E4x4[2][0];

		TYPE fDet =
			E4x4[0][0] * fCofactor00 +
			E4x4[0][1] * fCofactor10 +
			E4x4[0][2] * fCofactor20;
		return fDet;
	}


	/*TYPE Mat4::Determinant()
	{
	TYPE det = 0.0f;

	for (int col = 0; col < 4; col++)
	{
	const TYPE sign = ((col & 0x1) == 0x0) ? 1.0f : -1.0f;
	det += sign * E4x4[0][col] * _Determinant(0, col);
	}
	return det;
	}





	template<class TYPE>
	TYPE TMat4<TYPE>::_Determinant(int row, int col)
	{
	assert(row >= 0 && row < 4 && col >= 0 && col < 4);

	TYPE data[9];
	int current = 0;

	for (int index = 0; index < 16; index++)
	{
	if ((index / 4) == col || (index % 4) == row)
	{
	continue;
	}
	else
	{
	data[current++] = E16[index];
	}
	}


	//The newly created 3x3 matrix is also in column-major
	//form:

	//d0 d3 d6
	//d1 d4 d7
	//d2 d5 d8


	return
	data[0] * (data[4] * data[8] - data[7] * data[5]) -
	data[1] * (data[3] * data[8] - data[6] * data[5]) +
	data[2] * (data[3] * data[7] - data[6] * data[4]);
	}

	template<class TYPE>
	TMat4<TYPE> TMat4<TYPE>::Invert()
	{
	TYPE det = Determinant();
	//assert(fabs(det) > EPSILON);
	if(fabs(det) < EPSILON) det = EPSILON;
	Mat4 result;
	for (int row = 0; row < 4; row++) {
	for (int col = 0; col < 4; col++) {
	const TYPE sign = (((row + col) & 0x1) == 0x0) ? 1.0f : -1.0f;
	result.E16[col * 4 + row] = sign * _Determinant(col, row) / det;
	}
	}
	return result;
	}*/

	template<class TYPE>
	TMat4<TYPE> TMat4<TYPE>::Invert(void) const
	{
		TYPE m10 = E4x4[1][0], m11 = E4x4[1][1], m12 = E4x4[1][2];
		TYPE m20 = E4x4[2][0], m21 = E4x4[2][1], m22 = E4x4[2][2];

		TYPE t00 = m22 * m11 - m21 * m12;
		TYPE t10 = m20 * m12 - m22 * m10;
		TYPE t20 = m21 * m10 - m20 * m11;

		TYPE m00 = E4x4[0][0], m01 = E4x4[0][1], m02 = E4x4[0][2];

		TYPE invDet = 1 / (m00 * t00 + m01 * t10 + m02 * t20);

		t00 *= invDet; t10 *= invDet; t20 *= invDet;

		m00 *= invDet; m01 *= invDet; m02 *= invDet;

		TYPE r00 = t00;
		TYPE r01 = m02 * m21 - m01 * m22;
		TYPE r02 = m01 * m12 - m02 * m11;

		TYPE r10 = t10;
		TYPE r11 = m00 * m22 - m02 * m20;
		TYPE r12 = m02 * m10 - m00 * m12;

		TYPE r20 = t20;
		TYPE r21 = m01 * m20 - m00 * m21;
		TYPE r22 = m00 * m11 - m01 * m10;

		TYPE m03 = E4x4[0][3], m13 = E4x4[1][3], m23 = E4x4[2][3];

		TYPE r03 = -(r00 * m03 + r01 * m13 + r02 * m23);
		TYPE r13 = -(r10 * m03 + r11 * m13 + r12 * m23);
		TYPE r23 = -(r20 * m03 + r21 * m13 + r22 * m23);

		return TMat4<TYPE>(
			r00, r01, r02, r03,
			r10, r11, r12, r13,
			r20, r21, r22, r23,
			0, 0, 0, 1);
	}

	template<class TYPE>
	TVec3<TYPE> TMat4<TYPE>::GetRotationRadians() const
	{
		const TMat4<TYPE> &mat = *this;

		TYPE Y = -asin(mat[2][0]);
		//TYPE D = Y;
		TYPE C = cos(Y);
		//Y *= 180.000f/MY_PI;

		TYPE rotx, roty, X, Z;

		if (fabs(Y) > 0.0005f)
		{
			rotx = mat[2][2] / C;
			roty = mat[1][2] / C;
			X = atan2(roty, rotx);
			rotx = mat[0][0] / C;
			roty = mat[0][1] / C;
			Z = atan2(roty, rotx);
		}
		else
		{
			X = 0.0f;
			rotx = -mat[1][1];
			roty = mat[0][1];
			Z = atan2(roty, rotx);
		}

		if (fabs(X) >= 2 * GASS_PI) X = 0.00f;
		if (fabs(Y) >= 2 * GASS_PI) Y = 0.00f;
		if (fabs(Z) >= 2 * GASS_PI) Z = 0.00f;

		TVec3<TYPE> rot;
		rot.Set(X, Y, Z);
		return rot;
	}

	template<class TYPE>
	TYPE TMat4<TYPE>::GetEulerHeading() const
	{
		TVec3<TYPE> dir = GetZAxis();
		dir.Set(dir.x, 0, dir.z);
		dir.Normalize();
		TVec3<TYPE> north_dir(0, 0, 1);
		TYPE cos_h = north_dir.Dot(dir);
		TVec3<TYPE> cross = north_dir.Cross(dir);

		// Dot product may give values slightly higher than one due to normalization/precision error
		if (cos_h > 1.0)
			cos_h = 1.0;
		else if (cos_h < -1.0)
			cos_h = -1.0;

		TYPE h_rad = acos(cos_h);

		if (cross.y < 0)
		{
			h_rad = -h_rad;
		}
		return h_rad;
	}

	template<class TYPE>
	TYPE TMat4<TYPE>::GetEulerPitch() const
	{
		TVec3<TYPE> dir;

		dir = GetZAxis();
		TVec3<TYPE> xz_dir(dir.x, 0, dir.z);
		xz_dir.Normalize();
		TYPE cos_p = xz_dir.Dot(dir);
		TVec3<TYPE> cross = xz_dir.Cross(dir);
		cross.Normalize();
		cross = cross.Cross(xz_dir);

		// Dot product may give values slightly higher than one due to normalization/precision error
		if (cos_p > 1.0f)
			cos_p = 1.0f;
		else if (cos_p < -1.0f)
			cos_p = -1.0f;

		TYPE p_rad = acos(cos_p);
		if (cross.y > 0)
		{
			p_rad = -p_rad;
		}
		return p_rad;
	}

	template<class TYPE>
	TYPE TMat4<TYPE>::GetEulerRoll() const
	{
		TVec3<TYPE> left_dir;
		left_dir = GetXAxis();
		TVec3<TYPE> xz_dir(left_dir.x, 0, left_dir.z);
		xz_dir.Normalize();
		TYPE cos_r = xz_dir.Dot(left_dir);
		TVec3<TYPE> cross = xz_dir.Cross(left_dir);
		cross.Normalize();
		cross = cross.Cross(xz_dir);
		// Dot product may give values slightly higher than one due to normalization/precision error
		if (cos_r > 1.0f)
			cos_r = 1.0f;
		else if (cos_r < -1.0f)
			cos_r = -1.0f;
		TYPE r_rad = acos(cos_r);

		if (cross.y > 0)
		{
			r_rad = -r_rad;
		}
		return r_rad;
	}

	/*static Float Det2x2(Float a1, Float a2, Float b1, Float b2)
	{
	return a1 * b2 - b1 * a2;
	}

	static Float Det3x3(Float a1, Float a2, Float a3, Float b1, Float b2, Float b3, Float c1, Float c2, Float c3)
	{
	return a1 * Det2x2(b2, b3, c2, c3) - b1 * Det2x2(a2, a3, c2, c3) + c1 * Det2x2(a2, a3, b2, b3);
	}

	static void FastInvert(const Float A[4][4], Float B[4][4])
	{
	B[0][0] = Det3x3(A[1][1], A[1][2], A[1][3], A[2][1], A[2][2], A[2][3], A[3][1], A[3][2], A[3][3]);
	B[0][1] = -Det3x3(A[0][1], A[0][2], A[0][3], A[2][1], A[2][2], A[2][3], A[3][1], A[3][2], A[3][3]);
	B[0][2] = Det3x3(A[0][1], A[0][2], A[0][3], A[1][1], A[1][2], A[1][3], A[3][1], A[3][2], A[3][3]);
	B[0][3] = -Det3x3(A[0][1], A[0][2], A[0][3], A[1][1], A[1][2], A[1][3], A[2][1], A[2][2], A[2][3]);
	B[1][0] = -Det3x3(A[1][0], A[1][2], A[1][3], A[2][0], A[2][2], A[2][3], A[3][0], A[3][2], A[3][3]);
	B[1][1] = Det3x3(A[0][0], A[0][2], A[0][3], A[2][0], A[2][2], A[2][3], A[3][0], A[3][2], A[3][3]);
	B[1][2] = -Det3x3(A[0][0], A[0][2], A[0][3], A[1][0], A[1][2], A[1][3], A[3][0], A[3][2], A[3][3]);
	B[1][3] = Det3x3(A[0][0], A[0][2], A[0][3], A[1][0], A[1][2], A[1][3], A[2][0], A[2][2], A[2][3]);
	B[2][0] = Det3x3(A[1][0], A[1][1], A[1][3], A[2][0], A[2][1], A[2][3], A[3][0], A[3][1], A[3][3]);
	B[2][1] = -Det3x3(A[0][0], A[0][1], A[0][3], A[2][0], A[2][1], A[2][3], A[3][0], A[3][1], A[3][3]);
	B[2][2] = Det3x3(A[0][0], A[0][1], A[0][3], A[1][0], A[1][1], A[1][3], A[3][0], A[3][1], A[3][3]);
	B[2][3] = -Det3x3(A[0][0], A[0][1], A[0][3], A[1][0], A[1][1], A[1][3], A[2][0], A[2][1], A[2][3]);
	B[3][0] = -Det3x3(A[1][0], A[1][1], A[1][2], A[2][0], A[2][1], A[2][2], A[3][0], A[3][1], A[3][2]);
	B[3][1] = Det3x3(A[0][0], A[0][1], A[0][2], A[2][0], A[2][1], A[2][2], A[3][0], A[3][1], A[3][2]);
	B[3][2] = -Det3x3(A[0][0], A[0][1], A[0][2], A[1][0], A[1][1], A[1][2], A[3][0], A[3][1], A[3][2]);
	B[3][3] = Det3x3(A[0][0], A[0][1], A[0][2], A[1][0], A[1][1], A[1][2], A[2][0], A[2][1], A[2][2]);
	Float det = (A[0][0] * B[0][0]) + (A[1][0] * B[0][1]) + (A[2][0] * B[0][2]) + (A[3][0] * B[0][3]);
	det = 1 / det;
	B[0][0] *= det;
	B[0][1] *= det;
	B[0][2] *= det;
	B[0][3] *= det;
	B[1][0] *= det;
	B[1][1] *= det;
	B[1][2] *= det;
	B[1][3] *= det;
	B[2][0] *= det;
	B[2][1] *= det;
	B[2][2] *= det;
	B[2][3] *= det;
	B[3][0] *= det;
	B[3][1] *= det;
	B[3][2] *= det;
	B[3][3] *= det;
	}

	Mat4 Mat4::Invert2() const
	{
	Mat4 result;
	FastInvert(m_Data, result.m_Data);
	return result;
	}*/

	template<class TYPE>
	TVec3<TYPE> TMat4<TYPE>::GetXAxis() const
	{
		return TVec3<TYPE>(E16[0], E16[4], E16[8]);
	}

	template<class TYPE>
	TVec3<TYPE> TMat4<TYPE>::GetYAxis() const
	{
		return TVec3<TYPE>(E16[1], E16[5], E16[9]);
	}

	template<class TYPE>
	TVec3<TYPE> TMat4<TYPE>::GetZAxis() const
	{
		return TVec3<TYPE>(E16[2], E16[6], E16[10]);
	}

	template<class TYPE>
	void TMat4<TYPE>::SetXAxis(const TVec3<TYPE> &dir)
	{
		E16[0] = dir.x; E16[4] = dir.y; E16[8] = dir.z;
	}

	template<class TYPE>
	void TMat4<TYPE>::SetYAxis(const TVec3<TYPE> &dir)
	{
		E16[1] = dir.x; E16[5] = dir.y; E16[9] = dir.z;
	}

	template<class TYPE>
	void TMat4<TYPE>::SetZAxis(const TVec3<TYPE> &dir)
	{
		E16[2] = dir.x; E16[6] = dir.y; E16[10] = dir.z;
	}
}

