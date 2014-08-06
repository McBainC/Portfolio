#ifndef _ENEMY_H_
#define _ENEMY_H_


#include "Character.h"
#include "PathController.h"


/*
===================================================
Enemy
Child class of Character
Has logic to move around randomly while not aggroed
When aggroed will chase and attack its target
===================================================
*/
class Enemy : public Character {
public:
	Enemy::Enemy( float x, float y, float z );
	Enemy::~Enemy();
	
	/* Enemy AI State */
	enum EnemyState {
		PATROLLING,
		AGGROED,
		DEFEATED
	};
	
	void Update( void );
	
	/* External check for becoming aggroed */
	void CheckAggro( XYCoords playerCoords );
	
private:
	EnemyState mAIState;
	
	int mAITimer;           // time between patrols
	bool mAtPatrolPosition; // flag to return to original position
	
	XYCoords mOrigin;       // original position
	XYCoords mPatrolTarget; // patrol position
	
	/* Enemy Update Fucntions */
	void Patrol( void );
	void SetState( void );
	void UpdatePathfindingPosition( void );  
	
	void RandomStart( void );
};

#endif