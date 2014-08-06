#ifndef _CAMERACLASS_H_
#define _CAMERACLASS_H_


// Includes //
#include <d3dx10math.h>


// CameraClass                                  //
// Object that represents our view of the scene //
// Has been rewritten to only use DX functions  //
// The relfection view matrix is a cheat...     //
class CameraClass {
public:
	CameraClass();
	CameraClass( const CameraClass& );
	~CameraClass();

	void SetPosition( float, float, float );
	void SetRotation( float, float, float );
	void SetLookAt( float, float, float );

	D3DXVECTOR3 GetPosition();
	D3DXVECTOR3 GetRotation();

	void Render();
	void GetViewMatrix( D3DXMATRIX& );

	void RenderReflection( float );
	D3DXMATRIX GetReflectionViewMatrix();

	void Update();

private:
	// Camera Vectors 
	D3DXVECTOR3 mPosition;
	D3DXVECTOR3 mRotation;
	D3DXVECTOR3 mForward;
	D3DXVECTOR3 mUp;
	D3DXVECTOR3 mGlobalUp;
	D3DXVECTOR3 mRight;
	D3DXVECTOR3 mLookAt;
	D3DXVECTOR3 mGlobalLookAt;

	// Camera Variables 
	float mNewPitch, mNewYaw;
	float mForwardVelocity, mStrafeVelocity;         
	float mTurnVelocity;                             

	// Mouse Variables 
	int mCursorX, mCursorY;                         
	int mHalfScreenWidth, mHalfScreenHeight;         

	// View Matrices
	D3DXMATRIX mViewMatrix;
	D3DXMATRIX mReflectionViewMatrix;

	// Camera Functions
	void UpdateCursorCoords();
	void MoveCamera();
	float ConvertToRadians( float input );
};


#endif