#include "Map.h"
#include "GameTextureController.h"
#include <stdio.h>


/*
=========================================================================
Constructor
Sets mWorldWidth / mWorldHeight variables and creates mWorldMap 2D vector
=========================================================================
*/
Map::Map( int worldWidth, int worldHeight ) :
 mWorldWidth( worldWidth ), mWorldHeight( worldHeight ) {
	/* Setup up 2D vectors */
	mWorldMap.clear(); 
	for( int y = 0; y < mWorldHeight; y++ ) {
		/* Tile 2D vector */
		vector< Tile > tempMapRow;
		tempMapRow.resize( mWorldWidth );
		mWorldMap.push_back( tempMapRow );
	}
}


/*
=============
Destructor
Tidyup vector
=============
*/
Map::~Map() { 
	mWorldMap.clear();
}


/*
============================
Load2DVectors
Loads in data from levelData  
============================
*/
void Map::Load2DVectors( vector< vector< Tile > > &mapVector ) {
	for( int y = 0; y < mWorldHeight; y++ ) {
		for( int x = 0; x < mWorldWidth; x++ ) {
			/* Load TileData structs into mWorldMap */
			mWorldMap[ y ][ x ] = mapVector[ y ][ x ];
		}
	}
	SetTileCoords(); 
}


/*
======================================================
SetTileCoords
Sets the x & y coords of the <Tile>'s within pWorldMap
======================================================
*/
void Map::SetTileCoords( void ) {
	float halfMapWidth = ( mWorldWidth - 1 ) / 2;
	float halfMapHeight = ( mWorldHeight - 1 ) / 2;
	float zDepth = 1.0f;
	float offset = 0.0f;
	/* Set the Tiles X/Y/Z based on position in 2D vector */
	for( int y = 0; y < mWorldHeight; y++, zDepth++ ) {
		for( int x = 0; x < mWorldWidth; x++ ) {
			offset = 0.0f;
			/* Odd rows are offset */
			if( y % 2 != 0 ) {
				offset = 32.0f;
			}
			mWorldMap[ y ][ x ].x = ( ( x - halfMapWidth ) * 64 ) + offset;
			mWorldMap[ y ][ x ].y = ( y - halfMapHeight ) * 16;
			mWorldMap[ y ][ x ].z = zDepth;
		}
	}
}