#include "Object.h"
#include "GameTextureController.h"
#include "ScreenViewController.h"
#include "PathController.h"


/*
==============================================
Constructor
Creates a GameObject, sets its variables
Sets its position within the pathfinding array
Sets its depth for draw order sorting
Creates it image PS2TexQuad
==============================================
*/
Object::Object( float x, float y, float z, float width, float height, int u, int v, int tWidth, int tHeight, float positionOffset, int textureNumber ) :
 GameObject( x, y, z ), mPositionOffset( positionOffset ), mTextureNumber( textureNumber ), mMapMaxDepth( 36 ) {
	if( mPositionOffset < 48.0f ) {
		mPositionOffset = 0.0f;
	}
	mArrayCoords = SCREENVIEW -> GetArrayXYCoords( mX, ( mY + mPositionOffset ), false );
	pVisualObject = new PS2TexQuad( mX, mY - mPositionOffset, mZ, width, height, u, v, tWidth, tHeight );
	PATHFINDER -> SetPathfindingNode( mArrayCoords, Node::BLOCKED );
	UpdateDepth();
}


/*
===============================
Destructor
Tidies up new and pointer usage
===============================
*/
Object::~Object() {
	if( pVisualObject ) {
		delete pVisualObject;
		pVisualObject = NULL;
	}
}


/*
===========================================
Update
Objects dont move - but the SCREENVIEW does
===========================================
*/
void Object::Update() {
	if( pVisualObject ) {
		pVisualObject -> MoveTo( ( mX + SCREENVIEW -> GetX() ), ( mY + SCREENVIEW -> GetY() ), mZ );
	}
}


/*
========================================
Render
Selects correct texture and draws object
========================================
*/
void Object::Render() {
	if( pVisualObject ) {
		GAMETEXTURES -> SelectTexture( mTextureNumber );
		pVisualObject -> Render();
	}
}


/* 
=================================================================================
UpdateDepth
Sets mZ correctly in relations to characters Y posistion in the pathfinding array
=================================================================================
*/
void Object::UpdateDepth( void ) {
	mZ = mArrayCoords.mY + mMapMaxDepth;
}