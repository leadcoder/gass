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
#include "Core/Math/CoreMath.h"
#include "Core/Math/Matrix.h"
#include "Core/Math/Vector.h"




namespace GASS
{

	const Float Mat4::EPSILON = 0.0001;
		

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
		m_Data[3][0] = x;
		m_Data[3][1] = y;
		m_Data[3][2] = z;
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

		m_Data[3][0] = pos.x;
		m_Data[3][1] = pos.y;
		m_Data[3][2] = pos.z;

		Rotate(rot.x,rot.y,rot.z);

		m_Data[0][3] = 0;//scale.x;
		m_Data[1][3] = 0;//scale.y;
		m_Data[2][3] = 0;//scale.z;

		/*Mat4 h,p,r,res;

		h.RotateY(rot.h);p.RotateX(rot.p);r.RotateZ(rot.r);
		res = h*p*r;

		memcpy(m_Data,res.m_Data,sizeof(Float)*16);

		m_Data[3][0] = pos.x;
		m_Data[3][1] = pos.y;
		m_Data[3][2] = pos.z;*/


		/*Float cr = (Float)cos( rot.p );
		Float sr = (Float)sin( rot.p );
		Float cp = (Float)cos( rot.h );
		Float sp = (Float)sin( rot.h );
		Float cy = (Float)cos( rot.r );
		Float sy = (Float)sin( rot.r);

		m_Data[0][0] = (Float)( cp*cy );
		m_Data[0][1] = (Float)( cp*sy );
		m_Data[0][2] = (Float)( -sp );

		Float srsp = sr*sp;
		Float crsp = cr*sp;

		m_Data[1][0] = (Float)( srsp*cy-cr*sy );
		m_Data[1][1] = (Float)( srsp*sy+cr*cy );
		m_Data[1][2] = (Float)( sr*cp );

		m_Data[2][0] = (Float)( crsp*cy+sr*sy );
		m_Data[2][1] = (Float)( crsp*sy-sr*cy );
		m_Data[2][2] = (Float)( cr*cp );*/

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
		//m_Data[1][1] *= scale.y;
		//m_Data[2][2] *= scale.z;
	}


	void Mat4::RotateY(Float amount)
	{
		Float ch = (Float)cos( amount);
		Float sh = (Float)sin( amount);

		m_Data[0][0] = ch;
		m_Data[1][0] = 0;
		m_Data[2][0] = sh;
		m_Data[3][0] = 0;

		m_Data[0][1] = 0;
		m_Data[1][1] = 1;
		m_Data[2][1] = 0;
		m_Data[3][1] = 0;

		m_Data[0][2] = -sh;
		m_Data[1][2] = 0;
		m_Data[2][2] = ch;
		m_Data[3][2] = 0;

		m_Data[0][3] = 0;
		m_Data[1][3] = 0;
		m_Data[2][3] = 0;
		m_Data[3][3] = 1;
	}

	void Mat4::RotateX(Float amount)
	{
		Float cp = (Float)cos( amount);
		Float sp = (Float)sin( amount);

		m_Data[0][0] = 1;
		m_Data[1][0] = 0;
		m_Data[2][0] = 0;
		m_Data[3][0] = 0;

		m_Data[0][1] = 0;
		m_Data[1][1] = cp;
		m_Data[2][1] = -sp;
		m_Data[3][1] = 0;

		m_Data[0][2] = 0;
		m_Data[1][2] = sp;
		m_Data[2][2] = cp;
		m_Data[3][2] = 0;

		m_Data[0][3] = 0;
		m_Data[1][3] = 0;
		m_Data[2][3] = 0;
		m_Data[3][3] = 1;
	}

	void Mat4::RotateZ(Float amount)
	{
		Float cr = (Float)cos( amount);
		Float sr = (Float)sin( amount);

		m_Data[0][0] = cr;
		m_Data[1][0] = -sr;
		m_Data[2][0] = 0;
		m_Data[3][0] = 0;

		m_Data[0][1] = sr;
		m_Data[1][1] = cr;
		m_Data[2][1] = 0;
		m_Data[3][1] = 0;

		m_Data[0][2] = 0;
		m_Data[1][2] = 0;
		m_Data[2][2] = 1;
		m_Data[3][2] = 0;

		m_Data[0][3] = 0;
		m_Data[1][3] = 0;
		m_Data[2][3] = 0;
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
			m_Data[1][0] * vec.y + 
			m_Data[2][0] * vec.z + 
			m_Data[3][0];

		res.y = m_Data[0][1] * vec.x + 
			m_Data[1][1] * vec.y + 
			m_Data[2][1] * vec.z + 
			m_Data[3][1];

		res.z = m_Data[0][2] * vec.x + 
			m_Data[1][2] * vec.y + 
			m_Data[2][2] * vec.z + 
			m_Data[3][2];

		d = m_Data[0][3] * vec.x + 
			m_Data[1][3] * vec.y + 
			m_Data[2][3] * vec.z + 
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
			m_Data[1][0] * vec.y + 
			m_Data[2][0] * vec.z + 
			m_Data[3][0] * vec.w;

		res.y = m_Data[0][1] * vec.x + 
			m_Data[1][1] * vec.y + 
			m_Data[2][1] * vec.z + 
			m_Data[3][1] * vec.w;

		res.z = m_Data[0][2] * vec.x + 
			m_Data[1][2] * vec.y + 
			m_Data[2][2] * vec.z + 
			m_Data[3][2] * vec.w;

		res.w = m_Data[0][3] * vec.x + 
			m_Data[1][3] * vec.y + 
			m_Data[2][3] * vec.z + 
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
	Mat4 mat = Transpose();

	temp.x = vec.x*mat.m_Data2[0]+vec.y*mat.m_Data2[1]+vec.z*mat.m_Data2[2];
	temp.y = vec.x*mat.m_Data2[4]+vec.y*mat.m_Data2[5]+vec.z*mat.m_Data2[6];
	temp.z = vec.x*mat.m_Data2[8]+vec.y*mat.m_Data2[9]+vec.z*mat.m_Data2[10];

	vec = temp;


	}

	void Mat4::InverseTranslateVect( Vec3 &vec) 
	{
	Mat4 mat = Transpose();
	vec.x = vec.x -mat.m_Data2[12];
	vec.y = vec.y -mat.m_Data2[13];
	vec.z = vec.z -mat.m_Data2[14];

	}*/

	void Mat4::InverseRotateVect( Vec3 &vec) 
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

	}

	Vec3 Mat4::GetTranslation()  const
	{
		Vec3 ret;
		ret.x = m_Data[3][0];
		ret.y = m_Data[3][1];
		ret.z = m_Data[3][2];
		return ret;
	}




	Mat4 Mat4::GetRotation() const
	{

		Mat4 ret;
		ret = *this;
		//ret.m_Data[0][0] = 1;
		//ret.m_Data[1][1] = 1;
		//ret.m_Data[2][2] = 1;
		ret.m_Data[3][0] = 0;
		ret.m_Data[3][1] = 0;
		ret.m_Data[3][2] = 0;
		return ret;
	}

	Float Mat4::Determinant()
	{
		Float det = 0.0f;

		for (int col = 0; col < 4; col++) {
			Float sign = ((col & 0x1) == 0x0) ? 1.0f : -1.0f;
			det += sign * m_Data[0][col] * Determinant(0, col);
		}
		return det;
	}


	Float Mat4::Determinant(int row, int col) 
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

		return
			data[0] * (data[4] * data[8] - data[7] * data[5]) -
			data[1] * (data[3] * data[8] - data[6] * data[5]) +
			data[2] * (data[3] * data[7] - data[6] * data[4]);
	}


	Mat4 Mat4::Invert()
	{
		Float det = Determinant();
		//assert(fabs(det) > EPSILON);
		if(fabs(det) < EPSILON) det = EPSILON;

		Mat4 result;

		for (int row = 0; row < 4; row++) {
			for (int col = 0; col < 4; col++) {
				Float sign = (((row + col) & 0x1) == 0x0) ? 1.0f : -1.0f;
				result.m_Data2[col * 4 + row] = sign * Determinant(col, row) / det;
			}
		}
		return result;
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
			roty = mat[2][1]  / C; 
			X = atan2f( roty, rotx ); 
			rotx =  mat[0][0] / C; 
			roty = mat[1][0] / C; 
			Z = atan2f( roty, rotx ); 
		} 
		else 
		{
			X  = 0.0f; 
			rotx = -mat[1][1]; 
			roty = mat[1][0]; 
			Z  = atan2f( roty, rotx ); 
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

		Vec3 dir;

		dir.Set(m_Data2[8],0,m_Data2[10]);
		dir.Normalize();
		Vec3 north_dir(0,0,1);
		Float cos_h = Math::Dot(north_dir,dir);
		Vec3 cross = Math::Cross(north_dir,dir);

		// Dot product may give values slightly higher than one due to normalization/precision error
		if (cos_h > 1.0f)
			cos_h = 1.0f;
		else if (cos_h < -1.0f)
			cos_h = -1.0f;

		//Float h_deg = Math::Rad2Deg(acos(cos_h));
		Float h_deg = acos(cos_h);

		if(cross.y < 0)
		{
			h_deg = -h_deg;
		}
		return h_deg;
	}

	Float Mat4::GetEulerPitch() const
	{
		Vec3 dir;
		dir.Set(m_Data2[8],m_Data2[9],m_Data2[10]);
		Vec3 xz_dir(m_Data2[8],0,m_Data2[10]);
		xz_dir.Normalize();
		Float cos_h = Math::Dot(xz_dir,dir);
		Vec3 cross = Math::Cross(xz_dir,dir);
		cross.Normalize();
		cross = Math::Cross(cross,xz_dir);

		// Dot product may give values slightly higher than one due to normalization/precision error
		if (cos_h > 1.0f)
			cos_h = 1.0f;
		else if (cos_h < -1.0f)
			cos_h = -1.0f;

		//Float h_deg = Math::Rad2Deg(acos(cos_h));
		Float h_deg = acos(cos_h);
		if(cross.y > 0)
		{
			h_deg = -h_deg;
		}
		return h_deg;
	}
	Float Mat4::GetEulerRoll() const
	{
		Vec3 left_dir;

		left_dir.Set(m_Data2[0],m_Data2[1],m_Data2[2]);
		Vec3 xz_dir(m_Data2[0],0,m_Data2[2]);
		xz_dir.Normalize();

		Float cos_h = Math::Dot(xz_dir,left_dir);
		Vec3 cross = Math::Cross(xz_dir,left_dir);
		cross.Normalize();
		cross = Math::Cross(cross,xz_dir);

		//Float h_deg = Math::Rad2Deg(acos(cos_h));

		// Dot product may give values slightly higher than one due to normalization/precision error
		if (cos_h > 1.0f)
			cos_h = 1.0f;
		else if (cos_h < -1.0f)
			cos_h = -1.0f;
		Float h_deg = acos(cos_h);

		if(cross.y > 0)
		{
			h_deg = -h_deg;
		}
		return h_deg;
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

	Vec3 Mat4::GetRightVector() const
	{
		return Vec3(m_Data2[0] ,m_Data2[1] ,m_Data2[2]);
	}
	Vec3 Mat4::GetUpVector() const
	{
		return Vec3(m_Data2[4] ,m_Data2[5] ,m_Data2[6]);
	}
	Vec3 Mat4::GetViewDirVector() const
	{
		return Vec3(m_Data2[8] ,m_Data2[9] ,m_Data2[10]);
	}

	void Mat4::SetRightVector( const Vec3 &dir)
	{
		m_Data2[0] = dir.x; m_Data2[1] =dir.y; m_Data2[2] = dir.z;
	}
	void Mat4::SetUpVector(const Vec3 &dir)
	{
		m_Data2[4] = dir.x; m_Data2[5] =dir.y; m_Data2[6] = dir.z;
	}
	void Mat4::SetViewDirVector(const Vec3 &dir)
	{
		m_Data2[8] = dir.x; m_Data2[9] =dir.y; m_Data2[10] = dir.z;
	}
}
