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

#include "Core/Math/GASSQuaternion.h"
#include "Core/Math/GASSMath.h"
#include <math.h>

namespace GASS
{
	const Float Quaternion::ms_fEpsilon = (Float) 1e-03;
	const Quaternion Quaternion::ZERO(0.0,0.0,0.0,0.0);
	const Quaternion Quaternion::IDENTITY(1.0,0.0,0.0,0.0);

	//-----------------------------------------------------------------------
	Quaternion::Quaternion (Float fW, Float fX, Float fY, Float fZ)
	{
		w = fW;
		x = fX;
		y = fY;
		z = fZ;
	}
	//-----------------------------------------------------------------------
	Quaternion::Quaternion (const Quaternion& rkQ)
	{
		w = rkQ.w;
		x = rkQ.x;
		y = rkQ.y;
		z = rkQ.z;
	}

	Quaternion::Quaternion (const Vec3 &euler_rot)
	{
		FromEulerAngles(euler_rot);
	}

	void Quaternion::FromEulerAngles (const Vec3 &rot)
	{
		Quaternion qh;
		Quaternion qp;
		Quaternion qr;

		qh.FromAngleAxis( rot.x, Vec3(0,1,0));
		qp.FromAngleAxis( rot.y, Vec3(1,0,0));
		qr.FromAngleAxis( rot.z, Vec3(0,0,1));

		*this = qh * qp;
		*this = (*this) * qr;
	}

	//-----------------------------------------------------------------------
	void Quaternion::FromRotationMatrix (const Mat4& kRot)
	{
		// Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
		// article "Quaternion Calculus and Fast Animation".

		const Float fTrace = kRot.m_Data[0][0]+kRot.m_Data[1][1]+kRot.m_Data[2][2];
		Float froot;

		if ( fTrace > 0.0 )
		{
			// |w| > 1/2, may as well choose w > 1/2
			froot = (Float) sqrt(fTrace + 1.0);  // 2w
			w = 0.5f *froot;
			froot = 0.5f/froot;  // 1/(4w)
			x = (kRot.m_Data[2][1]-kRot.m_Data[1][2])*froot;
			y = (kRot.m_Data[0][2]-kRot.m_Data[2][0])*froot;
			z = (kRot.m_Data[1][0]-kRot.m_Data[0][1])*froot;
		}
		else
		{
			// |w| <= 1/2
			static int s_iNext[3] = { 1, 2, 0 };
			int i = 0;
			if ( kRot.m_Data[1][1] > kRot.m_Data[0][0] )
				i = 1;
			if ( kRot.m_Data[2][2] > kRot.m_Data[i][i] )
				i = 2;
			int j = s_iNext[i];
			int k = s_iNext[j];

			froot = sqrt(kRot.m_Data[i][i]-kRot.m_Data[j][j]-kRot.m_Data[k][k] + 1.0);
			Float* apkQuat[3] = { &x, &y, &z };
			*apkQuat[i] = 0.5*froot;
			froot = 0.5/froot;
			w = (kRot.m_Data[k][j]-kRot.m_Data[j][k])*froot;
			*apkQuat[j] = (kRot.m_Data[j][i]+kRot.m_Data[i][j])*froot;
			*apkQuat[k] = (kRot.m_Data[k][i]+kRot.m_Data[i][k])*froot;
		}
	}

	
	//-----------------------------------------------------------------------
	void Quaternion::ToRotationMatrix(Mat4& kRot) const
	{


		//If q is guaranteed to be a unit quaternion, s will always
		//be 1.  In that case, this calculation can be optimized out.

		const double	norm = Norm();
		const double	s = (norm > 0) ? 2/norm : 0,
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

		kRot.m_Data[kX][kX] = 1.0 - (yy + zz);
		kRot.m_Data[kY][kX] = xy + wz;
		kRot.m_Data[kZ][kX] = xz - wy;
		
		//y axis
		kRot.m_Data[kX][kY] = xy - wz;
		kRot.m_Data[kY][kY] = 1.0 - (xx + zz);
		kRot.m_Data[kZ][kY] = yz + wx;

		//z axis
		kRot.m_Data[kX][kZ] = xz + wy;
		kRot.m_Data[kY][kZ] = yz - wx;
		kRot.m_Data[kZ][kZ] = 1.0 - (xx + yy);

		/*
		4th row and column of 4x4 matrix
		Translation and scale are not stored in quaternions, so these
		values are set to default (no scale, no translation).
		For systems where m comes pre-loaded with scale and translation
		factors, this code can be excluded.
		*/
		kRot.m_Data[kW][kX] = kRot.m_Data[kW][kY] = kRot.m_Data[kW][kZ] = kRot.m_Data[kX][kW] = kRot.m_Data[kY][kW] = kRot.m_Data[kZ][kW] = 0.0;
		kRot.m_Data[kW][kW] = 1.0;

	}
	//-----------------------------------------------------------------------
	void Quaternion::FromAngleAxis (const Float& rfAngle,
		const Vec3& rkAxis)
	{
		// assert:  axis[] is unit length
		//
		// The quaternion representing the rotation is
		//   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

		const Float fHalfAngle = 0.5*rfAngle;
		const Float fSin = sin(fHalfAngle);
		w = cos(fHalfAngle);
		x = fSin*rkAxis.x;
		y = fSin*rkAxis.y;
		z = fSin*rkAxis.z;
	}
	//-----------------------------------------------------------------------
	void Quaternion::ToAngleAxis (Float& rfAngle, Vec3& rkAxis) const
	{
		// The quaternion representing the rotation is
		//   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

		const Float fSqrLength = x*x+y*y+z*z;
		if ( fSqrLength > 0.0)
		{
			rfAngle = 2.0*acos(w);
			const Float fInvLength = 1.0/sqrt(fSqrLength);
			rkAxis.x = x*fInvLength;
			rkAxis.y = y*fInvLength;
			rkAxis.z = z*fInvLength;
		}
		else
		{
			// angle is 0 (mod 2*pi), so any axis will do
			rfAngle = 0.0;
			rkAxis.x = 1.0;
			rkAxis.y = 0.0;
			rkAxis.z = 0.0;
		}
	}
	//-----------------------------------------------------------------------
	void Quaternion::FromAxes (const Vec3* akAxis)
	{
		Mat4 kRot;
		kRot.Identity();
		for (int iCol = 0; iCol < 3; iCol++)
		{
			kRot.m_Data[0][iCol] = akAxis[iCol].x;
			kRot.m_Data[1][iCol] = akAxis[iCol].y;
			kRot.m_Data[2][iCol] = akAxis[iCol].z;
		}
		FromRotationMatrix(kRot);
	}
	//-----------------------------------------------------------------------
	void Quaternion::FromAxes (const Vec3& xAxis, const Vec3& yAxis, const Vec3& zAxis)
	{
		Mat4 kRot;
		kRot.Identity();
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
	//-----------------------------------------------------------------------
	void Quaternion::ToAxes (Vec3* akAxis) const
	{
		Mat4 kRot;

		ToRotationMatrix(kRot);

		for (int iCol = 0; iCol < 3; iCol++)
		{
			akAxis[iCol].x = kRot.m_Data[0][iCol];
			akAxis[iCol].y = kRot.m_Data[1][iCol];
			akAxis[iCol].z = kRot.m_Data[2][iCol];
		}
	}
	//-----------------------------------------------------------------------
	void Quaternion::ToAxes (Vec3& xAxis, Vec3& yAxis, Vec3& zAxis)
	{
		Mat4 kRot;

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

	Vec3 Quaternion::GetXAxis(void) const
	{
		Float fTy  = 2.0f*y;
		Float fTz  = 2.0f*z;
		Float fTwy = fTy*w;
		Float fTwz = fTz*w;
		Float fTxy = fTy*x;
		Float fTxz = fTz*x;
		Float fTyy = fTy*y;
		Float fTzz = fTz*z;
		//return Vec3(1.0f-(fTyy+fTzz), fTxy-fTwz, fTxz+fTwy);
		return Vec3(1.0f-(fTyy+fTzz), fTxy+fTwz, fTxz-fTwy);
	}
	
	Vec3 Quaternion::GetYAxis(void) const
	{
		Float fTx  = 2.0f*x;
		Float fTy  = 2.0f*y;
		Float fTz  = 2.0f*z;
		Float fTwx = fTx*w;
		Float fTwz = fTz*w;
		Float fTxx = fTx*x;
		Float fTxy = fTy*x;
		Float fTyz = fTz*y;
		Float fTzz = fTz*z;
		//return Vec3(fTxy+fTwz, 1.0f-(fTxx+fTzz), fTyz-fTwx);
		return Vec3(fTxy-fTwz, 1.0f-(fTxx+fTzz), fTyz+fTwx);
	}
	
	Vec3 Quaternion::GetZAxis(void) const
	{
		Float fTx  = 2.0f*x;
		Float fTy  = 2.0f*y;
		Float fTz  = 2.0f*z;
		Float fTwx = fTx*w;
		Float fTwy = fTy*w;
		Float fTxx = fTx*x;
		Float fTxz = fTz*x;
		Float fTyy = fTy*y;
		Float fTyz = fTz*y;
		//return Vec3(fTxz-fTwy, fTyz+fTwx, 1.0f-(fTxx+fTyy));
		return Vec3(fTxz+fTwy, fTyz-fTwx, 1.0f-(fTxx+fTyy));
	}

	//-----------------------------------------------------------------------
	Quaternion& Quaternion::operator= (const Quaternion& rkQ)
	{
		w = rkQ.w;
		x = rkQ.x;
		y = rkQ.y;
		z = rkQ.z;
		return *this;
	}
	//-----------------------------------------------------------------------
	Quaternion Quaternion::operator+ (const Quaternion& rkQ) const
	{
		return Quaternion(w+rkQ.w,x+rkQ.x,y+rkQ.y,z+rkQ.z);
	}
	//-----------------------------------------------------------------------
	Quaternion Quaternion::operator- (const Quaternion& rkQ) const
	{
		return Quaternion(w-rkQ.w,x-rkQ.x,y-rkQ.y,z-rkQ.z);
	}
	//-----------------------------------------------------------------------
	Quaternion Quaternion::operator* (const Quaternion& rkQ) const
	{
		// NOTE:  Multiplication is not generally commutative, so in most
		// cases p*q != q*p.

		return Quaternion
			(
			w * rkQ.w - x * rkQ.x - y * rkQ.y - z * rkQ.z,
			w * rkQ.x + x * rkQ.w + y * rkQ.z - z * rkQ.y,
			w * rkQ.y + y * rkQ.w + z * rkQ.x - x * rkQ.z,
			w * rkQ.z + z * rkQ.w + x * rkQ.y - y * rkQ.x
			);
		
	}
	//-----------------------------------------------------------------------
	Quaternion Quaternion::operator* (Float fScalar) const
	{
		return Quaternion(fScalar*w,fScalar*x,fScalar*y,fScalar*z);
	}
	//-----------------------------------------------------------------------
	Quaternion operator* (Float fScalar, const Quaternion& rkQ)
	{
		return Quaternion(fScalar*rkQ.w,fScalar*rkQ.x,fScalar*rkQ.y,
			fScalar*rkQ.z);
	}
	//-----------------------------------------------------------------------
	Quaternion Quaternion::operator- () const
	{
		return Quaternion(-w,-x,-y,-z);
	}
	//-----------------------------------------------------------------------
	Float Quaternion::Dot (const Quaternion& rkQ) const
	{
		return w*rkQ.w+x*rkQ.x+y*rkQ.y+z*rkQ.z;
	}
	//-----------------------------------------------------------------------
	Float Quaternion::Norm () const
	{
		return w*w+x*x+y*y+z*z;
	}
	//-----------------------------------------------------------------------
	Quaternion Quaternion::Inverse () const
	{
		const Float fNorm = w*w+x*x+y*y+z*z;
		if ( fNorm > 0.0 )
		{
			const Float fInvNorm = 1.0/fNorm;
			return Quaternion(w*fInvNorm,-x*fInvNorm,-y*fInvNorm,-z*fInvNorm);
		}
		else
		{
			// return an invalid result to flag the error
			return ZERO;
		}
	}
	//-----------------------------------------------------------------------
	Quaternion Quaternion::UnitInverse () const
	{
		// assert:  'this' is unit length
		return Quaternion(w,-x,-y,-z);
	}
	//-----------------------------------------------------------------------
	Quaternion Quaternion::Exp () const
	{
		// If q = A*(x*i+y*j+z*k) where (x,y,z) is unit length, then
		// exp(q) = cos(A)+sin(A)*(x*i+y*j+z*k).  If sin(A) is near zero,
		// use exp(q) = cos(A)+A*(x*i+y*j+z*k) since A/sin(A) has limit 1.

		const Float fAngle = sqrt(x*x+y*y+z*z);
		const Float fSin = sin(fAngle);

		Quaternion kResult;
		kResult.w = cos(fAngle);

		if ( fabs(fSin) >= ms_fEpsilon )
		{
			Float fCoeff = fSin/fAngle;
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
	//-----------------------------------------------------------------------
	Quaternion Quaternion::Log () const
	{
		// If q = cos(A)+sin(A)*(x*i+y*j+z*k) where (x,y,z) is unit length, then
		// log(q) = A*(x*i+y*j+z*k).  If sin(A) is near zero, use log(q) =
		// sin(A)*(x*i+y*j+z*k) since sin(A)/A has limit 1.

		Quaternion kResult;
		kResult.w = 0.0;

		if ( fabs(w) < 1.0 )
		{
			const Float fAngle = acos(w);
			const Float fSin = sin(fAngle);
			if ( fabs(fSin) >= ms_fEpsilon )
			{
				Float fCoeff = fAngle/fSin;
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
	//-----------------------------------------------------------------------
	Vec3 Quaternion::operator* (const Vec3& v) const
	{
		// nVidia SDK implementation
		Vec3 uv, uuv; 
		Vec3 qvec;
		qvec.Set(x, y, z);
		uv = Math::Cross(qvec,v); 
		uuv = Math::Cross(qvec,uv); 
		uv = uv * (2.0f * w); 
		uuv = uuv * 2.0f; 

		return v + uv + uuv;

	}
	//-----------------------------------------------------------------------
	Quaternion Quaternion::Slerp (Float fT, const Quaternion& rkP,
		const Quaternion& rkQ)
	{
		const Float fCos = rkP.Dot(rkQ);
		const Float fAngle = acos(fCos);

		if ( fabs(fAngle) < ms_fEpsilon )
			return rkP;

		const Float fSin = sin(fAngle);
		const Float fInvSin = 1.0/fSin;
		const Float fCoeff0 = sin((1.0-fT)*fAngle)*fInvSin;
		const Float fCoeff1 = sin(fT*fAngle)*fInvSin;
		return fCoeff0*rkP + fCoeff1*rkQ;
	}
	//-----------------------------------------------------------------------
	Quaternion Quaternion::SlerpExtraSpins (Float fT,
		const Quaternion& rkP, const Quaternion& rkQ, int iExtraSpins)
	{
		const Float fCos = rkP.Dot(rkQ);
		const Float fAngle = acos(fCos);

		if ( fabs(fAngle) < ms_fEpsilon )
			return rkP;

		const Float fSin = sin(fAngle);
		const Float fPhase = GASS_PI*iExtraSpins*fT;
		const Float fInvSin = 1.0/fSin;
		const Float fCoeff0 = sin((1.0-fT)*fAngle - fPhase)*fInvSin;
		const Float fCoeff1 = sin(fT*fAngle + fPhase)*fInvSin;
		return fCoeff0*rkP + fCoeff1*rkQ;
	}
	//-----------------------------------------------------------------------
	void Quaternion::Intermediate (const Quaternion& rkQ0,
		const Quaternion& rkQ1, const Quaternion& rkQ2,
		Quaternion& rkA, Quaternion& rkB)
	{
		// assert:  q0, q1, q2 are unit quaternions

		const Quaternion kQ0inv = rkQ0.UnitInverse();
		const Quaternion kQ1inv = rkQ1.UnitInverse();
		const Quaternion rkP0 = kQ0inv*rkQ1;
		const Quaternion rkP1 = kQ1inv*rkQ2;
		const Quaternion kArg = 0.25*(rkP0.Log()-rkP1.Log());
		const Quaternion kMinusArg = -kArg;

		rkA = rkQ1*kArg.Exp();
		rkB = rkQ1*kMinusArg.Exp();
	}
	//-----------------------------------------------------------------------
	Quaternion Quaternion::Squad (Float fT,
		const Quaternion& rkP, const Quaternion& rkA,
		const Quaternion& rkB, const Quaternion& rkQ)
	{
		const Float fSlerpT = 2.0*fT*(1.0-fT);
		const Quaternion kSlerpP = Slerp2(fT,rkP,rkQ);
		const Quaternion kSlerpQ = Slerp2(fT,rkA,rkB);
		return Slerp2(fSlerpT,kSlerpP,kSlerpQ);
	}
	//-----------------------------------------------------------------------
	bool Quaternion::operator== (const Quaternion& rhs) const
	{
		return (rhs.x == x) && (rhs.y == y) &&
			(rhs.z == z) && (rhs.w == w);
	}

	bool Quaternion::operator!= (const Quaternion &q) const 
	{
		return !(q.x == x &&  q.y == y && q.z == z && q.w == w);
	}


	Quaternion Quaternion::Slerp2(Float t,const Quaternion &q1,const Quaternion &q2)
	{

		Quaternion qInterpolated;
		Quaternion q2_t = q2;

		if(q1.x == q2.x && q1.y == q2.y && q1.z == q2.z && q1.w == q2.w) 
			return q1;

		// Following the (b.a) part of the equation, we do a dot product between q1 and q2.
		// We can do a dot product because the same math applied for a 3D vector as a 4D vector.
		Float result = (q1.x * q2.x) + (q1.y * q2.y) + (q1.z * q2.z) + (q1.w * q2.w);

		// If the dot product is less than 0, the angle is greater than 90 degrees
		if(result < 0.0f)
		{
			// Negate the second quaternion and the result of the dot product
			q2_t = -q2;
			result = -result;
		}

		// Set the first and second scale for the interpolation
		Float scale0 = 1 - t, scale1 = t;

		// Next, we want to actually calculate the spherical interpolation.  Since this
		// calculation is quite computationally expensive, we want to only perform it
		// if the angle between the 2 quaternions is large enough to warrant it.  If the
		// angle is fairly small, we can actually just do a simpler linear interpolation
		// of the 2 quaternions, and skip all the complex math.  We create a "delta" value
		// of 0.1 to say that if the cosine of the angle (result of the dot product) between
		// the 2 quaternions is smaller than 0.1, then we do NOT want to perform the full on 
		// interpolation using.  This is because you won't Floatly notice the difference.

		// Check if the angle between the 2 quaternions was big enough to warrant such calculations
		if(1 - result > 0.1f)
		{
			// Get the angle between the 2 quaternions, and then store the sin() of that angle
			const Float theta = (Float)acos(result);
			const Float sinTheta = (Float)sin(theta);

			// Calculate the scale for q1 and q2, according to the angle and it's sine value
			scale0 = (Float)sin( ( 1 - t ) * theta) / sinTheta;
			scale1 = (Float)sin( ( t * theta) ) / sinTheta;
		}	

		// Calculate the x, y, z and w values for the quaternion by using a special
		// form of linear interpolation for quaternions.
		qInterpolated.x = (scale0 * q1.x) + (scale1 * q2_t.x);
		qInterpolated.y = (scale0 * q1.y) + (scale1 * q2_t.y);
		qInterpolated.z = (scale0 * q1.z) + (scale1 * q2_t.z);
		qInterpolated.w = (scale0 * q1.w) + (scale1 * q2_t.w);

		// Return the interpolated quaternion
		return qInterpolated;
	}
}
