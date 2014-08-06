#ifndef _LEVELDATA_H_
#define _LEVELDATA_H_


#include <vector.h>


/* Level One Data */
static const int LEVEL_ONE_WIDTH = 11;                             // tiles per row
static const int LEVEL_ONE_HEIGHT = 33;                            // tiles per column
static const int LEVEL_ONE_X_OFFSET = 288;                         // x offset due to center in middle
static const int LEVEL_ONE_Y_OFFSET = 248;                         // y offset due to center in middle
extern int LEVEL_ONE[ LEVEL_ONE_HEIGHT ][ LEVEL_ONE_WIDTH ];       // mTiles indices
extern int LEVEL_ONE_ARRAY[ LEVEL_ONE_HEIGHT ][ LEVEL_ONE_WIDTH ]; // mPathfindingArray ints


/*
=============================================
Tile Struct
Holds all nessicary data to draw a PS2TexQuad
=============================================
*/
struct Tile {
	float x, y, z; // coords
	int u, v,      // uv's
	width, height, // sprites width and height
	texNumber;     // spriteSheet position in mMapTextures
	
	/* Defines how far to offset when rendering */
	enum TileHeightState {
		INIT,
		ONE,
		TWO,
		THREE
	};
	TileHeightState tileHeight;     
};


/*
==================================================
Node 
Holds nessicary data and functions for pathfinding
==================================================
*/
class Node {
public:
	int mX, mY,         // coords
	mMoveNumber;        // number of moves from origin
	float mDistance;    // distance from target node
	
	/* Defines tile status for pathfinding */
	enum NodeState {
		FREE,
		PATH,
		BLOCKED
	};
	NodeState mNodeState;
	
	/* Defines what the tile contains */
	enum NodeContains {
		NOTHING,
		PLAYER,
		ENEMY,
		OBJECTIVE
	};
	NodeContains mNodeContains;
	
	bool mUsed;         // flag if node has been considered
	bool mIsStartNode;  // flag for start of pathfinding
	bool mIsTargetNode; // flag for target of pathfinding
	
	Node::Node();
	
	bool operator > ( const Node &otherNode ) const;
	float DistanceFromTarget() const;
	
	/* Node Setters */
	void SetAsStart( int moveNumber );
	void SetMoveNumber( int moveNumber );
	void SetAsTarget( void );
	void SetDistanceFromTarget( float distance );
	void UseNode( void );
	void SetNodeAsContaining( NodeContains object );
	void CleanNodeContainer( void );
	void CleanNode( void );
};


/*
==========================
LevelData
Container for Tile Structs
==========================
*/
class LevelData {
public:
	LevelData::LevelData();
	LevelData::~LevelData();
	
	/* 2D vectors */
	vector< vector< Tile > > mLevelOneMap; 
	vector< vector< int > > mLevelOneArray;

	void Init( void );
	
private:
	vector< Tile > mTiles; // level tile structs
	
	/* Creation Functions */
	void LoadTiles( void );
	void LoadSpriteSheet( vector< Tile > &vector, int vStart, int height, int vRange, int uStart, int width, int uRange, Tile::TileHeightState state, int texture );
	void CreateLevelOne( void );
};

#endif