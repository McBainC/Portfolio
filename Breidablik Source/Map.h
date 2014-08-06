#ifndef _MAP_H_
#define _MAP_H_


#include "LevelData.h"
#include <vector>


/*
===============================================================
Map
Contains a 2DVector for the WorldMap 
mWorldMap contains < Tile > structs for drawing the map sprites
Loads in from levelData class to create the current level
===============================================================
*/
class Map {
public:
	Map::Map( int worldWidth, int worldHeight );
	Map::~Map();
	
	vector< vector< Tile > > mWorldMap; 

	void Load2DVectors( vector< vector< Tile > > &mapVector );
	
private:
	int mWorldWidth, mWorldHeight;
	void SetTileCoords( void );
};

#endif