#include "GameTextureController.h"
#include <stdio.h>


/*
===========
TextureInfo
===========
*/


/*
======================================
Constructor
Creates TextureInfo and Initialises it
======================================
*/
TextureInfo::TextureInfo( string textureName ) {
	mTextureName = textureName;
	Init();
}


/*
===================
Destructor
Nothing to clean up
===================
*/
TextureInfo::~TextureInfo() {
}


/*
================================
Init
Loads texture into static memory
================================
*/
void TextureInfo::Init( void ) {
	if( !LoadBitmap( mTextureName.c_str(), true, false ) ) {
		printf( "Can't load Texture\n" );
	}
}


/*
=====================
GameTextureController
=====================
*/


GameTextureController* GameTextureController::instance = NULL;


/*
===========
Constructor
===========
*/
GameTextureController::GameTextureController() {
}


/*
=============================================================================
GetInstance
Returns instance of GameTextureController - or creates it if doesnt exist yet
=============================================================================
*/
GameTextureController* GameTextureController::GetInstance() {
	if( instance == NULL ) {
		instance = new GameTextureController();
	}
	return instance;
}


/*
=======================
Destructor
Tidyup mTextures vector
=======================
*/
GameTextureController::~GameTextureController() {
	for( vector< TextureInfo* >::iterator it = mTextures.begin(); it != mTextures.end(); it++ ) {
		delete *it;
	}
	mTextures.clear();
}


/*
================================================================
Init
Loads textures into vector and initialises font texture classes
================================================================
*/
void GameTextureController::Init( void ) {
	/* Initialise buffer variables */
	mTexBuffer480 = TEXBUF480;
	mTexBuffer496 = TEXBUF496;
	mTexBuffer1 = -1;
	mTexBuffer2 = -2;

	/* Load the font bitmap and data */
	if( !font_.Load( "PS2Framework/font.dat", true ) ) {
		printf( "Can't load font.dat\n" );
	}
	if( !font_tex.LoadBitmap( "PS2Framework/font.bmp", false, true ) ) {
		printf( "Can't load font.bmp\n" );
	}
	
	/* Push Textures */
	
	/* Tile Textures (0 - 7) */
	mTextures.push_back( new TextureInfo( "Bitmaps/GroundTiles.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/BridgeTiles.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/GroundToWaterTiles.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/GroundToWater2Tiles.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/CliffStraightTiles.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/CliffCornerTiles.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/CliffCorner2Tiles.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/CaveTiles.bmp" ) );
	
	/* Loading Screen (8) */
	mTextures.push_back( new TextureInfo( "Bitmaps/LoadingScreen.bmp" ) );
	
	/* Object Textures (9 - 10) */
	mTextures.push_back( new TextureInfo( "Bitmaps/Trees.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Trees2.bmp" ) );
	
	/* Knight IDLE Textures (11 - 18) */
	mTextures.push_back( new TextureInfo( "Bitmaps/Knight_IDLE/Knight_IDLE_N.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Knight_IDLE/Knight_IDLE_NE.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Knight_IDLE/Knight_IDLE_E.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Knight_IDLE/Knight_IDLE_SE.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Knight_IDLE/Knight_IDLE_S.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Knight_IDLE/Knight_IDLE_SW.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Knight_IDLE/Knight_IDLE_W.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Knight_IDLE/Knight_IDLE_NW.bmp" ) );
	
	/* Knight MOVING Textures (19 - 26) */
	mTextures.push_back( new TextureInfo( "Bitmaps/Knight_MOVING/Knight_MOVING_N.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Knight_MOVING/Knight_MOVING_NE.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Knight_MOVING/Knight_MOVING_E.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Knight_MOVING/Knight_MOVING_SE.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Knight_MOVING/Knight_MOVING_S.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Knight_MOVING/Knight_MOVING_SW.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Knight_MOVING/Knight_MOVING_W.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Knight_MOVING/Knight_MOVING_NW.bmp" ) );
	
	/* Knight ATTACKING Textures (27 - 34) */
	mTextures.push_back( new TextureInfo( "Bitmaps/Knight_ATTACKING/Knight_ATTACKING_N.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Knight_ATTACKING/Knight_ATTACKING_NE.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Knight_ATTACKING/Knight_ATTACKING_E.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Knight_ATTACKING/Knight_ATTACKING_SE.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Knight_ATTACKING/Knight_ATTACKING_S.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Knight_ATTACKING/Knight_ATTACKING_SW.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Knight_ATTACKING/Knight_ATTACKING_W.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Knight_ATTACKING/Knight_ATTACKING_NW.bmp" ) );
	
	/* Knight ACTION Textures (35 - 42) */
	mTextures.push_back( new TextureInfo( "Bitmaps/Knight_ACTION/Knight_ACTION_N.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Knight_ACTION/Knight_ACTION_NE.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Knight_ACTION/Knight_ACTION_E.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Knight_ACTION/Knight_ACTION_SE.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Knight_ACTION/Knight_ACTION_S.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Knight_ACTION/Knight_ACTION_SW.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Knight_ACTION/Knight_ACTION_W.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Knight_ACTION/Knight_ACTION_NW.bmp" ) );
	
	/* Knight DIEING Textures (43 - 50) */
	mTextures.push_back( new TextureInfo( "Bitmaps/Knight_DIEING/Knight_DIEING_N.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Knight_DIEING/Knight_DIEING_NE.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Knight_DIEING/Knight_DIEING_E.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Knight_DIEING/Knight_DIEING_SE.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Knight_DIEING/Knight_DIEING_S.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Knight_DIEING/Knight_DIEING_SW.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Knight_DIEING/Knight_DIEING_W.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Knight_DIEING/Knight_DIEING_NW.bmp" ) );
	
	/* Skeleton IDLE Textures (51 - 58) */
	mTextures.push_back( new TextureInfo( "Bitmaps/Skeleton_IDLE/Skeleton_IDLE_N.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Skeleton_IDLE/Skeleton_IDLE_NE.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Skeleton_IDLE/Skeleton_IDLE_E.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Skeleton_IDLE/Skeleton_IDLE_SE.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Skeleton_IDLE/Skeleton_IDLE_S.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Skeleton_IDLE/Skeleton_IDLE_SW.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Skeleton_IDLE/Skeleton_IDLE_W.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Skeleton_IDLE/Skeleton_IDLE_NW.bmp" ) );
	
	/* Skeleton MOVING Textures (59 - 66) */
	mTextures.push_back( new TextureInfo( "Bitmaps/Skeleton_MOVING/Skeleton_MOVING_N.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Skeleton_MOVING/Skeleton_MOVING_NE.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Skeleton_MOVING/Skeleton_MOVING_E.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Skeleton_MOVING/Skeleton_MOVING_SE.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Skeleton_MOVING/Skeleton_MOVING_S.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Skeleton_MOVING/Skeleton_MOVING_SW.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Skeleton_MOVING/Skeleton_MOVING_W.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Skeleton_MOVING/Skeleton_MOVING_NW.bmp" ) );
	
	/* Skeleton ATTACKING Textures (67 - 74) */
	mTextures.push_back( new TextureInfo( "Bitmaps/Skeleton_ATTACKING/Skeleton_ATTACKING_N.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Skeleton_ATTACKING/Skeleton_ATTACKING_NE.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Skeleton_ATTACKING/Skeleton_ATTACKING_E.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Skeleton_ATTACKING/Skeleton_ATTACKING_SE.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Skeleton_ATTACKING/Skeleton_ATTACKING_S.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Skeleton_ATTACKING/Skeleton_ATTACKING_SW.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Skeleton_ATTACKING/Skeleton_ATTACKING_W.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Skeleton_ATTACKING/Skeleton_ATTACKING_NW.bmp" ) );
	
	/* Skeleton ACTION Textures (75 - 82) */
	mTextures.push_back( new TextureInfo( "Bitmaps/Skeleton_ACTION/Skeleton_ACTION_N.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Skeleton_ACTION/Skeleton_ACTION_NE.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Skeleton_ACTION/Skeleton_ACTION_E.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Skeleton_ACTION/Skeleton_ACTION_SE.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Skeleton_ACTION/Skeleton_ACTION_S.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Skeleton_ACTION/Skeleton_ACTION_SW.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Skeleton_ACTION/Skeleton_ACTION_W.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Skeleton_ACTION/Skeleton_ACTION_NW.bmp" ) );
	
	/* Skeleton DIEING Textures (83 - 90) */
	mTextures.push_back( new TextureInfo( "Bitmaps/Skeleton_DIEING/Skeleton_DIEING_N.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Skeleton_DIEING/Skeleton_DIEING_NE.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Skeleton_DIEING/Skeleton_DIEING_E.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Skeleton_DIEING/Skeleton_DIEING_SE.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Skeleton_DIEING/Skeleton_DIEING_S.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Skeleton_DIEING/Skeleton_DIEING_SW.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Skeleton_DIEING/Skeleton_DIEING_W.bmp" ) );
	mTextures.push_back( new TextureInfo( "Bitmaps/Skeleton_DIEING/Skeleton_DIEING_NW.bmp" ) );
	
	/* UI Textures (91) */
	mTextures.push_back( new TextureInfo( "Bitmaps/UI.bmp" ) );
}


/*
==================================================================================
SelectTexture
Uploads and Selects a bitmap for rendering if that texture is not already selected
==================================================================================
*/
void GameTextureController::SelectTexture( int textureNumber ) {
	/* Check if safe */
	CheckVectorIndex( textureNumber );
	/* If its not already loaded */
	if( !CheckBuffers( textureNumber ) ) {
		mTextures[ textureNumber ]->Upload( TEXBUF480 ); // load it
		mTextures[ textureNumber ]->Select();            // select it
		mTexBuffer1 = textureNumber;                     // set buffer index
	}
}


/*
=====================================
UploadTexture
Forces a texture to a specific buffer
=====================================
*/
void GameTextureController::UploadTexture( int textureNumber, uint64 texBuffer ) {
	CheckVectorIndex( textureNumber );
	mTextures[ textureNumber ]->Upload( texBuffer );
	mTextures[ textureNumber ]->Select();
	if( texBuffer == TEXBUF480 ) {
		mTexBuffer1 = textureNumber;
	} else if ( texBuffer == TEXBUF496 ) {
		mTexBuffer2 = textureNumber;
	}
}


/*
================================
SelectFontTexture
Uploads and Selects Font Texture
================================
*/
void GameTextureController:: DrawText( float x, float y, int r, int g, int b, string text ) {
	font_tex.Upload( TEXBUF496 );
	font_tex.Select();
	mTexBuffer1 = -1;
	font_.printfC( x, y, r, g, b, 128, text.c_str() );
}


/*
=================================================
CheckBuffers
Checks if the requested texture is already loaded
If not it uploads the required texture
=================================================
*/
bool GameTextureController::CheckBuffers( int newTexture ) {
	/* Check if the texture is already loaded */
	if( ( mTexBuffer1 == newTexture ) ) {
		return true;
	} else {
		return false;
	}
}


/*
=======================================
CheckVectorIndex
Checks if vector index is within bounds
=======================================
*/
void GameTextureController::CheckVectorIndex( int index ) {
	if( index < 0 ){
		printf( "mTextures accesed with a negivite index..." );
	}
	if( index > int( mTextures.size() ) ) {
		printf( "mTextures overflowed..." );
	}
}