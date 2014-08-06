#ifndef _PLAYER_H_
#define _PLAYER_H_


// Includes //
#include "GameObject.h"
#include "Bullet.h"
#include "NetworkController.h"


// Player                                             //
// Child class of GameObject                          //  
// Has input based movement - turret faces the cursor //
// Contains and manages the players bullets           //
class Player : public GameObject {
public:
	Player( void );
	~Player( void );

	// Virtual Implementations
	void Update( void );
	void Render( void );

	// Bounds setter 
	void SetBounds( sf::Vector2i newBounds );

	// Fire!
	void Fire( void );

	// ChangeScore & GetScoreString
	void ChangeScore( sf::Uint32 newScore );
	std::string GetScoreString( void );

	float mTrajectory; // angle to cursor (radians)

	bool mHasFired; // player has fired

private:
	sf::Uint32 mScore; // players score
	
	sf::Vector2i mBounds; // window dimensions

	// Player texture & sprite 
	sf::Texture mTexture;
	sf::Sprite  mSprite;

	// Player score 
	std::string mPlayerTextStr;
	std::string mPlayerScoreStr;
	std::string ReturnScore( void );

	// Private functions
	void StayInWindow( void );
	void UpdateBullets( void );

	// Bullet container
	std::vector< Bullet > mBullets;
};


#endif