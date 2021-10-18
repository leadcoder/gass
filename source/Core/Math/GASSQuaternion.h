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
//NOTE THAT THIS FILE IS BASED ON CODE FROM: Ogre3d?

#ifndef GASS_QUARTERNION_H
#define GASS_QUARTERNION_H

#include "Core/Math/GASSMatrix.h"
#include "Core/Math/GASSVector.h"

namespace GASS
{

	/** \addtogroup GASSCore
	*  @{
	*/
	/** \addtogroup Math
	*  @{
	*/

	/**
	* Class holding TQuaternion<TYPE> information used for 
	   rotation representation.
	*/
	template<class TYPE>
	class TQuaternion
	{
	public:
		TQuaternion () = default;
		inline TQuaternion(TYPE fW, TYPE fX, TYPE fY, TYPE fZ);
		inline TQuaternion(const TVec3<TYPE>& xAxis, const TVec3<TYPE>& yAxis, const TVec3<TYPE>& zAxis);
		
		inline void FromEulerAnglesXYZ (const TVec3<TYPE> &rot);
		inline void FromEulerAnglesYXZ (const TVec3<TYPE> &rot);
		inline void FromRotationMatrix (const TMat4<TYPE>& kRot);
		
		inline void ToRotationMatrix (TMat4<TYPE>& kRot) const;

		inline TMat4<TYPE> GetRotationMatrix() const;


		inline void FromAngleAxis (const TYPE& rfAngle, const TVec3<TYPE>& rkAxis);
		inline void ToAngleAxis (TYPE& rfAngle, TVec3<TYPE>& rkAxis) const;
		inline void FromAxes (const TVec3<TYPE>* akAxis);
		
		/** Constructs the TQuaternion using 3 axes, the axes are assumed to be orthonormal
			@see ToAxes
		*/
		inline void FromAxes(const TVec3<TYPE>& xAxis, const TVec3<TYPE>& yAxis, const TVec3<TYPE>& zAxis);

		/** Gets the 3 orthonormal axes defining the TQuaternion. @see FromAxes */
		inline void ToAxes(TVec3<TYPE>* akAxis) const;
		inline void ToAxes(TVec3<TYPE>& xAxis, TVec3<TYPE>& yAxis, TVec3<TYPE>& zAxis) const;

		/** Returns the X orthonormal axis defining the TQuaternion. Same as doing
			xAxis = Vector3::UNIT_X * this. Also called the local X-axis
		*/
		inline TVec3<TYPE> GetXAxis() const;

		/** Returns the Y orthonormal axis defining the TQuaternion. Same as doing
			yAxis = Vector3::UNIT_Y * this. Also called the local Y-axis
		*/
		inline TVec3<TYPE> GetYAxis() const;

		/** Returns the Z orthonormal axis defining the TQuaternion. Same as doing
			zAxis = Vector3::UNIT_Z * this. Also called the local Z-axis
		*/
		inline TVec3<TYPE> GetZAxis() const;

		
		inline TQuaternion operator+ (const TQuaternion& rkQ) const;
		inline TQuaternion operator- (const TQuaternion& rkQ) const;
		inline TQuaternion operator* (const TQuaternion& rkQ) const;
		inline TQuaternion operator* (TYPE fScalar) const;

		inline friend TQuaternion operator* (TYPE fScalar,
			const TQuaternion& rkQ)
		{
				return TQuaternion(fScalar*rkQ.w, fScalar*rkQ.x, fScalar*rkQ.y,
					fScalar*rkQ.z);
		}
		inline TQuaternion operator- () const;
		inline bool operator== (const TQuaternion& rhs) const;
		inline bool operator!= (const TQuaternion &q) const;

		// functions of a TQuaternion
		inline TYPE Dot (const TQuaternion& rkQ) const;  // dot product
		inline TYPE Norm () const;  // squared-length
		inline TQuaternion Inverse () const;  // apply to non-zero TQuaternion
		inline TQuaternion UnitInverse () const;  // apply to unit-length TQuaternion
		inline TQuaternion Exp () const;
		inline TQuaternion Log () const;

		// rotation of a vector by a TQuaternion
		inline TVec3<TYPE> operator* (const TVec3<TYPE>& rkVector) const;

		// spherical linear interpolation
		inline static TQuaternion Slerp (TYPE fT, const TQuaternion& rkP,
			const TQuaternion& rkQ);

		inline static TQuaternion Slerp2(TYPE t, const TQuaternion &q1, const TQuaternion &q2);

		inline static TQuaternion SlerpExtraSpins (TYPE fT,
			const TQuaternion& rkP, const TQuaternion& rkQ,
			int iExtraSpins);

		// setup for spherical quadratic interpolation
		inline static void Intermediate (const TQuaternion& rkQ0,
			const TQuaternion& rkQ1, const TQuaternion& rkQ2,
			TQuaternion& rka, TQuaternion& rkB);

		// spherical quadratic interpolation
		inline static TQuaternion Squad (TYPE fT, const TQuaternion& rkP,
			const TQuaternion& rkA, const TQuaternion& rkB,
			const TQuaternion& rkQ);

		friend std::ostream& operator << (std::ostream& os, const TQuaternion& q)
		{
			os << std::setprecision(std::numeric_limits<TYPE>::digits10 + 1) << q.x << " " << q.y << " " << q.z << " " << q.w;
			return os;
		}

		friend std::istream& operator >> (std::istream& is, TQuaternion& q)
		{
			if(!(is >> q.x >>  q.y >> q.z >> q.w))
			{
				GASS_EXCEPT(Exception::ERR_INVALIDPARAMS, "Failed to parse TQuaternion","TQuaternion::>>");
			}
			return is;
		}

		// cutoff for sine near zero
		static const TYPE MS_F_EPSILON;

		// special values
		static const TQuaternion ZERO;
		static const TQuaternion IDENTITY;

		TYPE w = 1.0;
		TYPE x = 0.0;
		TYPE y = 0.0;
		TYPE z = 0.0;

		static TQuaternion CreateFromEulerXYZ(const TVec3<TYPE>& euler_rot_xyz)
		{
			TQuaternion rot;
			rot.FromEulerAnglesXYZ(euler_rot_xyz);
			return rot;
		}

		static TQuaternion CreateFromEulerYXZ(const TVec3<TYPE>& euler_rot_xyz)
		{
			TQuaternion rot;
			rot.FromEulerAnglesYXZ(euler_rot_xyz);
			return rot;
		}
	};

	using Quaternion = TQuaternion<Float>;
	using Quaternionf = TQuaternion<float>;
	using Quaterniond = TQuaternion<double>;

	template <class TYPE> const TYPE TQuaternion<TYPE>::MS_F_EPSILON = static_cast<TYPE>(1e-03);
	template <class TYPE> const TQuaternion<TYPE> TQuaternion<TYPE>::ZERO = TQuaternion<TYPE>(0 ,0, 0, 0);
	template <class TYPE> const TQuaternion<TYPE> TQuaternion<TYPE>::IDENTITY = TQuaternion<TYPE>(1, 0, 0, 0);

	template<class TYPE>
	TQuaternion<TYPE>::TQuaternion(TYPE fW, TYPE fX, TYPE fY, TYPE fZ) : w(fW),
		x(fX),
		y(fY),
		z(fZ)
	{
		
	}

	template<class TYPE>
	TQuaternion<TYPE>::TQuaternion(const TVec3<TYPE>& xAxis, const TVec3<TYPE>& yAxis, const TVec3<TYPE>& zAxis)
	{
		FromAxes(xAxis, yAxis, zAxis);
	}

	template<class TYPE>
	void TQuaternion<TYPE>::FromEulerAnglesYXZ(const TVec3<TYPE> &rot)
	{
		TQuaternion<TYPE> qx;
		TQuaternion<TYPE> qy;
		TQuaternion<TYPE> qz;

		qx.FromAngleAxis(rot.x, TVec3<TYPE>(1, 0, 0));
		qy.FromAngleAxis(rot.y, TVec3<TYPE>(0, 1, 0));
		qz.FromAngleAxis(rot.z, TVec3<TYPE>(0, 0, 1));

		*this = qy * qx;
		*this = (*this) * qz;
	}

	template<class TYPE>
	void TQuaternion<TYPE>::FromEulerAnglesXYZ(const TVec3<TYPE> &rot)
	{
		TQuaternion<TYPE> qx;
		TQuaternion<TYPE> qy;
		TQuaternion<TYPE> qz;

		qx.FromAngleAxis(rot.x, TVec3<TYPE>(1, 0, 0));
		qy.FromAngleAxis(rot.y, TVec3<TYPE>(0, 1, 0));
		qz.FromAngleAxis(rot.z, TVec3<TYPE>(0, 0, 1));
		*this = qx * qy;
		*this = (*this) * qz;
	}

	template<class TYPE>
	void TQuaternion<TYPE>::FromRotationMatrix(const TMat4<TYPE>& kRot)
	{
		// Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
		// article "TQuaternion<TYPE> Calculus and Fast Animation".

		const TYPE f_trace = kRot.m_Data[0][0] + kRot.m_Data[1][1] + kRot.m_Data[2][2];
		TYPE froot;

		if (f_trace > 0.0)
		{
			// |w| > 1/2, may as well choose w > 1/2
			froot = sqrt(f_trace + static_cast<TYPE>(1.0));  // 2w
			w = 0.5f *froot;
			froot = static_cast<TYPE>(0.5) / froot;  // 1/(4w)
			x = (kRot.m_Data[2][1] - kRot.m_Data[1][2])*froot;
			y = (kRot.m_Data[0][2] - kRot.m_Data[2][0])*froot;
			z = (kRot.m_Data[1][0] - kRot.m_Data[0][1])*froot;
		}
		else
		{
			// |w| <= 1/2
			static int s_i_next[3] = { 1, 2, 0 };
			int i = 0;
			if (kRot.m_Data[1][1] > kRot.m_Data[0][0])
				i = 1;
			if (kRot.m_Data[2][2] > kRot.m_Data[i][i])
				i = 2;
			int j = s_i_next[i];
			int k = s_i_next[j];

			froot = sqrt(kRot.m_Data[i][i] - kRot.m_Data[j][j] - kRot.m_Data[k][k] + static_cast<TYPE>(1.0));
			TYPE* apk_quat[3] = { &x, &y, &z };
			*apk_quat[i] = static_cast<TYPE>(0.5)*froot;
			froot = static_cast<TYPE>(0.5) / froot;
			w = (kRot.m_Data[k][j] - kRot.m_Data[j][k])*froot;
			*apk_quat[j] = (kRot.m_Data[j][i] + kRot.m_Data[i][j])*froot;
			*apk_quat[k] = (kRot.m_Data[k][i] + kRot.m_Data[i][k])*froot;
		}
	}

	template<class TYPE>
	TMat4<TYPE> TQuaternion<TYPE>::GetRotationMatrix() const
	{
		TMat4<TYPE> rot_mat;
		rot_mat.MakeIdentity();
		ToRotationMatrix(rot_mat);
		return rot_mat;
	}

	template<class TYPE>
	void TQuaternion<TYPE>::ToRotationMatrix(TMat4<TYPE>& kRot) const
	{
		//If q is guaranteed to be a unit TQuaternion<TYPE>, s will always
		//be 1.  In that case, this calculation can be optimized out.

		const TYPE	norm = Norm();
		const TYPE	s = (norm > 0) ? 2 / norm : 0,
			//Pre-calculate coordinate products
			xx = x * x * s,
			yy = y * y * s,
			zz = z * z * s,
			xy = x * y * s,
			xz = x * z * s,
			yz = y * z * s,
			wx = w * x * s,
			wy = w * y * s,
			wz = w * z * s;

		//Calculate 3x3 matrix from orthonormal basis

		const int k_x = 0;
		const int k_y = 1;
		const int k_z = 2;
		const int k_w = 3;

		//x axis

		kRot.m_Data[k_x][k_x] = static_cast<TYPE>(1.0) - (yy + zz);
		kRot.m_Data[k_y][k_x] = xy + wz;
		kRot.m_Data[k_z][k_x] = xz - wy;

		//y axis
		kRot.m_Data[k_x][k_y] = xy - wz;
		kRot.m_Data[k_y][k_y] = static_cast<TYPE>(1.0) - (xx + zz);
		kRot.m_Data[k_z][k_y] = yz + wx;

		//z axis
		kRot.m_Data[k_x][k_z] = xz + wy;
		kRot.m_Data[k_y][k_z] = yz - wx;
		kRot.m_Data[k_z][k_z] = static_cast<TYPE>(1.0) - (xx + yy);

		/*
		4th row and column of 4x4 matrix
		Translation and scale are not stored in TQuaternion<TYPE>s, so these
		values are set to default (no scale, no translation).
		For systems where m comes pre-loaded with scale and translation
		factors, this code can be excluded.
		*/
		kRot.m_Data[k_w][k_x] = kRot.m_Data[k_w][k_y] = kRot.m_Data[k_w][k_z] = kRot.m_Data[k_x][k_w] = kRot.m_Data[k_y][k_w] = kRot.m_Data[k_z][k_w] = 0.0;
		kRot.m_Data[k_w][k_w] = static_cast<TYPE>(1.0);

	}

	template<class TYPE>
	void TQuaternion<TYPE>::FromAngleAxis(const TYPE& rfAngle,
		const TVec3<TYPE>& rkAxis)
	{
		// assert:  axis[] is unit length
		//
		// The TQuaternion<TYPE> representing the rotation is
		//   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

		const TYPE f_half_angle = static_cast<TYPE>(0.5)*rfAngle;
		const TYPE f_sin = sin(f_half_angle);
		w = cos(f_half_angle);
		x = f_sin*rkAxis.x;
		y = f_sin*rkAxis.y;
		z = f_sin*rkAxis.z;
	}

	template<class TYPE>
	void TQuaternion<TYPE>::ToAngleAxis(TYPE& rfAngle, TVec3<TYPE>& rkAxis) const
	{
		// The TQuaternion<TYPE> representing the rotation is
		//   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

		const TYPE f_sqr_length = x*x + y*y + z*z;
		if (f_sqr_length > 0.0)
		{
			rfAngle = static_cast<TYPE>(2.0)*acos(w);
			const TYPE f_inv_length = static_cast<TYPE>(1.0) / sqrt(f_sqr_length);
			rkAxis.x = x*f_inv_length;
			rkAxis.y = y*f_inv_length;
			rkAxis.z = z*f_inv_length;
		}
		else
		{
			// angle is 0 (mod 2*pi), so any axis will do
			rfAngle = static_cast<TYPE>(0.0);
			rkAxis.x = static_cast<TYPE>(1.0);
			rkAxis.y = static_cast<TYPE>(0.0);
			rkAxis.z = static_cast<TYPE>(0.0);
		}
	}
	
	template<class TYPE>
	void TQuaternion<TYPE>::FromAxes(const TVec3<TYPE>* akAxis)
	{
		TMat4<TYPE> k_rot;
		k_rot.MakeIdentity();
		for (int i_col = 0; i_col < 3; i_col++)
		{
			k_rot.m_Data[0][i_col] = akAxis[i_col].x;
			k_rot.m_Data[1][i_col] = akAxis[i_col].y;
			k_rot.m_Data[2][i_col] = akAxis[i_col].z;
		}
		FromRotationMatrix(k_rot);
	}
	
	template<class TYPE>
	void TQuaternion<TYPE>::FromAxes(const TVec3<TYPE>& xAxis, const TVec3<TYPE>& yAxis, const TVec3<TYPE>& zAxis)
	{
		TMat4<TYPE> k_rot;
		k_rot.MakeIdentity();
		k_rot.m_Data[0][0] = xAxis.x;
		k_rot.m_Data[1][0] = xAxis.y;
		k_rot.m_Data[2][0] = xAxis.z;

		k_rot.m_Data[0][1] = yAxis.x;
		k_rot.m_Data[1][1] = yAxis.y;
		k_rot.m_Data[2][1] = yAxis.z;

		k_rot.m_Data[0][2] = zAxis.x;
		k_rot.m_Data[1][2] = zAxis.y;
		k_rot.m_Data[2][2] = zAxis.z;
		FromRotationMatrix(k_rot);
	}
	
	template<class TYPE>
	void TQuaternion<TYPE>::ToAxes(TVec3<TYPE>* akAxis) const
	{
		TMat4<TYPE> k_rot;

		ToRotationMatrix(k_rot);

		for (int i_col = 0; i_col < 3; i_col++)
		{
			akAxis[i_col].x = k_rot.m_Data[0][i_col];
			akAxis[i_col].y = k_rot.m_Data[1][i_col];
			akAxis[i_col].z = k_rot.m_Data[2][i_col];
		}
	}

	template<class TYPE>
	void TQuaternion<TYPE>::ToAxes(TVec3<TYPE>& xAxis, TVec3<TYPE>& yAxis, TVec3<TYPE>& zAxis) const
	{
		TMat4<TYPE> k_rot;

		ToRotationMatrix(k_rot);

		xAxis.x = k_rot.m_Data[0][0];
		xAxis.y = k_rot.m_Data[1][0];
		xAxis.z = k_rot.m_Data[2][0];

		yAxis.x = k_rot.m_Data[0][1];
		yAxis.y = k_rot.m_Data[1][1];
		yAxis.z = k_rot.m_Data[2][1];

		zAxis.x = k_rot.m_Data[0][2];
		zAxis.y = k_rot.m_Data[1][2];
		zAxis.z = k_rot.m_Data[2][2];
	}

	template<class TYPE>
	TVec3<TYPE> TQuaternion<TYPE>::GetXAxis() const
	{
		TYPE f_ty = static_cast<TYPE>(2.0)*y;
		TYPE f_tz = static_cast<TYPE>(2.0)*z;
		TYPE f_twy = f_ty*w;
		TYPE f_twz = f_tz*w;
		TYPE f_txy = f_ty*x;
		TYPE f_txz = f_tz*x;
		TYPE f_tyy = f_ty*y;
		TYPE f_tzz = f_tz*z;
		//return TVec3<TYPE>(1.0f-(fTyy+fTzz), fTxy-fTwz, fTxz+fTwy);
		return TVec3<TYPE>(static_cast<TYPE>(1.0) - (f_tyy + f_tzz), f_txy + f_twz, f_txz - f_twy);
	}

	template<class TYPE>
	TVec3<TYPE> TQuaternion<TYPE>::GetYAxis() const
	{
		TYPE f_tx = 2.0f*x;
		TYPE f_ty = 2.0f*y;
		TYPE f_tz = 2.0f*z;
		TYPE f_twx = f_tx*w;
		TYPE f_twz = f_tz*w;
		TYPE f_txx = f_tx*x;
		TYPE f_txy = f_ty*x;
		TYPE f_tyz = f_tz*y;
		TYPE f_tzz = f_tz*z;
		//return TVec3<TYPE>(fTxy+fTwz, 1.0f-(fTxx+fTzz), fTyz-fTwx);
		return TVec3<TYPE>(f_txy - f_twz, static_cast<TYPE>(1.0) - (f_txx + f_tzz), f_tyz + f_twx);
	}

	template<class TYPE>
	TVec3<TYPE> TQuaternion<TYPE>::GetZAxis() const
	{
		TYPE f_tx = 2.0f*x;
		TYPE f_ty = 2.0f*y;
		TYPE f_tz = 2.0f*z;
		TYPE f_twx = f_tx*w;
		TYPE f_twy = f_ty*w;
		TYPE f_txx = f_tx*x;
		TYPE f_txz = f_tz*x;
		TYPE f_tyy = f_ty*y;
		TYPE f_tyz = f_tz*y;
		//return TVec3<TYPE>(fTxz-fTwy, fTyz+fTwx, 1.0f-(fTxx+fTyy));
		return TVec3<TYPE>(f_txz + f_twy, f_tyz - f_twx, static_cast<TYPE>(1.0) - (f_txx + f_tyy));
	}

	template<class TYPE>
	TQuaternion<TYPE> TQuaternion<TYPE>::operator+ (const TQuaternion<TYPE>& rkQ) const
	{
		return TQuaternion<TYPE>(w + rkQ.w, x + rkQ.x, y + rkQ.y, z + rkQ.z);
	}
	
	template<class TYPE>
	TQuaternion<TYPE> TQuaternion<TYPE>::operator- (const TQuaternion<TYPE>& rkQ) const
	{
		return TQuaternion<TYPE>(w - rkQ.w, x - rkQ.x, y - rkQ.y, z - rkQ.z);
	}
	
	template<class TYPE>
	TQuaternion<TYPE> TQuaternion<TYPE>::operator* (const TQuaternion<TYPE>& rkQ) const
	{
		// NOTE:  Multiplication is not generally commutative, so in most
		// cases p*q != q*p.

		return TQuaternion<TYPE>
			(
				w * rkQ.w - x * rkQ.x - y * rkQ.y - z * rkQ.z,
				w * rkQ.x + x * rkQ.w + y * rkQ.z - z * rkQ.y,
				w * rkQ.y + y * rkQ.w + z * rkQ.x - x * rkQ.z,
				w * rkQ.z + z * rkQ.w + x * rkQ.y - y * rkQ.x
				);

	}
	
	template<class TYPE>
	TQuaternion<TYPE> TQuaternion<TYPE>::operator* (TYPE fScalar) const
	{
		return TQuaternion<TYPE>(fScalar*w, fScalar*x, fScalar*y, fScalar*z);
	}
	
	
	
	template<class TYPE>
	TQuaternion<TYPE> TQuaternion<TYPE>::operator- () const
	{
		return TQuaternion<TYPE>(-w, -x, -y, -z);
	}
	
	template<class TYPE>
	TYPE TQuaternion<TYPE>::Dot(const TQuaternion<TYPE>& rkQ) const
	{
		return w*rkQ.w + x*rkQ.x + y*rkQ.y + z*rkQ.z;
	}
	
	template<class TYPE>
	TYPE TQuaternion<TYPE>::Norm() const
	{
		return w*w + x*x + y*y + z*z;
	}
	
	template<class TYPE>
	TQuaternion<TYPE> TQuaternion<TYPE>::Inverse() const
	{
		const TYPE f_norm = w*w + x*x + y*y + z*z;
		if (f_norm > 0.0)
		{
			const TYPE f_inv_norm = static_cast<TYPE>(1.0) / f_norm;
			return TQuaternion<TYPE>(w*f_inv_norm, -x*f_inv_norm, -y*f_inv_norm, -z*f_inv_norm);
		}
		else
		{
			// return an invalid result to flag the error
			return ZERO;
		}
	}
	
	template<class TYPE>
	TQuaternion<TYPE> TQuaternion<TYPE>::UnitInverse() const
	{
		// assert:  'this' is unit length
		return TQuaternion<TYPE>(w, -x, -y, -z);
	}
	
	template<class TYPE>
	TQuaternion<TYPE> TQuaternion<TYPE>::Exp() const
	{
		// If q = A*(x*i+y*j+z*k) where (x,y,z) is unit length, then
		// exp(q) = cos(A)+sin(A)*(x*i+y*j+z*k).  If sin(A) is near zero,
		// use exp(q) = cos(A)+A*(x*i+y*j+z*k) since A/sin(A) has limit 1.

		const TYPE f_angle = sqrt(x*x + y*y + z*z);
		const TYPE f_sin = sin(f_angle);

		TQuaternion<TYPE> k_result;
		k_result.w = cos(f_angle);

		if (fabs(f_sin) >= MS_F_EPSILON)
		{
			TYPE f_coeff = f_sin / f_angle;
			k_result.x = f_coeff*x;
			k_result.y = f_coeff*y;
			k_result.z = f_coeff*z;
		}
		else
		{
			k_result.x = x;
			k_result.y = y;
			k_result.z = z;
		}

		return k_result;
	}
	
	template<class TYPE>
	TQuaternion<TYPE> TQuaternion<TYPE>::Log() const
	{
		// If q = cos(A)+sin(A)*(x*i+y*j+z*k) where (x,y,z) is unit length, then
		// log(q) = A*(x*i+y*j+z*k).  If sin(A) is near zero, use log(q) =
		// sin(A)*(x*i+y*j+z*k) since sin(A)/A has limit 1.

		TQuaternion<TYPE> k_result;
		k_result.w = 0.0;

		if (fabs(w) < static_cast<TYPE>(1.0))
		{
			const TYPE f_angle = acos(w);
			const TYPE f_sin = sin(f_angle);
			if (fabs(f_sin) >= MS_F_EPSILON)
			{
				TYPE f_coeff = f_angle / f_sin;
				k_result.x = f_coeff*x;
				k_result.y = f_coeff*y;
				k_result.z = f_coeff*z;
				return k_result;
			}
		}

		k_result.x = x;
		k_result.y = y;
		k_result.z = z;

		return k_result;
	}
	
	template<class TYPE>
	TVec3<TYPE> TQuaternion<TYPE>::operator* (const TVec3<TYPE>& v) const
	{
		// nVidia SDK implementation
		TVec3<TYPE> uv, uuv;
		TVec3<TYPE> qvec;
		qvec.Set(x, y, z);
		uv = TVec3<TYPE>::Cross(qvec, v);
		uuv = TVec3<TYPE>::Cross(qvec, uv);
		uv = uv * (static_cast<TYPE>(2.0) * w);
		uuv = uuv * static_cast<TYPE>(2.0);

		return v + uv + uuv;

	}
	
	template<class TYPE>
	TQuaternion<TYPE> TQuaternion<TYPE>::Slerp(TYPE fT, const TQuaternion<TYPE>& rkP,
		const TQuaternion<TYPE>& rkQ)
	{
		const TYPE f_cos = rkP.Dot(rkQ);
		const TYPE f_angle = acos(f_cos);

		if (fabs(f_angle) < MS_F_EPSILON)
			return rkP;

		const TYPE f_sin = sin(f_angle);
		const TYPE f_inv_sin = static_cast<TYPE>(1.0) / f_sin;
		const TYPE f_coeff0 = sin((static_cast<TYPE>(1.0) - fT)*f_angle)*f_inv_sin;
		const TYPE f_coeff1 = sin(fT*f_angle)*f_inv_sin;
		return f_coeff0*rkP + f_coeff1*rkQ;
	}
	
	template<class TYPE>
	TQuaternion<TYPE> TQuaternion<TYPE>::SlerpExtraSpins(TYPE fT,
		const TQuaternion<TYPE>& rkP, const TQuaternion<TYPE>& rkQ, int iExtraSpins)
	{
		const TYPE f_cos = rkP.Dot(rkQ);
		const TYPE f_angle = acos(f_cos);

		if (fabs(f_angle) < MS_F_EPSILON)
			return rkP;

		const TYPE f_sin = sin(f_angle);
		const TYPE f_phase = static_cast<TYPE>(GASS_PI)*iExtraSpins*fT;
		const TYPE f_inv_sin = static_cast<TYPE>(1.0) / f_sin;
		const TYPE f_coeff0 = sin((static_cast<TYPE>(1.0) - fT)*f_angle - f_phase)*f_inv_sin;
		const TYPE f_coeff1 = sin(fT*f_angle + f_phase)*f_inv_sin;
		return f_coeff0*rkP + f_coeff1*rkQ;
	}
	
	template<class TYPE>
	void TQuaternion<TYPE>::Intermediate(const TQuaternion<TYPE>& rkQ0,
		const TQuaternion<TYPE>& rkQ1, const TQuaternion<TYPE>& rkQ2,
		TQuaternion<TYPE>& rkA, TQuaternion<TYPE>& rkB)
	{
		// assert:  q0, q1, q2 are unit TQuaternion<TYPE>s

		const TQuaternion<TYPE> k_q0inv = rkQ0.UnitInverse();
		const TQuaternion<TYPE> k_q1inv = rkQ1.UnitInverse();
		const TQuaternion<TYPE> rk_p0 = k_q0inv*rkQ1;
		const TQuaternion<TYPE> rk_p1 = k_q1inv*rkQ2;
		const TQuaternion<TYPE> k_arg = 0.25*(rk_p0.Log() - rk_p1.Log());
		const TQuaternion<TYPE> k_minus_arg = -k_arg;

		rkA = rkQ1*k_arg.Exp();
		rkB = rkQ1*k_minus_arg.Exp();
	}
	
	template<class TYPE>
	TQuaternion<TYPE> TQuaternion<TYPE>::Squad(TYPE fT,
		const TQuaternion<TYPE>& rkP, const TQuaternion<TYPE>& rkA,
		const TQuaternion<TYPE>& rkB, const TQuaternion<TYPE>& rkQ)
	{
		const TYPE f_slerp_t = static_cast<TYPE>(2.0)*fT*(static_cast<TYPE>(1.0) - fT);
		const TQuaternion<TYPE> k_slerp_p = Slerp2(fT, rkP, rkQ);
		const TQuaternion<TYPE> k_slerp_q = Slerp2(fT, rkA, rkB);
		return Slerp2(f_slerp_t, k_slerp_p, k_slerp_q);
	}
	
	template<class TYPE>
	bool TQuaternion<TYPE>::operator== (const TQuaternion<TYPE>& rhs) const
	{
		return (rhs.x == x) && (rhs.y == y) &&
			(rhs.z == z) && (rhs.w == w);
	}

	template<class TYPE>
	bool TQuaternion<TYPE>::operator!= (const TQuaternion<TYPE> &q) const
	{
		return !(q.x == x &&  q.y == y && q.z == z && q.w == w);
	}

	template<class TYPE>
	TQuaternion<TYPE> TQuaternion<TYPE>::Slerp2(TYPE t, const TQuaternion<TYPE> &q1, const TQuaternion<TYPE> &q2)
	{

		TQuaternion<TYPE> q_interpolated;
		TQuaternion<TYPE> q2_t = q2;

		if (q1.x == q2.x && q1.y == q2.y && q1.z == q2.z && q1.w == q2.w)
			return q1;

		// Following the (b.a) part of the equation, we do a dot product between q1 and q2.
		// We can do a dot product because the same math applied for a 3D vector as a 4D vector.
		TYPE result = (q1.x * q2.x) + (q1.y * q2.y) + (q1.z * q2.z) + (q1.w * q2.w);

		// If the dot product is less than 0, the angle is greater than 90 degrees
		if (result < static_cast<TYPE>(0.0))
		{
			// Negate the second TQuaternion<TYPE> and the result of the dot product
			q2_t = -q2;
			result = -result;
		}

		// Set the first and second scale for the interpolation
		TYPE scale0 = 1 - t, scale1 = t;

		// Next, we want to actually calculate the spherical interpolation.  Since this
		// calculation is quite computationally expensive, we want to only perform it
		// if the angle between the 2 TQuaternion<TYPE>s is large enough to warrant it.  If the
		// angle is fairly small, we can actually just do a simpler linear interpolation
		// of the 2 TQuaternion<TYPE>s, and skip all the complex math.  We create a "delta" value
		// of 0.1 to say that if the cosine of the angle (result of the dot product) between
		// the 2 TQuaternion<TYPE>s is smaller than 0.1, then we do NOT want to perform the full on 
		// interpolation using.  This is because you won't TYPEly notice the difference.

		// Check if the angle between the 2 TQuaternion<TYPE>s was big enough to warrant such calculations
		if (static_cast<TYPE>(1.0) - result > static_cast<TYPE>(0.1))
		{
			// Get the angle between the 2 TQuaternion<TYPE>s, and then store the sin() of that angle
			const TYPE theta = acos(result);
			const TYPE sin_theta = sin(theta);

			// Calculate the scale for q1 and q2, according to the angle and it's sine value
			scale0 = sin((static_cast<TYPE>(1.0) - t) * theta) / sin_theta;
			scale1 = sin((t * theta)) / sin_theta;
		}

		// Calculate the x, y, z and w values for the TQuaternion<TYPE> by using a special
		// form of linear interpolation for TQuaternion<TYPE>s.
		q_interpolated.x = (scale0 * q1.x) + (scale1 * q2_t.x);
		q_interpolated.y = (scale0 * q1.y) + (scale1 * q2_t.y);
		q_interpolated.z = (scale0 * q1.z) + (scale1 * q2_t.z);
		q_interpolated.w = (scale0 * q1.w) + (scale1 * q2_t.w);

		// Return the interpolated TQuaternion<TYPE>
		return q_interpolated;
	}
}
#endif // #ifndef QUARTERNION_HH
