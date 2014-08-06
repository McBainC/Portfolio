#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_


/*
=============================================================
GameObject
Abstract Base Class for Game Depth Sorting
Contains a set of local coords and a getter for the z-coord -
for later depth sorting for correct draw order
=============================================================
*/
class GameObject {
public:
	GameObject::GameObject( float x, float y, float z );
	virtual GameObject::~GameObject();	
	
	/* Pure Virtual Functions */
	virtual void Update( void ) = 0;
	virtual void Render( void ) = 0;
	
	/* Depth Getter */
	float GetmZ( void ) const;

protected:
	float mX, mY, mZ; // objects local coords 
};

#endif