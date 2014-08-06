#include "ScreenViewController.h"
#include "GameTextureController.h"


ScreenViewController* ScreenViewController::instance = NULL;


/*
===================
Default Constructor
===================
*/
ScreenViewController::ScreenViewController() { 
}


/*
============================================================================
Getinstance
Returns instance of ScreenViewController - or creates it if doesnt exist yet
============================================================================
*/
ScreenViewController* ScreenViewController::GetInstance() {
	if( instance == NULL ) {
		instance = new ScreenViewController();
	}
	return instance;
}


/*
================================
Destructor
Tidies up pointers and new usage
================================
*/
ScreenViewController::~ScreenViewController() {
	if( pVisualObject ) {
		delete pVisualObject;
		pVisualObject = NULL;
	}
	if( pGridQuad ) {
		delete pGridQuad;
		pGridQuad = NULL;
	}
}


/*
====================================================
Init
Sets ScreenViewController variables and creates two 
sprites to be used to render the map and grid
====================================================
*/
void ScreenViewController::Init( void ) {
	/* Screen Dimensions */
	SCREENWIDTH = 640.0f;
	SCREENHEIGHT = 512.0f;
	
	/* ScreenViews initial position */
	mX = 0.0f; 
	mY = 0.0f; 
	
	/* Initialise Variables */
	mSpeed = 5.0f;
	mWorldWidth = LEVEL_ONE_WIDTH; 
	mWorldHeight = LEVEL_ONE_HEIGHT;
	mXPixelOffset = LEVEL_ONE_X_OFFSET; 
	mYPixelOffset = LEVEL_ONE_Y_OFFSET;
	mState = Tile::INIT; 
	
	/* Create VisualObjects for Rendering */
	pVisualObject = new PS2TexQuad( 0.0f, 0.0f, 0.0f, 64.0f, 32.0f, 0, 0, 64, 32 ); 
	pGridQuad = new PS2Quad( 0.0f, 0.0f, 0.0f, 4.0f, 4.0f );
}


/*
============================================
Draw
Renders Tiles that should be one screen only
============================================
*/
void ScreenViewController::Draw( vector< vector< Tile > > &vectorName ) {
	for( int y = 0; y < mWorldHeight; y++ ) {
		for( int x = 0; x < mWorldWidth; x++ ) {
			/* Check if next tile is a different height */
			if( CheckTileOffset( vectorName[ y ][ x ].tileHeight ) ) {
				/* If it is only need to move to its x/y/z */
				pVisualObject->MoveTo( vectorName[ y ][ x ].x + mX, vectorName[ y ][ x ].y + mY - mTileYOffset, vectorName[ y ][ x ].z ); // move to tiles x/y/z
			/* Otherwise resize pVisualObject */
			} else {
				pVisualObject->MoveTo( vectorName[ y ][ x ].x + mX, vectorName[ y ][ x ].y + mY - mTileYOffset, vectorName[ y ][ x ].z ); // move to tiles x/y/z
				pVisualObject->SetWidthAndHeight( vectorName[ y ][ x ].width, vectorName[ y ][ x ].height );                              // set width and height
				pVisualObject->SetUVWidthHeight( vectorName[ y ][ x ].width, vectorName[ y ][ x ].height );                               // set UV width/height
			}
			
			/* Always set UV's, GAMETEXTURES tests if texture buffer needs to be changed!, and render! */
			pVisualObject->SetUVCoords( vectorName[ y ][ x ].u, vectorName[ y ][ x ].v ); // set UV coords
			GAMETEXTURES->SelectTexture( vectorName[ y ][ x ].texNumber );                // change active texture
			pVisualObject->Render();                                                      // render the tile
		}
	}
}


/*
=================================================
DrawGrid
Draws PS2Quads on tiles for pathfinding debugging
=================================================
*/
void ScreenViewController::DrawGrid ( vector< vector< Tile > > &mapVector, vector< vector< Node > > &arrayVector ) {
	for( int y = 0; y < mWorldHeight; y++ ) {
		for( int x = 0; x < mWorldWidth; x++ ) {
			pGridQuad->MoveTo( mapVector[ y ][ x ].x + mX,  mapVector[ y ][ x ].y + mY, 1000.0f );
			
			/* Display Pathfinding */
			
			/* PATH = blue */
			if ( arrayVector[ y ][ x ].mNodeState == Node::PATH ) {
				pGridQuad->SetColour( 0, 0, 255, 255 );
			/* Used = purple */
			} else if( arrayVector[ y ][ x ].mUsed ) {
				pGridQuad->SetColour( 255, 0, 255, 255 );
			/* FREE = green */
			} else if( arrayVector[ y ][ x ].mNodeState == Node::FREE ) { 
				pGridQuad->SetColour( 0, 255, 0, 255 );
			/* BLOCKED = red */
			} else if( arrayVector[ y ][ x ].mNodeState == Node::BLOCKED ) {
				pGridQuad->SetColour( 255, 0, 0, 255 );
			} 
			
			/* Display objects */
			
			/* PLAYER = white */
			if( arrayVector[ y ][ x ].mNodeContains == Node::PLAYER ) {
				pGridQuad->SetColour( 255, 255, 255, 255 );
			/* ENEMY = black */
			} else if( arrayVector[ y ][ x ].mNodeContains == Node::ENEMY ) {
				pGridQuad->SetColour( 0, 0, 0, 255 );
			/* OBJECTIVE = pink */
			} else if( arrayVector[ y ][ x ].mNodeContains == Node::OBJECTIVE ) {
				pGridQuad->SetColour( 255, 0, 255, 255 );
			}
			
			pGridQuad->Render();
		}
	}
}


/*
==========================================================
Update
Moves ScreenViewController with floats from analogue stick
==========================================================
*/
void ScreenViewController::Update( float xAxisValue, float yAxisValue ) {
	mX += -xAxisValue * mSpeed; 
	mY += -yAxisValue * mSpeed; 
}


/*
=====================================
GetWorldWidth/Height
Returns the size of the current level
=====================================
*/
int ScreenViewController::GetWorldWidth( void ) const {
	return mWorldWidth;
}
int ScreenViewController::GetWorldHeight( void ) const {
	return mWorldHeight;
}


/*
==================================================
GetX/Y
Returns current positions of the screenview object
Offset center of screen to top left ( +320, +256 )
and offsetting grid by ( -32, - 8 ) 
==================================================
*/
float ScreenViewController::GetX( void ) const {
	return mX;
}
float ScreenViewController::GetY( void ) const {
	return mY;
}


/*
===================================
SetSpeed
Alters screenview's scrolling speed
===================================
*/
void ScreenViewController::SetSpeed( float newSpeed ){
	mSpeed = newSpeed;
}


/*
===============================================================================
CheckTileOffset
First check will always be false (no tile with 0 height!) and set mTileYOffset
Afterwards only triggers when tile drawn is of different height, forcing Draw()
to resize pVisualObjects attributes to fit the new tile size
===============================================================================
*/
bool ScreenViewController::CheckTileOffset( Tile::TileHeightState tileHeight ) {
	/* If incoming tile is of same height */
	if( mState == tileHeight ) {
		return true;
	/* Otherwise set new state */
	} else {
		mState = tileHeight;
	}
	/* Alter mTileOffset */
	switch( mState ) {
		case Tile::INIT: {
			break;
		}
		case Tile::ONE: {
			mTileYOffset = 0.0f;
			break;
		}
		case Tile::TWO: {
			mTileYOffset = 16.0f;
			break;
		}
		case Tile::THREE: {
			mTileYOffset = 32.0f;
			break;
		}
	}
	return false;
}


/*
=============================================================
GetArrayCoords
Gets an objects current pixel location relative to the map,
then returns a XYCoords  as its location within the 2D vector
=============================================================
*/
XYCoords ScreenViewController::GetArrayXYCoords( float x, float y, bool relativeToView ) {
	/* Calculate Y position in array */ 
	int currentY;
	
	/* Subtract ScreenViews location if needed */
	if( relativeToView ) {
		currentY = int( y - mY ) + mYPixelOffset; 
	} else {
		currentY = int( y ) + mYPixelOffset; 
	}
	int tempY = int( currentY / 16 );
	int arrayY = tempY + 1;
	
	/* Correct Y if on top boundary */
	if( currentY < 0 ) {
		arrayY -= 1;
	}
	
	/* Calculate X position in array */
	int xOffset = 0;
	int currentX = 0;
	int tempX = 0;
	
	/* If on an odd row offset right by 32 pixels */
	if( arrayY % 2 != 0 ) {
		xOffset = 32;
	}  
	
	/* Subtract ScreenViews location if needed */
	if( relativeToView ) {
		currentX = int ( x - mX ) + mXPixelOffset + xOffset;
	} else {
		currentX = int ( x ) + mXPixelOffset + xOffset;
	}
	tempX = ( int )( currentX / 64 );
	int arrayX = tempX + 1;
	
	/* Correct X if on left boundary */
	if( currentX < 0 ) {
		arrayX -= 1;
	}
	
	/* One tile to the left if on odd row */
	if( arrayY % 2 != 0 ) {
		arrayX -= 1;
	} 
	
	/* Saftey conditions for accessing 2D vector */
	if( arrayY < 0 ) {
		arrayY = 0;
	}
	if( arrayY > ( mWorldHeight - 1 ) ) {
		arrayY = ( mWorldHeight - 1 );
	}
	if( arrayX < 0 ) {
		arrayX = 0;
	}
	if( arrayX > ( mWorldWidth - 1 ) ) {
		arrayX = ( mWorldWidth - 1 );
	}
	
	/* Create XYCoords and return it */
	XYCoords arrayCoords( arrayX, arrayY );
	return arrayCoords;
}