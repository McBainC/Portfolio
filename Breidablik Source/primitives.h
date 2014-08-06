#ifndef _PRIMITIVES_H_
#define _PRIMITIVES_H_


#include <sps2tags.h>
#include <sps2regstructs.h>
#include "PS2Defines.h"
#include "ps2vector4.h"


/*
===========================================================================
-> primitives <-
Contains classes to render basic sprites on the PS2
PS2Sprite - Abstract base class
PS2Quad and PS2Polygon - Children of PS2Sprite to render Quads and Polygons
Colours - Holds RGBA data for use in PS2Quad and PS2Polygon
PS2TexQuad - Textured version of PS2Quad
===========================================================================
*/


/*
==================================================
Colours
Holds RGBA data for Non Textured PS2Sprite classes
==================================================
*/
class Colours {
public:
	Colours();
	~Colours();

	uint8 mR; // red
	uint8 mG; // green
	uint8 mB; // blue
	uint8 mA; // alpha
};


/*
=====================
PS2Sprite
Abstract Sprite Class 
=====================
*/
class PS2Sprite {
public:
	PS2Sprite::PS2Sprite();
	PS2Sprite::PS2Sprite( const float x, const float y, const float z );
	virtual PS2Sprite::~PS2Sprite();
	
	/* Pure Virtual Functions */
	virtual void Initialise() = 0;
	virtual void Render( void ) const = 0;
	virtual void SetVertexColour( int vertex, uint8 R, uint8 G, uint8 B ) = 0;
	
	/* Common Functions */
	void MoveTo( const float x, const float y, const float z );
	void MoveDeltaX( const float x );
	void MoveDeltaY( const float y );
	void MoveDeltaZ( const float z );
	void Rotate( const float a );
	void Scale( const float s );
	void ScaleX( const float s );
	void ScaleY( const float s );
	
	void SetColour( uint8 r, uint8 g, uint8 b, uint8 a );
	Colours ReturnColour( int vertex );
	
	float GetX( void ) const;
	float GetY( void ) const;	
	float GetZ( void ) const;
	
protected:
	float mX, mY, mZ;      // x & y coords (center), z depth (big = near)
	
	int mNumOfVerts;       // number of vertices
	
	Vector4* pVerts;       // vertex array
	Colours* pVertColours; // vertex colour array
};


/*
==============
PS2Quad
Creates a Quad
==============
*/
class PS2Quad : public PS2Sprite {
public:
	PS2Quad::PS2Quad();
	PS2Quad( float x, float y, float z, float width, float height );
	PS2Quad::~PS2Quad();
	
	void SetWidthAndHeight( float width, float height );
	
	float GetWidth( void ) const;
	float GetHeight( void ) const;
	
	void SetVertexColour( int vertex, uint8 r, uint8 g, uint8 b );
	
	void Render( void ) const;
	
	void Initialise( void );
	
protected:
	float mW, mH; // width and height
};


/*
=======================
PS2TexQuad
Creates a Textured Quad
=======================
*/ 
class PS2TexQuad : public PS2Quad {
public:
	PS2TexQuad::PS2TexQuad();
	PS2TexQuad( float x, float y, float z, float width, float height, int u, int v, int tWidth, int tHeight );
	PS2TexQuad::~PS2TexQuad();
	
	void Render( void ) const;
	
	void SetupUVs( void );
	
	//void PS2TexQuad::SetupAnimation( int numFrames );
	//void PS2TexQuad::Animate( void );
	
	/* UV Texture Functions */
	void SetUVCoords( const int u, const int v );
	void SetUVWidthHeight( const int width, const int height );
	
	int GetTU( void ) const;
	int GetTV( void ) const;
	int GetTW( void ) const;
	int GetTH( void ) const;

protected:
	int mTU, mTV;     // texture coords (top left)
	int mTW, mTH;     // texture coords (width and height)
	
	Vector4* pTexVerts; // texture vertices    
};


/*
=================
PS2Polygon
Creates a Polygon
=================
*/
class PS2Polygon : public PS2Sprite {
public:
	PS2Polygon::PS2Polygon();
	PS2Polygon( float x, float y, float z, float radius, int triangles );
	PS2Polygon::~PS2Polygon();
	
	float GetR( void ) const; 
	
	void SetVertexColour( int vertex, uint8 r, uint8 g, uint8 b );
	
	void Render( void ) const;
	
	void Initialise( void ); 
	
private:	
	float mRadius;  // radius
	int mTriangles; // number of sides
};

#endif