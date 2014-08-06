#pragma once


#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>


#include <process.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <string>
#include <sys/types.h>
#include <sys/timeb.h>


#include "TextureName.h"
#include "Texture.h"


using namespace std;


/*
=============
TextureLoader
=============
*/
class TextureLoader {
public:
	TextureLoader( void );
	~TextureLoader( void );

	void Init( void );
	void LoadTextures( void );

	vector< TextureInfo > mTextures;

private:
	vector< TextureName > mNames;
	static const int NUM_OF_THREADS = 14;
	HANDLE hThread[ NUM_OF_THREADS ];

	static unsigned int __stdcall LoadTexture( void *data );
};