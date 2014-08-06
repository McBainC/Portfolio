#include "Enemy.h"
#include "ScreenViewController.h"


/*
===========================================================
Default Constructor
Creates a character 
51 marks where the enemy sprite sheets start
Scales up the CharacterShadow as enemies are slighty bigger
Sets its position within the pathfinding array
Generates a random start direction and AITimer
===========================================================
*/
Enemy::Enemy( float x, float y, float z ) :
 Character( x, y, z, 51 ), mAIState( PATROLLING ), mAITimer( 1 ),
 mAtPatrolPosition( false ), mOrigin( mArrayCoords ), mPatrolTarget( mArrayCoords ) {
	pCharacterShadow -> Scale( 1.2f );
	PATHFINDER -> SetObjectNode( mArrayCoords, Node::ENEMY );
	mWeaponDamage = 6;
	RandomStart();
}


/*
===============================
Destructor
Tidies up new and pointer usage
===============================
*/
Enemy::~Enemy() {
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
void Enemy::Update( void ) {
	/* If Alive */
	if( mIsAlive ) {
		CheckHealth();                // check if still alive
		UpdateHealthBar();            // updates healthbar width
		UpdateDepth();                // set z depth
		Patrol();                     // random movement
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
==========================================================
CheckAggro
If player gets too close enemies will close in and attack!
==========================================================
*/
void Enemy::CheckAggro( XYCoords playerCoords ) {
	if( PATHFINDER -> GetObjectDistance( mArrayCoords, playerCoords ) < 134.0f ) {
		mAIState = AGGROED;
		mTargetCoords = playerCoords;
	}
}


/*
==================================================
Patrol
Generates a random patrol location and moves to it
Then returns to its original location
==================================================
*/
void Enemy::Patrol( void ) {
	if( mAIState == PATROLLING ) {
		mAITimer++;
		/* Every small amount of time */
		if( mAITimer % 300 == 0 ) {
			/* If at origin - patrol to a random location */
			if( !mAtPatrolPosition ) {
				/* Generate a new target (+- 2 tiles) */
				mPatrolTarget.mX += ( ( rand() % 5 ) - 2 ); 
				mPatrolTarget.mY += ( ( rand() % 5 ) - 2 ); 
				/* Only set if its legal */
				if( PATHFINDER -> IsNodeFree( mPatrolTarget ) ) {
					SetTarget( mPatrolTarget );
					mAtPatrolPosition = true;
				/* Otherwise reset */
				} else {
					mPatrolTarget = mArrayCoords;
				}
			/* If at a patrol position move back to origin */
			} else {
				SetTarget( mOrigin );
				mAtPatrolPosition = false;
			}
			mAITimer = ( ( rand() % 100 ) + 1 ); // reset timer +- ~150
		}
	}
}


/*
========================================
SetState
Determines what state mNextNode requires
========================================
*/
void Enemy::SetState() {
	/* There is a player - attack! */
	if ( PATHFINDER -> NodeContainsPlayer( mNextCoords ) ) {
		mNextState = ATTACKING;
	/* Its free - move! */
	} else if( PATHFINDER -> IsNodeFree( mNextCoords ) ) {
		mNextState = MOVING;
	/* Must be blocked/illegal - stay still */
	} else {
		mNextState = IDLE;
	}
}


/*
========================================================================================
UpdatePathfindingPosition
Updates where the enemy is within the pathfinding array - only when moving between tiles
========================================================================================
*/
void Enemy::UpdatePathfindingPosition( void ) {
	if( ( !mIsActive ) && ( mArrayCoords != mTargetCoords ) && ( !PATHFINDER -> NodeContainsPlayer( mNextCoords ) ) ) {
		PATHFINDER -> CleanObjectNode( mArrayCoords );
		PATHFINDER -> SetObjectNode( mNextCoords, Node::ENEMY );
	}
}


/*
===============================================
RandomStart
Creates a random starting direction and AITimer
===============================================
*/
void Enemy::RandomStart( void ) {
	/* Randomise AITimer */
	mAITimer = ( rand() % 298 + 1);
	
	/* Randomise initial direction */
	int randomDirection = rand() % 8;
	if( randomDirection == 0 ) {
		mDirection = NORTH;
	}
	if( randomDirection == 1 ) {
		mDirection = NORTH_EAST;
	}
	if( randomDirection == 2 ) {
		mDirection = EAST;
	}
	if( randomDirection == 3 ) {
		mDirection = SOUTH_EAST;
	}
	if( randomDirection == 4 ) {
		mDirection = SOUTH;
	}
	if( randomDirection == 5 ) {
		mDirection = SOUTH_WEST;
	}
	if( randomDirection == 6 ) {
		mDirection = WEST;
	}
	if( randomDirection == 7 ) {
		mDirection = NORTH_WEST;
	}
	
	/* Set NextDirection */
	mNextDirection = mDirection;
}