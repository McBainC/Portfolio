#pragma once


/*
==========================================================
Vector3
A Vector class on X/Y/Z planes with Vector maths functions
==========================================================
*/
class Vector3 {
public:

	Vector3( void );
	~Vector3( void );

	float x, y, z;

	Vector3( float x_, float y_, float z_ );

	static Vector3 sumOfVectors( const Vector3 & v1, const Vector3 & v2 );
	static Vector3 crossProduct( const Vector3 & v1, const Vector3 & v2 );
	static Vector3 normalise( const Vector3 v1 );

	static Vector3 crossProduct( const Vector3 & v1 );

	static Vector3 threeDRotation( const Vector3 & v1, float rotation );

	/* PS2 Framework */

	// Operations with other vectors
	Vector3 & operator += ( const Vector3 & rhs );
	Vector3 & operator -= ( const Vector3 & rhs );

	// Special arithmetic
	float Dot3( const Vector3 & rhs ) const;
	//float Dot4( const Vector3 & rhs ) const;
	Vector3 Cross( const Vector3 & rhs ) const;

	Vector3 & operator *= ( const float s );
	Vector3 & operator /= ( const float s );

	bool operator == ( const Vector3 & rhs ) const;

	// Miscellaneous
	float Length() const;
	float LengthSqr() const;
	Vector3 Normalise();
	void NormaliseSelf();

	static Vector3 UP;
	static Vector3 FORWARD;
	static Vector3 RIGHT;
};


/*
==========================
Vector3 operator overloads
==========================
*/


inline Vector3 operator + ( const Vector3 &v1,
						    const Vector3 &v2 ) {

	return Vector3( v1.x + v2.x, v1.y + v2.y, v1.z + v2.z );
}


inline Vector3 operator - ( const Vector3 &v1,
						    const Vector3 &v2 ) {

	return Vector3( v1.x - v2.x, v1.y - v2.y, v1.z - v2.z );
}


inline Vector3 operator - ( const Vector3 &v1 ) {

	return Vector3( -v1.x, -v1.y, -v1.z );
}


inline Vector3 operator * ( const Vector3 &v,
						    const float &s ) {

	return Vector3( v.x * s, v.y * s, v.z * s );
}


inline Vector3 operator * ( const float & s,
						    const Vector3 &v ) {

	return Vector3( v.x * s, v.y * s, v.z * s );
}


inline Vector3 operator / ( const Vector3 &v,
						    const float & s ) {

	return Vector3( v.x / s, v.y / s, v.z / s );
}


inline Vector3 CrossProduct( const Vector3 &v1,
							 const Vector3 &v2 ) {

	return Vector3( v1.y * v2.z - v1.z * v2.y,
				    v1.z * v2.x - v1.x * v2.z,
				    v1.x * v2.y - v1.y * v2.x );
}


/*
inline float DotProduct4( const Vector3 &v1,
						  const Vector3 &v2 ) {

	return ( v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w );
}
*/


inline float DotProduct3( const Vector3 &v1,
						  const Vector3 &v2 ) {

	return( v1.x * v2.x + v1.y * v2.y + v1.z * v2.z );
}


inline Vector3 Normalise( const Vector3 &v ) {

	return v / v.Length();
}