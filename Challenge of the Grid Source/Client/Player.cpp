#include "Player.h"
#include "Window.h"


#include <math.h>   // atan2
#include <iostream> // cout


// Constructor                                          //
// Sets up the GameObject and the Player Objects sprite //
Player::Player( void ) :
 GameObject(), mTrajectory( 0.0f ) {
	mRadius   = 32.0f; // new radius
	mVelocity = 2.0f;  // new velocity
	mScore    = 0;     // start @ 0

	// Load the player texture
	if( !mTexture.loadFromFile( "PlayerShip.png" ) ) {
		std::cout << "Error loading PlayerShip!" << std::endl;
	}
	mTexture.setSmooth( true );

	// Setup the player sprite (cyan)
	mSprite.setPosition( mPosition );
	mSprite.setOrigin( mRadius, mRadius );
	mSprite.setColor( sf::Color( 0, 255, 255, 255 ) );
	mSprite.setTexture( mTexture );

	// Setup player score text
	mPlayerTextStr = "Player: 0";
}


// Destructor        //
Player::~Player( void ) {
	mBullets.clear();
}


// SetBounds                                    //
// Gets window boundaries from window singleton //
void Player::SetBounds( sf::Vector2i newBounds ) {
	mBounds = newBounds;
}


// Fire                                           //
// Creates a bullet travelling towards the cursor //
void Player::Fire( void ) {
	Bullet newBullet( mPosition, mTrajectory, sf::Color( 0, 255, 255, 255 ) );
	mBullets.push_back( newBullet );
}


// ChangeScore //
void Player::ChangeScore( sf::Uint32 newScore ) {
	mScore = newScore;
}


// GetScoreString //
std::string Player::GetScoreString( void ) {
	return mPlayerTextStr;
}


// ReturnScore //
std::string Player::ReturnScore( void ) {
	char uint32String[ 10 ];
	std::sprintf( uint32String, "%i", mScore );
	return uint32String;
}


// Update                                 //
// Takes in keyboard input for movement   //
// And mouse position for turret rotation //
void Player::Update( void ) {
	// Add current score to the string
	mPlayerTextStr = "Player: ";
	mPlayerScoreStr = ReturnScore();
	mPlayerTextStr += mPlayerScoreStr;

	mDirection = sf::Vector2f( 0.0f, 0.0f );
	// WASD Movement 
	// Up
	if( sf::Keyboard::isKeyPressed( sf::Keyboard::W ) ) {
		mDirection.y = -mVelocity;
	}
	// Right
	if( sf::Keyboard::isKeyPressed( sf::Keyboard::D ) ) {
		mDirection.x = mVelocity;
	}
	// Down
	if( sf::Keyboard::isKeyPressed( sf::Keyboard::S ) ) {
		mDirection.y = mVelocity;
	}
	// Left
	if( sf::Keyboard::isKeyPressed( sf::Keyboard::A ) ) {
		mDirection.x = -mVelocity;
	}
	
	// Set trajectory
	// Get mouse coords relative to window
	sf::Vector2i mousePosition = sf::Mouse::getPosition( Window::GetInstance()->mWindow );
	
	// Get dy and dx 
	float dy = mPosition.y - mousePosition.y;
	float dx = mPosition.x - mousePosition.x;
	
	// Get angle to cursor
	mTrajectory = ( float )atan2( dy, dx );
	
	// Convert to degrees
	float angle = mTrajectory * 180.0f / 3.14159265f;
	
	// Rotate sprite towards cursor
	mSprite.setRotation( angle );

	// Keep player within window
	StayInWindow();

	// Update position
	mPosition += mDirection;

	// Update sprite positions
	mSprite.setPosition( mPosition );

	// Update Bullets
	UpdateBullets();
}


// Render         //
// Render sprites //
void Player::Render( void ) {
	// Render player
	Window::GetInstance()->mWindow.draw( mSprite );

	// Render bullets
	for( int i = 0; i < ( int ) mBullets.size(); i++ ) {
		mBullets[ i ].Render();
	}
}


// StayOnScreen                 //
// Keeps player object onscreen //
void Player::StayInWindow( void ) {
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


// UpdateBullets                                                               //
// Calls each bullets update and removes out them once they move out of bounds //
void Player::UpdateBullets( void ) {
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

