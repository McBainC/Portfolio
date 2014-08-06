#pragma once


#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include "Texture.h"
#include <vector>


using namespace std;


/*
================================
TextureName
Loads in the OpenGL tga textures
================================
*/
class TextureName {
public:
	TextureName( char *name );
	~TextureName( void );

	char *pName;                            // pointer to string for .tga file
	int mID;                                // vector index
	vector< TextureInfo > *vectorReference; // pointer reference to vector
};