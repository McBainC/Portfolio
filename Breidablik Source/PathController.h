#ifndef _PATHCONTROLLER_H_
#define _PATHCONTROLLER_H_


#include "XYCoords.h"
#include "LevelData.h"
#include <queue>
#include <vector>


/*
===========================================================================
PathController
Singleton to control Character pathfinding
Takes in current position in maps 2D vector and calculates shortest route
to a target position - returns the first node along the path
Allows external access to what pathfinding array contains for hit detection
===========================================================================
*/


/* Mask for singleton object */
#define PATHFINDER PathController::GetInstance()


class PathController {
public:
	static PathController* GetInstance();
	PathController::~PathController();
	
	/* 2D vector of nodes */
	vector< vector< Node > > mPathfindingArray;
	
	void Init( void );
	
	/* External access to mPathfindingArray */
	void LoadPathfindingArray( vector< vector< int > > &arrayVector );
	void SetPathfindingNode( XYCoords position, Node::NodeState newState );
	void SetObjectNode( XYCoords position, Node::NodeContains newObject );
	void CleanObjectNode( XYCoords position );
	void ClearPathfindingArray( void );
	void ClearObjectArray( void );
	
	/* A* (ish) Pathfinding Algorithm */
	XYCoords FindPath( XYCoords start, XYCoords target );
	
	/* External checks for character objects */
	bool IsNodeFree( XYCoords position );
	bool NodeContainsEnemy( XYCoords position );
	bool NodeContainsPlayer( XYCoords position );
	float GetObjectDistance( XYCoords currentPosition, XYCoords targetPosition );
	
private:
	PathController::PathController();
	static PathController* instance;
	
	int mWorldWidth, mWorldHeight;
	
	Node mActiveNode;  // point of reference node

	Node mSteps[8];    // legal moves array
	int mStepCounter;  // legal moves counter
	
	bool mTargetFound; // traceback kill flag
	
	priority_queue< Node, vector< Node >, greater< Node > > mWorkingSet;
	priority_queue< Node, vector< Node >, greater< Node > > mTracebackSet;
	
	/* A* Functions */
	void GenerateMoves( vector< vector< Node > > &arrayVector, Node currentNode ); 
	float GetDistance( Node currentNode, Node targetNode );
	
	/* Saftey Condition */
	bool CheckIfInRange( XYCoords position );
};

#endif