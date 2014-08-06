#include "primitives.h"
#include <sps2lib.h>
#include <sps2tags.h>
#include <sps2util.h>
#include <sps2regstructs.h>
#include "PS2Defines.h"
#include "ps2matrix4x4.h"
#include "sps2wrap.h"
#include "dma.h"


/*
=======
Colours
=======
*/


/*
===========
Constructor
===========
*/
Colours::Colours() :
 mR( 0x80 ), mG( 0x80 ), mB( 0x80 ), mA( 0x80 ) {
}


/*
==========
Destructor
==========
*/
Colours::~Colours() {
	// Nothing to clean up!
}


/*
=========
PS2Sprite
=========
*/


/*
===========
Constructor
===========
*/
PS2Sprite::PS2Sprite() :
 mX( 0.0f ), mY( 0.0f ), mZ( 128.0f ) { // *** whys Z 128.0f ?
}


/*
===============================
Constructor with initial coords
===============================
*/
PS2Sprite::PS2Sprite( float x, float y, float z ) :
 mX( x ), mY( y ), mZ( z ) {
}


/*
==========
Destructor
==========
*/
PS2Sprite::~PS2Sprite() {
	// Nothing to clean-up
}


/*
==============
Move To (x, y)
==============
*/ 
void PS2Sprite::MoveTo( const float x, const float y, const float z ) {
	mX = x; 
	mY = y;
	mZ = z;
}


/*
=========
Move by x
=========
*/ 
void PS2Sprite::MoveDeltaX( const float x ) {
	mX += x;
}


/*
=========
Move by y
=========
*/ 
void PS2Sprite::MoveDeltaY( const float y ) {
	mY += y;
}


/*
=========
Move by z
=========
*/
void PS2Sprite::MoveDeltaZ( const float z ) {
	mZ += z;
}


/*
=============
Rotate Sprite
=============
*/ 
void PS2Sprite::Rotate( const float a ) {
	Matrix4x4 rotationMatrix; 
	rotationMatrix.RotationZ( a ) ; // apply required rotation to matrix
	for( int i = 0; i < mNumOfVerts; i++ ) { // apply to Vertex Vectors
		pVerts[ i ] = rotationMatrix * pVerts[ i ];
	}
}


/*
=============
Uniform Scale
=============
*/
void PS2Sprite::Scale( const float s ) {
	Matrix4x4 rotationMatrix; 
	rotationMatrix.Scale( s ); // apply required scaling to matrix
	for( int i = 0; i < mNumOfVerts; i++ ) { // apply to Vertex Vectors
		pVerts[ i ] = rotationMatrix * pVerts[ i ];
	}
}


/*
==========
Scale by X
==========
*/
void PS2Sprite::ScaleX( const float s ) {
	Matrix4x4 rotationMatrix; 
	rotationMatrix.ScaleX( s ); 
	for( int i = 0; i < mNumOfVerts; i++ ) {
		pVerts[ i ] = rotationMatrix * pVerts[ i ];
	}
}


/*
==========
Scale by Y
==========
*/
void PS2Sprite::ScaleY( const float s ) {
	Matrix4x4 rotationMatrix; 
	rotationMatrix.ScaleY( s );
	for( int i = 0; i < mNumOfVerts; i++ ) {
		pVerts[ i ] = rotationMatrix * pVerts[ i ];
	}
}


/*
=================
Set Sprite Colour
=================
*/
void PS2Sprite::SetColour( uint8 r, uint8 g, uint8 b, uint8 a ) {
	for( int i = 0; i < mNumOfVerts; i++ ) {
		pVertColours[ i ].mR = r;
		pVertColours[ i ].mG = g;
		pVertColours[ i ].mB = b;
		pVertColours[ i ].mA = a;
	}
}


/*
=============================
Return an instance of Colours
=============================
*/
Colours PS2Sprite::ReturnColour( int vertex ) {
	return pVertColours[ vertex ];
}


/*
===========
Get X Coord
===========
*/ 
float PS2Sprite::GetX( void ) const {
	return mX;
}


/*
===========	
Get Y Coord
===========
*/ 
float PS2Sprite::GetY( void ) const {
	return mY;
}


/*
===========
Get Z Coord
===========
*/
float PS2Sprite::GetZ( void ) const {
	return mY;
}


/*
=======
PS2Quad
=======
*/ 


/*
========================
Default Quad Constructor
========================
*/
PS2Quad::PS2Quad() :
 mW( 32.0f ), mH( 32.0f ) {
	Initialise();
}


/*
================
Quad Constructor
================
*/
PS2Quad::PS2Quad( float x, float y, float z, float width, float height ) :
 PS2Sprite( x, y, z ), mW( width ), mH( height ) {
	mNumOfVerts = 4;                           // number of vertices              
	pVerts = new Vector4[ mNumOfVerts ];       // declare vertex array
	pVertColours = new Colours[ mNumOfVerts ]; // declare vertex colour array
	Initialise();
}


/*
==========
Destructor
==========
*/
PS2Quad::~PS2Quad() {
	delete[] pVerts;
	delete[] pVertColours;
	pVerts = NULL;
	pVertColours = NULL;
}


/*
========================
Quad Initialise Function
Sets the quads vertices
========================
*/ 
void PS2Quad::Initialise( void ) {
	pVerts[ 0 ] = Vector4( ( -mW / 2.0f ), ( -mH / 2.0f ), mZ, 1.0f ); // top-left
	pVerts[ 1 ] = Vector4( ( -mW / 2.0f), ( mH / 2.0f), mZ, 1.0f );    // bottom-left
	pVerts[ 2 ] = Vector4( ( mW / 2.0f), ( -mH / 2.0f), mZ, 1.0f );    // top-right
	pVerts[ 3 ] = Vector4( ( mW / 2.0f), ( mH / 2.0f), mZ, 1.0f );     // bottom-right
}


/*
========================
SetWidthAndHeight
Set new width and height
========================
*/
void PS2Quad::SetWidthAndHeight( float width, float height ) {
	mW = width;
	mH = height; 
	Initialise();
}


/*
=========
Get Width
=========
*/ 
float PS2Quad::GetWidth( void ) const {
	return mW;
}


/*
==========	
Get Height
==========
*/ 
float PS2Quad::GetHeight( void ) const {
	return mH;
}


/*
=================
Set Vertex Colour
=================
*/
void PS2Quad::SetVertexColour( int vertex, uint8 r, uint8 g, uint8 b ) {
	pVertColours[ vertex ].mR = r;
	pVertColours[ vertex ].mG = g;
	pVertColours[ vertex ].mB = b;
}


/*
====================
Quad Render Function
====================
*/
void PS2Quad::Render( void ) const {
	VIFDynamicDMA.StartDirect();
	/* Add the GIFTag */
	VIFDynamicDMA.Add128( GS_GIFTAG_BATCH(	mNumOfVerts,				    // NLOOP 
											1,							    // EOP
											1,							    // PRE
											GS_PRIM( PRIM_TRI_STRIP, 	    // PRIM
												PRIM_IIP_GOURAUD,           // GOURAUD SHADING
												PRIM_TME_OFF,
												PRIM_FGE_OFF, 
												PRIM_ABE_OFF, 
												PRIM_AA1_OFF, 
												PRIM_FST_UV, 
												PRIM_CTXT_CONTEXT1, 
												PRIM_FIX_NOFIXDDA ),
											GIF_FLG_PACKED,				    // FLG
											GS_BATCH_2(	GIF_REG_RGBAQ, 
														GIF_REG_XYZ2 ) ) ); // BATCH	
	for( int i = 0; i < mNumOfVerts; i++ ) {
		/* Add the colour */
		VIFDynamicDMA.Add128( PACKED_RGBA ( pVertColours[ i ].mR, pVertColours[ i ].mG, pVertColours[ i ].mB, pVertColours[ i ].mA ) );	
		/* Add the position */   
		VIFDynamicDMA.Add128( PACKED_XYZ2( ( int )( ( 2048.0f + mX + pVerts[ i ].x ) * 16.0f ), 
										   ( int )( ( 2048.0f + mY + pVerts[ i ].y ) * 16.0f ),  
										   ( int )mZ, 
										   0 ) );
	}
	VIFDynamicDMA.EndDirect();
}


/*
==========
PS2TexQuad
==========
*/


/*
===================
Default Constructor
===================
*/
PS2TexQuad::PS2TexQuad() :
 mTU( 0 ), mTV( 0 ), mTW( 256 ), mTH( 256 ) {
	pTexVerts = new Vector4[ 4 ]; 
	Initialise();
}


/*
===========
Constructor
===========
*/
PS2TexQuad::PS2TexQuad( float x, float y, float z, float width, float height, int u, int v, int tWidth, int tHeight ) :
 PS2Quad( x, y, z, width, height ), 
 mTU( u ), mTV( v ), mTW( tWidth ), mTH( tHeight ) {
	pTexVerts = new Vector4[ 4 ];
	SetupUVs();
}


/*
==========
Destructor
==========
*/
PS2TexQuad::~PS2TexQuad() {
	delete[] pTexVerts;
	pTexVerts = NULL;
}


/*
==========================
SetupUVs
Sets quad texture vertices
==========================
*/ 
void PS2TexQuad::SetupUVs( void ) {
	pTexVerts[ 0 ] = Vector4( mTU, mTV, 1.0f, 1.0f );                     // top-left
	pTexVerts[ 1 ] = Vector4( mTU, ( mTV + mTH ), 1.0f, 1.0f );           // bottom-left
	pTexVerts[ 2 ] = Vector4( ( mTU + mTW ), mTV, 1.0f, 1.0f );           // top-right
	pTexVerts[ 3 ] = Vector4( ( mTU + mTW ), ( mTV + mTH ), 1.0f, 1.0f ); // bottom-right
}


/*
======================
Alter UV source coords
======================
*/
void PS2TexQuad::SetUVCoords( const int u, const int v ) {
	mTU = u;
	mTV = v;
	SetupUVs();
}


/*
===================================
Alter UV width and height variables
===================================
*/
void PS2TexQuad::SetUVWidthHeight( const int width, const int height ) {
	mTW = width;
	mTH = height;
	SetupUVs();
}


/*
=================
Animation Getters
=================
*/
int PS2TexQuad::GetTU( void ) const {
	return mTU;
}
int PS2TexQuad::GetTV( void ) const {
	return mTV;
}
int PS2TexQuad::GetTW( void ) const {
	return mTW;
}
int PS2TexQuad::GetTH( void ) const {
	return mTH;
}


/*
=============================
Textured Quad Render Function
=============================
*/
void PS2TexQuad::Render( void ) const {
	VIFDynamicDMA.StartDirect();
	/* Add the GIFTag */
	VIFDynamicDMA.Add128( GS_GIFTAG_BATCH(	mNumOfVerts,				    // NLOOP
											1,							    // EOP
											1,							    // PRE
											GS_PRIM( PRIM_TRI_STRIP, 	    // PRIM
												PRIM_IIP_FLAT, 
												PRIM_TME_ON,
												PRIM_FGE_OFF, 
												PRIM_ABE_ON, 
												PRIM_AA1_OFF, 
												PRIM_FST_UV, 
												PRIM_CTXT_CONTEXT1, 
												PRIM_FIX_NOFIXDDA ),
											GIF_FLG_PACKED,				    // FLG
											GS_BATCH_3(	GIF_REG_RGBAQ,
														GIF_REG_UV, 
														GIF_REG_XYZ2 ) ) ); // BATCH	
	for( int i = 0; i < mNumOfVerts; i++ ) {
		/* Add the colour */
		VIFDynamicDMA.Add128( PACKED_RGBA( pVertColours[ i ].mR, pVertColours[ i ].mG, pVertColours[ i ].mB, pVertColours[ i ].mA ) );
		/* Add UV texture coordinates */ 
		VIFDynamicDMA.Add128( PACKED_UV( ( ( ( int )pTexVerts[ i ].x ) << 4 ), ( ( ( int )pTexVerts[ i ] .y ) << 4 ) ) );
		/* Add the position */  
		VIFDynamicDMA.Add128( PACKED_XYZ2( ( int )( ( 2048.0f + mX + pVerts[ i ].x ) * 16.0f ), 
										   ( int )( ( 2048.0f + mY + pVerts[ i ].y ) * 16.0f ),  
										   ( int )mZ, 
										   0 ) );
	}
	VIFDynamicDMA.EndDirect();
}


/*
==========
PS2Polygon
==========
*/


/*
===================
Default Constructor
===================
*/
PS2Polygon::PS2Polygon() :
 mRadius( 32.0f ), mTriangles( 3 ) {
	Initialise();
}


/*
===================
Polygon Constructor
===================
*/
PS2Polygon::PS2Polygon( float x, float y, float z, float radius, int triangles ) :
 PS2Sprite( x, y, z ), mRadius( radius ), mTriangles( triangles ) {
	Initialise();
}


/*
==========
Destructor
==========
*/
PS2Polygon::~PS2Polygon() {
	delete[] pVerts;
	delete[] pVertColours;
	pVerts = NULL;
	pVertColours = NULL;
}


/*
===========================
Polygon Initialise Function
===========================
*/ 
void PS2Polygon::Initialise( void ) {
	float twoPi = 2.0f * 3.14159f;        		     // approx of Pi
	mNumOfVerts = ( mTriangles + 2 );     		     // center & m_triangles & Back to FIRST Triangle = m_triangles + 2!
	pVerts = new Vector4[ mNumOfVerts ];   		     // delcare vertex array
	pVerts[ 0 ] = Vector4( 0.0f, 0.0f, mZ, 1.0f );   // first point is center
	for( int i = 1; i < ( mNumOfVerts - 1 ); i++ ) { // set triangle points (not first [0], or last [(mNumOfVerts - 1)])
		pVerts[ i ] = Vector4( ( mRadius * Sin( ( i - 1 ) * twoPi / mTriangles ) ), ( mRadius * -Cos( ( i - 1 ) * twoPi / mTriangles ) ), mZ, 1.0f );
	}
	pVerts[ ( mNumOfVerts - 1 ) ] = pVerts[ 1 ];     // last point is FIRST triangle of triangle fan
	pVertColours = new Colours[ mNumOfVerts ];       // declare vertex colour array
}


/*
==========
Get Radius
==========
*/ 
float PS2Polygon::GetR( void ) const {
	return mRadius;
}


/*
=================
Set Vertex Colour
=================
*/
void PS2Polygon::SetVertexColour( int vertex, uint8 r, uint8 g, uint8 b ) {
	pVertColours[ vertex ].mR = r;
	pVertColours[ vertex ].mG = g;
	pVertColours[ vertex ].mB = b;
	/* if 1st triangle point is changed - set last point to match */
	if( vertex == 1 ) { 
		pVertColours[ ( mNumOfVerts - 1 ) ].mR = r;
		pVertColours[ ( mNumOfVerts - 1 ) ].mG = g;
		pVertColours[ ( mNumOfVerts - 1 ) ].mB = b;
	}
	/* if final triangle point is changed - set first to match */
	if( vertex == ( mNumOfVerts - 1 ) ) { 
		pVertColours[ 1 ].mR = r;
		pVertColours[ 1 ].mG = g;
		pVertColours[ 1 ].mB = b;
	}
}


/*
==============
Render Polygon
==============
*/ 
void PS2Polygon::Render( void ) const {
	VIFDynamicDMA.StartDirect();
	/* Add the GIFTag */
	VIFDynamicDMA.Add128( GS_GIFTAG_BATCH(	mNumOfVerts,				    // NLOOP 
											1,							    // EOP
											1,							    // PRE
											GS_PRIM( PRIM_TRI_FAN, 	        // PRIM (TRIANGLE FAN)
												PRIM_IIP_GOURAUD,           // GOURAUD SHADING
												PRIM_TME_OFF,
												PRIM_FGE_OFF, 
												PRIM_ABE_OFF, 
												PRIM_AA1_OFF,               
												PRIM_FST_UV, 
												PRIM_CTXT_CONTEXT1, 
												PRIM_FIX_NOFIXDDA ),
											GIF_FLG_PACKED,				    // FLG
											GS_BATCH_2(	GIF_REG_RGBAQ, 
														GIF_REG_XYZ2 ) ) ); // BATCH	
	for( int i = 0; i < mNumOfVerts; i++ ) {
		/* Add the colour */ 
		VIFDynamicDMA.Add128( PACKED_RGBA( pVertColours[ i ].mR, pVertColours[ i ].mG, pVertColours[ i ].mB, pVertColours[ i ].mA ) );			
		/* Add the position */
		VIFDynamicDMA.Add128( PACKED_XYZ2( ( int )( ( 2048.0f + mX + pVerts[ i ].x ) * 16.0f ), 
										   ( int )( ( 2048.0f + mY + pVerts[ i ].y ) * 16.0f ),  
										   ( int )mZ, 
										   0 ) );
	}
	VIFDynamicDMA.EndDirect();
}

