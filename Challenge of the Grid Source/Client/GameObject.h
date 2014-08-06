#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_


// Includes //
#include < SFML/Graphics.hpp >


// GameObject                                                  //
// Abstract Base Class for all the games sprite based objects  //
class GameObject {
public:
	GameObject( void );
	virtual ~GameObject( void );

	// Pure Virtual Functions
	virtual void Update( void ) = 0;
	virtual void Render( void ) = 0;

	// Position Getter & Setter
	sf::Vector2f GetPosition( void );
	sf::Vector2f GetDirection( void );
	void SetPosition( sf::Vector2f newPosition );
	void SetDirection( sf::Vector2f newDirection );

	// Radius getter for hit detection
	float GetRadius( void );

protected:
	sf::Vector2f mPosition;  // local coordinates
	sf::Vector2f mDirection; // direction vector
	float mRadius;           // object radius
	float mVelocity;         // objects velocity
};


#endif