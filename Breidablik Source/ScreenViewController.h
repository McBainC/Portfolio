#ifndef _SCREENVIEWCONTROLLER_H_
#define _SCREENVIEWCONTROLLER_H_


#include "primitives.h"
#include "LevelData.h"
#include "XYCoords.h"
#include <vector>


/*
===================================================================================
ScreenViewController
Singleton to control the current view of the game
Draws the portion of the Maps mWorldMap which is onscreen using a single PS2TexQuad
Moves around with right analogue stick
===================================================================================
*/


/* Mask for singleton object */
#define SCREENVIEW ScreenViewController::GetInstance()


class ScreenViewController {
public:
	static ScreenViewController* GetInstance();
	
	ScreenViewController::~ScreenViewController();
	
	/* Screen Dimensions */
	float SCREENWIDTH, SCREENHEIGHT;
	
	void Init( void );
	void Draw( vector< vector< Tile > > &vectorName );
	void DrawGrid ( vector< vector< Tile > > &mapVector, vector< vector< Node > > &arrayVector );
	void Update( float xAxisValue, float yAxisValue );
	
	/* External Getters/Setters */
	int GetWorldWidth( void ) const;
	int GetWorldHeight( void ) const;
	float GetX( void ) const;
	float GetY( void ) const;
	void SetSpeed( float newSpeed );
	
	/* External Getter for GameObjects */
	XYCoords GetArrayXYCoords( float x, float y, bool relativeToView );
	
private:
	ScreenViewController::ScreenViewController();
	static ScreenViewController* instance;
	
	float mX, mY;                     // screenview coords
	float mSpeed;                     // scrolling spped
	int mWorldWidth, mWorldHeight;    // world boundaries
	int mXPixelOffset, mYPixelOffset; // world offsets
	
	PS2TexQuad* pVisualObject;        // sprite used to render map
	PS2Quad* pGridQuad;               // sprite used to render grid
	
	float mTileYOffset;               // map tile offset
	
	Tile::TileHeightState mState;
	
	bool CheckTileOffset( Tile::TileHeightState tileHeight );
};			  

#endif