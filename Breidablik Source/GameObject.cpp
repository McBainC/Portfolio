#include "GameObject.h"


/*
=====================================
Constructor
Base constructor setting local coords
To be called by child classes
=====================================
*/
GameObject::GameObject( float x, float y, float z ) :
 mX( x ), mY( y ), mZ( z ) {
}


/*
============================================
Destructor
Required empty body as Destructor is virtual
============================================
*/
GameObject::~GameObject() {
}


/*
=====================================
GetmZ
Returns the z-depth of the GameObject
=====================================
*/
float GameObject::GetmZ( void ) const {
	return mZ;
}