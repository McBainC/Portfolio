#pragma once


#include "Vector3.h"
#include <windows.h>


class Input;


/*
=======================================================
Camera
A Camera Object class - with functionality for movement
=======================================================
*/
class Camera {
public:
	Input& mInput;

	Camera( Input& input, int hWidth, int hHeight );
	~Camera( void );

	/* Camera Vectors */
	Vector3 mPosition; 
	Vector3 mForward;
	Vector3 mUp;
	Vector3 mRight;
	Vector3 mPointOfInterest;

	float mRoll, mPitch, mYaw; // axis rotations

	void Update( void );
	void AdjustScreenSize( int width,int height );

private:
	/* Vector Functions */
	void UpdateCursorCoords( void );
	void UpdateVectors( void );
	void MoveCamera( void );
	void RotateCamera( void );

	/* Camera Variables */
	float mNewRoll, mNewPitch, mNewYaw;
	float mForwardVelocity, mStrafeVelocity;         // velocity variables
	float mMaxForwardVelocity, mMaxBackwardVelocity; // max min velocites
	float mTurnVelocity;                             // turn veloicty
	int mCursorX, mCursorY;                          // cursor position
	int mHalfScreenWidth, mHalfScreenHeight;         // screen dimensions
};