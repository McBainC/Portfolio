#include "Bullet.h"
#include "Window.h"


#include <math.h> // atan2


// Constructor                                         //
// Sets up the GameObject and the Enemy Objects sprite //
Bullet::Bullet( sf::Vector2f initialPosition, float trajectory, sf::Color color ) :
 GameObject() {
	SetPosition( initialPosition ); // set initial position
	mRadius = 3.0f;                 // new radius
	mVelocity = 12.0f;              // new velocity

	// Set bullet direction vector
	mDirection.x = cos( trajectory ) * mVelocity;
	mDirection.y = sin( trajectory ) * mVelocity;

	// Setup the bullet circle sprite
	mSprite.setPosition( mPosition );
	mSprite.setRadius( mRadius );
	mSprite.setOrigin( mRadius, mRadius );
	mSprite.setFillColor( color );
}


// Destructor         //
// Nothing to tidy up //
Bullet::~Bullet( void ) {
}


// Update                                                       //
// Move by its direction vector and update the sprites position //
void Bullet::Update( void ) {
	mPosition -= mDirection;
	mSprite.setPosition( mPosition );
}


// Render                                        //
// Render to render window through its singleton //
void Bullet::Render( void ) {
	Window::GetInstance()->mWindow.draw( mSprite );
}

