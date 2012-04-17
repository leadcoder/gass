
#ifndef VECTOR_I
#define VECTOR_I

%{
#include "Core/Math/Vector.h"
%}

typedef double Float;
class Vec3
	{
	public:
		static Vec3 m_UnitX;
		static Vec3 m_UnitY;
		static Vec3 m_UnitZ;
		inline Vec3(Float _x,Float _y,Float _z);
		inline Vec3();
		inline void Set(Float _x,Float _y,Float _z);
		inline Vec3 operator+ (const Vec3 &v) const;
		inline Vec3& operator += ( const Vec3& v );
		inline Vec3 operator- (const Vec3 &v) const;
		inline Vec3 operator- () const;
		inline Vec3 operator* (const Float scalar) const;
		inline Vec3 operator* (const Vec3 &v) const;
		inline Vec3 operator/ (const Vec3 &v) const;
		inline 	Float* operator [] ( unsigned element );
		inline friend Vec3 operator * ( Float scalar, const Vec3& v );
		inline bool operator== (const Vec3 &v) const;
		inline bool operator!= (const Vec3 &v) const;
		inline Float SquaredLength() const;
		inline Float Length() const;
		inline void Normalize();
		void FastNormalize();
		Float FastLength() const;
		Float FastInvLength() const;
		bool InSphere(Vec3 &v, Float radius) const;
		std::string ToString(const std::string &separator);
	};


#endif