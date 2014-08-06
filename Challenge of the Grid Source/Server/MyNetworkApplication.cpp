#include "MyNetworkApplication.h"


// Constructor                  //
// Initializing known variables //
MyNetworkApplication::MyNetworkApplication( void ) : 
	mTimeSinceLastUpdate ( sf::Time::Zero ) {
}


// Run                            //
// Initializes then runs the game //
void MyNetworkApplication::Run( void ) {
	Initialize();

	// Main loop
	while( true ) {
		// Restart returns the value of mClock before resetting
		sf::Time elapsedTime = mClock.restart();
		mTimeSinceLastUpdate += elapsedTime;

		// Loop @ 60 fps
		while( mTimeSinceLastUpdate > mTimePerFrame ) {
			mTimeSinceLastUpdate -= mTimePerFrame;

			// Update!
			Update();
		}
	}
}


// Initialize //
void MyNetworkApplication::Initialize( void ) {
}


// Update                                                                 //
// Checks the states of the Network & GameControllers to update correctly //
void MyNetworkApplication::Update( void ) {
	// Receive from clients
	mNetworkController.Receive();

	// Deal with clients
	DealWithClient( 0 );
	DealWithClient( 1 );

	// Exit condition
	if( sf::Keyboard::isKeyPressed( sf::Keyboard::Escape ) ) {
		exit( 0 );
	}
}


// DealWithClient //
void MyNetworkApplication::DealWithClient( int clientNumber ) {
	// If new data has been received for clientNumber
	if( mNetworkController.PacketReceived( clientNumber ) ) {
		// Set playerData struct
		mPlayerData[ clientNumber ] = mNetworkController.ReturnClientData( clientNumber );

		// If clientNumber has fired a new bullet
		if( mNetworkController.ClientHasFired( clientNumber ) > 0 ) {
			// Set clientNumber's new bullet flag
			mNetworkController.CreateClientPacket( mPlayerData[ clientNumber ], 1 );
		} else {
			// Pack player data
			mNetworkController.CreateClientPacket( mPlayerData[ clientNumber ], 0 );
		}

		// Send it other clientNumber
		if( mNetworkController.CheckNetworkState( NetworkController::ACTIVE ) ) {
			if( clientNumber == 0 ) {
				mNetworkController.Send( 1 );
			} else if ( clientNumber == 1 ) {
				mNetworkController.Send( 0 );
			}
		}
	}
}

