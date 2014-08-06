#ifndef _PLAYER_H_
#define _PLAYER_H_


#include "Character.h"
#include "PathController.h"


/*
=======================================
Player
Child class of Character
Deals with logic specific to the player
=======================================
*/
class Player : public Character {
public:
	Player::Player( float x, float y, float z );
	Player::~Player();
	
	void Update( void );
	
private:
	void SetState( void );
	void UpdatePathfindingPosition( void );
};

#endif