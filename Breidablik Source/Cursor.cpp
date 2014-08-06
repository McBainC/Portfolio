#include "Cursor.h"
#include "GameStateController.h"
#include "GameTextureController.h"
#include "ScreenViewController.h"
#include "PathController.h"


/*
===========================================
Default Constructor
Cursor starts in the center of the screen
Sets cursor speed and inital state
Creates a PS2TexQuad for the cursors sprite
===========================================
*/
Cursor::Cursor() :
 mX( 0.0f ), mY( 0.0f ), mHalfScreenWidth( 320.0f ), mHalfScreenHeight( 256.0f ), 
 mU( 0 ), mV( 0 ), mArrayCoords( SCREENVIEW -> GetArrayXYCoords( mX, mY, true ) ), mSpeed( 8.0f ), mState( MENU ) {
	pCursorVisualObject = new PS2TexQuad( 0.0f, 0.0f, 1001.0f, 64.0f, 32.0f, mU, mV, 64, 32 );
}


/*
=============================
Destructor
Tidy up new and pointer usage
=============================
*/
Cursor::~Cursor() {
	if( pCursorVisualObject ) {
		delete pCursorVisualObject;
		pCursorVisualObject = NULL;
	}
}


/*
=======================================================================
Update
Moves cursor (and its sprite) according to PS2 Analogue stick positions 
Sets cursors current location within the pathfinding array
=======================================================================
*/
void Cursor::Update( float xAxisValue, float yAxisValue ) {
	SetArrayPosition();
	UpdateCursorState();
	UpdateVisualObject();
	
	/* Update positions */
	mX += xAxisValue * mSpeed; 
	mY += yAxisValue * mSpeed;
	StayOnScreen();
	if( pCursorVisualObject ) {
		pCursorVisualObject->MoveTo( mX, mY , 1001.0f );
	}
}


/*
===============================
Render
Displays cursors current sprite
===============================
*/
void Cursor::Render( void ) {
	if( pCursorVisualObject ) {
		GAMETEXTURES -> SelectTexture( 91 );
		pCursorVisualObject->Render();
	}
}


/*
=============================================
Reset
Resets the cursor to the center of the screen
=============================================
*/
void Cursor::Reset( void ) {
	mX = mY = 0.0f;
}


/*
==========================
StayOnScreen
Keeps cursor on the screen
==========================
*/
void Cursor::StayOnScreen( void ) {
	/* Left boundary */
	if( mX < -mHalfScreenWidth ) {
		mX = -mHalfScreenWidth;
	}
	/* Right boundary */
	if( mX > mHalfScreenWidth ) {
		mX = mHalfScreenWidth;
	}
	/* Top boundary */
	if( mY < -mHalfScreenHeight ) {
		mY = -mHalfScreenHeight;
	}
	/* Bottom boundary */
	if( mY > mHalfScreenHeight ) {
		mY = mHalfScreenHeight;
	}
}


/*
=====================================================================================
GetArrayXYCoords
Returns cursors current position within the 2D pathfinding array for external objects
=====================================================================================
*/
XYCoords Cursor::GetArrayXYCoords( void ) const {	
	return mArrayCoords;
}


/*
==========================================================
SetArrayPosition
Gets cursors current pixel location relative to the map,
then sets mArrayCoords as its location within the 2Dvector
==========================================================
*/
void Cursor::SetArrayPosition( void ) {
	mArrayCoords = SCREENVIEW -> GetArrayXYCoords( mX, mY, true );
}


/*
=======================================
UpdateVisualObject
Changes VisualObject sprite as required
=======================================
*/
void Cursor::UpdateVisualObject( void ) {
	if( pCursorVisualObject ) {
		switch( mState ) {
			case MENU: {
				mU = mV = 0;
				break;
			}
			case MOVEMENT: {
				mU = 128;
				mV = 0;
				break;
			}
			case ATTACKING: {
				mU = 64;
				mV = 0;
				break;
			}
		}
		pCursorVisualObject -> SetUVCoords( mU, mV );	
	}
}


/*
======================================================
UpdateCursorState
Sets the cursors state depending on current GameState,
or what the node contains if the game is running
======================================================
*/
void Cursor::UpdateCursorState( void ) {
    /* Running State */
	if( GAMESTATE -> CheckGameState( GameStateController::RUNNING ) ) {
		/* Attacking Cursor */
		if ( PATHFINDER -> NodeContainsEnemy( mArrayCoords ) ) {
			mState = ATTACKING;
		/* Movement Cursor */
		} else {
			mState = MOVEMENT;
		}
	}
}


/*
=================================
CheckGameState
Confirms current state externally
=================================
*/
bool Cursor::CheckState( CursorState testState ) {
	if( mState == testState ) {
		return true;
	}
	return false;
}


/*
=====================
ChangeGameState
Changes current state
=====================
*/
void Cursor::SetState( CursorState newState ) {
	mState = newState;
}