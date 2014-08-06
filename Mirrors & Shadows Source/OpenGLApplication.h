#pragma once


#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include "Texture.h"
#include "Camera.h"
#include "Cube.h"
#include "Circle.h"
#include "Sphere.h"
#include "Cone.h"
#include "MilkshapeModel.h"
#include "TextureLoader.h"
#include "TextureName.h"


/*
======================================================================================
OpenGLApplication
< Graphics Programming >
Contains code to generate the "Mirrors and Shadows" scene 
< Operating Enviroments >
Texture loading and generation of the sphere have been threaded
References for Stencil Buffer:
http://nehe.gamedev.net/tutorial/clipping__reflections_using_the_stencil_buffer/17004/
https://developer.nvidia.com/sites/default/files/akamai/gamedev/docs/stencil.pdf
======================================================================================
*/
class OpenGLApplication {
public:
	/* Input and Camera References */
	Input& mInput;
	Camera& mCamera;

	OpenGLApplication( Input& input, Camera& camera );
	~OpenGLApplication( void );

	void Init( void );
	void Render ( void );

	bool mDisplayControls; // flag if control scheme is to be displayed

	long long int mStartTime, mFinishTime, mTotalTime;
	long long int mSphereThreadTime, mTextureThreadTime;
	long long int GetTime( void );

private:
	bool mRunning;
	
	/* Threaded Texture Loading */
	TextureLoader mTextureLoader;
	TextureCreator mTextureCreator;

	/* Scene Draw Functions */
	void DrawGoodSkyBox( void );
	void DrawEvilSkyBox( void );
	void DrawGoodScene( void );
	void DrawEvilScene( void );
	void DrawShadow( void );
	void DrawJumpPoint( void );
	void DrawOverlay( void );

	/* Scene Shapes */
	Sphere mSphere;
	Circle mCircle;
	Cone mCone;

	/* MilkShake Model */
	Model* pShadowShip; 

	/* Scene Rotation Variables */
	float mRotation; 
	double mTex; 

	/* Scene Textures */
	GLuint GoodSkyBoxTexture;
	GLuint EvilSkyBoxTexture;
	GLuint SunTexture;
	GLuint DeadSunTexture;
	GLuint GoodPlanetTexture;
	GLuint CloudTexture;
	GLuint MoonTexture;
	GLuint DeadPlanetTexture;
	GLuint RingTexture;
	GLuint JumpPointTexture;
	GLuint SkinTexture1;
	GLuint SkinTexture2;
	GLuint ControlsTexture;
	GLuint HUDTexture;
};