#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>


#include "Cube.h"


/* Offset for Skybox UV's */
float texelWidth = 1.0f / 4096.0f;


/* Vertices */
float CUBE_VERTS[] = { 1.0, -1.0,  1.0, // Front		
					   1.0,  1.0,  1.0,			
					  -1.0,  1.0,  1.0,			
					  -1.0, -1.0,  1.0,			
					   1.0, -1.0, -1.0, // Right		
					   1.0,  1.0, -1.0,
					   1.0,  1.0,  1.0,		
					   1.0, -1.0,  1.0,			
					  -1.0, -1.0, -1.0, // Back			
					  -1.0,  1.0, -1.0,			
					   1.0,  1.0, -1.0,			
					   1.0, -1.0, -1.0,			
					  -1.0, -1.0,  1.0, // Left			
					  -1.0,  1.0,  1.0,
					  -1.0,  1.0, -1.0,		
					  -1.0, -1.0, -1.0,			
					   1.0,  1.0,  1.0, // Top			
					   1.0,  1.0, -1.0,
					  -1.0,  1.0, -1.0,
					  -1.0,  1.0,  1.0,			
					   1.0, -1.0, -1.0, // Botton	
					   1.0, -1.0,  1.0,
					  -1.0, -1.0,  1.0,
					  -1.0, -1.0, -1.0 };		


/* Normals */
float CUBE_NORMALS[] = { 0.0,  0.0,  1.0, // Front		
						 0.0,  0.0,  1.0,			
 						 0.0,  0.0,  1.0,			
						 0.0,  0.0,  1.0,			
						 1.0,  0.0,  0.0, // Right			
					     1.0,  0.0,  0.0,			
						 1.0,  0.0,  0.0,
						 1.0,  0.0,  0.0,			
						 0.0,  0.0, -1.0, // Back			
 						 0.0,  0.0, -1.0,			
						 0.0,  0.0, -1.0,			
						 0.0,  0.0, -1.0,			
						-1.0,  0.0,  0.0, // Left			
						-1.0,  0.0,  0.0,			
						-1.0,  0.0,  0.0,
						-1.0,  0.0,  0.0,			
					     0.0,  1.0,  0.0, // Top			
 						 0.0,  1.0,  0.0,			
						 0.0,  1.0,  0.0,			
						 0.0,  1.0,  0.0,			
						 0.0, -1.0,  0.0, // Bottom			
						 0.0, -1.0,  0.0,			
						 0.0, -1.0,  0.0,
						 0.0, -1.0,  0.0 };					


/* UV's */
float CUBE_T_COORDS[]= { 1.0, 0.0, 			
 						 1.0, 1.0, 			
						 0.0, 1.0, 		
						 0.0, 0.0, 		
						 1.0, 0.0,		
						 1.0, 1.0,
						 0.0, 1.0, 		
						 0.0, 0.0, 		
 						 1.0, 0.0, 			
						 1.0, 1.0, 		
						 0.0, 1.0, 		
						 0.0, 0.0, 	
						 1.0, 0.0,		
						 1.0, 1.0,
						 0.0, 1.0, 		
						 0.0, 0.0, 			
 						 1.0, 0.0, 			
						 1.0, 1.0, 		
						 0.0, 1.0, 		
						 0.0, 0.0, 	
						 1.0, 0.0,		
						 1.0, 1.0,
						 0.0, 1.0,
                         0.0, 0.0 };	


/* SkyBox UV's */
float SKYBOX_T_COORDS[]= { 0.5f - texelWidth, 0.5f + texelWidth, 			
 						   0.5f - texelWidth, 0.75f - texelWidth, 			
						   0.25f + texelWidth, 0.75f - texelWidth, 		
						   0.25f + texelWidth, 0.5f + texelWidth, 		
						   0.75f - texelWidth, 0.5f + texelWidth,		
					  	   0.75f - texelWidth, 0.75f - texelWidth,
						   0.5f + texelWidth, 0.75f - texelWidth, 		
						   0.5f + texelWidth, 0.5f + texelWidth, 		
 						   1.0f - texelWidth, 0.5f + texelWidth, 			
						   1.0f - texelWidth, 0.75f - texelWidth, 		
						   0.75f + texelWidth, 0.75f - texelWidth, 		
						   0.75f + texelWidth, 0.5f + texelWidth, 	
						   0.25f - texelWidth, 0.5f + texelWidth,		
						   0.25f - texelWidth, 0.75f - texelWidth,
						   0.0f + texelWidth, 0.75f - texelWidth, 		
						   0.0f + texelWidth, 0.5f + texelWidth, 			
 						   0.5f - texelWidth, 0.75f + texelWidth, 			
						   0.5f - texelWidth, 1.0f - texelWidth, 		
						   0.25f + texelWidth, 1.0f - texelWidth, 		
						   0.25f + texelWidth, 0.75f + texelWidth, 	
						   0.5f - texelWidth, 0.25f + texelWidth,		
						   0.5f - texelWidth, 0.5f - texelWidth,
						   0.25f + texelWidth, 0.5f - texelWidth,
                           0.25f + texelWidth, 0.25f + texelWidth };	


/*
============================
Draw_CUBE
Draws a procedural unit cube
============================
*/
void Draw_CUBE( void ) {
	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_NORMAL_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );

	glVertexPointer( 3, GL_FLOAT, 0, CUBE_VERTS );
	glNormalPointer( GL_FLOAT, 0, CUBE_NORMALS );
	glTexCoordPointer( 2, GL_FLOAT, 0, CUBE_T_COORDS );

	glDrawArrays( GL_QUADS, 0, 24 );

	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_NORMAL_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
}

/*
=======================================================================
Draw_Skybox
Draws a Skybox - uses its own TexCoords so a single texture can be used
=======================================================================
*/
void Draw_SKYBOX( void ) {
	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_NORMAL_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );

	glVertexPointer( 3, GL_FLOAT, 0, CUBE_VERTS );
	glNormalPointer( GL_FLOAT, 0, CUBE_NORMALS );
	glTexCoordPointer( 2, GL_FLOAT, 0, SKYBOX_T_COORDS );

	glDrawArrays( GL_QUADS, 0, 24 );

	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_NORMAL_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
}