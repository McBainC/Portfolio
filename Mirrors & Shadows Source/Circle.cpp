#include "Circle.h"
#include <math.h>


/*
===========
Constructor
===========
*/
Circle::Circle( void ) {
	Create( 50, 1.0f );
}


/*
===================
Destructor
Nothing to clean up
===================
*/
Circle::~Circle( void ) {
}


/*
===============================
Create
Procedurally generates a circle
===============================
*/
void Circle::Create( int numOfTriangles, float radius ) {
	float theta = ( 2.0f * 3.1415f ) / float( numOfTriangles ); // angle between vertices
	float angle = 0;                                            // current angle

	NUM_OF_VERTS = numOfTriangles + 2;                          // number of triangles + center + returning to first point (triangle fan)

	/* Set Vertices */

	/* First 3 are center vertex (0, 0, 0) */
	PRIM_VERTS.push_back( Vector3() );                          // Vector3 is (0.0f, 0.0f, 0.0f) by default 

	/* Sets each triangle fan point - except the last */
	for( int i = 1; i < NUM_OF_VERTS; i++, angle += theta ) {
		PRIM_VERTS.push_back( Vector3( ( radius * cos( angle ) ), ( radius * sin( angle ) ), 0.0f ) );
	}

	/* Last triangle fan point is equal to FIRST triangle fan point */
	PRIM_VERTS[ NUM_OF_VERTS - 1 ] = PRIM_VERTS[ 1 ];

	/* Set Normals */
	for( int i = 0; i < NUM_OF_VERTS; i++ ) {
		PRIM_NORMALS.push_back( Vector3( 0.0f, 1.0f, 0.0f ) );
	}
	
	/* Set uv's */
	PRIM_TCOORDS.push_back( Vector2( 0.5f, 0.5f ) );
	for( int i = 1; i < NUM_OF_VERTS; i++ ) {
		PRIM_TCOORDS.push_back( Vector2( ( ( PRIM_VERTS[ i ].x / ( radius * 2.0f ) ) + 0.5f ) , ( ( PRIM_VERTS[ i ].y / 2.0f ) + 0.5f ) ) );
	}
}


/*
=============================================
Draw
Renders circle in OpenGl using a vertex array
=============================================
*/
void Circle::Draw( void ) const {
	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_NORMAL_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );

	glVertexPointer( 3, GL_FLOAT, 0, &PRIM_VERTS[ 0 ] );
	glNormalPointer( GL_FLOAT, 0, &PRIM_NORMALS[ 0 ] );
	glTexCoordPointer( 2, GL_FLOAT, 0, &PRIM_TCOORDS[ 0 ] ); 
	glDrawArrays( GL_TRIANGLE_FAN, 0, NUM_OF_VERTS );

	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_NORMAL_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
}