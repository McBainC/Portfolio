#include "Vector3.h"
#include <math.h>


Vector3 Vector3::UP( 0.0f, 1.0f, 0.0f );
Vector3 Vector3::FORWARD( 0.0f, 0.0f, 1.0f );
Vector3 Vector3::RIGHT( 1.0f, 0.0f, 0.0f );


/*
=============================
Default Constructor - all 0's
=============================
*/
Vector3::Vector3( void ) : 
x( 0.0f ), y( 0.0f ), z( 0.0f ) {
}


/*
===========
Constructor
===========
*/
Vector3::Vector3( float x_, float y_, float z_ ) {
	x = x_;
	y = y_;
	z = z_;
}


/*
===================
Destructor
Nothing to clean up
===================
*/
Vector3::~Vector3( void ) {
}


/*
====================================================
sumOfVectors
Takes in two vectors and returns the sum as a vector
====================================================
*/
Vector3 Vector3::sumOfVectors( const Vector3 & v1, const Vector3 & v2 ) {
	float sumX = v1.x + v2.x;
	float sumY = v1.y + v2.y;
	float sumZ = v1.z + v2.z;

	return Vector3( sumX, sumY, sumZ );
}


/*
==================================================
crossProduct
Takes in two vectors and returns the cross product
==================================================
*/
Vector3 Vector3::crossProduct( const Vector3 & v1, const Vector3 & v2 ) {
	float crossX = ( ( v1.y * v2.z ) - ( v1.z * v2.y ) );
	float crossY = -( ( v1.x * v2.z ) - ( v1.z * v2.x ) );
	float crossZ = ( ( v1.x * v2.y ) - ( v1.y * v2.x ) );

	return Vector3( crossX, crossY, crossZ );
}


/*
========================================================
normalise
Takes in a single vector and returns a normalized vector
========================================================
*/
Vector3 Vector3::normalise( const Vector3 v1 ) {
	float length = sqrt( ( v1.x * v1.x ) + ( v1.y * v1.y ) + ( v1.z * v1.z ) );

	float normalX = v1.x / length;
	float normalY = v1.y / length;
	float normalZ = v1.z / length;

	return Vector3( normalX, normalY, normalZ );
}


/*
=====================
threeDRotation
Rotate a vector in 3D
=====================
*/
Vector3 threeDRotation( const Vector3 & v1, float rotation ) {
	float cos0 = cosf( rotation );
	float sin0 = sinf( rotation );
	float oneMinusCos0 = ( 1 - cos0 );

	/*

	r1c1 r1c2 r1c3
	r2c1 r2c2 r2c3
	r3c1 r3c2 r3c3

	*/

	float r1c1 = ( ( ( v1.x * v1.x ) * ( oneMinusCos0 ) ) + cos0 );
	float r1c2 = ( ( ( v1.x * v1.y ) * ( oneMinusCos0 ) ) + ( v1.z * sin0 ) );
	float r1c3 = ( ( ( v1.x * v1.z ) * ( oneMinusCos0 ) ) - ( v1.y * sin0 ) );

	float r2c1 = ( ( ( v1.x * v1.y ) * ( oneMinusCos0 ) ) - ( v1.z * sin0 ) );
	float r2c2 = ( ( ( v1.y * v1.y ) * ( oneMinusCos0 ) ) + cos0 );
	float r2c3 = ( ( ( v1.y * v1.z ) * ( oneMinusCos0 ) ) + ( v1.x * sin0 ) );

	float r3c1 = ( ( ( v1.x * v1.z ) * ( oneMinusCos0 ) ) + ( v1.y * sin0 ) );
	float r3c2 = ( ( ( v1.y * v1.z ) * ( oneMinusCos0 ) ) - ( v1.x * sin0 ) );
	float r3c3 = ( ( ( v1.z * v1.z ) * ( oneMinusCos0 ) ) + cos0 );

	float threeDx = ( ( v1.x * r1c1 ) + ( v1.y * r2c1 ) + ( v1.z * r3c1 ) );
	float threeDy = ( ( v1.x * r1c2 ) + ( v1.y * r2c2 ) + ( v1.z * r3c2 ) );
	float threeDz = ( ( v1.x * r1c3 ) + ( v1.y * r2c3 ) + ( v1.z * r3c3 ) );

	return Vector3( threeDx, threeDy, threeDz );
}


/* PS2 Framework */
Vector3 & Vector3::operator += ( const Vector3 &rhs ) {
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;

	return *this;
}


Vector3 & Vector3::operator -= ( const Vector3 &rhs ) {
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;

	return *this;
}


/*
float Vector4::Dot4( const Vector4 &rhs ) const {
	return ( x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w );
}
*/


float Vector3::Dot3( const Vector3 &rhs ) const {
	return ( x * rhs.x + y * rhs.y + z * rhs.z );
}


Vector3 Vector3::Cross( const Vector3 &rhs ) const {
	return Vector3( y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x ); 
}


Vector3 & Vector3::operator *= ( const float s ) {
	x *= s;
	y *= s;
	z *= s;

	return *this;
}


Vector3 & Vector3::operator /= ( const float s ) {
	x /= s;
	y /= s;
	z /= s;

	return *this;
}


float Vector3::Length() const {
	return sqrt( x * x + y * y + z * z );
}


float Vector3::LengthSqr() const {
	return( x * x + y * y + z * z) ;
}


bool Vector3::operator == ( const Vector3 & rhs ) const {
	return( ( x == rhs.x ) && ( y == rhs.y ) && ( z == rhs.z ) );
}


Vector3 Vector3::Normalise() {
	return ( *this / this->Length() );
}


void Vector3::NormaliseSelf() {
	*this /= this->Length();
}