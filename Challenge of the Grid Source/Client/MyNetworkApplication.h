#ifndef _MYNETWORKAPPLICATION_H_
#define _MYNETWORKAPPLICATION_H_


#include <SFML/Graphics.hpp> // sfml graphics
#include <SFML/Network.hpp>  // sfml network
#include <iostream>          // cout


// Includes //
#include "GameController.h"
#include "NetworkController.h"
#include "Player.h"
#include "Challenger.h"


// 10th of a second
static const sf::Time mTimePerFrame = sf::seconds( 1.0f / 10.0f );


// MyNetworkApplication //
class MyNetworkApplication {
public:
	MyNetworkApplication( void );
	void Run( void );

private:
	void Initialize( void );
	void Events( void );
	void Update( void );
	void Render( void );

	void UpdatePlayer( void );
	void UpdateChallenger( void );
	
	GameController mGameController;
	NetworkController mNetworkController;

	// Background texture and sprite
	sf::Texture mBackgroundTexture;
	sf::Sprite mBackgroundSprite;

	// DestructoBeam font
	sf::Font mDestructoBeamFont;
	
	// Game text
	sf::Text mChallengeOfTheGrid;
	sf::Text mPrepareForBattle;
	sf::Text mPlayerScore;
	sf::Text mChallengerScore;
	
	// Player & Challenger objects
	Player mPlayer; 
	Challenger mChallenger;

	// Player has fired;
	bool mPlayerHasFired;

	// Timer
	sf::Clock mClock;
	sf::Time mTimeSinceLastUpdate;
};


#endif