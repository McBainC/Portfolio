#include "Camera.h"
#include "Input.h"
#include <math.h>


/*
==============================
Constructor
Setup to look foward initially
==============================
*/
Camera::Camera( Input& input, int hWidth, int hHeight  ) : 
 mInput( input ),                  // input reference
 mPosition( 0.0f, 0.0f, 6.0f ),    // initial position - infront of the screen
 mForward( 0.0f, 0.0f, -1.0f ),    // therefore forward is -1 on the z-axis
 mUp( 0.0f, 1.0f, 0.0f ),          // up is +1 on the y-axis
 mRight( 1.0f, 0.0f, 0.0f ),       // right is +1 on the x-axis
 mForwardVelocity( 0.0f ),         // starts now moving
 mStrafeVelocity( 0.02f ),         // constant low value
 mMaxForwardVelocity( 5.0f ),      
 mMaxBackwardVelocity( -2.0f ),
 mTurnVelocity( 0.5f ),
 mCursorX( 0 ), mCursorY( 0 ),   
 mHalfScreenWidth( hWidth ), 
 mHalfScreenHeight( hHeight ) {
	mRoll = mPitch = mYaw = 0.0f; // starting with 0 rotation
	mNewRoll = mNewPitch = mNewYaw = 0.0f;
}


/*
===================
Destructor
Nothing to clean up
===================
*/
Camera::~Camera( void ) {
}


/*
====================================
Update
Update vectors and reposition camera
====================================
*/
void Camera::Update( void ) {
	UpdateCursorCoords();
	MoveCamera();
	RotateCamera();
	UpdateVectors();
}


/*
==============================================
AdjustScreenSize
Alters screen variables when window is resized
==============================================
*/
void Camera::AdjustScreenSize( int width,int height ) {
	mHalfScreenWidth = width / 2; 
	mHalfScreenHeight = height / 2;
}


/*
======================================================================
UpdateCursorCoords
Takes input's mouse coords anfd moves them to the center of the screen
======================================================================
*/
void Camera::UpdateCursorCoords( void ) {
	/* Get MouseCoords and offset to center */
	mCursorX = mInput.mMouseX - mHalfScreenWidth;
	mCursorY = mInput.mMouseY - mHalfScreenHeight;

	/* Scale them down for rotation */
	mNewYaw = float( mCursorX * 0.005 );
	mNewPitch = float( mCursorY * 0.005 );
}


/*
=====================================================
Update
Updates the camera's forward/up/right and POI vectors
Using its roll/pitch and yaw variables
=====================================================
*/
void Camera::UpdateVectors() {
	/* Temp values for inputed roll/pitch/yaw */
	float cosR, cosP, cosY; 
	float sinR, sinP, sinY; 

	/* Calculations */
	cosY = cosf( float( mYaw * 3.1415 / 180 ) );
	cosP = cosf( float( mPitch * 3.1415 / 180 ) );
	cosR = cosf( float( mRoll * 3.1415 / 180 ) );
	sinY = sinf( float( mYaw * 3.1415 / 180 ) );
	sinP = sinf( float( mPitch * 3.1415 / 180 ) );
	sinR = sinf( float( mRoll * 3.1415 / 180 ) );

	/* Update forward vector with axis rotations */
	mForward.x = float( sinY * cosP );
	mForward.y = float( sinP );
	mForward.z = float( cosP * -cosY );

	/* Normalise forward vector */
	mForward.NormaliseSelf();

	/* Update up vector with axis rotations */
	mUp.x = float( -cosY * sinR - sinY * sinP * cosR );
	mUp.y = float( cosP * cosR );
	mUp.z = float( -sinY * sinR - sinP * cosR * -cosY );

	/* Normalise up vector  */
	mUp.NormaliseSelf();

	/* Right vector = cross of up and forward */
	mRight = CrossProduct( mUp, mForward );

	/* Up vector = cross of forward and right */
	mUp = CrossProduct( mForward, mRight );

	/* POI vector = sum of position and forward vectors */
	mPointOfInterest = ( mPosition + mForward );
}


/*
===============================================
MoveCamera
Change the camera's position through user input
Strafing with WASD and Z,X for throttle control
===============================================
*/
void Camera::MoveCamera( void ) {
	/* Left or Right - reversed! */
	if( mInput.A ) {
		mPosition += ( mRight * mStrafeVelocity );
	} else if( mInput.D ) {
		mPosition -= ( mRight * mStrafeVelocity );
	}

	/* Up or Down */
	if( mInput.W ) {
		mPosition += ( mUp * mStrafeVelocity );
	} else if( mInput.S ) {
		mPosition -= ( mUp * mStrafeVelocity );
	}

	/* Throttle up and down */
	if( mInput.Z ) {
		mForwardVelocity += 0.01f;
		if( mForwardVelocity > mMaxForwardVelocity ) {
			mForwardVelocity = mMaxForwardVelocity;
		}
	} else if( mInput.X ) {
		mForwardVelocity -= 0.01f;
		if( mForwardVelocity < mMaxBackwardVelocity ) {
			mForwardVelocity = mMaxBackwardVelocity;
		}
	}

	/* DeadStop */
	if( mInput.mKeys[ VK_TAB ] ) {
		mForwardVelocity = 0.0f;
	}

	/* Update Position Vector */
	mPosition += ( mForward * mForwardVelocity );
}


/*
========================================
RotateCamera
Rotate the camera through mouse position
(with right mouse button down)
Or by numberpad 
========================================
*/
void Camera::RotateCamera( void ) {
	/* If right mouse is down */
	if( GetKeyState( VK_RBUTTON ) ) {
		mYaw += mNewYaw;
		mPitch -= mNewPitch;
	}
	
	/* Roll Left or Right */
	if( mInput.Q ) {
		mRoll += mTurnVelocity;
	} else if( mInput.E ) {
		mRoll -= mTurnVelocity;
	}

	/* Turn Left or Right */
	if( mInput.mKeys[ VK_NUMPAD4 ] ) {
		mYaw -= mTurnVelocity;
	} else if( mInput.mKeys[ VK_NUMPAD6 ] ) {
		mYaw += mTurnVelocity;
	}

	/* Look Up or Down */
	if( mInput.mKeys[ VK_NUMPAD8 ] ) {
		mPitch += mTurnVelocity;
	} else if( mInput.mKeys[ VK_NUMPAD2 ] ) {
		mPitch -= mTurnVelocity;
	}
}