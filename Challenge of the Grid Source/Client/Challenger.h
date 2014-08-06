#ifndef _CHALLENGER_H_
#define _CHALLENGER_H_


// Includes //
#include "GameObject.h"
#include "Bullet.h"
#include "NetworkController.h"


// Challenger                //
// Child Class of GameObject //
// Shadow of Player Class    //
class Challenger : public GameObject {
public:
	Challenger( void );
	~Challenger( void );

	// Virtual Implementations
	void Update( void );
	void Render( void );

	// Bounds setter
	void SetBounds( sf::Vector2i newBounds );

	// Dead reckoning setter
	void NewDataReceived( PlayerObjectData newChallengerData );

	// Fire! - now simply adds the bullet as its direction is already known
	void Fire( /*GameObjectData newBulletData*/ void );

	// ChangeScore & GetScoreString
	void ChangeScore( sf::Uint32 newScore );
	std::string GetScoreString( void );

private:
	sf::Uint32 mScore; // challengers score
	float mTrajectory; // angle of challenger sprites rotation

	sf::Vector2i mBounds;

	// Challenger texture & sprite
	sf::Texture mTexture;
	sf::Sprite  mSprite;

	// Challenger score
	std::string mChallengerTextStr;
	std::string mChallengerScoreStr;

	std::string ReturnScore( void );

	void StayInWindow( void );
	void UpdateBullets( void );

	std::vector< Bullet > mBullets;

	bool mNewPacketReceived;
	PlayerObjectData mNewPacketData;
};


#endif