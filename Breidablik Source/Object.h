#ifndef _OBJECT_H_
#define _OBJECT_H_


#include "GameObject.h"
#include "primitives.h"
#include "XYCoords.h"


/*
===================================================
Object
Inherits from GameObject
Class to display non character objects on the level
===================================================
*/
class Object : public GameObject {
public:
	Object::Object( float x, float y, float z, float width, float height, int u, int v, int tWidth, int tHeight, float positionOffset, int textureNumber ); 
	Object::~Object();
	
	void Update( void );
	void Render( void );
	
private:
	float mPositionOffset;     // offset for sprite height
	int mTextureNumber;        // texture index
	int mMapMaxDepth;          // maps maximum depth
	XYCoords mArrayCoords;     // position within pathfinding array

	PS2TexQuad* pVisualObject; // objects sprite
	
	void UpdateDepth( void );
};


#endif