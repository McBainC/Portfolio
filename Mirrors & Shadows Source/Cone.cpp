#include "Cone.h"


/*
===========
Constructor
===========
*/
Cone::Cone( void ) {
	Create( 50, 1.0f );
}


/*
===================
Destructor
Nothing to clean up
===================
*/
Cone::~Cone( void ) {
}


/*
=====================================
Create
Creates a procedurally generated cone
=====================================
*/
void Cone::Create( int resolution, float radius ) {
	/* Setup Variables */
	const float PI = 3.14159f;         // approx of PI
	float delta = 2 * PI / resolution; // angle between vertices
	float angle = 0.0f;                // current angle
	
	/* Set Vertices */
	PRIM_VERTS.push_back( Vector3() );       // Vector3 is (0.0f, 0.0f, 0.0f) by default 
	for (int i = 0 ; i <= resolution ; i++ ) {
		PRIM_VERTS.push_back( Vector3( cos( angle ), sin( angle ), 2.0f ) );
		angle += delta ;
	}
	NUM_OF_VERTS = int( PRIM_VERTS.size() ); // set number of vertices

	/* Set UV's */
	PRIM_TCOORDS.push_back( Vector2( 0.5f, 0.5f ) );
	for( int i = 1; i < NUM_OF_VERTS; i++ ) {
		PRIM_TCOORDS.push_back( Vector2( ( ( PRIM_VERTS[ i ].x / ( radius * 2.0f ) ) + 0.5f ) , ( ( PRIM_VERTS[ i ].y / 2.0f ) + 0.5f ) ) );
	}
}


/*
==============================================
Draw
Renders the cone in OpenGl using vertex arrays
==============================================
*/
void Cone::Draw( void ) const {
	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_NORMAL_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );

	glVertexPointer( 3, GL_FLOAT, 0, &PRIM_VERTS[ 0 ] );
	glNormalPointer( GL_FLOAT, 0, &PRIM_VERTS[ 0 ] );
	glTexCoordPointer( 2, GL_FLOAT, 0, &PRIM_TCOORDS[ 0 ] ); 
	glDrawArrays( GL_TRIANGLE_FAN, 0, NUM_OF_VERTS );

	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_NORMAL_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
}