#ifndef _CURSOR_H_
#define _CURSOR_H_


#include "primitives.h"
#include "XYCoords.h"


/*
============================================================
Cursor
Cursor functionality - move by left analogue stick input
Image changes depending on GameState and what cursor is over
============================================================
*/
class Cursor {
public:
	Cursor::Cursor();
	Cursor::~Cursor();
	
	PS2TexQuad* pCursorVisualObject; 
	
	enum CursorState {
		MENU,
		MOVEMENT,
		ATTACKING
	};
	
	void Update( float xAxisValue, float yAxisValue );
	void Render( void );
	
	/* Getter for cursors position XYCoords */
	XYCoords GetArrayXYCoords( void ) const;
	
	/* External Reset */
	void Reset( void );
	
	/* External State Functions */
	bool CheckState( CursorState testState );
	void SetState( CursorState newState );
	
private:
	float mX, mY;                              // x & y coords
	float mHalfScreenWidth, mHalfScreenHeight; // screen boundaries
	int mU, mV;                                // visualObject UV's
	XYCoords mArrayCoords;		               // x & y positions in pathfinding array         
	float mSpeed;                              // movement speed
	CursorState mState;                        // state changes CursorVisualObject
	
	/* Cursor Update Functions */
	void StayOnScreen( void );
	void SetArrayPosition( void );
	void UpdateVisualObject( void );
	void UpdateCursorState( void );
};

#endif