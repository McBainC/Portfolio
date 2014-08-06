#ifndef _MYNETWORKAPPLICATION_H_
#define _MYNETWORKAPPLICATION_H_


#include <SFML/Graphics.hpp> // sfml graphics
#include <SFML/Network.hpp>  // sfml network
#include <iostream>          // cout


// Includes //
#include "NetworkController.h"


// 10th of a second
static const sf::Time mTimePerFrame = sf::seconds( 1.0f / 10.0f );


// MyNetworkApplication //
class MyNetworkApplication {
public:
	MyNetworkApplication( void );
	void Run( void );

private:
	// Initialize & Update
	void Initialize( void );
	void Update( void );

	void DealWithClient( int clientNumber );

	void AddBullet( int clientNumber );

	// Controller objects
	NetworkController mNetworkController;
	
	// Server client data
	PlayerObjectData mPlayerData[ 2 ];
	
	// Timer
	sf::Clock mClock;
	sf::Time mTimeSinceLastUpdate;
};


#endif