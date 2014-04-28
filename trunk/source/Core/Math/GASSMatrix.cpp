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

#include <math.h>
#include "Core/Math/GASSMath.h"
#include "Core/Math/GASSMatrix.h"
#include "Core/Math/GASSVector.h"
#include "Core/Math/GASSQuaternion.h"

namespace GASS
{

	const Float Mat4::EPSILON = 0.0001;

	Mat4::Mat4()
	{

	}

	Mat4::Mat4(
		Float m00, Float m01, Float m02, Float m03,
		Float m10, Float m11, Float m12, Float m13,
		Float m20, Float m21, Float m22, Float m23,
		Float m30, Float m31, Float m32, Float m33 )
	{
		m_Data[0][0] = m00;
		m_Data[0][1] = m01;
		m_Data[0][2] = m02;
		m_Data[0][3] = m03;
		m_Data[1][0] = m10;
		m_Data[1][1] = m11;
		m_Data[1][2] = m12;
		m_Data[1][3] = m13;
		m_Data[2][0] = m20;
		m_Data[2][1] = m21;
		m_Data[2][2] = m22;
		m_Data[2][3] = m23;
		m_Data[3][0] = m30;
		m_Data[3][1] = m31;
		m_Data[3][2] = m32;
		m_Data[3][3] = m33;
	}

	Mat4 Mat4::operator* (const Mat4 &mat) const
	{
		Mat4 ret;
		for (int iRow = 0; iRow < 4; iRow++)
		{
			for (int iCol = 0; iCol < 4; iCol++)
			{
				ret.m_Data[iRow][iCol] =
					m_Data[iRow][0] * mat.m_Data[0][iCol] +
					m_Data[iRow][1] * mat.m_Data[1][iCol] +
					m_Data[iRow][2] * mat.m_Data[2][iCol] + 
					m_Data[iRow][3] * mat.m_Data[3][iCol];
			}
		}
		return ret;
	}

	void Mat4::Zero()
	{
		m_Data[0][0] = 0;
		m_Data[0][1] = 0;
		m_Data[0][2] = 0;
		m_Data[0][3] = 0;
		m_Data[1][0] = 0;
		m_Data[1][1] = 0;
		m_Data[1][2] = 0;
		m_Data[1][3] = 0;
		m_Data[2][0] = 0;
		m_Data[2][1] = 0;
		m_Data[2][2] = 0;
		m_Data[2][3] = 0;
		m_Data[3][0] = 0;
		m_Data[3][1] = 0;
		m_Data[3][2] = 0;
		m_Data[3][3] = 0;
	}

	void Mat4::Identity()
	{
		m_Data[0][0] = 1;
		m_Data[0][1] = 0;
		m_Data[0][2] = 0;
		m_Data[0][3] = 0;
		m_Data[1][0] = 0;
		m_Data[1][1] = 1;
		m_Data[1][2] = 0;
		m_Data[1][3] = 0;
		m_Data[2][0] = 0;
		m_Data[2][1] = 0;
		m_Data[2][2] = 1;
		m_Data[2][3] = 0;
		m_Data[3][0] = 0;
		m_Data[3][1] = 0;
		m_Data[3][2] = 0;
		m_Data[3][3] = 1;
	}

	Mat4 Mat4::Transpose()
	{
		Mat4 ret; 
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				ret.m_Data[j][i] = m_Data[i][j];
			}
		}
		return ret;
	}

	void Mat4::Translate(Float x,Float y,Float z)
	{
		Identity();
		m_Data[0][3] = x;
		m_Data[1][3] = y;
		m_Data[2][3] = z;
	}

	void Mat4::Scale(Float sx,Float sy,Float sz)
	{
		Identity();
		m_Data[0][0] = sx;
		m_Data[1][1] = sy;
		m_Data[2][2] = sz;
	}

	void Mat4::Rotate(Float h,Float p,Float r)
	{

		Float cp = (Float)cos(p);
		Float sp = (Float)sin(p);
		Float ch = (Float)cos(h);
		Float sh = (Float)sin(h);
		Float cr = (Float)cos(r);
		Float sr = (Float)sin(r);


		Float spsh = sp*sh;
		Float crsp = cr*sp;


		m_Data[0][0] = ( Float )( cr*ch+sr*sp*sh );
		m_Data[0][1] = ( Float )( sr*cp );
		m_Data[0][2] = ( Float )( -cr*sh+sr*sp*ch );

		m_Data[1][0] = ( Float )( -sr*ch+cr*sp*sh );
		m_Data[1][1] = ( Float )( cr*cp );
		m_Data[1][2] = ( Float )( sr*sh+cr*sp*ch);

		m_Data[2][0] = ( Float )( cp*sh );
		m_Data[2][1] = ( Float )( -sp );
		m_Data[2][2] = ( Float )( cp*ch );

	}

	void Mat4::SetTransformation(const Vec3 &pos,const Vec3 &rot,const Vec3 &scale)
	{
		Identity();

		m_Data[0][3] = pos.x;
		m_Data[1][3] = pos.y;
		m_Data[2][3] = pos.z;

		Rotate(rot.x,rot.y,rot.z);

		m_Data[3][0] = 0;
		m_Data[3][1] = 0;
		m_Data[3][2] = 0;

		//Scale, 
		//we should only scale diagonal!
		m_Data[0][0] *= scale.x;
		m_Data[0][1] *= scale.x;
		m_Data[0][2] *= scale.x;
		m_Data[0][3] *= scale.x;

		m_Data[1][0] *= scale.y;
		m_Data[1][1] *= scale.y;
		m_Data[1][2] *= scale.y;
		m_Data[1][3] *= scale.y;

		m_Data[2][0] *= scale.z;
		m_Data[2][1] *= scale.z;
		m_Data[2][2] *= scale.z;
		m_Data[2][3] *= scale.z;
		
	}

	void Mat4::SetTransformation(const Vec3 &pos,const Quaternion &rot,const Vec3 &scale)
	{
		Identity();
		rot.ToRotationMatrix(*this);

		m_Data[0][3] = pos.x;
		m_Data[1][3] = pos.y;
		m_Data[2][3] = pos.z;

		m_Data[3][0] = 0;
		m_Data[3][1] = 0;
		m_Data[3][2] = 0;

		//Scale
		m_Data[0][0] *= scale.x;
		m_Data[0][1] *= scale.x;
		m_Data[0][2] *= scale.x;
		m_Data[0][3] *= scale.x;

		m_Data[1][0] *= scale.y;
		m_Data[1][1] *= scale.y;
		m_Data[1][2] *= scale.y;
		m_Data[1][3] *= scale.y;

		m_Data[2][0] *= scale.z;
		m_Data[2][1] *= scale.z;
		m_Data[2][2] *= scale.z;
		m_Data[2][3] *= scale.z;
	}


	void Mat4::RotateY(Float amount)
	{
		Float ch = (Float)cos( amount);
		Float sh = (Float)sin( amount);

		m_Data[0][0] = ch;
		m_Data[0][1] = 0;
		m_Data[0][2] = sh;
		m_Data[0][3] = 0;

		m_Data[1][0] = 0;
		m_Data[1][1] = 1;
		m_Data[1][2] = 0;
		m_Data[1][3] = 0;

		m_Data[2][0] = -sh;
		m_Data[2][1] = 0;
		m_Data[2][2] = ch;
		m_Data[2][3] = 0;

		m_Data[3][0] = 0;
		m_Data[3][1] = 0;
		m_Data[3][2] = 0;
		m_Data[3][3] = 1;



	}

	void Mat4::RotateX(Float amount)
	{
		Float cp = (Float)cos( amount);
		Float sp = (Float)sin( amount);
		
		m_Data[0][0] = 1;
		m_Data[0][1] = 0;
		m_Data[0][2] = 0;
		m_Data[0][3] = 0;

		m_Data[1][0] = 0;
		m_Data[1][1] = cp;
		m_Data[1][2] = -sp;
		m_Data[1][3] = 0;

		m_Data[2][0] = 0;
		m_Data[2][1] = sp;
		m_Data[2][2] = cp;
		m_Data[2][3] = 0;

		m_Data[3][0] = 0;
		m_Data[3][1] = 0;
		m_Data[3][2] = 0;
		m_Data[3][3] = 1;
	}

	void Mat4::RotateZ(Float amount)
	{
		Float cr = (Float)cos( amount);
		Float sr = (Float)sin( amount);

		m_Data[0][0] = cr;
		m_Data[0][1] = -sr;
		m_Data[0][2] = 0;
		m_Data[0][3] = 0;

		m_Data[1][0] = sr;
		m_Data[1][1] = cr;
		m_Data[1][2] = 0;
		m_Data[1][3] = 0;

		m_Data[2][0] = 0;
		m_Data[2][1] = 0;
		m_Data[2][2] = 1;
		m_Data[2][3] = 0;

		m_Data[3][0] = 0;
		m_Data[3][1] = 0;
		m_Data[3][2] = 0;
		m_Data[3][3] = 1;
	}

	void Mat4::RelScale(Vec3 scale)
	{
		m_Data[0][0] *= scale.x;
		m_Data[0][1] *= scale.x;
		m_Data[0][2] *= scale.x;
		m_Data[0][3] *= scale.x;
		m_Data[1][0] *= scale.y;
		m_Data[1][1] *= scale.y;
		m_Data[1][2] *= scale.y;
		m_Data[1][3] *= scale.y;
		m_Data[2][0] *= scale.z;
		m_Data[2][1] *= scale.z;
		m_Data[2][2] *= scale.z;
		m_Data[2][3] *= scale.z;
	}

	Vec3 Mat4::operator * ( const Vec3 &vec) const
	{
		Vec3 res;
		Float d;
		res.x = m_Data[0][0] * vec.x + 
			m_Data[0][1] * vec.y + 
			m_Data[0][2] * vec.z + 
			m_Data[0][3];

		res.y = m_Data[1][0] * vec.x + 
			m_Data[1][1] * vec.y + 
			m_Data[1][2] * vec.z + 
			m_Data[1][3];

		res.z = m_Data[2][0] * vec.x + 
			m_Data[2][1] * vec.y + 
			m_Data[2][2] * vec.z + 
			m_Data[2][3];

		d = m_Data[3][0] * vec.x + 
			m_Data[3][1] * vec.y + 
			m_Data[3][2] * vec.z + 
			m_Data[3][3];

		res.x /= d;
		res.y /= d;
		res.z /= d;

		return res;
	}

	Vec4 Mat4::operator * ( const Vec4 &vec) const
	{
		Vec4 res;
		//	Float d;
		res.x = m_Data[0][0] * vec.x + 
			m_Data[0][1] * vec.y + 
			m_Data[0][2] * vec.z + 
			m_Data[0][3] * vec.w;

		res.y = m_Data[1][0] * vec.x + 
			m_Data[1][1] * vec.y + 
			m_Data[1][2] * vec.z + 
			m_Data[1][3] * vec.w;

		res.z = m_Data[2][0] * vec.x + 
			m_Data[2][1] * vec.y + 
			m_Data[2][2] * vec.z + 
			m_Data[2][3] * vec.w;

		res.w = m_Data[3][0] * vec.x + 
			m_Data[3][1] * vec.y + 
			m_Data[3][2] * vec.z + 
			m_Data[3][3] * vec.w;

		/*d = m_Data[0][3] * vec.x + 
		m_Data[1][3] * vec.y + 
		m_Data[2][3] * vec.z + 
		m_Data[3][3] * vec.w;

		res.x /= d;
		res.y /= d;
		res.z /= d;
		res.w /= d;*/

		return res;
	}

	/*void Mat4::InverseRotateVect( Vec3 &vec) 
	{
		Vec3 temp;

		temp.x = vec.x*m_Data2[0]+vec.y*m_Data2[1]+vec.z*m_Data2[2];
		temp.y = vec.x*m_Data2[4]+vec.y*m_Data2[5]+vec.z*m_Data2[6];
		temp.z = vec.x*m_Data2[8]+vec.y*m_Data2[9]+vec.z*m_Data2[10];
		vec = temp;
	}

	void Mat4::InverseTranslateVect( Vec3 &vec) 
	{
		vec.x = vec.x -m_Data2[12];
		vec.y = vec.y -m_Data2[13];
		vec.z = vec.z -m_Data2[14];
	}*/

	Vec3 Mat4::GetTranslation()  const
	{
		Vec3 ret;
		ret.x = m_Data[0][3];
		ret.y = m_Data[1][3];
		ret.z = m_Data[2][3];
		return ret;
	}

	Mat4 Mat4::GetRotation() const
	{

		Mat4 ret;
		ret = *this;
		ret.m_Data[0][3] = 0;
		ret.m_Data[1][3] = 0;
		ret.m_Data[2][3] = 0;
		return ret;
	}

	Float Mat4::Determinant() const
	{
		Float fCofactor00 = m_Data[1][1]*m_Data[2][2] -
			m_Data[1][2]*m_Data[2][1];
		Float fCofactor10 = m_Data[1][2]*m_Data[2][0] -
			m_Data[1][0]*m_Data[2][2];
		Float fCofactor20 = m_Data[1][0]*m_Data[2][1] -
			m_Data[1][1]*m_Data[2][0];

		Float fDet =
			m_Data[0][0]*fCofactor00 +
			m_Data[0][1]*fCofactor10 +
			m_Data[0][2]*fCofactor20;

		return fDet;
	}


	/*Float Mat4::Determinant()
	{
		Float det = 0.0f;

		for (int col = 0; col < 4; col++) 
		{
			const Float sign = ((col & 0x1) == 0x0) ? 1.0f : -1.0f;
			det += sign * m_Data[0][col] * _Determinant(0, col);
		}
		return det;
	}

	


	/*Float Mat4::_Determinant(int row, int col) 
	{
		assert(row >= 0 && row < 4 && col >= 0 && col < 4);

		Float data[9];
		int current = 0;

		for (int index = 0; index < 16; index++) 
		{
			if ((index / 4) == col || (index % 4) == row) 
			{
				continue;
			} 
			else 
			{
				data[current++] = m_Data2[index];
			}
		}

		/* 
		The newly created 3x3 matrix is also in column-major
		form:

		d0 d3 d6
		d1 d4 d7
		d2 d5 d8
		*/

		/*return
			data[0] * (data[4] * data[8] - data[7] * data[5]) -
			data[1] * (data[3] * data[8] - data[6] * data[5]) +
			data[2] * (data[3] * data[7] - data[6] * data[4]);
	}*/


	/*Mat4 Mat4::Invert()
	{
		Float det = Determinant();
		//assert(fabs(det) > EPSILON);
		if(fabs(det) < EPSILON) det = EPSILON;
		Mat4 result;
		for (int row = 0; row < 4; row++) {
			for (int col = 0; col < 4; col++) {
				const Float sign = (((row + col) & 0x1) == 0x0) ? 1.0f : -1.0f;
				result.m_Data2[col * 4 + row] = sign * _Determinant(col, row) / det;
			}
		}
		return result;
	}*/

	Mat4 Mat4::Invert(void) const
	{
		Float m10 = m_Data[1][0], m11 = m_Data[1][1], m12 = m_Data[1][2];
		Float m20 = m_Data[2][0], m21 = m_Data[2][1], m22 = m_Data[2][2];

		Float t00 = m22 * m11 - m21 * m12;
		Float t10 = m20 * m12 - m22 * m10;
		Float t20 = m21 * m10 - m20 * m11;

		Float m00 = m_Data[0][0], m01 = m_Data[0][1], m02 = m_Data[0][2];

		Float invDet = 1 / (m00 * t00 + m01 * t10 + m02 * t20);

		t00 *= invDet; t10 *= invDet; t20 *= invDet;

		m00 *= invDet; m01 *= invDet; m02 *= invDet;

		Float r00 = t00;
		Float r01 = m02 * m21 - m01 * m22;
		Float r02 = m01 * m12 - m02 * m11;

		Float r10 = t10;
		Float r11 = m00 * m22 - m02 * m20;
		Float r12 = m02 * m10 - m00 * m12;

		Float r20 = t20;
		Float r21 = m01 * m20 - m00 * m21;
		Float r22 = m00 * m11 - m01 * m10;

		Float m03 = m_Data[0][3], m13 = m_Data[1][3], m23 = m_Data[2][3];

		Float r03 = - (r00 * m03 + r01 * m13 + r02 * m23);
		Float r13 = - (r10 * m03 + r11 * m13 + r12 * m23);
		Float r23 = - (r20 * m03 + r21 * m13 + r22 * m23);

		return Mat4(
			r00, r01, r02, r03,
			r10, r11, r12, r13,
			r20, r21, r22, r23,
			0,   0,   0,   1);
	}


	Vec3 Mat4::GetRotationRadians() const
	{
		const Mat4 &mat = *this;

		Float Y = -asin(mat[2][0]); 
		Float D = Y;
		Float C = cos(Y); 
		//Y *= 180.000f/MY_PI; 

		Float rotx, roty, X, Z;

		if (fabs(Y)>0.0005f) 
		{ 
			rotx = mat[2][2] / C; 
			roty = mat[1][2]  / C; 
			X = atan2( roty, rotx ); 
			rotx =  mat[0][0] / C; 
			roty = mat[0][1] / C; 
			Z = atan2( roty, rotx ); 
		} 
		else 
		{
			X  = 0.0f; 
			rotx = -mat[1][1]; 
			roty = mat[0][1]; 
			Z  = atan2( roty, rotx ); 
		} 

		if (fabs(X)>=2*MY_PI) X=0.00f; 
		if (fabs(Y)>=2*MY_PI) Y=0.00f; 
		if (fabs(Z)>=2*MY_PI) Z=0.00f; 

		Vec3 rot;
		rot.Set(X,Y,Z); 
		return rot; 
	}	

	Float Mat4::GetEulerHeading() const
	{
		Vec3 dir = GetZAxis();
		dir.Set(dir.x, 0, dir.z);
		dir.Normalize();
		Vec3 north_dir(0,0,1);
		Float cos_h = Math::Dot(north_dir,dir);
		Vec3 cross = Math::Cross(north_dir,dir);

		// Dot product may give values slightly higher than one due to normalization/precision error
		if (cos_h > 1.0f)
			cos_h = 1.0f;
		else if (cos_h < -1.0f)
			cos_h = -1.0f;

		Float h_rad = acos(cos_h);

		if(cross.y < 0)
		{
			h_rad = -h_rad;
		}
		return h_rad;
	}

	Float Mat4::GetEulerPitch() const
	{
		Vec3 dir;
		
		dir = GetZAxis();
		Vec3 xz_dir(dir.x,0,dir.z);
		xz_dir.Normalize();
		Float cos_p = Math::Dot(xz_dir,dir);
		Vec3 cross = Math::Cross(xz_dir,dir);
		cross.Normalize();
		cross = Math::Cross(cross,xz_dir);

		// Dot product may give values slightly higher than one due to normalization/precision error
		if (cos_p > 1.0f)
			cos_p = 1.0f;
		else if (cos_p < -1.0f)
			cos_p = -1.0f;

		Float p_rad = acos(cos_p);
		if(cross.y > 0)
		{
			p_rad = -p_rad;
		}
		return p_rad;
	}
	Float Mat4::GetEulerRoll() const
	{
		Vec3 left_dir;
		left_dir = GetXAxis();
		Vec3 xz_dir(left_dir.x,0,left_dir.z);
		xz_dir.Normalize();
		Float cos_r = Math::Dot(xz_dir,left_dir);
		Vec3 cross = Math::Cross(xz_dir,left_dir);
		cross.Normalize();
		cross = Math::Cross(cross,xz_dir);
		// Dot product may give values slightly higher than one due to normalization/precision error
		if (cos_r > 1.0f)
			cos_r = 1.0f;
		else if (cos_r < -1.0f)
			cos_r = -1.0f;
		Float r_rad = acos(cos_r);

		if(cross.y > 0)
		{
			r_rad = -r_rad;
		}
		return r_rad;
	}

	static Float Det2x2(Float a1, Float a2, Float b1, Float b2)
	{
		return a1 * b2 - b1 * a2;
	}

	static Float Det3x3(Float a1, Float a2, Float a3, Float b1, Float b2, Float b3, Float c1, Float c2, Float c3)
	{
		return a1 * Det2x2(b2, b3, c2, c3) - b1 * Det2x2(a2, a3, c2, c3) + c1 * Det2x2(a2, a3, b2, b3);
	}

	static void FastInvert(const Float A[4][4], Float B[4][4])
	{
		B[0][0] =  Det3x3(A[1][1], A[1][2], A[1][3], A[2][1], A[2][2], A[2][3], A[3][1], A[3][2], A[3][3]);
		B[0][1] = -Det3x3(A[0][1], A[0][2], A[0][3], A[2][1], A[2][2], A[2][3], A[3][1], A[3][2], A[3][3]);
		B[0][2] =  Det3x3(A[0][1], A[0][2], A[0][3], A[1][1], A[1][2], A[1][3], A[3][1], A[3][2], A[3][3]);
		B[0][3] = -Det3x3(A[0][1], A[0][2], A[0][3], A[1][1], A[1][2], A[1][3], A[2][1], A[2][2], A[2][3]);
		B[1][0] = -Det3x3(A[1][0], A[1][2], A[1][3], A[2][0], A[2][2], A[2][3], A[3][0], A[3][2], A[3][3]);
		B[1][1] =  Det3x3(A[0][0], A[0][2], A[0][3], A[2][0], A[2][2], A[2][3], A[3][0], A[3][2], A[3][3]);
		B[1][2] = -Det3x3(A[0][0], A[0][2], A[0][3], A[1][0], A[1][2], A[1][3], A[3][0], A[3][2], A[3][3]);
		B[1][3] =  Det3x3(A[0][0], A[0][2], A[0][3], A[1][0], A[1][2], A[1][3], A[2][0], A[2][2], A[2][3]);
		B[2][0] =  Det3x3(A[1][0], A[1][1], A[1][3], A[2][0], A[2][1], A[2][3], A[3][0], A[3][1], A[3][3]);
		B[2][1] = -Det3x3(A[0][0], A[0][1], A[0][3], A[2][0], A[2][1], A[2][3], A[3][0], A[3][1], A[3][3]);
		B[2][2] =  Det3x3(A[0][0], A[0][1], A[0][3], A[1][0], A[1][1], A[1][3], A[3][0], A[3][1], A[3][3]);
		B[2][3] = -Det3x3(A[0][0], A[0][1], A[0][3], A[1][0], A[1][1], A[1][3], A[2][0], A[2][1], A[2][3]);
		B[3][0] = -Det3x3(A[1][0], A[1][1], A[1][2], A[2][0], A[2][1], A[2][2], A[3][0], A[3][1], A[3][2]);
		B[3][1] =  Det3x3(A[0][0], A[0][1], A[0][2], A[2][0], A[2][1], A[2][2], A[3][0], A[3][1], A[3][2]);
		B[3][2] = -Det3x3(A[0][0], A[0][1], A[0][2], A[1][0], A[1][1], A[1][2], A[3][0], A[3][1], A[3][2]);
		B[3][3] =  Det3x3(A[0][0], A[0][1], A[0][2], A[1][0], A[1][1], A[1][2], A[2][0], A[2][1], A[2][2]);
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

	Mat4 Mat4::Invert2()
	{
		Mat4 result;
		FastInvert(m_Data,result.m_Data);
		return result;
	}

	Vec3 Mat4::GetXAxis() const
	{
		//return Vec3(m_Data2[0] ,m_Data2[1] ,m_Data2[2]);
		return Vec3(m_Data2[0] ,m_Data2[4] ,m_Data2[8]);
	}
	Vec3 Mat4::GetYAxis() const
	{
		//return Vec3(m_Data2[4] ,m_Data2[5] ,m_Data2[6]);
		return Vec3(m_Data2[1] ,m_Data2[5] ,m_Data2[9]);
	}
	Vec3 Mat4::GetZAxis() const
	{
		//return Vec3(m_Data2[8] ,m_Data2[9] ,m_Data2[10]);
		return Vec3(m_Data2[2] ,m_Data2[6] ,m_Data2[10]);
	}

	void Mat4::SetXAxis( const Vec3 &dir)
	{
		//m_Data2[0] = dir.x; m_Data2[1] =dir.y; m_Data2[2] = dir.z;
		m_Data2[0] = dir.x; m_Data2[4] =dir.y; m_Data2[8] = dir.z;
	}
	void Mat4::SetYAxis(const Vec3 &dir)
	{
		//m_Data2[4] = dir.x; m_Data2[5] =dir.y; m_Data2[6] = dir.z;
		m_Data2[1] = dir.x; m_Data2[5] =dir.y; m_Data2[9] = dir.z;
	}
	void Mat4::SetZAxis(const Vec3 &dir)
	{
		//m_Data2[8] = dir.x; m_Data2[9] =dir.y; m_Data2[10] = dir.z;
		m_Data2[2] = dir.x; m_Data2[6] =dir.y; m_Data2[10] = dir.z;
	}
}
