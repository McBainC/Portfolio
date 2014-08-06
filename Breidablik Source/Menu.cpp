#include "Menu.h"
#include "GameTextureController.h"


/*
========================================================
Constructor
Sets menu cursor position floats and game flags to false
Creates MenuSKull and MenuCursor PS2TexQuads
========================================================
*/
Menu::Menu() :
 mMenuX( 70.0f ), mMenuY( 170.0f ), 
 mDisplayGrid( false ), mHardCoreMode( false ),
 mRunning( false ), mQuitting( false ),
 mMenuStatus( MAIN ), mState( PLAY ) {
	pMenuSkull = new PS2TexQuad( 0.0f, 0.0f, 1.0f, 256.0f, 256.0f, 0, 0, 256, 256 );
	pMenuCursor = new PS2TexQuad( mMenuX, mMenuY, 1.0f, 64.0f, 32.0f, 0, 0, 64, 32 );
}


/*
===============================
Destructor
Tidies up new and pointer usage
===============================
*/
Menu::~Menu() {
	if( pMenuSkull ) {
		delete pMenuSkull;
		pMenuSkull = NULL;
	}
	if( pMenuCursor ) {
		delete pMenuCursor;
		pMenuCursor = NULL;
	}
}


/*
============================================
Update
Sets State depending on which menu ss active
Then where the menu cursor is located
============================================
*/
void Menu::Update( void ) {
	/* Main Menu */
	if( mMenuStatus== MAIN ) {
		if( mMenuY == 170.0f ) {
			mState = PLAY;
		}
		if( mMenuY == 200.0f ) {
			mState = OPTIONS;
		}
		if( mMenuY == 230.0f ) {
			mState = QUIT;
		}
	/* Settings Menu */
	} else if ( mMenuStatus == SETTINGS ) {
		if( mMenuY == 170.0f ) {
			mState = DISPLAY_GRID;
		}
		if( mMenuY == 200.0f ) {
			mState = HARDCOREMODE;
		}
		if( mMenuY == 230.0f ) {
			mState = RETURN;
		}
	}
}


/*
=============================
Render
Renders menu sprites and text
=============================
*/
void Menu::Render( void ) {
	/* Always Render Skull */
	if( pMenuSkull ) {
		GAMETEXTURES -> SelectTexture( 8 );
		pMenuSkull -> Render();
	}
	
	/* Main Menu Text */
	if( mMenuStatus == MAIN ) {
		mMenuX = 70.0f;
		GAMETEXTURES -> DrawText( 0.0f, 140.0f, 128, 128, 128, "Play" );
		GAMETEXTURES -> DrawText( 0.0f, 170.0f, 128, 128, 128, "Options" );
		GAMETEXTURES -> DrawText( 0.0f, 200.0f, 128, 128, 128, "Quit" );
	}
	
	/* Setting Menu Text */
	if( mMenuStatus == SETTINGS ) {
		mMenuX = 80.0f;
		/* Red if false */
		if( !mDisplayGrid ) {
			GAMETEXTURES -> DrawText( 0.0f, 140.0f, 128, 0, 0, "DisplayGrid" );
		/* Green if true */
		} else {
			GAMETEXTURES -> DrawText( 0.0f, 140.0f, 0, 128, 0, "DisplayGrid" );
		}
		/* Red if false */
		if( !mHardCoreMode ) {
			GAMETEXTURES -> DrawText( 0.0f, 170.0f, 128, 0, 0, "HardCoreMode" );
		/* Green if true */
		} else {
			GAMETEXTURES -> DrawText( 0.0f, 170.0f, 0, 128, 0, "HardCoreMode" );
		}
		GAMETEXTURES -> DrawText( 0.0f, 200.0f, 128, 128, 128, "Return" );
	}
	
	/* Always Render Glove */
	if( pMenuCursor ) {
		GAMETEXTURES -> SelectTexture( 91 );
		pMenuCursor -> MoveTo( mMenuX, mMenuY, 1.0f );
		pMenuCursor -> Render();
	}
}


/*
===============================
Select
External selection of an option
===============================
*/
void Menu::Select( void ) {
	switch( mState ) {
		case PLAY: {
			mRunning = true; 
			break;
		}
		case OPTIONS: {
			mMenuStatus = SETTINGS;
			mMenuY = 170.0f;
			break;
		}
		case QUIT: {
			mQuitting = true;
			break;
		}
		case DISPLAY_GRID: {
			if( !mDisplayGrid ) {
				mDisplayGrid = true;
			} else {
				mDisplayGrid = false;
			}
			break;
		}
		case HARDCOREMODE: {
			if( !mHardCoreMode ) {
				mHardCoreMode = true;
			} else {
				mHardCoreMode = false;
			}
			break;
		}
		case RETURN: {
			mMenuStatus = MAIN;
			mMenuY = 170.0f;
			break;
		}
	}
}


/*
=================
SetRunning
Set mRunning flag
=================
*/
void Menu::SetRunning( bool newBool ) {
	mRunning = newBool;
}


/*
====================
MoveUp
Moves menu cursor up
====================
*/
void Menu::MoveUp( void ) {
	if( mMenuY > 170.0f ) {
		mMenuY -= 30.0f;
	} else {
		mMenuY = 230.0f;
	}
}


/*
======================
MoveDown
Moves menu cursor down
======================
*/
void Menu::MoveDown( void ) {
	if( mMenuY < 230.0f ) {
		mMenuY += 30.0f;
	} else {
		mMenuY = 170.0f;
	}
}


/*
==============================================
DisplayingGrid
External check if DisplayGrid has been enabled
==============================================
*/
bool Menu::DisplayingGrid( void ) {
	if( mDisplayGrid ) {
		return true;
	}
	return false;
}


/*
===============================================
IsHardCoreMode
External check if HardCoreMode has been enabled
===============================================
*/
bool Menu::IsHardCoreMode( void ) {
	if( mHardCoreMode ) {
		return true;
	}
	return false;
}


/*
=====================================
IsRunning
External check for is game is running
=====================================
*/
bool Menu::IsRunning( void ) {
	if( mRunning ) {
		return true;
	}
	return false;
}


/*
======================================
IsQuitting
External check for it game is quitting
======================================
*/
bool Menu::IsQuitting( void ) {
	if( mQuitting ) {
		return true;
	}
	return false;
}