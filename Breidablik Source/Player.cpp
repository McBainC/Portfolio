#include "Player.h"
#include "ScreenViewController.h"


/*
==============================================
Default Constructor
Creates a character
11 marks where the Knight sprite sheets start
Sets initial position within pathfinding array
==============================================
*/
Player::Player( float x, float y, float z ) :
 Character( x, y, z, 11 ) {
	PATHFINDER -> SetObjectNode( mArrayCoords, Node::PLAYER );
	mWeaponDamage = 30;
}


/*
============================
Destructor
Tidyup new and pointer usage
============================
*/
Player::~Player() {
	if( pCharacterHealthBar ) {
		delete pCharacterHealthBar;
		pCharacterHealthBar = NULL;
	}
	if( pCharacterSprite ) {
		delete pCharacterSprite;
		pCharacterSprite = NULL;
	}
	if( pCharacterShadow ) {
		delete pCharacterShadow;
		pCharacterShadow = NULL;
	}
}


/*
=============================
Update
Update all required variables
=============================
*/
void Player::Update( void ) {
	/* If Alive */
	if( mIsAlive ) {
		CheckHealth();                // check if still alive
		UpdateHealthBar();            // updates healthbar width
		UpdateDepth();                // set z depth
		SetArrayPosition();           // set position within pathfinding array 
		SetSpeedAndSpriteVariables(); // set speed and sprite based on states
		SetDirection();               // finds nextNode and sets direction
		UpdatePathfindingPosition();  // update PATHFINDER
		CheckState();                 // check if not performing an action and change states if nessicary
		MoveCharacter();              // move player
		PerformAction();              // lockout further actions 
	}
}


/*
=======================================
SetState
Determine what state mNextNode requires
=======================================
*/
void Player::SetState() {	
	/* If next node contains an enemy */
	if ( PATHFINDER -> NodeContainsEnemy( mNextCoords ) ) {
		mNextState = ATTACKING;
	/* If next node is free */
	} else if( PATHFINDER -> IsNodeFree( mNextCoords ) ) {
		mNextState = MOVING;
	/* Default */
	} else {
		mNextState = IDLE;
	}
}


/*
========================================================
UpdatePathfindingPosition
Updates where the player is within the pathfinding array
========================================================
*/
void Player::UpdatePathfindingPosition( void ) {
	/* Only call when not active, when next node is set and it does not contain an enemy */
	if( ( !mIsActive ) && ( mArrayCoords != mTargetCoords ) && ( !PATHFINDER -> NodeContainsEnemy( mNextCoords ) ) ) {
		PATHFINDER -> CleanObjectNode( mArrayCoords );
		PATHFINDER -> SetObjectNode( mNextCoords, Node::PLAYER );
	}
}