#include "PathController.h"
#include "LevelData.h"
#include <stdio.h>


PathController* PathController::instance = NULL;


/*
=============================
Constructor
Sets level boundary variables
=============================
*/
PathController::PathController() :
 mWorldWidth( LEVEL_ONE_WIDTH ), mWorldHeight( LEVEL_ONE_HEIGHT ), mTargetFound( false ) {
}


/*
======================================================================
GetInstance
Returns instance of PathController - or creates it if doesnt exist yet
======================================================================
*/
PathController* PathController::GetInstance() {
	if( instance == NULL ) {
		instance = new PathController();
	}
	return instance;
}


/*
================
Destructor
Tidies up vector
================
*/
PathController::~PathController() {
	mPathfindingArray.clear();
}


/*
=================
Init
Creates 2D vector
=================
*/
void PathController::Init( void ) {
	/* Setup up 2D vectors */
	mPathfindingArray.clear(); 
	for( int y = 0; y < mWorldHeight; y++ ) {
		/* Node 2D vector */
		vector< Node > tempGridRow;
		tempGridRow.resize( mWorldWidth );
		mPathfindingArray.push_back( tempGridRow );
	}
}


/*
============================
LoadPathfindingArray
Loads in data from levelData
============================
*/
void PathController::LoadPathfindingArray( vector< vector< int > > &arrayVector ) {
	for( int y = 0; y < mWorldHeight; y++ ) {
		for( int x = 0; x < mWorldWidth; x++ ) {
			/* Use TileData int array to set mPathfindingArray */
			mPathfindingArray[ y ][ x ].mX = x;
			mPathfindingArray[ y ][ x ].mY = y;
			
			/* 0 == FREE */
			if( arrayVector[ y ][ x ] == 0 ) {
				mPathfindingArray[ y ][ x ].mNodeState = Node::FREE;
			/* 1 == BLOCKED */
			} else if( arrayVector[ y ][ x ] == 1 ) {
				mPathfindingArray[ y ][ x ].mNodeState = Node::BLOCKED;
			}
		}
	}
}


/*
=============================
SetPathfindingArray
External Setter for NodeState
=============================
*/
void PathController::SetPathfindingNode( XYCoords position, Node::NodeState newState ) {
	if( CheckIfInRange( position ) ) {
		mPathfindingArray[ position.mY ][ position.mX ].mNodeState = newState;
	}
}


/*
======================================================
SetObjectNode
External Setter for what the pathfinding node contains
======================================================
*/
void PathController::SetObjectNode( XYCoords position, Node::NodeContains newObject ) {
	if( CheckIfInRange( position ) ) {
		if( mPathfindingArray[ position.mY ][ position.mX ].mNodeContains == Node::NOTHING ) {
			mPathfindingArray[ position.mY ][ position.mX ].mNodeContains = newObject;
		}
	}
}


/*
==================
CleanObjectNode
Resets object node
==================
*/
void PathController::CleanObjectNode( XYCoords position ) {
	if( CheckIfInRange( position ) ) {
		mPathfindingArray[ position.mY ][ position.mX ].CleanNodeContainer();
	}
}


/*
============================
ClearPathfindingArray
Resets pathfinding variables
============================
*/
void PathController::ClearPathfindingArray( void ) {
	for( int y = 0; y < mWorldHeight; y++ ) {
		for( int x = 0; x < mWorldWidth; x++ ) {
			mPathfindingArray[ y ][ x ].CleanNode();
		}
	}
}


/*
=============================================
ClearObjectArray
Clears object components of pathfinding array
=============================================
*/
void PathController::ClearObjectArray( void ) {
	for( int y = 0; y < mWorldHeight; y++ ) {
		for( int x = 0; x < mWorldWidth; x++ ) {
			mPathfindingArray[ y ][ x ].CleanNodeContainer();
		}
	}
}


/*
========================
FindPath
A* Pathfinding Algorithm
========================
*/
XYCoords PathController::FindPath( XYCoords start, XYCoords target ) {
	XYCoords nextNode( -1, -1 ); // a default value thats illegal - if target cant be reached
	if( ( CheckIfInRange( start ) ) && ( CheckIfInRange( target ) ) ) {
		mTargetFound = false;
		
		/* Set start - only if selected node is not blocked */
		if( mPathfindingArray[ start.mY ][ start.mX ].mNodeState != Node::BLOCKED ) {
			mPathfindingArray[ start.mY ][ start.mX ].SetAsStart( 0 ); // set start node
			mActiveNode = mPathfindingArray[ start.mY ][ start.mX ];   // set as node of interest                
			mWorkingSet.push( mActiveNode );                           // push to priority queue
			mPathfindingArray[ target.mY ][ target.mX ].SetAsTarget(); // mark target node
		}
		
		/* Wave Expansion */
		
		/* While the working set is NOT empty */
		while(!mWorkingSet.empty() ) {
			mActiveNode = mWorkingSet.top();                 // set node of interest as the top
			mWorkingSet.pop();                               // remove it
			int moveNumber = mActiveNode.mMoveNumber + 1;    // increase moveNumber
			GenerateMoves( mPathfindingArray, mActiveNode ); // generate legal moves
			
			/* Check the legal moves for target */
			for( int i = 0; i < mStepCounter; i++ ) {
				/* If the target is one of the next moves */
				if( mSteps[ i ].mIsTargetNode ) { 
					mSteps[ i ].SetMoveNumber( moveNumber ); // set its moveNumber 
					mTracebackSet.push( mSteps[ i ] );       // add for traceback
					break;                                  
				}
			}
			
			/* If target was found */
			if(!mTracebackSet.empty() ) {
				break;
			}
			
			/* For legal moves */
			for( int i = 0; i < mStepCounter; i++ ) {
				if( !mPathfindingArray[ mSteps[ i ].mY ][ mSteps[ i ].mX ].mUsed ) {
					mPathfindingArray[ mSteps[ i ].mY ][ mSteps[ i ].mX ].SetMoveNumber( moveNumber );
					mPathfindingArray[ mSteps[ i ].mY ][ mSteps[ i ].mX ].SetDistanceFromTarget( GetDistance( mPathfindingArray[ mSteps[ i ].mY ][ mSteps[ i ].mX ], mPathfindingArray[ target.mY ][ target.mX ] ) ); 
					mWorkingSet.push( mPathfindingArray[ mSteps[ i ].mY ][ mSteps[ i ].mX ] ); // add to priority queue
				}
			}
		}
		
		/* Traceback */

		/* While the traceback set is NOT empty */
		while( !mTracebackSet.empty() ) {
			/* Start at target */
			mActiveNode = mTracebackSet.top();
			
			/* Clear traceback set */
			while( !mTracebackSet.empty() ) {
				mTracebackSet.pop();
			}
			
			/* Set as 'path' */
			mPathfindingArray[ mActiveNode.mY ][ mActiveNode.mX ].mNodeState = Node::PATH; 
			
			/* Generate legal moves */
			GenerateMoves( mPathfindingArray, mActiveNode );
			
			/* Check the legal moves for target */
			for( int i = 0; i < mStepCounter; i++ ) {
				/* If the start is one of the next moves */
				if( mSteps[ i ].mIsStartNode ) {
					mTargetFound = true;
					/* Set return node */
					nextNode.mX = mActiveNode.mX;
					nextNode.mY = mActiveNode.mY;      
					break;                                  
				}
			}
			
			/* If target was found */
			if( mTargetFound ) {
				break;
			}
			
			/* Loop through possibles */
			for( int i = 0; i < mStepCounter; i++ ) {
				if( ( mSteps[ i ].mMoveNumber < mActiveNode.mMoveNumber ) && ( mSteps[ i ].mUsed ) ) {
					mTracebackSet.push( mPathfindingArray[ mSteps[ i ].mY ][ mSteps[ i ].mX ] );
				}
			}
		}
			
		/* Clear priority queues */
		while( !mWorkingSet.empty() ) {
			mWorkingSet.pop();
		}
		while( !mTracebackSet.empty() ) {
			mTracebackSet.pop();
		}
		
		/* Return node */
		return nextNode;
	}
	
	/* Illegal Condition */
	return nextNode;
	printf( "Illegal Pathfinding Call \n" );
}


/*
=====================================================
IsNodeFree
A check for characters to see if node can be moved to
=====================================================
*/
bool PathController::IsNodeFree( XYCoords position ) {
	if( CheckIfInRange( position ) ) {
		if( mPathfindingArray[ position.mY ][ position.mX ].mNodeState != Node::BLOCKED ) {
			return true;
		}
	}
	return false;
}


/*
========================================================
NodeContainsEnemy
A check for the player to see if node has an enemy on it
========================================================
*/
bool PathController::NodeContainsEnemy( XYCoords position ) {
	if( CheckIfInRange( position ) ) {
		if( mPathfindingArray[ position.mY ][ position.mX ].mNodeContains == Node::ENEMY ) {
			return true;
		}
	}
	return false;
}


/*
===========================================================
NodeContainsPlayer
A check for the enemies to see if node has the player on it
===========================================================
*/
bool PathController::NodeContainsPlayer( XYCoords position ) {
	if( CheckIfInRange( position ) ) {
		if( mPathfindingArray[ position.mY ][ position.mX ].mNodeContains == Node::PLAYER ) {
			return true;
		}
	}
	return false;
}


/*
=====================================================
GetObjectDistance
External Version
Gets distance between objects using pythagoras
Offsets odd rows by 32 pixels
As moving North or South is y+-2, y is * by 32 not 16
=====================================================
*/
float PathController::GetObjectDistance( XYCoords currentPosition, XYCoords targetPosition ) {
	if( ( CheckIfInRange( currentPosition ) ) && ( CheckIfInRange( targetPosition ) ) ) {
		
		/* Get current obeject pixel coords */
		int	currentY = currentPosition.mY * 32;
		int xOffset = 0;
		if( currentPosition.mY % 2 != 0 ) {
			xOffset = 32;
		}
		int currentX = ( currentPosition.mX * 64 ) + xOffset;
		
		/* Get target object pixel coords */
		int	targetY = targetPosition.mY * 32;
		xOffset = 0;
		if( targetPosition.mY % 2 != 0 ) {
			xOffset = 32;
		}
		int targetX = ( targetPosition.mX * 64 ) + xOffset;
		
		/* Calculate distance */
		int dy = currentY - targetY;
		int dx = currentX - targetX;
		float distance = float( sqrt( ( dy * dy ) + ( dx * dx ) ) );
		
		/* Return Distance */
		return distance;
	}
	
	/* Saftey Condition */
	printf( "Illegal GetObjectDistance Call \n" );
	return 0.0f;
}


/*
======================================================
GenerateMoves
Fills steps array with legal moves from current tile
Checks if each directions move is within the 2D vector
Then if that node is 'free' - ie a legal move
======================================================
*/
void PathController::GenerateMoves( vector< vector< Node > > &mPathfindingArray, Node currentNode ) {
	mStepCounter = 0;          // reset stepCounter
	mActiveNode = currentNode; // set reference node
	bool oddRow = false;       // different diagonal's apply to odd / even row'd nodes
	
	/* Test if mActiveNode is on an odd row */
	if( mActiveNode.mY % 2 != 0 ) {
		oddRow = true;
	}
	
	/* North */
	if( ( mActiveNode.mY - 2 ) >= 0 ) {                                                               // if within 2D vector
		if( mPathfindingArray[ mActiveNode.mY - 2 ][ mActiveNode.mX ].mNodeState != Node::BLOCKED ) { // if node is not blocked
			mSteps[ mStepCounter ] = mPathfindingArray[ mActiveNode.mY - 2 ][ mActiveNode.mX ];       // add to mSteps as a legal move
			mStepCounter++;                                                                           // increase step counter
		}
	}
	/* NorthEast (odd row) */
	if( oddRow ) {
		if( ( ( mActiveNode.mY - 1 ) >= 0 ) && ( ( mActiveNode.mX + 1 ) < mWorldWidth ) ) {  
			if( mPathfindingArray[ mActiveNode.mY - 1 ][ mActiveNode.mX + 1 ].mNodeState != Node::BLOCKED ) { 
				mSteps[ mStepCounter ] = mPathfindingArray[ mActiveNode.mY - 1 ][ mActiveNode.mX + 1 ];   
				mStepCounter++;
			}
		}
	/* NorthEast (even row) */
	} else {
		if( ( mActiveNode.mY - 1 ) >= 0 ) {  
			if( mPathfindingArray[ mActiveNode.mY - 1 ][ mActiveNode.mX ].mNodeState != Node::BLOCKED ) { 
				mSteps[ mStepCounter ] = mPathfindingArray[ mActiveNode.mY - 1 ][ mActiveNode.mX ];   
				mStepCounter++;
			}
		}
	}
	/* East */
	if( ( mActiveNode.mX + 1 ) < mWorldWidth ) {
		if( mPathfindingArray[ mActiveNode.mY ][ mActiveNode.mX + 1].mNodeState != Node::BLOCKED ) {
			mSteps[ mStepCounter ] = mPathfindingArray[ mActiveNode.mY ][ mActiveNode.mX + 1 ];   
			mStepCounter++;
		}
	}
	/* South East (odd row) */
	if( oddRow ) {
		if( ( ( mActiveNode.mY + 1 ) < mWorldHeight ) && ( ( mActiveNode.mX + 1 ) < mWorldWidth ) ) {
			if( mPathfindingArray[ mActiveNode.mY + 1 ][ mActiveNode.mX + 1 ].mNodeState != Node::BLOCKED ) {
				mSteps[ mStepCounter ] = mPathfindingArray[ mActiveNode.mY + 1 ][ mActiveNode.mX + 1 ];   
				mStepCounter++;
			}
		}
	/* South East (even row) */
	} else {
		if( ( mActiveNode.mY + 1 ) < mWorldHeight ) {
			if( mPathfindingArray[ mActiveNode.mY + 1 ][ mActiveNode.mX ].mNodeState != Node::BLOCKED ) {
				mSteps[ mStepCounter ] = mPathfindingArray[ mActiveNode.mY + 1 ][ mActiveNode.mX ];   
				mStepCounter++;
			}
		}
	}
	/* South */
	if( ( mActiveNode.mY + 2 ) < mWorldHeight ) {
		if( mPathfindingArray[ mActiveNode.mY + 2 ][ mActiveNode.mX ].mNodeState != Node::BLOCKED ) {
			mSteps[ mStepCounter ] = mPathfindingArray[ mActiveNode.mY + 2 ][ mActiveNode.mX ];   
			mStepCounter++;
		}
	}
	/* South West (odd row) */
	if( oddRow ) {
		if( ( mActiveNode.mY + 1 ) < mWorldHeight ) {
			if( mPathfindingArray[ mActiveNode.mY + 1 ][ mActiveNode.mX ].mNodeState != Node::BLOCKED ) {
				mSteps[ mStepCounter ] = mPathfindingArray[ mActiveNode.mY + 1 ][ mActiveNode.mX ];   
				mStepCounter++;
			}
		}
	/* South West (even row) */
	} else {
		if( ( ( mActiveNode.mY + 1 ) < mWorldHeight ) && ( ( mActiveNode.mX - 1 ) >= 0 ) ) {
			if( mPathfindingArray[ mActiveNode.mY + 1 ][ mActiveNode.mX - 1 ].mNodeState != Node::BLOCKED ) {
				mSteps[ mStepCounter ] = mPathfindingArray[ mActiveNode.mY + 1 ][ mActiveNode.mX - 1 ];   
				mStepCounter++;
			}
		}
	}
	/* West */
	if( ( mActiveNode.mX - 1 ) >= 0 ) {
		if( mPathfindingArray[ mActiveNode.mY ][ mActiveNode.mX - 1 ].mNodeState != Node::BLOCKED ) {
			mSteps[ mStepCounter ] = mPathfindingArray[ mActiveNode.mY ][ mActiveNode.mX - 1 ];   
			mStepCounter++;
		}
	}
	/* North West (odd row) */
	if( oddRow ) {
		if( ( mActiveNode.mY - 1 ) >= 0 ) {
			if( mPathfindingArray[ mActiveNode.mY - 1 ][ mActiveNode.mX ].mNodeState != Node::BLOCKED ) {
				mSteps[ mStepCounter ] = mPathfindingArray[ mActiveNode.mY - 1 ][ mActiveNode.mX ];   
				mStepCounter++;
			}
		}
	/* North West (even row) */
	} else {
		if( ( ( mActiveNode.mY - 1 ) >= 0 ) && ( ( mActiveNode.mX - 1 ) >= 0 ) ) {
			if( mPathfindingArray[ mActiveNode.mY - 1 ][ mActiveNode.mX - 1 ].mNodeState != Node::BLOCKED ) {
				mSteps[ mStepCounter ] = mPathfindingArray[ mActiveNode.mY - 1 ][ mActiveNode.mX - 1 ];   
				mStepCounter++;
			}
		}
	}
} 


/*
=====================================================
GetDistance
Gets distance between nodes using pythagoras
Offsets odd rows by 32 pixels
As moving North or South is y+-2, y is * by 32 not 16
=====================================================
*/
float PathController::GetDistance( Node currentNode, Node targetNode ) {
	/* Get current nodes pixel coords */
	int	currentY = currentNode.mY * 32;
	int xOffset = 0;
	if( currentNode.mY % 2 != 0 ) {
		xOffset = 32;
	}
	int currentX = ( currentNode.mX * 64 ) + xOffset;
	
	/* Get target nodes pixel coords */
	int	targetY = targetNode.mY * 32;
	xOffset = 0;
	if( targetNode.mY % 2 != 0 ) {
		xOffset = 32;
	}
	int targetX = ( targetNode.mX * 64 ) + xOffset;
	
	/* Calculate distance */
	int dy = currentY - targetY;
	int dx = currentX - targetX;
	float distance = float( sqrt( ( dy * dy ) + ( dx * dx ) ) );
	return distance;
}


/*
================================================
CheckIfInRange
Check that incoming XYCoords is within 2D vector
================================================
*/
bool PathController::CheckIfInRange( XYCoords position ) {
	if( ( position.mX < 0 ) || (  position.mX >= mWorldWidth ) ) {
		return false;
	}	
	if( ( position.mY < 0 ) || (  position.mY >= mWorldHeight ) ) {
		return false;
	}	
	return true;
}