#include "CameraClass.h"
#include "InputSingleton.h"


// CameraClass                                      //
// Default Constructor - initalizes know variables  //
CameraClass::CameraClass() :
 mPosition     ( D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) ),       // local position
 mRotation     ( D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) ),       // local rotation
 mForward      ( D3DXVECTOR3( 0.0f, 0.0f, 1.0f ) ),       // local forward vector
 mUp           ( D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) ),       // local up vector
 mGlobalUp     ( D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) ),       // global up vector
 mRight        ( D3DXVECTOR3( 1.0f, 0.0f, 0.0f ) ),       // local right vector
 mLookAt       ( D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) ),       // local - where camera is looking
 mGlobalLookAt ( D3DXVECTOR3( 0.0f, 0.0f, 1.0f ) ),       // global - where camera starts looking at
 mNewPitch( 0.0f ), mNewYaw( 0.0f ),                      // mouse rotation variables
 mForwardVelocity( 0.1f ), mStrafeVelocity( 0.1f ),       // camera movement velocities 
 mTurnVelocity( 0.1f ), mCursorX( 800 ), mCursorY( 450 ), // inital cursor location  
 mHalfScreenWidth( 800 ), mHalfScreenHeight( 450 ) {      // hard coded screen half-dimensions
}


// CameraClass //
// Constructor //
CameraClass::CameraClass( const CameraClass& other ) {
}


// CameraClass //
// Destructor  //
CameraClass::~CameraClass() {
}



// SetPosition                           //
// Set Cameras Position within the scene //
void CameraClass::SetPosition( float x, float y, float z ) {
	mPosition.x = x;
	mPosition.y = y;
	mPosition.z = z;

	return;
}


// SetRotation          //
// Set Cameras Rotation //
void CameraClass::SetRotation( float x, float y, float z ) {
	mRotation.x = x;
	mRotation.y = y;
	mRotation.z = z;

	return;
}


// SetLookAt          //
// Set Cameras LookAt //
void CameraClass::SetLookAt( float x, float y, float z ) {
	mLookAt.x = x;
	mLookAt.y = y;
	mLookAt.z = z;

	return;
}


// GetPosition //
D3DXVECTOR3 CameraClass::GetPosition() {
	return mPosition;
}


// GetRotation //
D3DXVECTOR3 CameraClass::GetRotation() {
	return mRotation;
}


// Render               // 
// Use Camera Variables //
void CameraClass::Render() {
	D3DXMATRIX rotationMatrix;
	
	// Setup the vector that points upwards
	mUp = mGlobalUp;

	// Setup where the camera is looking 
	mLookAt = mGlobalLookAt;

	// Create the rotation matrix from the yaw, pitch, and roll values
	D3DXMatrixRotationYawPitchRoll( &rotationMatrix, 
		                            ConvertToRadians( mRotation.y ), 
									ConvertToRadians( mRotation.x ), 
									ConvertToRadians( mRotation.z ) );

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin
	D3DXVec3TransformCoord( &mLookAt, &mLookAt, &rotationMatrix );
	D3DXVec3TransformCoord( &mUp, &mUp, &rotationMatrix );

	// Set forward vector
	mForward = mLookAt;

	// Get right vector
	D3DXVec3Cross( &mRight, &mUp, &mForward );
	D3DXVec3Normalize( &mRight, &mRight );

	// Translate the rotated camera position to the location of the viewer
	mLookAt = mPosition + mLookAt;

	// Finally create the view matrix from the three updated vectors
	D3DXMatrixLookAtLH( &mViewMatrix, &mPosition, &mLookAt, &mUp );

	return;
}


// ConvertToRadians //
float CameraClass::ConvertToRadians( float input ) {
	float temp = input * 0.0174532925f;

	return temp;
}


// GetViewMatrix //
void CameraClass::GetViewMatrix( D3DXMATRIX& viewMatrix ) {
	viewMatrix = mViewMatrix;

	return;
}


// RenderReflection //
void CameraClass::RenderReflection( float height ) {
	// Local copies for reflection
	D3DXVECTOR3 up, position, lookAt;

	// Setup the vector that points upwards
	up = mUp;

	// Setup the position of the camera in the world
	// For planar reflection invert the Y position of the camera
	position.x = mPosition.x;
	position.y = -mPosition.y + ( height * 2.0f );
	position.z = mPosition.z;

	// Setup where the camera is looking
	lookAt.x = mLookAt.x;
	lookAt.y = -mLookAt.y + ( height * 2.0f );
	lookAt.z = mLookAt.z;

	// Create the view matrix from the three vectors
	D3DXMatrixLookAtLH( &mReflectionViewMatrix, &position, &lookAt, &up );

	return;
}


// GetReflectionViewMatrix //
D3DXMATRIX CameraClass::GetReflectionViewMatrix() {
	return mReflectionViewMatrix;
}


// Update //
void CameraClass::Update() {
	UpdateCursorCoords();
	MoveCamera();
}


// UpdateCursorCoords //
void CameraClass::UpdateCursorCoords() {
	// Get mouse coords and offset to window center
	mCursorX = InputSingleton::GetInstance()->mMouseX - mHalfScreenWidth;
	mCursorY = InputSingleton::GetInstance()->mMouseY - mHalfScreenHeight;

	// Scale down for rotation
	mNewYaw   = float( mCursorX * 0.002f );
	mNewPitch = float( mCursorY * 0.002f );
}


// MoveCamera //
// Deals with cameras position and rotation changes through user input //
void CameraClass::MoveCamera() {
	// POSITION //

	// STRAFE //
	// Strafe left
	if( InputSingleton::GetInstance()->IsKeyDown('A') ) {
		mPosition -= ( mRight * mStrafeVelocity );
	// Strafe right
	} else if( InputSingleton::GetInstance()->IsKeyDown('D') ) {
		mPosition += ( mRight * mStrafeVelocity );
	}

	// VERTICAL //
	// Move up
	if( InputSingleton::GetInstance()->IsKeyDown('R') ) {
		mPosition += ( mUp * mStrafeVelocity );
	// Move down
	} else if( InputSingleton::GetInstance()->IsKeyDown('F') ) {
		mPosition -= ( mUp * mStrafeVelocity );
	}

	// FORWARD //
	// Move forward
	if( InputSingleton::GetInstance()->IsKeyDown('W') ) {
		mPosition += ( mForward * mForwardVelocity );
	// Move backward
	} else if( InputSingleton::GetInstance()->IsKeyDown('S') ) {
		mPosition -= ( mForward * mForwardVelocity );
	}

	// ROTATION //

	// PITCH
	// Look Up or Down 
	if( InputSingleton::GetInstance()->IsKeyDown( VK_NUMPAD8 ) ) {
		mRotation.x -= mTurnVelocity;
	} else if( InputSingleton::GetInstance()->IsKeyDown( VK_NUMPAD2 ) ) {
		mRotation.x += mTurnVelocity;
	}

	// YAW //
	// Turn Left or Right 
	if( InputSingleton::GetInstance()->IsKeyDown( VK_NUMPAD4 ) ) {
		mRotation.y -= mTurnVelocity;
	} else if( InputSingleton::GetInstance()->IsKeyDown( VK_NUMPAD6 ) ) {
		mRotation.y += mTurnVelocity;
	}

	// NOT REQUIRED //

	/*
	// ROLL //
	// Roll Left or Right 
	if( InputSingleton::GetInstance()->IsKeyDown( 'Q' ) ) {
		mRotation.z += mTurnVelocity;
	} else if( InputSingleton::GetInstance()->IsKeyDown( 'E' ) ) {
		mRotation.z -= mTurnVelocity;
	}
	*/

	// MOUSE //
	// Mouse rotation
	UpdateCursorCoords();
	if ( GetKeyState( VK_RBUTTON ) ) {
		mRotation.x += mNewPitch; 
		mRotation.y += mNewYaw;
	}
}

