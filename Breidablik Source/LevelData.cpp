#include "LevelData.h"


/*
====
Node
====
*/


/*
==============================
Constructor
Sets all pathfinding variables
==============================
*/
Node::Node() :
 mX( -1 ), mY( -1 ), mMoveNumber( -1 ), mDistance( 0.0f ), 
 mNodeState( FREE ), mNodeContains( NOTHING ), 
 mUsed( false ), mIsStartNode( false ), mIsTargetNode( false ) {
}


/*
=========================
Priority Queue > overload
=========================
*/
bool Node::operator > ( const Node &otherNode ) const {
	return DistanceFromTarget() > otherNode.DistanceFromTarget();
}


/*
============================
DistanceFromTarget
Returns distance from target
============================
*/
float Node::DistanceFromTarget() const {
	return mDistance;
}


/*
===================================
SetAsStart
Marks node as start for pathfinding
===================================
*/
void Node::SetAsStart( int moveNumber ) {
	mMoveNumber = moveNumber;
	mUsed = true;
	mIsStartNode = true;
}


/*
============================
SetMoveNumber
Number of 'moves' from start
============================
*/
void Node::SetMoveNumber( int moveNumber ) {
	mMoveNumber = moveNumber;
	mUsed = true;
}


/*
====================================
SetAsTarget
Marks node as target for pathfinding
====================================
*/
void Node::SetAsTarget( void ) {
	mIsTargetNode = true;
}


/*
=========================
SetDistanceFromTarget
Distance from target node
=========================
*/
void Node::SetDistanceFromTarget( float distance ) {
	mDistance = distance;
}


/*
========================================
UseNode
Mark node as 'considered' by Pathfinding 
========================================
*/
void Node::UseNode( void ) {
	mUsed = true;
}


/*
========================================
SetNodeAsContaining
To set node as containing a player/enemy
========================================
*/
void Node::SetNodeAsContaining( NodeContains object ) {
	/* Only if it is currently empty */
	if( mNodeContains == NOTHING ) {
		mNodeContains = object;
	}
}


/*
====================
CleanNodeContainer
Resets mNodeContains
====================
*/
void Node::CleanNodeContainer( void ) {
	mNodeContains = NOTHING;
}


/*
================
CleanNode
Resets variables
================
*/
void Node::CleanNode( void ) {
	mDistance = 0.0f;
	mMoveNumber = -1;
	mUsed = false;
	mIsStartNode = false;
	mIsTargetNode = false;
	/* Only reset state of PATH nodes */
	if( mNodeState != BLOCKED ) {
		mNodeState = FREE;
	}
}


/*
=========
LevelData
=========
*/


/*
===================
Default Constructor
===================
*/
LevelData::LevelData() {
	Init();
}


/*
==============
Destructor
Tidyup vectors
==============
*/
LevelData::~LevelData() {
	mLevelOneMap.clear();
	mLevelOneArray.clear();
	mTiles.clear();
}


/*
========================================================================
Initialise Level Data
First loads in the uv data into mTiles to create all the level tiles
Then sets a 2D vector using a 2D array of int indices to build the level
========================================================================
*/
void LevelData::Init( void ) {
	LoadTiles();
	CreateLevelOne();
}


/*
============================================
LoadTiles
Loads in all the uv data to create the tiles
This vector has 121 tiles [ 0 - 120 ] 
============================================
*/
void LevelData::LoadTiles( void ) {
	LoadSpriteSheet( mTiles,   0,  32, 256,   0,  64, 256,   Tile::ONE, 0 ); // Creates the 32 GroundTiles
	LoadSpriteSheet( mTiles,   0,  64, 256,   0,  64, 256,   Tile::TWO, 1 ); // Creates the 16 BridgeTiles
	LoadSpriteSheet( mTiles,   0,  64, 128,   0,  64, 256,   Tile::TWO, 2 ); // Creates the 8 GroundToWaterTiles
	LoadSpriteSheet( mTiles, 128,  32, 256,   0,  64, 256,   Tile::ONE, 2 ); // Creates the 16 GroundToWaterTiles
	LoadSpriteSheet( mTiles,   0,  64, 256,   0,  64, 256,   Tile::TWO, 3 ); // Creates the 16 GroundToWater2Tiles
	LoadSpriteSheet( mTiles,  64,  96, 256,   0,  64, 256, Tile::THREE, 4 ); // Creates the 8 CliffStraightTiles
	LoadSpriteSheet( mTiles,  64,  96, 256,   0,  64, 256, Tile::THREE, 5 ); // Creates the 8 CliffCornerTiles
	LoadSpriteSheet( mTiles,  64,  96, 256,   0,  64, 256, Tile::THREE, 6 ); // Creates the 8 CliffCorner2Tiles
	LoadSpriteSheet( mTiles,  64,  96, 256,   0,  64, 256, Tile::THREE, 7 ); // Creates the 8 CaveTiles
	LoadSpriteSheet( mTiles,   0,  32,  32,   0,  64,  64,   Tile::ONE, 4 ); // Creates a blank tile
}


/*
=================================
LoadSpriteSheet
Loads in regular tile struct data
=================================
*/
void LevelData::LoadSpriteSheet( vector< Tile > &vector, int vStart, int height, int vRange, int uStart, int width, int uRange, Tile::TileHeightState state, int texture ) {
	for( int v = vStart; v < vRange; v += height ) {
		for( int u = uStart; u < uRange; u += width ) {
			Tile temp;
			temp.u = u;
			temp.v = v;
			temp.width = width;
			temp.height = height;
			temp.tileHeight = state;
			temp.texNumber = texture;
			vector.push_back( temp );
		}
	}
}


/*
=================================================================
CreateLevelOne
Uses LEVEL_ONE int array as indices for mTiles to load in the map
=================================================================
*/
void LevelData::CreateLevelOne( void ) {
	/* Setup 2D vectors */
	mLevelOneMap.clear();
	mLevelOneArray.clear();
	for( int y = 0; y < LEVEL_ONE_HEIGHT; y++ ) {
		vector< Tile > tempMapRow;
		vector< int > tempArrayRow;
		/* Load in data from externs */
		for( int x = 0; x < LEVEL_ONE_WIDTH; x++ ) {
			tempMapRow.push_back( mTiles[ LEVEL_ONE[ y ][ x ] ] ); 
			tempArrayRow.push_back( LEVEL_ONE_ARRAY[ y ][ x ] ); 
		}
		mLevelOneMap.push_back( tempMapRow );
		mLevelOneArray.push_back( tempArrayRow );
	}
}


/* Level One - mTiles indices */                                                                                 // <- Extra Tile!
int LEVEL_ONE[ LEVEL_ONE_HEIGHT ][ LEVEL_ONE_WIDTH ] = { {   0,   1,  22,  15,   7,  30,  27,   6, 120, 120, 120    },
														 {      2,   0,   3, 120,   8,   5,  28, 120, 120, 120,  92 },
													 	 {  24,  31,  14, 120, 120,  17, 120, 120, 120, 104,  88    },
														 {      8,   3, 120, 120, 120, 120, 120, 120,  88,  96,   2 },
														 { 120,   3, 120, 120, 120, 120, 120, 120,  92,   0,   2    },
														 {    120, 120, 120, 104, 120, 120, 120,  88,  24,   1,  19 },
														 { 120, 120, 120,  92,  89, 120, 108,  92,  25,   0,  24    },
														 {    120, 120, 115,   0,  93,  88, 100,  26,   0,   0,  13 },
														 {  89, 120, 114,   3,  26,  96,   1,  27,   9,   0,  21    },
														 {     93,  88,   2,  26,   4,  25,   0,  30,   1,   0,  11 },
														 {  30, 100,  24,  25,   0,   3,   9,  17,  18,   0,   4    },
														 {     25,   1,  10,   4,  24,   3,   0,   3,  1 ,   0,   2 },
														 {  25,   0,   0,   2,  16,  29,   1,   0,   8,   9,  31    },
														 {     28,  25,   0,   5,  24,  26,  18,   6,   0,  10,   9 },
														 {  14,  31,  24,   3,  19,  12,   0,   2,   8,   2,   7    },
														 {      3,  21,  27,   0,   9,  10,   4,  13,  28,   0,   6 },
														 {   2,   5,   0,   0,   2,  24,  28,   0,  16,   9,   5    },
														 {      8,  18,  10,  20,   9,   0,   6,   3,   0,   0,  27 },
														 {   6,   4,   2,   3,   8,  18,   2,   0,  16,   2,   4    },
														 {     19,  17,   2,  11,  10,  25,  27,  17,   1,   2, 120 },
														 {   9,   2,  11,   1,  19,   0, 120,  25,   0,   0, 120    },
														 {     12,   3,   0,   5,   3, 120, 120,   2,  20, 120,  52 },
														 {  17,   0,   6,   0,   2, 120,  52, 120, 120, 120,  80    },
														 {      3,  12,   9,   1, 120,  72,  81, 120,  120, 72,  58 },
														 {   1,  31,  22,  30, 120,  80,  61,  73,  48,  80,  71    },
														 {     22,   7,  19, 120,  32,  69,  62,  84,  76,  68,  60 },
														 {   6, 120,  11, 120,  32,  42,  59,  65,  67,  66,  58    },
														 {    120, 120, 120,  80,  40,  38,  62,  66,  64,  71,  57 },
														 { 120,  48, 120,  72,  66,  36,  57,  67,  70,  59,  61    },
														 {     80,  73,  80,  56,  57,  58,  59,  60,  61,  62,  63 },
														 {  72,  62,  76,  71,  70,  69,  68,  67,  66,  65,  64    },
														 {     67,  68,  69,  70,  71,  56,  57,  58,  59,  60,  61 },
													 	 {  56,  57,  58,  59,  60,  61,  62,  63,  64,  65,  66    } };					

														 
/* Level One Status - 0 Tile can be moved to, 1 Tile is blocked */                                                      // <- Extra Tile!
int LEVEL_ONE_ARRAY[ LEVEL_ONE_HEIGHT ][ LEVEL_ONE_WIDTH ] = { {   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1    },
															   {      0,   0,   0,   1,   0,   0,   0,   1,   1,   1,   1 },
															   {   1,   0,   0,   1,   1,   0,   1,   1,   1,   1,   1    },
															   {      0,   0,   1,   1,   1,   1,   1,   1,   1,   1,   1 },
															   {   1,   0,   1,   1,   1,   1,   1,   1,   1,   0,   1    },
															   {      1,   1,   1,   1,   1,   1,   1,   1,   0,   0,   1 },
															   {   1,   1,   1,   1,   1,   1,   1,   1,   0,   0,   1    },
															   {      1,   1,   0,   0,   1,   1,   1,   0,   0,   0,   1 },
															   {   1,   1,   1,   0,   0,   1,   0,   0,   0,   0,   1    },
															   {      1,   1,   0,   0,   0,   0,   0,   0,   0,   0,   1 },
															   {   1,   1,   0,   0,   0,   0,   0,   0,   0,   0,   1    },
															   {      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1 },
															   {   1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1    },
															   {      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1 },
															   {   1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1    },
															   {      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1 },
															   {   1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1    },
															   {      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1 },
															   {   1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1    },
															   {      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1 },
															   {   1,   0,   0,   0,   0,   0,   1,   0,   0,   0,   1    },
															   {      0,   0,   0,   0,   0,   1,   1,   0,   0,   1,   1 },
															   {   1,   0,   0,   0,   0,   1,   1,   1,   1,   1,   1    },
															   {      0,   0,   0,   0,   0,   1,   1,   1,   1,   1,   1 },
															   {   1,   0,   0,   0,   0,   0,   1,   1,   1,   1,   1    },
															   {      0,   0,   0,   1,   0,   0,   1,   1,   1,   1,   1 },
															   {   1,   1,   0,   1,   1,   0,   1,   1,   1,   1,   1    },
															   {      1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1 },
															   {   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1    },
															   {      1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1 },
															   {   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1    },
															   {      1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1 },
															   {   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1    } };					