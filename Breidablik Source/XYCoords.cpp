#include "XYCoords.h"


/*
===================
Defualt Constructor
Initialises to 0,0
===================
*/
XYCoords::XYCoords() :
 mX( 0 ), mY( 0 ) {
}


/*
===============================
Constructor
Initialises to specified coords
===============================
*/
XYCoords::XYCoords( int x, int y ) :
 mX( x ), mY( y ) {
}


/*
===================
Destructor
Nothing to clean up
===================
*/
XYCoords::~XYCoords() {
}


/*
==============
Equal overload
==============
*/
bool XYCoords::operator == ( const XYCoords &otherXYCoords ) const {
	if( ( mX == otherXYCoords.mX ) && ( mY == otherXYCoords.mY ) ) {
		return true;
	}
	return false;
}


/*
==================
Not equal overload
==================
*/
bool XYCoords::operator != ( const XYCoords &otherXYCoords ) const {
	if( ( mX != otherXYCoords.mX ) || ( mY != otherXYCoords.mY ) ) {
		return true;
	}
	return false;
}	