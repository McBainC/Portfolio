#include "OpenGLApplication.h"


/* Static Light Variables */
static GLfloat Light_Ambient[] = { 0.4f, 0.4f, 0.4f, 1.0f };
static GLfloat Light_Diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
static GLfloat Light_Position[]= { 0.0f, 0.0f, 1.5f, 1.0f };


/* 
================================
Constructor
Sets Input and Camera References
Initializes variables
================================
*/
OpenGLApplication::OpenGLApplication( Input& input, Camera& camera ) :
 mInput( input ), mCamera( camera ), 
 mDisplayControls( false ), mRunning( true ),
 mRotation( 0.0f ), mTex( 0.0f ) {
}


/*
=============================
Destructor
Tidy up new and pointer usage
=============================
*/
OpenGLApplication::~OpenGLApplication() {
	if( pShadowShip ) {
		delete pShadowShip;
		pShadowShip = NULL;
	}
}


/*
===============================================
GetTime
To return time taken for threaded functionality
===============================================
*/
long long int OpenGLApplication::GetTime( void ) {
	struct _timeb timebuffer;
	_ftime64_s( &timebuffer );
	return ( timebuffer.time * 1000LL ) + timebuffer.millitm;
}


/*
=============================================================
Init
Loads in the Milkshake model (a shadow vessel from Babylon 5)
Loads in the tga texture files (one thread per texture)
Creates OpenGL textures for the scene
=============================================================
*/
void OpenGLApplication::Init( void ) {
	/* Load Milkshake Model */
	pShadowShip = new MilkshapeModel();                                 // create a new milkshapemodel
	if( pShadowShip -> loadModelData( "SHADOW_SHIP.ms3d" ) == false ) {	// attempt to load the model
		MessageBox( NULL, "Couldn't load the model data/model.ms3d",
			        "Error", MB_OK | MB_ICONERROR );                    // if the model fails to load output error
	}
	
	/* Load in Textures with threads */
	mTextureLoader.Init();         // sets up thread data structs

	/* Time Threading */
	mStartTime = GetTime();
	mTextureLoader.LoadTextures(); // loads textures with threading

	/* Create Textures */
	mTextureCreator.CreateGLTexture( mTextureLoader.mTextures[ 0 ], GoodSkyBoxTexture );
	mTextureCreator.CreateGLTexture( mTextureLoader.mTextures[ 1 ], EvilSkyBoxTexture );
	mTextureCreator.CreateGLTexture( mTextureLoader.mTextures[ 2 ], SunTexture );
	mTextureCreator.CreateGLTexture( mTextureLoader.mTextures[ 3 ], DeadSunTexture );
	mTextureCreator.CreateGLTexture( mTextureLoader.mTextures[ 4 ], GoodPlanetTexture );
	mTextureCreator.CreateGLTexture( mTextureLoader.mTextures[ 5 ], CloudTexture );
	mTextureCreator.CreateGLTexture( mTextureLoader.mTextures[ 6 ], MoonTexture );
	mTextureCreator.CreateGLTexture( mTextureLoader.mTextures[ 7 ], DeadPlanetTexture );
	mTextureCreator.CreateGLTexture( mTextureLoader.mTextures[ 8 ], RingTexture );
	mTextureCreator.CreateGLTexture( mTextureLoader.mTextures[ 9 ], JumpPointTexture );	
	mTextureCreator.CreateGLTexture( mTextureLoader.mTextures[ 10 ], SkinTexture1 );
	mTextureCreator.CreateGLTexture( mTextureLoader.mTextures[ 11 ], SkinTexture2 );
	mTextureCreator.CreateGLTexture( mTextureLoader.mTextures[ 12 ], ControlsTexture );
	mTextureCreator.CreateGLTexture( mTextureLoader.mTextures[ 13 ], HUDTexture );

	/* Get TimeTaken */
	mFinishTime = GetTime();
	mTotalTime = mFinishTime - mStartTime;
}


/*
=================
Render
Creates the scene
=================
*/
void OpenGLApplication::Render ( void ) {
	/* Get Threading Times */
	mSphereThreadTime = mSphere.mTotalTime;
	mTextureThreadTime = mTotalTime;

	/* Proper SkyBox */
	DrawGoodSkyBox();
	
	/* Setup JumpPoint */
	
	/* Enable the stencil testing */
	glEnable( GL_STENCIL_TEST ); 
		glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE ); // disable the colour mask
		glStencilFunc( GL_ALWAYS, 1, 1 );                      // set stencil buffer to true for what follows
		glStencilOp( GL_KEEP, GL_KEEP, GL_REPLACE );              
		glDisable( GL_DEPTH_TEST );  
		mCircle.Draw();                                        // draw object to stencil buffer
		glEnable( GL_DEPTH_TEST );
		glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );     // enable the colour mask
		glStencilFunc( GL_EQUAL, 1, 1 );
		glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );              // set the stencil buffer to keep our next lot of data

		/* Reflected (Evil) Scene */
		glPushMatrix();
			glScalef( 1.0f, 1.0f, -1.0f ); // flip the reflection 

			/* Dont draw if camera is behind 'mirror' */
			if( mCamera.mPosition.z < 0 ) {
				glDisable( GL_LIGHT1 ); // disable 'Good' scenes sun while rendering
				DrawEvilSkyBox();       // fake (giant cube) skybox - so it can only be seen through a stencil test
				DrawEvilScene();
				DrawShadow();
			} 
		glPopMatrix();

	/* Disable the stencil testing */
	glDisable( GL_STENCIL_TEST ); 
	
	/* Good Scene */
	glDisable( GL_LIGHT2 ); // disable 'Evil" scenes sun while rendering
	DrawGoodScene();

	/* The JumpPoint */
	DrawJumpPoint();

	/* Display HUD / Controls */
	DrawOverlay();

	/* Alter Variables */
	mRotation += 1;
	mTex += 0.001;
}


/*
======================================================================
DrawOverlay
Renders a quad aligned to the camera textured with the HUD or controls
======================================================================
*/
void OpenGLApplication::DrawOverlay( void ) {
	glPushMatrix();
	    /* Slightly infront of camera */
		glTranslatef( mCamera.mPosition.x + mCamera.mForward.x,
		              mCamera.mPosition.y + mCamera.mForward.y, 
					  mCamera.mPosition.z + mCamera.mForward.z );

		/* Align to camera */
		glRotatef( mCamera.mRoll, 0, 0, 1 );
		glRotatef( mCamera.mPitch, 1, 0, 0 ); 
		glRotatef( -mCamera.mYaw, 0, 1, 0 );  
		glEnable( GL_BLEND ); 
		glDisable( GL_LIGHTING ); // not affected by lighting 
		glColor4f( 1, 1, 1, GLfloat( 0.7 ) );
		glBlendFunc( GL_ONE, GL_ONE );

		/* Either HUD or Controls */
		if( mDisplayControls ) {
			glBindTexture( GL_TEXTURE_2D, ControlsTexture );
		} else {
			glBindTexture( GL_TEXTURE_2D, HUDTexture );
		}

		/* Draw Rectangle */
		glBegin( GL_QUADS );
			glNormal3f( 0.0f, 0.0f, 1.0f ); 
			glTexCoord2f( 1.0f, 0.0f );
			glVertex3f( 0.4f, -0.4f, 0.0f );
			glNormal3f( 0.0f, 0.0f, 1.0f ); 
			glTexCoord2f( 1.0f, 1.0f );
			glVertex3f( 0.4f, 0.4f, 0.0f );
			glNormal3f( 0.0f, 0.0f, 1.0f ); 
			glTexCoord2f( 0.0f, 1.0f );
			glVertex3f( -0.4f, 0.4f, 0.0f );
			glNormal3f( 0.0f, 0.0f, 1.0f ); 
			glTexCoord2f( 0.0f, 0.0f );
			glVertex3f( -0.4f, -0.4f, 0.0f );
		glEnd();
		glDisable( GL_BLEND ); 
		glEnable( GL_LIGHTING ); 
	glPopMatrix();
}


/*
================================================================================
DrawGoodSkyBox
Creates and renders the 'Good' Skybox as a unit cube where the camera is located
================================================================================
*/
void OpenGLApplication::DrawGoodSkyBox( void ) {
	glPushMatrix();
		glDisable( GL_LIGHTING );                                                      // lighting not to affect it
		glDisable( GL_DEPTH_TEST );                                                    // no depth testing
		glTranslatef( mCamera.mPosition.x, mCamera.mPosition.y, mCamera.mPosition.z ); // move to cameras location 
		glBindTexture( GL_TEXTURE_2D, GoodSkyBoxTexture );
		Draw_SKYBOX();
		glEnable( GL_DEPTH_TEST );
		glEnable( GL_LIGHTING );
	glPopMatrix();
}


/*
==================================================================================
DrawEvilSkyBox
Creates and renders the 'Evil' *Sky*Box - unlike the Good this one is a large cube
So when it is seen through the Stencil Mask it will draw as a reflected version
==================================================================================
*/
void OpenGLApplication::DrawEvilSkyBox( void ) {
	glPushMatrix();
		glDisable( GL_LIGHTING );
		glScalef( 75.0f, 75.0f, 75.0f ); // big enough to cover whole scene
		glBindTexture( GL_TEXTURE_2D, EvilSkyBoxTexture );
		Draw_SKYBOX();
		glEnable( GL_LIGHTING );
	glPopMatrix();
}


/*
========================
DrawGoodScene
Creates the 'Good' scene
========================
*/
void OpenGLApplication::DrawGoodScene( void ) {
	/* The Sun */
	glPushMatrix();
		glTranslatef( 10, 0, -50 );
		glPushMatrix();
			/* Disable lighting for the sun */
			glDisable( GL_LIGHTING );
			glScalef( 4.0f, 4.0f, 4.0f );
			glRotatef( ( GLfloat )( mRotation * 0.02 ), 0, 1, 0 );
			glBindTexture( GL_TEXTURE_2D, SunTexture );
			mSphere.Draw();  
			glEnable( GL_LIGHTING );

			/* Light 1 */
			glLightfv( GL_LIGHT1, GL_AMBIENT,  Light_Ambient );
			glLightfv( GL_LIGHT1, GL_DIFFUSE,  Light_Diffuse );
			glLightfv( GL_LIGHT1, GL_POSITION, Light_Position );
			glEnable( GL_LIGHT1 );
		glPopMatrix();

		/* Orbit of the Sun */
		glPushMatrix();
			/* Planets orbit */
			glRotatef( GLfloat( -25 + ( mRotation * 0.002 ) ), 0, 1, 0 ); 
			glTranslatef( 0, 0, 40 );

			/* Planet */
			glPushMatrix();
				/* Planets Rotation */
				glRotatef( ( GLfloat )( mRotation * 0.03 ), 0, 1, 0 ); 
				glScalef( 0.99f, 0.99f, 0.99f );
				glBindTexture( GL_TEXTURE_2D, GoodPlanetTexture );
				mSphere.Draw();                                  
			glPopMatrix();

			/* Planets Clouds */
			glPushMatrix();
				/* Clouds Rotation */
				glRotatef( GLfloat( mRotation * 0.05 ), 0, 1, 0 ); 
				glEnable( GL_BLEND );                                  
				glColor4f( 1, 1, 1, GLfloat( 0.7 ) );
				glBlendFunc( GL_ONE, GL_ONE ); 
				glBindTexture( GL_TEXTURE_2D, CloudTexture );
				mSphere.Draw();
				glDisable( GL_BLEND );
			glPopMatrix();

			/* Planets Moon */
			glPushMatrix();
				/* Moons Orbit */
				glRotatef( GLfloat( -25 + ( mRotation * 0.05 ) ), 0, 1, 0 ); 
				glTranslatef( 0, 0, 6 );
				glPushMatrix();
					/* Moons Rotation */
					glRotatef( GLfloat( mRotation * 0.05 ), 0, 1, 0 ); 
					glScalef( 0.25f, 0.25f, 0.25f );
					glBindTexture( GL_TEXTURE_2D, MoonTexture );
					mSphere.Draw();    
				glPopMatrix();
			glPopMatrix();
		glPopMatrix();
	glPopMatrix();    
}


/*
===============================================
DrawEvilScene
Creates the 'Evil' ie reflected scene
The moon has been destroyed - replaced by rings
===============================================
*/
void OpenGLApplication::DrawEvilScene( void ) {
	/* The Sun */
	glPushMatrix();
		glTranslatef( 10, 0, -50 );
		glPushMatrix();
			/* Disable lighting for the sun */
			glDisable( GL_LIGHTING );
			glScalef( 4.0f, 4.0f, 4.0f );
			glRotatef( ( GLfloat )( mRotation * 0.02 ), 0, 1, 0 );
			glBindTexture( GL_TEXTURE_2D, DeadSunTexture );
			mSphere.Draw();  
			glEnable( GL_LIGHTING );

			/* Light 2 */
			glLightfv( GL_LIGHT2, GL_AMBIENT,  Light_Ambient );
			glLightfv( GL_LIGHT2, GL_DIFFUSE,  Light_Diffuse );
			glLightfv( GL_LIGHT2, GL_POSITION, Light_Position );
			glEnable( GL_LIGHT2 );
		glPopMatrix();

		/* Orbit of the Sun */
		glPushMatrix();
			/* Planets Orbit */
			glRotatef( GLfloat( -25 + ( mRotation * 0.002 ) ), 0, 1, 0 );
			glTranslatef( 0, 0, 40 );

			/* Planet */
			glPushMatrix();
				/* Planets Rotation */
				glRotatef( ( GLfloat )( mRotation * 0.05 ), 0, 1, 0 );
				glScalef( 0.99f, 0.99f, 0.99f );
				glBindTexture( GL_TEXTURE_2D, DeadPlanetTexture );
				mSphere.Draw();                                  
			glPopMatrix();

			/* Planets Rings */
			glPushMatrix();
				/* Slight Rotation - so rings can be seen */
				glRotatef( -80 , 1, 0, 0 );
				glScalef( 7.0f, 7.0f, 7.0f );
				glEnable( GL_BLEND );                                  
				glColor4f( 1, 1, 1, GLfloat( 0.7 ) );
				glBlendFunc( GL_ONE, GL_ONE ); 
				glBindTexture( GL_TEXTURE_2D, RingTexture );
				mCircle.Draw();
				glDisable( GL_BLEND );
			glPopMatrix();
		glPopMatrix();
	glPopMatrix();   
}


/*
====================================================================
DrawShadow
Its a 'Shadow' Vessel - http://babylon5.wikia.com/wiki/Shadow_Vessel
Draws the model twice - to give the look of being multi-textured
====================================================================
*/
void OpenGLApplication::DrawShadow( void ) {
	glPushMatrix();
		/* Move to cameras position minus a little */
		glTranslatef( mCamera.mPosition.x, mCamera.mPosition.y, ( mCamera.mPosition.z - 2 ) );

		/* Negate all camera variables as 'Evil' scene has already been flipped */
		glRotatef( -mCamera.mRoll, 0, 0, 1 );
		glRotatef( ( -mCamera.mPitch + 270 ), 1, 0, 0 ); // +270 degree correction for model orientation correction 
		glRotatef( -mCamera.mYaw, 0, 0, 1 );           
		glScalef( 0.1f, 0.1f, 0.1f );                    // its huge / scenes tiny
		/* Render First Layer */
		glPushMatrix();
			glScalef( 0.999f, 0.999f, 0.999f );

			/* Animate Shadow Vessel Texture */
			glMatrixMode( GL_TEXTURE );
			glPushMatrix();
				glTranslatef( GLfloat( mTex ), 0, 0 );
				glMatrixMode( GL_MODELVIEW );
				glBindTexture( GL_TEXTURE_2D, SkinTexture1 );
				pShadowShip -> draw();

				/* Stop Animating Textures */
				glMatrixMode( GL_TEXTURE );
			glPopMatrix();
			glMatrixMode( GL_MODELVIEW );
		glPopMatrix();

		/* Render 2nd Layer */
		glPushMatrix();
			glEnable( GL_BLEND );
			glColor4f( 1, 1, 1, GLfloat( 0.7 ) );
			glBlendFunc( GL_ONE, GL_ONE );

			/* Animate Shadow Vessel Texture */
			glMatrixMode( GL_TEXTURE );
			glPushMatrix();
				glTranslatef( GLfloat( -mTex ), 0, 0 );
				glMatrixMode( GL_MODELVIEW );
				glScalef( 1.001f, 1.001f, 1.001f );
				glBindTexture( GL_TEXTURE_2D, SkinTexture2 );
				pShadowShip -> draw();

				/* Stop Animating Textures */
				glMatrixMode( GL_TEXTURE );
			glPopMatrix();
			glMatrixMode( GL_MODELVIEW );
			glDisable( GL_BLEND );
		glPopMatrix();
	glPopMatrix();
}


/*
============================================================
DrawJumpPoint
Creates a transparent cone, rotating to create the jumppoint
============================================================
*/
void OpenGLApplication::DrawJumpPoint( void ) {
	glDisable( GL_LIGHTING ); // not affected by scenes lights
	glEnable( GL_BLEND );                                  
	glColor4f( 1, 1, 1, GLfloat( 0.7 ) );
	glBlendFunc( GL_ONE, GL_ONE );                          
	glBindTexture( GL_TEXTURE_2D, JumpPointTexture );
	glPushMatrix();
		glTranslatef( 0.0f, 0.0f, 2.0f );
		glScalef( 2.0f, 2.0f, 2.0f );
		glRotatef( 180, 0, 1, 0 );
		glRotatef( ( GLfloat )( mRotation * -0.1 ), 0, 0, 1 ); // wormholes rotation                                 
		mCone.Draw();   
	glPopMatrix();
	glDisable( GL_BLEND );
	glEnable( GL_LIGHTING );   
}