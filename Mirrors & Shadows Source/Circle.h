#pragma once


#include "primitives.h"


/*
=====================================
Circle
Inherits from Primitives
Draws a procedurally generated circle
=====================================
*/
class Circle : public Primitives {
public:
	Circle( void );
	~Circle( void );

	void Create( int numOfTriangles, float radius );
	void Draw( void ) const;
};