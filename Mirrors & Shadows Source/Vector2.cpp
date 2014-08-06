#include "Vector2.h"


/*
=============================
Default Constructor - all 0's
=============================
*/
Vector2::Vector2( void ) : 
x( 0.0f ), y( 0.0f ) {
}


/*
===========
Constructor
===========
*/
Vector2::Vector2( float x_, float y_ ) {
	x = x_;
	y = y_;
}


/*
===================
Destructor
Nothing to clean up
===================
*/
Vector2::~Vector2( void ) {
}