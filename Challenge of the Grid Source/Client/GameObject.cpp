#include "GameObject.h"


// Constructor                                                             //
// Sets the objects initial position and reference to the rendering window //
GameObject::GameObject( void )  :
 mPosition( sf::Vector2f( 0.0f, 0.0f ) ),
 mDirection( sf::Vector2f( 0.0f, 0.0f ) ),
 mRadius( 0.0f ), mVelocity( 0.0f ) {
}


// Destructor        //
// Nothing to tidyup // 
GameObject::~GameObject( void ) {
}


// GetPosition                                            //
// Returns SFML Vector2f for GameObjects current position //
sf::Vector2f GameObject::GetPosition( void ) {
	return mPosition;
}


// GetDirection                                                //
// Returns SFML Vector2f for the GameObjects current direction //
sf::Vector2f GameObject::GetDirection( void ) {
	return mDirection;
}


// SetPosition               //
// Sets GameObjects position //
void GameObject::SetPosition( sf::Vector2f newPosition  ) {
	mPosition = newPosition;
}


// SetDirection                          //
// Sets the GameObjects direction vector //
void GameObject::SetDirection( sf::Vector2f newDirection ) {
	mDirection = newDirection;
}


// GetRadius                  //
// Returns the objects radius //
float GameObject::GetRadius( void ) {
	return mRadius;
}

