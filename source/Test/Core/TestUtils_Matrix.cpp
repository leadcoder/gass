#include "Core/Math/GASSMatrix.h"
#include "Core/Math/GASSQuaternion.h"
#include "catch.hpp"

TEST_CASE("Test Mat4")
{
	SECTION("Test SRT transformation constructor")
	{
		GASS::Mat4 mat(GASS::Vec3(1, 1, 1), GASS::Vec3(0, 0, 0), GASS::Vec3(1, 1, 1));
		REQUIRE(mat.E16[0] == 1);
		REQUIRE(mat.E16[1] == 0);
		REQUIRE(mat.E16[2] == 0);
		REQUIRE(mat.E16[3] == 1);
		REQUIRE(mat.E16[4] == 0);
		REQUIRE(mat.E16[5] == 1);
		REQUIRE(mat.E16[6] == 0);
		REQUIRE(mat.E16[7] == 1);
		REQUIRE(mat.E16[8] == 0);
		REQUIRE(mat.E16[9] == 0);
		REQUIRE(mat.E16[10] == 1);
		REQUIRE(mat.E16[11] == 1);
		REQUIRE(mat.E16[12] == 0);
		REQUIRE(mat.E16[13] == 0);
		REQUIRE(mat.E16[14] == 0);
		REQUIRE(mat.E16[15] == 1);
	}

	SECTION("Test element constructor")
	{
		GASS::Mat4 mat(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
		for (int i = 0; i < 16; i++)
		{
			REQUIRE(mat.E16[i] == static_cast<GASS::Float>(i));
		}
	}

	SECTION("Test operator [] const")
	{
		GASS::Mat4 mat(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				REQUIRE(mat[i][j] == static_cast<GASS::Float>(j + i * 4));
			}
		}
	}

	SECTION("Test operator []")
	{
		GASS::Mat4 mat;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				GASS::Float test_value = static_cast<GASS::Float>(j + i * 4);
				mat[i][j] = test_value;
				REQUIRE(mat.m_Data[i][j] == test_value);
			}
		}
	}

	SECTION("Test Concatenate")
	{
		GASS::Mat4 mat1(0, 13, 22, 32, 24, 15, 36, 37, 18, 49, 510, 11, 122, 123, 4, 5);
		GASS::Mat4 mat2(0, 6, 6, 34, 64, 65, 76, 87, 48, 39, 31, 11, 2, 133, 214, 1);
		GASS::Mat4 ret = mat1.Concatenate(mat2);
		GASS::Float test[4][4];
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				test[i][j] = 0;
				for (int k = 0; k < 4; k++)
				{
					test[i][j] += mat1.E4x4[i][k] * mat2.E4x4[k][j];
				}
			}
		}
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				REQUIRE(ret.E4x4[i][j] == test[i][j]);
			}
		}
	}

	SECTION("Test operator * Mat4")
	{
		//Just test random mult
		GASS::Mat4 mat1(0, 13, 22, 32, 24, 15, 36, 37, 18, 49, 510, 11, 122, 123, 4, 5);
		GASS::Mat4 mat2(0, 6, 6, 34, 64, 65, 76, 87, 48, 39, 31, 11, 2, 133, 214, 1);
		GASS::Mat4 ret = mat1 * mat2;
		GASS::Float test[4][4];
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				test[i][j] = 0;
				for (int k = 0; k < 4; k++)
				{
					test[i][j] += mat1.E4x4[i][k] * mat2.E4x4[k][j];
				}
			}
		}
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				REQUIRE(ret.E4x4[i][j] == test[i][j]);
			}
		}
	}

	SECTION("Test operator * Vec3")
	{

		
		GASS::Mat4 mat1(GASS::Vec3(1, 1, 1), GASS::Vec3(0, 0, 0), GASS::Vec3(1, 1, 1));
		GASS::Vec3 point(0,0,0);
		point = mat1*point;
		REQUIRE(point == GASS::Vec3(1,1,1));

		
		//rotate around y-axis (heading)
		GASS::Mat4 mat2(GASS::Vec3(0, 0, 0), GASS::Vec3(0, GASS_PI*0.5, 0), GASS::Vec3(1, 1, 1));
		point.Set(1, 0, 0);
		point = mat2*point;
		
		REQUIRE(point.Equal(GASS::Vec3(0, 0,-1), 1.0e-10));
		
		//scale, rotate and translate
		GASS::Mat4 mat3(GASS::Vec3(2, 0, 0), GASS::Vec3(0, 0, GASS_PI*0.5), GASS::Vec3(2, 2, 2));
		point.Set(1, 0, 0);
		point = mat3*point;

		REQUIRE(point.Equal(GASS::Vec3(2, 2, 0), 1.0e-10));

		/*GASS::Mat4 mat5(GASS::Vec3(0, 0, 0), GASS::Vec3(GASS_PI*0.5, GASS_PI*0.5, 0), GASS::Vec3(1, 1, 1));

		GASS::Mat4 maty;
		maty.MakeRotationY(GASS_PI*0.5);
		GASS::Mat4 matx;
		matx.MakeRotationX(GASS_PI*0.5);

	//	GASS::Mat4 mat5 = maty*matx;

		GASS::Quaternion rt(GASS::Vec3(GASS_PI*0.5, GASS_PI*0.5, 0));
		GASS::Vec3 tx = rt.GetXAxis();
		GASS::Vec3 ty = rt.GetYAxis();
		GASS::Vec3 tz = rt.GetZAxis();
		
		GASS::Vec3 mtx = mat5.GetXAxis();
		GASS::Vec3 mty = mat5.GetYAxis();
		GASS::Vec3 mtz = mat5.GetZAxis();
		std::cout << mtz;*/
	}

	SECTION("Test operator * Vec4")
	{
		GASS::Mat4 mat1(GASS::Vec3(1, 1, 1), GASS::Vec3(0, 0, 0), GASS::Vec3(1, 1, 1));
		GASS::Vec4 point(0, 0, 0, 1);
		point = mat1*point;
		REQUIRE(point == GASS::Vec4(1, 1, 1, 1));

		//rotate around y-axis (heading)
		GASS::Mat4 mat2(GASS::Vec3(0, 0, 0), GASS::Vec3(0, GASS_PI*0.5, 0), GASS::Vec3(1, 1, 1));
		point.Set(1, 0, 0, 1);
		point = mat2*point;

		REQUIRE(point.x == Approx(0));
		REQUIRE(point.y == Approx(0));
		REQUIRE(point.z == Approx(-1));
		REQUIRE(point.w == Approx(1));

		//translate, rotate and scale
		GASS::Mat4 mat3(GASS::Vec3(2, 0, 0), GASS::Vec3(0, 0, GASS_PI*0.5), GASS::Vec3(2, 2, 2));
		point.Set(1, 0, 0, 1);
		point = mat3*point;

		REQUIRE(point.x == Approx(2));
		REQUIRE(point.y == Approx(2));
		REQUIRE(point.z == Approx(0));
		REQUIRE(point.w == Approx(1));

		//Test operation order
		GASS::Vec3 scale(2, 1, 1);
		GASS::Vec3 rotate(0, GASS_PI*0.5, 0);
		GASS::Mat4 mat4(GASS::Vec3(2, 0, 0), rotate, scale);
		GASS::Mat4 mat_scale = GASS::Mat4::CreateScale(scale);
	
		GASS::Mat4 mat_pos = GASS::Mat4::CreateTranslation(GASS::Vec3(0,0,0));
		
		GASS::Mat4 mat_rot;
		mat_rot.MakeRotationYXZ(rotate);

		GASS::Mat4 mat_trans = mat_pos*mat_rot*mat_scale;
		if(mat_trans.Equal(mat4))
			std::cout << mat_trans.Determinant();

	}

	SECTION("Test MakeZero")
	{
		GASS::Mat4 mat;
		mat.MakeZero();
		for (int i = 0; i < 16; i++)
			REQUIRE(mat.E16[i] == 0);
	}


	SECTION("Test MakeRotateX")
	{
		GASS::Mat4 mat;
		mat.MakeRotationX(GASS::Math::Deg2Rad(90));
		GASS::Vec3 temp = mat.GetYAxis();
		REQUIRE(mat.GetXAxis().Equal(GASS::Vec3::m_UnitX, 1.0e-10));
		REQUIRE(mat.GetYAxis().Equal(GASS::Vec3::m_UnitZ, 1.0e-10));
		REQUIRE(mat.GetZAxis().Equal(-GASS::Vec3::m_UnitY, 1.0e-10));
	}

	SECTION("Test MakeRotateY")
	{
		GASS::Mat4 mat;
		mat.MakeRotationY(GASS::Math::Deg2Rad(90));
		REQUIRE(mat.GetXAxis().Equal(-GASS::Vec3::m_UnitZ, 1.0e-10));
		REQUIRE(mat.GetYAxis().Equal(GASS::Vec3::m_UnitY, 1.0e-10));
		REQUIRE(mat.GetZAxis().Equal(GASS::Vec3::m_UnitX, 1.0e-10));
	}

	SECTION("Test MakeRotateZ")
	{
		GASS::Mat4 mat;
		GASS::Float heading = GASS::Math::Deg2Rad(90);
		mat.MakeRotationY(heading);
		GASS::Vec3 xaxis = mat.GetXAxis();
		REQUIRE(mat.GetXAxis().Equal(-GASS::Vec3::m_UnitZ, 1.0e-10));
		REQUIRE(mat.GetYAxis().Equal(GASS::Vec3::m_UnitY, 1.0e-10));
		REQUIRE(mat.GetZAxis().Equal(GASS::Vec3::m_UnitX, 1.0e-10));
	}

	SECTION("Test SetScale")
	{
		GASS::Mat4 mat;
		mat.SetScale(GASS::Vec3(1,2,3));
		REQUIRE(mat.E4x4[0][0] == 1);
		REQUIRE(mat.E4x4[1][1] == 2);
		REQUIRE(mat.E4x4[2][2] == 3);
	}

	SECTION("Test MakeIdentity")
	{
		GASS::Mat4 mat;
		mat.MakeIdentity();

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if(i != j)
					REQUIRE(mat[i][j] == 0);
				else
					REQUIRE(mat[i][j] == 1);
			}
		}
	}


	SECTION("Test GetTranspose")
	{
		GASS::Mat4 mat(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
		GASS::Mat4 tmat = mat.GetTranspose();
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				GASS::Float value =  j * 4 + i;
				REQUIRE(tmat[i][j] == value);
			}
		}
	}

	SECTION("Test MakeTranslation")
	{
		GASS::Mat4 mat;;
		mat.MakeTranslation(GASS::Vec3(1, 2, 3));
		REQUIRE(mat.E16[0] == 1);
		REQUIRE(mat.E16[1] == 0);
		REQUIRE(mat.E16[2] == 0);
		REQUIRE(mat.E16[3] == 1);
		REQUIRE(mat.E16[4] == 0);
		REQUIRE(mat.E16[5] == 1);
		REQUIRE(mat.E16[6] == 0);
		REQUIRE(mat.E16[7] == 2);
		REQUIRE(mat.E16[8] == 0);
		REQUIRE(mat.E16[9] == 0);
		REQUIRE(mat.E16[10] == 1);
		REQUIRE(mat.E16[11] == 3);
		REQUIRE(mat.E16[12] == 0);
		REQUIRE(mat.E16[13] == 0);
		REQUIRE(mat.E16[14] == 0);
		REQUIRE(mat.E16[15] == 1);
	}

	SECTION("Test SetTranslation")
	{
		GASS::Mat4 mat;
		mat.SetTranslation(GASS::Vec3(1, 2, 3));
		REQUIRE(mat.E4x4[0][3] == 1);
		REQUIRE(mat.E4x4[1][3] == 2);
		REQUIRE(mat.E4x4[2][3] == 3);
	}

	SECTION("Test GetTranslation")
	{
		GASS::Mat4 mat(GASS::Vec3(1, 2, 3));
		REQUIRE(mat.GetTranslation() == GASS::Vec3(1, 2, 3));
	}

	SECTION("Test MakeTransformationSRT (Euler rot)")
	{
		GASS::Mat4 mat;
		mat.MakeTransformationSRT(GASS::Vec3(33, 34, 35), GASS::Vec3(GASS_PI*0.5, 0, GASS_PI*0.5), GASS::Vec3(2, 3, 4));
		GASS::Mat4 expected_mat(0, -3, 0, 33, 0, 0, -4, 34, 2, 0, 0, 35, 0, 0, 0, 1);
		REQUIRE(mat.Equal(expected_mat, 1.0e-10));
		
	}

#if 0
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

	//TODO: why use this order
	inline friend TVec4<TYPE> operator* (TVec4<TYPE> vec, const TMat4<TYPE> &mat)
	{
		TVec4<TYPE> ret;
		ret.x = vec.x*mat.m_Data[0][0] + vec.y*mat.m_Data[1][0] + vec.z*mat.m_Data[2][0] + vec.w*mat.m_Data[3][0];
		ret.y = vec.x*mat.m_Data[0][1] + vec.y*mat.m_Data[1][1] + vec.z*mat.m_Data[2][1] + vec.w*mat.m_Data[3][1];
		ret.z = vec.x*mat.m_Data[0][2] + vec.y*mat.m_Data[1][2] + vec.z*mat.m_Data[2][2] + vec.w*mat.m_Data[3][2];
		ret.w = vec.x*mat.m_Data[0][3] + vec.y*mat.m_Data[1][3] + vec.z*mat.m_Data[2][3] + vec.w*mat.m_Data[3][3];
		return ret;
	}
#endif
}