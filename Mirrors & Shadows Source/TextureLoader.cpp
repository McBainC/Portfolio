#include "TextureLoader.h"


/*
===========
Constructor
===========
*/
TextureLoader::TextureLoader( void ) {
}


/*
==========
Destructor
==========
*/
TextureLoader::~TextureLoader( void ) {
}


/*
========================================
Init
Initializes mNames vector chars and id's
========================================
*/
void TextureLoader::Init( void ) {
	/* Setting TextureName Vector */
	mNames.push_back( TextureName( "GoodSkyBox.tga" ) );
	mNames.push_back( TextureName( "EvilSkyBox.tga" ) );
	mNames.push_back( TextureName( "Sun.tga" ) );
	mNames.push_back( TextureName( "DeadSun.tga" ) );
	mNames.push_back( TextureName( "GoodPlanet.tga" ) );
	mNames.push_back( TextureName( "Clouds.tga" ) );
	mNames.push_back( TextureName( "Moon.tga" ) );
	mNames.push_back( TextureName( "DeadPlanet.tga" ) );
	mNames.push_back( TextureName( "Rings.tga" ) );
	mNames.push_back( TextureName( "JumpPoint.tga" ) );
	mNames.push_back( TextureName( "SKIN.tga" ) );
	mNames.push_back( TextureName( "SKIN2.tga" ) );
	mNames.push_back( TextureName( "Controls.tga" ) );
	mNames.push_back( TextureName( "HUD.tga" ) );

	/* Set mTextures size to match mNames */
	mTextures.resize( ( int ) mNames.size() );

	/* Set Texture ID's */
	for( int i = 0; i < ( int )mNames.size(); i++ ) {
		mNames[ i ].mID = i;
		mNames[ i ].vectorReference = &mTextures;
	}
}


/*
============
LoadTextures
============
*/
void TextureLoader::LoadTextures( void ) {
	/* Thread Texture Loading */
	for( int i = 0; i < NUM_OF_THREADS; i++ ) {
		hThread[ i ] = ( HANDLE ) _beginthreadex( NULL, 0, LoadTexture, ( void* )&mNames[ i ], 0, NULL );
	}

	/* Wait for all textures to be loaded */
	WaitForMultipleObjects( NUM_OF_THREADS, hThread, TRUE, INFINITE );
}


/*
===============================================
LoadTexture
Thread Function to load a .tga file into OpenGL 
===============================================
*/
unsigned int __stdcall TextureLoader::LoadTexture( void *data ) {
	TextureName* args = ( TextureName* ) data;
	TextureCreator temp;
	(*args -> vectorReference)[ args -> mID ] = temp.LoadTGA( args -> pName );
	return 0;
}