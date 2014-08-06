#pragma once


#include <windows.h>
#include <stdio.h>
#include <gl/gl.h>
#include <gl/glu.h>


/*
===================================================
Basic texture loading functions - Matthew Bett 2006
Modified for threading - Chris McBain 2013
===================================================
*/


/*
===============
TGA File Header
===============
*/
struct TGAHeader {
	GLubyte Header[ 12 ];		  
};


/*
===
TGA
===
*/
struct TGA {
	GLubyte	header[ 6 ];   // First 6 Useful Bytes From The Header
	GLuint	bytesPerPixel; // Holds Number Of Bytes Per Pixel Used In The TGA File
	GLuint	imageSize;	   // Used To Store The Image Size When Setting Aside Ram
	GLuint	temp;		   // Temporary Variable
	GLuint	type;	
	GLuint	Height;		   // Height of Image
	GLuint	Width;		   // Width ofImage
	GLuint	Bpp;		   // Bits Per Pixel
};


/*
==============================================
TextureInfo
Struct to store required data for glTexImage2D
==============================================
*/
struct TextureInfo {
	bool     mCorrectlyLoaded;
	GLuint   mTextureType;								
	GLubyte* pTextureImageData;
	GLuint   mTextureWidth; 
	GLuint   mTextureHeight;
	GLuint   mTextureBpp;
};


/*
============================================================================================
TextureCreator - Chris McBain 2013
Takes in required loaded data from .tga to create a OpenGL Texture
OpenGL calls separated from LoadTexture functionality to enable threading of texture loading
============================================================================================
*/
class TextureCreator {
public:
	TextureCreator( void );
	~TextureCreator( void );

	TGAHeader tgaheader;
	TGA tga;

	/* TGA Load Functions */
	TextureInfo LoadTGA( char *fname );
	TextureInfo LoadUncompressedTGA( char * filename, FILE * fTGA );
	TextureInfo LoadCompressedTGA( char * filename, FILE * fTGA );

	/* OpenGL Create Function */
	bool CreateGLTexture( TextureInfo texture, GLuint & TexID  );
};