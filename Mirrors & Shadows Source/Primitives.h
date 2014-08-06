#pragma once

#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include "Vector3.h"
#include "Vector2.h"
#include <vector>


/*
==================================================
Primitives
Virtual Base Class to build Proceedural Primitives
==================================================
*/
class Primitives {
public:
	Primitives( void );
	virtual ~Primitives( void );

	virtual void Draw( void ) const = 0;

protected:
	int NUM_OF_VERTS;                    // counter for number of vertices

	std::vector< Vector3 > PRIM_VERTS;   // vector3's for vectices
	std::vector< Vector3 > PRIM_NORMALS; // vector3's for normals
	std::vector< Vector2 > PRIM_TCOORDS; // vector2's for uv's
};