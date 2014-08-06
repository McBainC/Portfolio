#include "Challenger.h"
#include "Window.h"


#include <iostream> // cout


// Constructor                                              //
// Sets up the GameObject and the Challenger Objects sprite //
Challenger::Challenger( void ) :
 GameObject(), mTrajectory( 0.0f ) {
	mRadius   = 32.0f; // new radius
	mVelocity = 2.0f;  // new velocity
	mScore    = 0;     // start & 0

	// Load the player(challenger) texture
	if( !mTexture.loadFromFile( "PlayerShip.png" ) ) {
		std::cout << "Error loading ChallengerShip!" << std::endl;
	}
	mTexture.setSmooth( true );

	// Setup the challenger sprite (orange)
	mSprite.setPosition( mPosition );
	mSprite.setOrigin( mRadius, mRadius );
	mSprite.setColor( sf::Color( 255, 255, 0, 255 ) );
	mSprite.setTexture( mTexture );

	// Setup challenger score text
	mChallengerTextStr = "Challenger: 0";

	// Dead reckoning flag
	mNewPacketReceived = false;
}


// Destructor //
Challenger::~Challenger( void ) {
	mBullets.clear();
}


// SetBounds                                    //
// Gets window boundaries from window singleton //
void Challenger::SetBounds( sf::Vector2i newBounds ) {
	mBounds = newBounds;
}


// NewDataReceived                 //
// Set mNewPacketData and its flag //
void Challenger::NewDataReceived( PlayerObjectData newChallengerData ) {
	mNewPacketReceived = true;
	mNewPacketData = newChallengerData;
}


// Fire                                                  //
// Creates a new bullet at the challengers position      //
// Uses its trajectory to calculate its direction vector //
void Challenger::Fire( void ) {
	Bullet newBullet( mPosition, mTrajectory, sf::Color( 255, 255, 0, 255 ) );
	mBullets.push_back( newBullet );
}


// ChangeScore                  //
// Alters the challengers score //
void Challenger::ChangeScore( sf::Uint32 newScore ) {
	mScore = newScore;
}


// GetScoreString //
std::string Challenger::GetScoreString( void ) {
	return mChallengerTextStr;
}


// ReturnScore //
std::string Challenger::ReturnScore( void ) {
	char uint32String[ 10 ];
	std::sprintf( uint32String, "%i", mScore );
	return uint32String;
}


// Update //
void Challenger::Update( void ) {
	// If a new data packet has been received this loop
	if( mNewPacketReceived ) {
		// Use its data to update the challenger
		mPosition.x  = mNewPacketData.pX;
		mPosition.y  = mNewPacketData.pY;
		mDirection.x = mNewPacketData.dX;
		mDirection.y = mNewPacketData.dY;
		mTrajectory  = mNewPacketData.angle;
	// Otherwise dead reckoning
	} else {
		// Position by the last known direction vector
		mPosition += mDirection;
	}
	
	// Add current score to the string
	mChallengerTextStr = "Challenger: ";
	mChallengerScoreStr = ReturnScore();
	mChallengerTextStr += mChallengerScoreStr;

	// Convert to degrees
	float angle = mTrajectory * 180.0f / 3.14159265f;
	
	// Rotate sprite towards cursor
	mSprite.setRotation( angle );

	// Keep player within window
	StayInWindow();

	// Update sprite positions
	mSprite.setPosition( mPosition );

	// Update Bullets
	UpdateBullets();

	// Reset dead reckoning flag
	mNewPacketReceived = false;
}


// Render         //
// Render sprites //
void Challenger::Render( void ) {
	// Render player
	Window::GetInstance()->mWindow.draw( mSprite );

	// Render bullets
	for( int i = 0; i < ( int ) mBullets.size(); i++ ) {
		mBullets[ i ].Render();
	}
}


// StayOnScreen                      //
// Keep challenger within the window //
void Challenger::StayInWindow( void ) {
	// Left boundary
	if( ( mPosition.x - mRadius ) < 0.0f ) {
		mPosition.x = mRadius;
	}
	// Top boundary
	if( ( mPosition.y - mRadius ) < 0.0f ) {
		mPosition.y = mRadius;
	}
	// Right boundary
	if( mPosition.x > ( mBounds.x - mRadius ) ) {
		mPosition.x = ( mBounds.x - mRadius );
	}
	// Bottom boundary
	if( mPosition.y > ( mBounds.y - mRadius ) ) {
		mPosition.y = ( mBounds.y - mRadius );
	}
}


// UpdateBullets                                               //
// Calls each bullets update and removes out of bounds bullets //
void Challenger::UpdateBullets( void ) {
	// Using an iterator to loop through mBullets as outofbounds bullets are removed
	for( std::vector< Bullet >::iterator it = mBullets.begin(); it != mBullets.end(); ) {
		// Update
		it->Update();

		// Get bullets position
		sf::Vector2f position = it->GetPosition();

		// Get dy & dx (from center of window)
		float dx = position.x - ( mBounds.x / 2.0f );
		float dy = position.y - ( mBounds.y / 2.0f );

		// Radial detection without square root
		// 529984.0f = 728^2 
		// 725(distance from center to corner) + 3(bullet radius)
		if( ( ( dx * dx ) + ( dy * dy ) ) > 573049.0f ) {
			it = mBullets.erase( it ); // erase points to the bullet after the one removed
		} else {
			it++; // go to next bullet
		}
	}
}

