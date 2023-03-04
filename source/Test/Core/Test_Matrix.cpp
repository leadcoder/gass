#include "Core/Math/GASSMatrix.h"
#include "Core/Math/GASSQuaternion.h"
#include "catch.hpp"


//convert angle
GASS::Float ToPosAngle(GASS::Float value)
{
	if (value < 0)
	{
		return value + 2 * GASS_PI;
	}
	return value;
}

TEST_CASE("Test Mat4")
{
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

	SECTION("Test MakeZero")
	{
		GASS::Mat4 mat;
		mat.MakeZero();
		for (int i = 0; i < 16; i++)
			REQUIRE(mat.E16[i] == 0);
	}

	SECTION("Test SetScale")
	{
		GASS::Mat4 mat;
		mat.SetScale(GASS::Vec3(1, 2, 3));
		REQUIRE(mat.E4x4[0][0] == 1);
		REQUIRE(mat.E4x4[1][1] == 2);
		REQUIRE(mat.E4x4[2][2] == 3);
	}


	SECTION("Test GetXAxis")
	{
		GASS::Mat4 mat = GASS::Mat4::CreateIdentity();
		REQUIRE(mat.GetXAxis() == GASS::Vec3::m_UnitX);
	}

	SECTION("Test SetXAxis")
	{
		GASS::Mat4 mat;
		mat.SetXAxis(GASS::Vec3::m_UnitX);
		REQUIRE(mat.GetXAxis() == GASS::Vec3::m_UnitX);
	}

	SECTION("Test GetYAxis")
	{
		GASS::Mat4 mat = GASS::Mat4::CreateIdentity();
		REQUIRE(mat.GetYAxis() == GASS::Vec3::m_UnitY);
	}

	SECTION("Test SetYAxis")
	{
		GASS::Mat4 mat;
		mat.SetYAxis(GASS::Vec3::m_UnitY);
		REQUIRE(mat.GetYAxis() == GASS::Vec3::m_UnitY);
	}

	SECTION("Test GetZAxis")
	{
		GASS::Mat4 mat = GASS::Mat4::CreateIdentity();
		REQUIRE(mat.GetZAxis() == GASS::Vec3::m_UnitZ);
	}

	SECTION("Test SetZAxis")
	{
		GASS::Mat4 mat;
		mat.SetZAxis(GASS::Vec3::m_UnitZ);
		REQUIRE(mat.GetZAxis() == GASS::Vec3::m_UnitZ);
	}

	SECTION("Test MakeRotateX")
	{
		GASS::Mat4 mat;
		mat.MakeRotationX(GASS::Math::Deg2Rad(90.0));
		REQUIRE(mat.GetXAxis().Equal(GASS::Vec3::m_UnitX, 1.0e-10));
		REQUIRE(mat.GetYAxis().Equal(GASS::Vec3::m_UnitZ, 1.0e-10));
		REQUIRE(mat.GetZAxis().Equal(-GASS::Vec3::m_UnitY, 1.0e-10));
	}

	SECTION("Test MakeRotateY")
	{
		GASS::Mat4 mat;
		mat.MakeRotationY(GASS::Math::Deg2Rad(90.0));
		REQUIRE(mat.GetXAxis().Equal(-GASS::Vec3::m_UnitZ, 1.0e-10));
		REQUIRE(mat.GetYAxis().Equal(GASS::Vec3::m_UnitY, 1.0e-10));
		REQUIRE(mat.GetZAxis().Equal(GASS::Vec3::m_UnitX, 1.0e-10));
	}

	SECTION("Test MakeRotateZ")
	{
		GASS::Mat4 mat;
		GASS::Float heading = GASS::Math::Deg2Rad(90.0);
		mat.MakeRotationZ(heading);
		REQUIRE(mat.GetXAxis().Equal(GASS::Vec3::m_UnitY, 1.0e-10));
		REQUIRE(mat.GetYAxis().Equal(-GASS::Vec3::m_UnitX, 1.0e-10));
		REQUIRE(mat.GetZAxis().Equal(GASS::Vec3::m_UnitZ, 1.0e-10));
	}

	SECTION("Test MakeIdentity")
	{
		GASS::Mat4 mat;
		mat.MakeIdentity();

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if (i != j)
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
				GASS::Float value = j * 4 + i;
				REQUIRE(tmat[i][j] == value);
			}
		}
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

	SECTION("Test MakeTranslation")
	{
		GASS::Mat4 mat;;
		mat.MakeTranslation(GASS::Vec3(1, 2, 3));
		GASS::Mat4 expected(1, 0, 0, 1, 0, 1, 0, 2, 0, 0, 1, 3, 0, 0, 0, 1);
		REQUIRE(mat == expected);
	}

	SECTION("Test MakeTransformationSRT")
	{
		GASS::Mat4 mat;
		//Test zero translation and rotation
		GASS::Quaternion rot = GASS::Quaternion::CreateFromEulerYXZ(GASS::Vec3(0, 0, 0));
		mat = GASS::Mat4::CreateTransformationSRT(GASS::Vec3(1, 1, 1), rot, GASS::Vec3(0, 0, 0));

		//Expect identity
		REQUIRE(mat.Equal(GASS::Mat4::CreateIdentity(), 1.0e-10));

		//Test scale
		mat = GASS::Mat4::CreateTransformationSRT(GASS::Vec3(2, 3, 4), rot, GASS::Vec3(0, 0, 0));

		GASS::Mat4 scale_exp = GASS::Mat4::CreateIdentity();
		scale_exp.SetScale(GASS::Vec3(2, 3, 4));
		REQUIRE(mat == scale_exp);

		//Test translation, scale and rotaion should not effect translation when usingg S*R*T
		GASS::Vec3 translation(33, 34, 35);
		rot = GASS::Quaternion::CreateFromEulerYXZ(GASS::Vec3(0, 0, GASS_HALF_PI));
		mat = GASS::Mat4::CreateTransformationSRT(GASS::Vec3(1, 1, 1), rot , translation);

		//Check translation
		REQUIRE(mat.GetTranslation() == translation);

		//Check that we get expected rotation matrix for 90deg rotation around Z-axis
		REQUIRE(mat.GetXAxis().Equal(GASS::Vec3::m_UnitY, 1.0e-10));
		REQUIRE(mat.GetYAxis().Equal(-GASS::Vec3::m_UnitX, 1.0e-10));
		REQUIRE(mat.GetZAxis().Equal(GASS::Vec3::m_UnitZ, 1.0e-10));

		//Test against known result matrix
		rot = GASS::Quaternion::CreateFromEulerYXZ(GASS::Vec3(0, GASS_HALF_PI, GASS_HALF_PI));
		mat = GASS::Mat4::CreateTransformationSRT(GASS::Vec3(2, 3, 4), rot, GASS::Vec3(33, 34, 35));
		GASS::Mat4 expected_mat(0, 0, 4, 33, 2, 0, 0, 34, 0, 3, 0, 35, 0, 0, 0, 1);
		REQUIRE(mat.Equal(expected_mat, 1.0e-10));
	}
	
	SECTION("Test ToEulerAnglesYXZ")
	{
		//first test single axis rotation
		GASS::Mat4 mat;
		{
			//PI rotation
			GASS::Float expected_rot = GASS_HALF_PI;
			GASS::Vec3 euler;

			//Y
			mat.MakeRotationY(expected_rot);
			bool unique = mat.ToEulerAnglesYXZ(euler);
			euler.y = ToPosAngle(euler.y);
			REQUIRE(euler.y == Approx(expected_rot));

			//X
			mat.MakeRotationX(expected_rot);
			unique = mat.ToEulerAnglesYXZ(euler);
			euler.x = ToPosAngle(euler.x);
			REQUIRE(euler.x == Approx(expected_rot));

			//Z
			mat.MakeRotationZ(expected_rot);
			unique = mat.ToEulerAnglesYXZ(euler);
			euler.z = ToPosAngle(euler.z);
			REQUIRE(euler.z == Approx(expected_rot));
		}
		{
			//0 rotation
			GASS::Float expected_rot = 0;
			GASS::Vec3 euler;

			//Y
			mat.MakeRotationY(expected_rot);
			bool unique = mat.ToEulerAnglesYXZ(euler);
			euler.y = ToPosAngle(euler.y);
			REQUIRE(euler.y == Approx(expected_rot));

			//X
			mat.MakeRotationX(expected_rot);
			unique = mat.ToEulerAnglesYXZ(euler);
			euler.x = ToPosAngle(euler.x);
			REQUIRE(euler.x == Approx(expected_rot));

			//Z
			mat.MakeRotationZ(expected_rot);
			unique = mat.ToEulerAnglesYXZ(euler);
			euler.z = ToPosAngle(euler.z);
			REQUIRE(euler.z == Approx(expected_rot));
		}

		{

			GASS::Float expected_rot = GASS_PI*0.4;
			GASS::Vec3 euler;

			//Y
			mat.MakeRotationY(expected_rot);
			bool unique = mat.ToEulerAnglesYXZ(euler);
			euler.y = ToPosAngle(euler.y);
			REQUIRE(euler.y == Approx(expected_rot));

			//X
			mat.MakeRotationX(expected_rot);
			unique = mat.ToEulerAnglesYXZ(euler);
			euler.x = ToPosAngle(euler.x);
			REQUIRE(euler.x == Approx(expected_rot));

			//Z
			mat.MakeRotationZ(expected_rot);
			unique = mat.ToEulerAnglesYXZ(euler);
			euler.z = ToPosAngle(euler.z);
			REQUIRE(euler.z == Approx(expected_rot));
		}

		{
			GASS::Float expected_rot = -GASS_HALF_PI*0.4;
			GASS::Vec3 euler;

			//Y
			mat.MakeRotationY(expected_rot);
			bool unique = mat.ToEulerAnglesYXZ(euler);
			REQUIRE(euler.y == Approx(expected_rot));

			//X
			mat.MakeRotationX(expected_rot);
			unique = mat.ToEulerAnglesYXZ(euler);
			REQUIRE(euler.x == Approx(expected_rot));

			//Z
			mat.MakeRotationZ(expected_rot);
			unique = mat.ToEulerAnglesYXZ(euler);
			REQUIRE(euler.z == Approx(expected_rot));
		}

		//Test combined rotation (within +-PI_HALF
		{
			GASS::Vec3 expected_rot(GASS_HALF_PI*0.2,
				-GASS_HALF_PI*0.6,
				-GASS_HALF_PI*0.7);
			GASS::Vec3 euler;

			//Y
			mat.MakeRotationYXZ(expected_rot);
			bool unique = mat.ToEulerAnglesYXZ(euler);
			REQUIRE(unique == true);
			REQUIRE(euler.Equal(expected_rot));
		}

		//Test combined free rotation
		{
			GASS::Vec3 expected_rot(GASS_HALF_PI*2.1,
				-GASS_HALF_PI * 2,
				-GASS_HALF_PI*1.2);
			GASS::Vec3 euler;

			//Y
			mat.MakeRotationYXZ(expected_rot);
			bool unique = mat.ToEulerAnglesYXZ(euler);
			REQUIRE(unique == true);
			//Test that we get same rotation matrix...euler angles will differ when we are outside +- HALF_PI			
			GASS::Mat4 res_mat;
			res_mat.MakeRotationYXZ(euler);
			REQUIRE(res_mat.Equal(mat, 0.00001));
		}
	}


	SECTION("Test operator * Vec3")
	{
		GASS::Mat4 mat1 = GASS::Mat4::CreateTranslation(GASS::Vec3(1, 1, 1));
		GASS::Vec3 point(0, 0, 0);
		point = mat1*point;
		REQUIRE(point == GASS::Vec3(1, 1, 1));


		//rotate around y-axis (heading)
		GASS::Mat4 mat2;
		mat2.MakeRotationY(GASS_HALF_PI);
		point.Set(1, 0, 0);
		point = mat2*point;

		REQUIRE(point.Equal(GASS::Vec3(0, 0, -1), 1.0e-10));

		//scale, rotate and translate
		GASS::Quaternion rot = GASS::Quaternion::CreateFromEulerYXZ(GASS::Vec3(0, 0, GASS_HALF_PI));
		GASS::Mat4 mat3 = GASS::Mat4::CreateTransformationSRT(GASS::Vec3(2, 2, 2), rot, GASS::Vec3(2, 0, 0));
		point.Set(1, 0, 0);
		point = mat3*point;
		REQUIRE(point.Equal(GASS::Vec3(2, 2, 0), 1.0e-10));
	}



	SECTION("Test operator * Vec4")
	{
		GASS::Mat4 mat1 = GASS::Mat4::CreateTranslation(GASS::Vec3(1, 1, 1));
		GASS::Vec4 point(0, 0, 0, 1);
		point = mat1*point;
		REQUIRE(point == GASS::Vec4(1, 1, 1, 1));

		//rotate around y-axis (heading)
		GASS::Quaternion rot = GASS::Quaternion::CreateFromEulerYXZ(GASS::Vec3(0, GASS_HALF_PI, 0));
		GASS::Mat4 mat2 = GASS::Mat4::CreateTransformationSRT(GASS::Vec3(1, 1, 1), rot, GASS::Vec3(0, 0, 0));
		point.Set(1, 0, 0, 1);
		point = mat2*point;

		REQUIRE(point.x == Approx(0));
		REQUIRE(point.y == Approx(0));
		REQUIRE(point.z == Approx(-1));
		REQUIRE(point.w == Approx(1));

		//translate, rotate and scale
		rot = GASS::Quaternion::CreateFromEulerYXZ(GASS::Vec3(0, 0, GASS_HALF_PI));
		GASS::Mat4 mat3 = GASS::Mat4::CreateTransformationSRT(GASS::Vec3(2, 2, 2), rot, GASS::Vec3(2, 0, 0));
		point.Set(1, 0, 0, 1);
		point = mat3*point;

		REQUIRE(point.x == Approx(2));
		REQUIRE(point.y == Approx(2));
		REQUIRE(point.z == Approx(0));
		REQUIRE(point.w == Approx(1));
	}


	SECTION("Test friend operator * Vec4")
	{
		GASS::Quaternion rot = GASS::Quaternion::CreateFromEulerYXZ(GASS::Vec3(0, 0, 0));
		GASS::Mat4 mat1 = GASS::Mat4::CreateTransformationSRT(GASS::Vec3(1, 1, 1), rot, GASS::Vec3(1, 1, 1));
		GASS::Vec4 point(0, 0, 0, 1);
		point = point*mat1;
		REQUIRE(point == GASS::Vec4(0, 0, 0, 1));

		//rotate around y-axis (heading)
		rot = GASS::Quaternion::CreateFromEulerYXZ(GASS::Vec3(0, GASS_HALF_PI, 0));
		GASS::Mat4 mat2 = GASS::Mat4::CreateTransformationSRT(GASS::Vec3(1, 1, 1), rot, GASS::Vec3(0, 0, 0));
		point.Set(1, 0, 0, 1);
		point = point*mat2;

		REQUIRE(point.x == Approx(0));
		REQUIRE(point.y == Approx(0));
		REQUIRE(point.z == Approx(1));
		REQUIRE(point.w == Approx(1));

		//translate, rotate and scale
		rot = GASS::Quaternion::CreateFromEulerYXZ(GASS::Vec3(0, 0, GASS_HALF_PI));
		GASS::Mat4 mat3 = GASS::Mat4::CreateTransformationSRT(GASS::Vec3(2, 2, 2), rot, GASS::Vec3(2, 0, 0));
		point.Set(1, 0, 0, 1);
		point = point*mat3;

		REQUIRE(point.x == Approx(0));
		REQUIRE(point.y == Approx(-2));
		REQUIRE(point.z == Approx(0));
		REQUIRE(point.w == Approx(3));
	}


	SECTION("Test GetInvert")
	{
		GASS::Mat4 mat;
		//Test against known result matrix
		GASS::Quaternion rot = GASS::Quaternion::CreateFromEulerYXZ(GASS::Vec3(0, GASS_HALF_PI, GASS_HALF_PI));
		mat.MakeTransformationSRT(GASS::Vec3(33, 34, 35), rot, GASS::Vec3(2, 3, 4));
		GASS::Mat4 mat_inv = mat.GetInvert();

		//concatenate 
		GASS::Mat4 identity_mat = mat*mat_inv;

		//expect identity
		GASS::Mat4 expected = GASS::Mat4::CreateIdentity();
		REQUIRE(identity_mat.Equal(expected, 1.0e-10));
	}

	SECTION("Test Compare Mat4 rotations with Quaternion rotations")
	{
		GASS::Vec3 euler_rot(0, GASS_HALF_PI, GASS_HALF_PI);
		GASS::Mat4 qmat(GASS::Quaternion::CreateFromEulerYXZ(euler_rot));
		GASS::Mat4 mat = GASS::Mat4::CreateRotationYXZ(euler_rot);
		REQUIRE(mat.Equal(qmat, 1.0e-10));
		
		euler_rot = GASS::Vec3(GASS_HALF_PI*0.1, GASS_HALF_PI*0.2, GASS_HALF_PI*0.3);
		qmat = GASS::Mat4(GASS::Quaternion::CreateFromEulerYXZ(euler_rot));
		mat = GASS::Mat4::CreateRotationYXZ(euler_rot);
		REQUIRE(mat.Equal(qmat, 1.0e-10));

		euler_rot = GASS::Vec3(GASS_HALF_PI*2.1, GASS_HALF_PI*2.2, GASS_HALF_PI*2.3);
		qmat = GASS::Mat4(GASS::Quaternion::CreateFromEulerYXZ(euler_rot));
		mat = GASS::Mat4::CreateRotationYXZ(euler_rot);
		REQUIRE(mat.Equal(qmat, 1.0e-10));

		euler_rot = GASS::Vec3(-GASS_HALF_PI*2.1, -GASS_HALF_PI*2.2, -GASS_HALF_PI*2.3);
		qmat = GASS::Mat4(GASS::Quaternion::CreateFromEulerYXZ(euler_rot));
		mat = GASS::Mat4::CreateRotationYXZ(euler_rot);
		REQUIRE(mat.Equal(qmat, 1.0e-10));
	}

	
}