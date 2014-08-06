#ifndef _XYCOORDS_H_
#define _XYCOORDS_H_


/*
=====================================================================
XYCoords
Holds ints for array coords and required comparion operator overloads
=====================================================================
*/
class XYCoords {
public:
	int mX, mY;
	
	XYCoords::XYCoords();
	XYCoords::XYCoords( int x, int y );
	XYCoords::~XYCoords();
	
	bool operator == ( const XYCoords &otherXYCoords ) const;
	bool operator != ( const XYCoords &otherXYCoords ) const;
};

#endif