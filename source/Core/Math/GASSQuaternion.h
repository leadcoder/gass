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
		inline TQuaternion (
			TYPE fW = 1.0,
			TYPE fX = 0.0, TYPE fY = 0.0, TYPE fZ = 0.0);
		inline TQuaternion (const TQuaternion& rkQ);
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

		inline TQuaternion& operator= (const TQuaternion& rkQ);
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
		static const TYPE ms_fEpsilon;

		// special values
		static const TQuaternion ZERO;
		static const TQuaternion IDENTITY;

		TYPE w, x, y, z;

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

	typedef TQuaternion<Float> Quaternion;
	typedef TQuaternion<float> Quaternionf;
	typedef TQuaternion<double> Quaterniond;

	template <class TYPE> const TYPE TQuaternion<TYPE>::ms_fEpsilon = static_cast<TYPE>(1e-03);
	template <class TYPE> const TQuaternion<TYPE> TQuaternion<TYPE>::ZERO = TQuaternion<TYPE>(0 ,0, 0, 0);
	template <class TYPE> const TQuaternion<TYPE> TQuaternion<TYPE>::IDENTITY = TQuaternion<TYPE>(1, 0, 0, 0);
	//const TYPE TQuaternion::ms_fEpsilon = static_cast<TYPE>(1e-03);
	//const TQuaternion TQuaternion::ZERO(0.0, 0.0, 0.0, 0.0);
	//const TQuaternion TQuaternion::IDENTITY(1.0, 0.0, 0.0, 0.0);

	template<class TYPE>
	TQuaternion<TYPE>::TQuaternion(TYPE fW, TYPE fX, TYPE fY, TYPE fZ)
	{
		w = fW;
		x = fX;
		y = fY;
		z = fZ;
	}

	template<class TYPE>
	TQuaternion<TYPE>::TQuaternion(const TQuaternion<TYPE>& rkQ) :w(rkQ.w), x(rkQ.x), y(rkQ.y), z(rkQ.z)
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

		const TYPE fTrace = kRot.m_Data[0][0] + kRot.m_Data[1][1] + kRot.m_Data[2][2];
		TYPE froot;

		if (fTrace > 0.0)
		{
			// |w| > 1/2, may as well choose w > 1/2
			froot = sqrt(fTrace + static_cast<TYPE>(1.0));  // 2w
			w = 0.5f *froot;
			froot = static_cast<TYPE>(0.5) / froot;  // 1/(4w)
			x = (kRot.m_Data[2][1] - kRot.m_Data[1][2])*froot;
			y = (kRot.m_Data[0][2] - kRot.m_Data[2][0])*froot;
			z = (kRot.m_Data[1][0] - kRot.m_Data[0][1])*froot;
		}
		else
		{
			// |w| <= 1/2
			static int s_iNext[3] = { 1, 2, 0 };
			int i = 0;
			if (kRot.m_Data[1][1] > kRot.m_Data[0][0])
				i = 1;
			if (kRot.m_Data[2][2] > kRot.m_Data[i][i])
				i = 2;
			int j = s_iNext[i];
			int k = s_iNext[j];

			froot = sqrt(kRot.m_Data[i][i] - kRot.m_Data[j][j] - kRot.m_Data[k][k] + static_cast<TYPE>(1.0));
			TYPE* apkQuat[3] = { &x, &y, &z };
			*apkQuat[i] = static_cast<TYPE>(0.5)*froot;
			froot = static_cast<TYPE>(0.5) / froot;
			w = (kRot.m_Data[k][j] - kRot.m_Data[j][k])*froot;
			*apkQuat[j] = (kRot.m_Data[j][i] + kRot.m_Data[i][j])*froot;
			*apkQuat[k] = (kRot.m_Data[k][i] + kRot.m_Data[i][k])*froot;
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

		const int kX = 0;
		const int kY = 1;
		const int kZ = 2;
		const int kW = 3;

		//x axis

		kRot.m_Data[kX][kX] = static_cast<TYPE>(1.0) - (yy + zz);
		kRot.m_Data[kY][kX] = xy + wz;
		kRot.m_Data[kZ][kX] = xz - wy;

		//y axis
		kRot.m_Data[kX][kY] = xy - wz;
		kRot.m_Data[kY][kY] = static_cast<TYPE>(1.0) - (xx + zz);
		kRot.m_Data[kZ][kY] = yz + wx;

		//z axis
		kRot.m_Data[kX][kZ] = xz + wy;
		kRot.m_Data[kY][kZ] = yz - wx;
		kRot.m_Data[kZ][kZ] = static_cast<TYPE>(1.0) - (xx + yy);

		/*
		4th row and column of 4x4 matrix
		Translation and scale are not stored in TQuaternion<TYPE>s, so these
		values are set to default (no scale, no translation).
		For systems where m comes pre-loaded with scale and translation
		factors, this code can be excluded.
		*/
		kRot.m_Data[kW][kX] = kRot.m_Data[kW][kY] = kRot.m_Data[kW][kZ] = kRot.m_Data[kX][kW] = kRot.m_Data[kY][kW] = kRot.m_Data[kZ][kW] = 0.0;
		kRot.m_Data[kW][kW] = static_cast<TYPE>(1.0);

	}

	template<class TYPE>
	void TQuaternion<TYPE>::FromAngleAxis(const TYPE& rfAngle,
		const TVec3<TYPE>& rkAxis)
	{
		// assert:  axis[] is unit length
		//
		// The TQuaternion<TYPE> representing the rotation is
		//   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

		const TYPE fHalfAngle = static_cast<TYPE>(0.5)*rfAngle;
		const TYPE fSin = sin(fHalfAngle);
		w = cos(fHalfAngle);
		x = fSin*rkAxis.x;
		y = fSin*rkAxis.y;
		z = fSin*rkAxis.z;
	}

	template<class TYPE>
	void TQuaternion<TYPE>::ToAngleAxis(TYPE& rfAngle, TVec3<TYPE>& rkAxis) const
	{
		// The TQuaternion<TYPE> representing the rotation is
		//   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

		const TYPE fSqrLength = x*x + y*y + z*z;
		if (fSqrLength > 0.0)
		{
			rfAngle = static_cast<TYPE>(2.0)*acos(w);
			const TYPE fInvLength = static_cast<TYPE>(1.0) / sqrt(fSqrLength);
			rkAxis.x = x*fInvLength;
			rkAxis.y = y*fInvLength;
			rkAxis.z = z*fInvLength;
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
		TMat4<TYPE> kRot;
		kRot.MakeIdentity();
		for (int iCol = 0; iCol < 3; iCol++)
		{
			kRot.m_Data[0][iCol] = akAxis[iCol].x;
			kRot.m_Data[1][iCol] = akAxis[iCol].y;
			kRot.m_Data[2][iCol] = akAxis[iCol].z;
		}
		FromRotationMatrix(kRot);
	}
	
	template<class TYPE>
	void TQuaternion<TYPE>::FromAxes(const TVec3<TYPE>& xAxis, const TVec3<TYPE>& yAxis, const TVec3<TYPE>& zAxis)
	{
		TMat4<TYPE> kRot;
		kRot.MakeIdentity();
		kRot.m_Data[0][0] = xAxis.x;
		kRot.m_Data[1][0] = xAxis.y;
		kRot.m_Data[2][0] = xAxis.z;

		kRot.m_Data[0][1] = yAxis.x;
		kRot.m_Data[1][1] = yAxis.y;
		kRot.m_Data[2][1] = yAxis.z;

		kRot.m_Data[0][2] = zAxis.x;
		kRot.m_Data[1][2] = zAxis.y;
		kRot.m_Data[2][2] = zAxis.z;
		FromRotationMatrix(kRot);
	}
	
	template<class TYPE>
	void TQuaternion<TYPE>::ToAxes(TVec3<TYPE>* akAxis) const
	{
		TMat4<TYPE> kRot;

		ToRotationMatrix(kRot);

		for (int iCol = 0; iCol < 3; iCol++)
		{
			akAxis[iCol].x = kRot.m_Data[0][iCol];
			akAxis[iCol].y = kRot.m_Data[1][iCol];
			akAxis[iCol].z = kRot.m_Data[2][iCol];
		}
	}

	template<class TYPE>
	void TQuaternion<TYPE>::ToAxes(TVec3<TYPE>& xAxis, TVec3<TYPE>& yAxis, TVec3<TYPE>& zAxis) const
	{
		TMat4<TYPE> kRot;

		ToRotationMatrix(kRot);

		xAxis.x = kRot.m_Data[0][0];
		xAxis.y = kRot.m_Data[1][0];
		xAxis.z = kRot.m_Data[2][0];

		yAxis.x = kRot.m_Data[0][1];
		yAxis.y = kRot.m_Data[1][1];
		yAxis.z = kRot.m_Data[2][1];

		zAxis.x = kRot.m_Data[0][2];
		zAxis.y = kRot.m_Data[1][2];
		zAxis.z = kRot.m_Data[2][2];
	}

	template<class TYPE>
	TVec3<TYPE> TQuaternion<TYPE>::GetXAxis() const
	{
		TYPE fTy = static_cast<TYPE>(2.0)*y;
		TYPE fTz = static_cast<TYPE>(2.0)*z;
		TYPE fTwy = fTy*w;
		TYPE fTwz = fTz*w;
		TYPE fTxy = fTy*x;
		TYPE fTxz = fTz*x;
		TYPE fTyy = fTy*y;
		TYPE fTzz = fTz*z;
		//return TVec3<TYPE>(1.0f-(fTyy+fTzz), fTxy-fTwz, fTxz+fTwy);
		return TVec3<TYPE>(static_cast<TYPE>(1.0) - (fTyy + fTzz), fTxy + fTwz, fTxz - fTwy);
	}

	template<class TYPE>
	TVec3<TYPE> TQuaternion<TYPE>::GetYAxis() const
	{
		TYPE fTx = 2.0f*x;
		TYPE fTy = 2.0f*y;
		TYPE fTz = 2.0f*z;
		TYPE fTwx = fTx*w;
		TYPE fTwz = fTz*w;
		TYPE fTxx = fTx*x;
		TYPE fTxy = fTy*x;
		TYPE fTyz = fTz*y;
		TYPE fTzz = fTz*z;
		//return TVec3<TYPE>(fTxy+fTwz, 1.0f-(fTxx+fTzz), fTyz-fTwx);
		return TVec3<TYPE>(fTxy - fTwz, static_cast<TYPE>(1.0) - (fTxx + fTzz), fTyz + fTwx);
	}

	template<class TYPE>
	TVec3<TYPE> TQuaternion<TYPE>::GetZAxis() const
	{
		TYPE fTx = 2.0f*x;
		TYPE fTy = 2.0f*y;
		TYPE fTz = 2.0f*z;
		TYPE fTwx = fTx*w;
		TYPE fTwy = fTy*w;
		TYPE fTxx = fTx*x;
		TYPE fTxz = fTz*x;
		TYPE fTyy = fTy*y;
		TYPE fTyz = fTz*y;
		//return TVec3<TYPE>(fTxz-fTwy, fTyz+fTwx, 1.0f-(fTxx+fTyy));
		return TVec3<TYPE>(fTxz + fTwy, fTyz - fTwx, static_cast<TYPE>(1.0) - (fTxx + fTyy));
	}

	template<class TYPE>
	TQuaternion<TYPE>& TQuaternion<TYPE>::operator= (const TQuaternion<TYPE>& rkQ)
	{
		w = rkQ.w;
		x = rkQ.x;
		y = rkQ.y;
		z = rkQ.z;
		return *this;
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
		const TYPE fNorm = w*w + x*x + y*y + z*z;
		if (fNorm > 0.0)
		{
			const TYPE fInvNorm = static_cast<TYPE>(1.0) / fNorm;
			return TQuaternion<TYPE>(w*fInvNorm, -x*fInvNorm, -y*fInvNorm, -z*fInvNorm);
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

		const TYPE fAngle = sqrt(x*x + y*y + z*z);
		const TYPE fSin = sin(fAngle);

		TQuaternion<TYPE> kResult;
		kResult.w = cos(fAngle);

		if (fabs(fSin) >= ms_fEpsilon)
		{
			TYPE fCoeff = fSin / fAngle;
			kResult.x = fCoeff*x;
			kResult.y = fCoeff*y;
			kResult.z = fCoeff*z;
		}
		else
		{
			kResult.x = x;
			kResult.y = y;
			kResult.z = z;
		}

		return kResult;
	}
	
	template<class TYPE>
	TQuaternion<TYPE> TQuaternion<TYPE>::Log() const
	{
		// If q = cos(A)+sin(A)*(x*i+y*j+z*k) where (x,y,z) is unit length, then
		// log(q) = A*(x*i+y*j+z*k).  If sin(A) is near zero, use log(q) =
		// sin(A)*(x*i+y*j+z*k) since sin(A)/A has limit 1.

		TQuaternion<TYPE> kResult;
		kResult.w = 0.0;

		if (fabs(w) < static_cast<TYPE>(1.0))
		{
			const TYPE fAngle = acos(w);
			const TYPE fSin = sin(fAngle);
			if (fabs(fSin) >= ms_fEpsilon)
			{
				TYPE fCoeff = fAngle / fSin;
				kResult.x = fCoeff*x;
				kResult.y = fCoeff*y;
				kResult.z = fCoeff*z;
				return kResult;
			}
		}

		kResult.x = x;
		kResult.y = y;
		kResult.z = z;

		return kResult;
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
		const TYPE fCos = rkP.Dot(rkQ);
		const TYPE fAngle = acos(fCos);

		if (fabs(fAngle) < ms_fEpsilon)
			return rkP;

		const TYPE fSin = sin(fAngle);
		const TYPE fInvSin = static_cast<TYPE>(1.0) / fSin;
		const TYPE fCoeff0 = sin((static_cast<TYPE>(1.0) - fT)*fAngle)*fInvSin;
		const TYPE fCoeff1 = sin(fT*fAngle)*fInvSin;
		return fCoeff0*rkP + fCoeff1*rkQ;
	}
	
	template<class TYPE>
	TQuaternion<TYPE> TQuaternion<TYPE>::SlerpExtraSpins(TYPE fT,
		const TQuaternion<TYPE>& rkP, const TQuaternion<TYPE>& rkQ, int iExtraSpins)
	{
		const TYPE fCos = rkP.Dot(rkQ);
		const TYPE fAngle = acos(fCos);

		if (fabs(fAngle) < ms_fEpsilon)
			return rkP;

		const TYPE fSin = sin(fAngle);
		const TYPE fPhase = static_cast<TYPE>(GASS_PI)*iExtraSpins*fT;
		const TYPE fInvSin = static_cast<TYPE>(1.0) / fSin;
		const TYPE fCoeff0 = sin((static_cast<TYPE>(1.0) - fT)*fAngle - fPhase)*fInvSin;
		const TYPE fCoeff1 = sin(fT*fAngle + fPhase)*fInvSin;
		return fCoeff0*rkP + fCoeff1*rkQ;
	}
	
	template<class TYPE>
	void TQuaternion<TYPE>::Intermediate(const TQuaternion<TYPE>& rkQ0,
		const TQuaternion<TYPE>& rkQ1, const TQuaternion<TYPE>& rkQ2,
		TQuaternion<TYPE>& rkA, TQuaternion<TYPE>& rkB)
	{
		// assert:  q0, q1, q2 are unit TQuaternion<TYPE>s

		const TQuaternion<TYPE> kQ0inv = rkQ0.UnitInverse();
		const TQuaternion<TYPE> kQ1inv = rkQ1.UnitInverse();
		const TQuaternion<TYPE> rkP0 = kQ0inv*rkQ1;
		const TQuaternion<TYPE> rkP1 = kQ1inv*rkQ2;
		const TQuaternion<TYPE> kArg = 0.25*(rkP0.Log() - rkP1.Log());
		const TQuaternion<TYPE> kMinusArg = -kArg;

		rkA = rkQ1*kArg.Exp();
		rkB = rkQ1*kMinusArg.Exp();
	}
	
	template<class TYPE>
	TQuaternion<TYPE> TQuaternion<TYPE>::Squad(TYPE fT,
		const TQuaternion<TYPE>& rkP, const TQuaternion<TYPE>& rkA,
		const TQuaternion<TYPE>& rkB, const TQuaternion<TYPE>& rkQ)
	{
		const TYPE fSlerpT = static_cast<TYPE>(2.0)*fT*(static_cast<TYPE>(1.0) - fT);
		const TQuaternion<TYPE> kSlerpP = Slerp2(fT, rkP, rkQ);
		const TQuaternion<TYPE> kSlerpQ = Slerp2(fT, rkA, rkB);
		return Slerp2(fSlerpT, kSlerpP, kSlerpQ);
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

		TQuaternion<TYPE> qInterpolated;
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
			const TYPE sinTheta = sin(theta);

			// Calculate the scale for q1 and q2, according to the angle and it's sine value
			scale0 = sin((static_cast<TYPE>(1.0) - t) * theta) / sinTheta;
			scale1 = sin((t * theta)) / sinTheta;
		}

		// Calculate the x, y, z and w values for the TQuaternion<TYPE> by using a special
		// form of linear interpolation for TQuaternion<TYPE>s.
		qInterpolated.x = (scale0 * q1.x) + (scale1 * q2_t.x);
		qInterpolated.y = (scale0 * q1.y) + (scale1 * q2_t.y);
		qInterpolated.z = (scale0 * q1.z) + (scale1 * q2_t.z);
		qInterpolated.w = (scale0 * q1.w) + (scale1 * q2_t.w);

		// Return the interpolated TQuaternion<TYPE>
		return qInterpolated;
	}
}
#endif // #ifndef QUARTERNION_HH
