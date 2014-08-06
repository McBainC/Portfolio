#include "Character.h"
#include "GameTextureController.h"
#include "ScreenViewController.h"
#include "PathController.h"
#include <stdio.h>


/*
============================================
Constructor
Initialises all Character variables
Sets the XYCoords with its starting position
Creates its image and shadow PS2Sprites
============================================
*/
Character::Character( float x, float y, float z, int spriteSheetIndex ) :
 GameObject( x, ( y - 32.0f ), z ),
 mHorizontalSpeed( 0.0f ), mVerticalSpeed( 0.0f ), mHealthWidth( 50.0f ),
 mU( 0 ), mV( 0 ), mW( 128 ), mH( 128 ), 
 mInitialSpriteSheetIndex( spriteSheetIndex ), 
 mAnimCountMax( 5 ), mAnimCounter( 0 ), mActionCounter( 0 ), mHealth( 100 ), 
 mArrayCoords( SCREENVIEW -> GetArrayXYCoords( mX, ( mY + 32.0f ), false ) ),
 mNextCoords( SCREENVIEW -> GetArrayXYCoords( mX, ( mY + 32.0f ), false ) ),
 mTargetCoords( SCREENVIEW -> GetArrayXYCoords( mX, ( mY + 32.0f ), false ) ),
 mState( IDLE ), mNextState( mState ), mDirection ( SOUTH ), mNextDirection( mDirection ), 
 mIsPathfinding( false ), mIsActive( false ), mIsAlive( true ), mIsDealingDamage( false ) {
	pCharacterHealthBar = new PS2TexQuad( mX, mY - 36.0f, mZ, mHealthWidth, 8.0f, 0, 160, 64, 8 );
	pCharacterSprite = new PS2TexQuad( mX, mY, mZ, 128.0f, 128.0f, mU, mV, mW, mH );
	pCharacterShadow = new PS2Polygon( mX, mY + 32.0f, mZ - 0.5f, 16.0f, 20); 
	pCharacterShadow -> SetColour( 0, 40, 0, 0 ); // dark green shadow
	pCharacterShadow -> ScaleY( 0.5f );           // make it an oval
	UpdateDepth();                                // set initial depth
}


/*
==============
Destructor
Abstract class
==============
*/
Character::~Character() {
}


/*
=================================
CheckCharacterState
Confirms current state externally
=================================
*/
bool Character::CheckCharacterState( CharacterState testState ) {
	if( mState == testState ) {
		return true;
	}
	return false;
}


/*
====================
ChangeCharacterState
Changes next State
====================
*/
void Character::ChangeCharacterState( CharacterState newState ) {
	mNextState = newState;
}


/*
===========================================================
Render
Moves sprites to correct locations and renders them
Shadow and healthbar stop rendering when character has died
Resizes healthbar has character takes damage / heals
===========================================================
*/
void Character::Render( void ) {
	/* If alive render shadow */
	if( ( pCharacterShadow ) && ( mIsAlive ) ) {
		pCharacterShadow -> MoveTo( mX + SCREENVIEW -> GetX(), mY + SCREENVIEW -> GetY() + 32.0f, mZ - 1.0f );
		pCharacterShadow -> Render();
	}
	
	/* Render Character Sprite */
	if( pCharacterSprite ) {
		Animate();
		GAMETEXTURES -> SelectTexture( mSpriteSheetIndex );
		pCharacterSprite -> MoveTo( mX + SCREENVIEW -> GetX(), mY + SCREENVIEW -> GetY(), mZ );
		pCharacterSprite -> Render();
	}
	
	/* If alive render health bar */
	if( ( pCharacterHealthBar ) && ( mIsAlive ) ) {
		GAMETEXTURES -> SelectTexture( 91 );
		pCharacterHealthBar -> MoveTo( mX + SCREENVIEW -> GetX(), mY + SCREENVIEW -> GetY() - 36.0f, mZ );
		pCharacterHealthBar -> SetWidthAndHeight( mHealthWidth, 8.0f );
		pCharacterHealthBar -> Render();
	}
}


/*
======================================================
GetArrayXYCoords
Returns characters coords within the pathfinding array
======================================================
*/

XYCoords Character::GetArrayXYCoords( void ) const {
	return mArrayCoords;
}


/*
================================
GetNextXYCoords
Returns characters target coords
================================
*/
XYCoords Character::GetTargetXYCoords( void ) const {
	return mTargetCoords;
}


/*
=====================================================================
SetTarget
Character receives a postion in the pathfinding array to move towards
=====================================================================
*/
void Character::SetTarget( XYCoords target ) {
	if( PATHFINDER -> IsNodeFree( target ) ) {
		mTargetCoords = target;
	}	
}


/*
==============================================
CheckIfDealingDamage
Returns if character has hit another character
==============================================
*/
bool Character::CheckIfDealingDamage( void ) {
	if( mIsDealingDamage ) {
		return true;
	}
	return false;
}


/*
================================
WeaponDamage
Returns characters weapon damage
================================
*/
int Character::WeaponDamage( void ) const {
	return mWeaponDamage;
}


/*
========================
TakeDamage
Reduces character health
========================
*/
void Character::TakeDamage( int incomingDamage ) {
	mHealth -= incomingDamage;
}


/*
========================
MoveCharacter
Moves object if required
========================
*/
void Character::MoveCharacter( void ) {
	/* Move character */
	if( mState == MOVING ) {
		mX += mHorizontalSpeed;
		mY += mVerticalSpeed;
	}
}


/* 
=================================================================================
UpdateDepth
Sets mZ correctly in relations to characters Y posistion in the pathfinding array
=================================================================================
*/
void Character::UpdateDepth( void ) {
	mZ = mArrayCoords.mY + 36;
}


/*
===========================================================================
CheckHealth
DeathConditon - resets the animation frame then allows one cycle to display
Then kills any further updates by setting mState to DIEING
===========================================================================
*/
void Character::CheckHealth( void ) {
	/* If health is 0 or less and not active */
	if( ( mHealth <= 0 ) && ( !mIsActive ) ){
		mU = mV = 0; // reset animation to first frame
		if( pCharacterSprite ) {
			pCharacterSprite -> SetUVCoords( mU, mV );
		}
		mIsActive = true;
		mState = DIEING;
	}
}


/*
=============================================
UpdateHealthBar
Changes width of healthbar as mHealth changes
=============================================
*/
void Character::UpdateHealthBar( void ) {
	mHealthWidth = ( mHealth / 2.0f );
	if( mHealthWidth < 0 ) {
		mHealthWidth = 0;
	}
}


/*
======================================================================================
SetSpeedAndSpriteVariables
Updates character attributes for animation and movement based on mState and mDirection
======================================================================================
*/
void Character::SetSpeedAndSpriteVariables( void ) {
	int textureIndex = mInitialSpriteSheetIndex;
	
	/* Adjusts for current direction */
	switch( mDirection ) {
		case NORTH: {
			// no adjustment for textureIndex
			mHorizontalSpeed = 0.0f;
			mVerticalSpeed = -2.0f;
			break;
		}
		case NORTH_EAST: {
			mHorizontalSpeed = 2.0f;
			mVerticalSpeed = -1.0f;
			textureIndex += 1;
			break;
		}
		case EAST: {
			mHorizontalSpeed = 4.0f;
			mVerticalSpeed = 0.0f;
			textureIndex += 2;
			break;
		}
		case SOUTH_EAST: {
			mHorizontalSpeed = 2.0f;
			mVerticalSpeed = 1.0f;
			textureIndex += 3;
			break;
		}
		case SOUTH: {
			mHorizontalSpeed = 0.0f;
			mVerticalSpeed = 2.0f;
			textureIndex += 4;
			break;
		}
		case SOUTH_WEST: {
			mHorizontalSpeed = -2.0f;
			mVerticalSpeed = 1.0f;
			textureIndex += 5;
			break;
		}
		case WEST: {
			mHorizontalSpeed = -4.0f;
			mVerticalSpeed = 0.0f;
			textureIndex += 6;
			break;
		}
		case NORTH_WEST: {
			mHorizontalSpeed = -2.0f;
			mVerticalSpeed = -1.0f;
			textureIndex += 7;
			break;
		}
	}
	
	/* Adjusts for current state */
	switch( mState ) {
		case IDLE: {
			// default (textureIndex = mSpriteSheetIndex)
			break;
		}
		case MOVING: {
			textureIndex += 8;
			break;
		}
		case ATTACKING: {
			textureIndex += 16;
			break;
		}
		case ACTION: {
			textureIndex += 24;
			break;
		}
		case DIEING: {
			textureIndex += 32;
			break;
		}
	}
	
	/* Set SpriteSheetIndex to be rendered */
	mSpriteSheetIndex = textureIndex;
}


/*
============================================
Animate
Creates character animation by altering UV's
============================================
*/
void Character::Animate( void ) {
	if( mIsAlive ) {
		/* Animation Speed */
		if( ( mAnimCounter++ ) % mAnimCountMax == 0 ) {
			mU += mW;
			/* 1st and 2nd frame (top row) */
			if( ( mU > mW ) && ( mV == 0 ) ) { 
				mU = 0;
				mV = mH;
			}
			
			/* 3rd and 4th frame (bottom row) */
			if( ( mU > mW ) && ( mV == 128 ) ) {
				mU = 0;
				mV = 0;
			}
			if( pCharacterSprite ) {
				pCharacterSprite -> SetUVCoords( mU, mV );	
			}
		}
		
		/* Counter reset */
		if( mAnimCounter > 8000 ) {
			mAnimCounter = 0;
		}
	}
}


/*
=====================================================
SetArrayPosition
Sets characters position within the pathfinding array
32 pixels added for character sprites offset
=====================================================
*/
void Character::SetArrayPosition( void ) {
	mArrayCoords = SCREENVIEW -> GetArrayXYCoords( mX, ( mY + 32.0f ), false );
}


/*
===================================================================================
CheckState
Only allows characters state or direction to be changed if not performing an action
===================================================================================
*/
void Character::CheckState( void ) {
	/* If not currently in an action */
	if( !mIsActive ) {
		/* Alter direction if required */
		if( mDirection != mNextDirection ) {
			mDirection = mNextDirection;
		}
		
		/* Alter state if required */
		if( mState != mNextState ) {
			mState = mNextState;
		}
		
		/* If not IDLE - then character is active */
		if( mState != IDLE ) {
			mIsActive = true;
		}
	}
}


/*
==========================================================
SetDirection
Gets and sets the next node on the path towards the target
Sets the characters direction based on the next node
==========================================================
*/
void Character::SetDirection() {
	if( mState != DIEING ) {
		/* Only if not in action and there is a target */
		if( ( ( !mIsActive ) && ( mArrayCoords != mTargetCoords ) && ( !mIsPathfinding ) ) ) {
			/* Get coords of the next tile */
			mNextCoords = PATHFINDER -> FindPath( mArrayCoords, mTargetCoords );
			PATHFINDER -> ClearPathfindingArray();
			
			/* Prevent further calls for pathfinding in this cycle */
			mIsPathfinding = true;
			
			/* Determine what state based on what next tile contains */
			SetState();
			
			/* Determine which direction character needs to face */
			
			/* Odd condtion - NE/SE/SW/NW are different on odd/even Y rows */
			bool oddRow = false;
			if( mArrayCoords.mY % 2 != 0 ) {
				oddRow = true;
			} 
			
			/* North */
			if( ( mNextCoords.mX == mArrayCoords.mX ) && ( mNextCoords.mY == ( mArrayCoords.mY - 2 ) ) ) {
				mNextDirection = NORTH;
			}
			/* NorthEast (odd row) */
			if( oddRow ) {
				if( ( mNextCoords.mX == ( mArrayCoords.mX + 1 ) ) && ( mNextCoords.mY == ( mArrayCoords.mY - 1 ) ) ) {
					mNextDirection = NORTH_EAST;
				}
			/* NorthEast (even row) */
			} else {
				if( ( mNextCoords.mX == mArrayCoords.mX ) && ( mNextCoords.mY == mArrayCoords.mY - 1 ) ) {
					mNextDirection = NORTH_EAST;
				}
			}
			/* East */
			if( ( mNextCoords.mX == ( mArrayCoords.mX + 1 ) ) && ( mNextCoords.mY ==  mArrayCoords.mY ) ) {
				mNextDirection = EAST;
			}
			/* SouthEast (odd row) */
			if( oddRow ) {
				if( ( mNextCoords.mX == ( mArrayCoords.mX + 1 ) ) && ( mNextCoords.mY == ( mArrayCoords.mY + 1 ) ) ) {
					mNextDirection = SOUTH_EAST;
				}
			/* SouthEast (even row) */
			} else {
				if( ( mNextCoords.mX == mArrayCoords.mX ) && ( mNextCoords.mY == mArrayCoords.mY + 1 ) ) {
					mNextDirection = SOUTH_EAST;
				}
			}
			/* South */
			if( ( mNextCoords.mX == mArrayCoords.mX ) && ( mNextCoords.mY == ( mArrayCoords.mY + 2 ) ) ) {
				mNextDirection = SOUTH;
			}
			/* SouthWest (odd row) */
			if( oddRow ) {
				if( ( mNextCoords.mX == mArrayCoords.mX ) && ( mNextCoords.mY == ( mArrayCoords.mY + 1 ) ) ) {
					mNextDirection = SOUTH_WEST;
				}
			/* SouthWest (even row) */
			} else {
				if( ( mNextCoords.mX == ( mArrayCoords.mX - 1 ) ) && ( mNextCoords.mY == mArrayCoords.mY + 1 ) ) {
					mNextDirection = SOUTH_WEST;
				}
			}
			/* West */
			if( ( mNextCoords.mX == ( mArrayCoords.mX - 1 ) ) && ( mNextCoords.mY == mArrayCoords.mY ) ) {
				mNextDirection = WEST;
			}
			/* NorthWest (odd row) */
			if( oddRow ) {
				if( ( mNextCoords.mX == mArrayCoords.mX ) && ( mNextCoords.mY == ( mArrayCoords.mY - 1 ) ) ) {
					mNextDirection = NORTH_WEST;
				}
			/* NorthWest (even row) */
			} else {
				if( ( mNextCoords.mX == ( mArrayCoords.mX - 1 ) ) && ( mNextCoords.mY == mArrayCoords.mY - 1 ) ) {
					mNextDirection = NORTH_WEST;
				}
			}
			
			/* Saftey Condition */
			if( ( mNextCoords.mX == -1 ) || ( mNextCoords.mY == -1 ) ) {
				mNextDirection = mDirection;
				mNextState = mState;
				printf( "Illegal Node received! \n" );
			}
		}
	}
}


/*
===================================================
PerformAction
Each action is counted by 16 ticks 
Sets flags if character is dealing damage or dieing
===================================================
*/
void Character::PerformAction( void ) {
	if( mIsActive ) {
		mActionCounter++;
		mIsDealingDamage = false;
		
		/* Deal damage condition (2 swings per cycle) */
		if( mActionCounter % 8 == 0 ) {
			if( mState == ATTACKING ) {
				 mIsDealingDamage = true;
			}
		}
		
		/* End action condition */
		if( mActionCounter == 16 ) { 
			/* If still alive - reset variables for next cycle */
			if( mState != DIEING ) {
				/* Action Condition */
				if( mState == ACTION ) {
					mHealth += 20;
					if( mHealth > 100 ) {
						mHealth = 100;
					}
				}
				mIsActive = false;
				mNextState = IDLE;
				mActionCounter = 0;
				mIsPathfinding = false;
			
			/* Otherwise flag as dead to stop further updates */
			} else {
				mIsAlive = false;
				PATHFINDER -> CleanObjectNode( mArrayCoords );
				mZ -= 0.75f;
			}
		}
	}
}