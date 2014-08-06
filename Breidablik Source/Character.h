#ifndef _CHARACTER_H_
#define _CHARACTER_H_


#include "GameObject.h"
#include "primitives.h"
#include "LevelData.h"
#include "XYCoords.h"


/*
================================================
Character
Inherits from GameObject
Base class for player and enemy classes
Contains generic logic and sprite animation code
================================================
*/
class Character : public GameObject {
public:
	Character::Character( float x, float y, float z, int spriteSheetIndex );
	virtual Character::~Character();
	
	/* CharacterState defines what type of texture should be displayed */
	enum CharacterState {
		IDLE,
		MOVING,
		ATTACKING,
		ACTION,
		DIEING
	};
	
	bool CheckCharacterState( CharacterState testState );
	void ChangeCharacterState( CharacterState newState );
	
	/* CharacterDirection defines which direction should be displayed */
	enum CharacterDirection {
		NORTH,
		NORTH_EAST,
		EAST,
		SOUTH_EAST,
		SOUTH,
		SOUTH_WEST,
		WEST,
		NORTH_WEST
	};
	
	void Render( void );
	
	/* Getters for characters current and target XYCoords */
	XYCoords GetArrayXYCoords( void ) const;
	XYCoords GetTargetXYCoords( void ) const;
	
	/* External Setter for characters target XYCoords */
	void SetTarget( XYCoords target );
	
	/* External functions for dealing/taking damage */
	bool CheckIfDealingDamage( void );
	int WeaponDamage( void ) const;
	void TakeDamage( int incomingDamage );
	
protected:
	float mHorizontalSpeed, mVerticalSpeed; // movement variables
	float mHealthWidth;                     // healthbar sprite width
	int mU, mV, mW, mH;                     // animation texture variables
	int mInitialSpriteSheetIndex;           // marks where character sprite sheets begin
	int mSpriteSheetIndex;                  // marks current character sprite sheet for GameTextures
	int mAnimCountMax, mAnimCounter;        // animation variables
	int mActionCounter;                     // action counter
	int mHealth;                            // amount of damage can be taken before dieing         
	int mWeaponDamage;                      // amount of damage per swing
	
	XYCoords mArrayCoords;                  // position within pathfinding array
	XYCoords mNextCoords;                   // next node within pathfinding array
	XYCoords mTargetCoords;                 // target within pathfinding array
	
	PS2TexQuad* pCharacterHealthBar;        // health bar sprite
	PS2TexQuad* pCharacterSprite;           // character sprite
	PS2Polygon* pCharacterShadow;           // shadow polygon

	CharacterState mState;                  // current state
	CharacterState mNextState;              // next active state
	CharacterDirection mDirection;          // current direction
	CharacterDirection mNextDirection;      // next direction to turn towards
	
	bool mIsPathfinding;                    // flag to allow only one pathfinding search per action
	bool mIsActive;                         // flag to lock out state changes during an action
	bool mIsAlive;                          // flag to signal death
	bool mIsDealingDamage;                  // flag to signal this character has hit another
	
	virtual void SetState( void ) = 0;      // each child class will have a SetState
	
	/* Character Logic Functions */
	void MoveCharacter( void );
	void UpdateDepth( void );
	void CheckHealth( void );
	void UpdateHealthBar( void );
	void SetSpeedAndSpriteVariables( void );
	void Animate( void );
	void SetArrayPosition( void );
	void CheckState( void );
	void PerformAction( void );
	void SetDirection( void );
};

#endif