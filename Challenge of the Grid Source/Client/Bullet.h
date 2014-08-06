#ifndef _BULLET_H_
#define _BULLET_H_


// Includes //
#include "GameObject.h"


// Bullet                                                             //
// Bullet class requires a trajectory angle to calculate its velocity //
class Bullet : public GameObject {
public:
	Bullet( sf::Vector2f initialPosition, float trajectory, sf::Color color );
	~Bullet( void );

	// Virtual Implementations
	void Update( void );
	void Render( void );

private:
	sf::CircleShape mSprite; // bullet sprite
};


#endif