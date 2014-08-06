#pragma once


#include "primitives.h"


/*
==========================================================
Cone
Inherits from Primitives
Creates and renders a procedurally generated top-less cone
==========================================================
*/
class Cone : Primitives {
public:
	Cone( void );
	~Cone( void );

	void Create( int resolution, float radius );
	void Draw( void ) const;
};