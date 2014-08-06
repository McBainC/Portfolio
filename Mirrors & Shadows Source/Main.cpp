/*
==========================================================================
Windows & OpenGl Setup Code - Paul Robertson 2012
1102810_Coursework_S2 - "Mirrors and Shadows" - Chris McBain 2012/13
==========================================================================
*/


/* Includes */
#include <windows.h>
#include <stdio.h>
#include <mmsystem.h>
#include <math.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include "Texture.h"
#include "Input.h"
#include "OpenGLApplication.h"


/* Colour and Stencil Buffer Defines */
#define COLOUR_DEPTH 16	
#define STENCIL_BITS 8  


/* Windows Setup */
HWND           ghwnd;
HDC			   ghdc;
HGLRC		   ghrc;	   // hardware RENDERING CONTEXT
HINSTANCE	   ghInstance;
RECT		   gRect;
RECT		   screenRect;
HCURSOR        newCursor;
int S_WIDTH	 = 1024;       // client area resolution
int S_HEIGHT = 768;


/* Prototype */
LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );


/* Input & Camera */
Input mInput;
Camera mCamera( mInput, ( S_WIDTH / 2 ), ( S_HEIGHT / 2 ) );


/*
===============
CreateOurWindow
===============
*/
HWND CreateOurWindow( LPSTR strWindowName, int width, int height, DWORD dwStyle, bool bFullScreen, HINSTANCE hInstance ) {
	HWND hwnd;
	WNDCLASS wcex;

	memset( &wcex, 0, sizeof( WNDCLASS ) );
	wcex.style			= CS_HREDRAW | CS_VREDRAW;		
	wcex.lpfnWndProc	= WndProc;		
	wcex.hInstance		= hInstance;						
	wcex.hIcon			= LoadIcon( NULL, IDI_APPLICATION );
	wcex.hCursor		= LoadCursor( NULL, IDC_CROSS ); // replace arrow with a crosshair		
	wcex.hbrBackground	= ( HBRUSH ) ( COLOR_WINDOW + 1 );
	wcex.lpszMenuName	= NULL;	
	wcex.lpszClassName	= "FirstWindowClass";	

	RegisterClass( &wcex );  // register the class

	dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	ghInstance	= hInstance; // assign our global hInstance to the window's hInstance

	/* Set the Client area of the window to be our resolution */
	RECT glwindow;
	glwindow.left		= 0;		
	glwindow.right		= width;	
	glwindow.top		= 0;		
	glwindow.bottom		= height;	

	AdjustWindowRect( &glwindow, dwStyle, false );

	/* Create the window */
	hwnd = CreateWindow( "FirstWindowClass", 
						 "Mirrors and Shadows", 
						 dwStyle, 
						 0, 
						 0,
						 glwindow.right  - glwindow.left,
						 glwindow.bottom - glwindow.top, 
						 NULL,
						 NULL,
						 hInstance,
						 NULL );

	if( !hwnd ) return NULL; // if we could get a handle, return NULL

	ShowWindow( hwnd, SW_SHOWNORMAL );	
	UpdateWindow( hwnd );					
	SetFocus( hwnd );						

	return hwnd;
}


/*
==============
SetPixelFormat
==============
*/
bool SetPixelFormat( HDC hdc ) {
    PIXELFORMATDESCRIPTOR pfd = { 0 }; 
    int pixelformat; 
 
    pfd.nSize = sizeof( PIXELFORMATDESCRIPTOR ); // set the size of the structure
    pfd.nVersion = 1;							 // slways set this to 1
	
	/* Pass in the appropriate OpenGL flags */
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER; 
    pfd.dwLayerMask = PFD_MAIN_PLANE;			 // standard mask (this is ignored anyway)
    pfd.iPixelType = PFD_TYPE_RGBA;				 // RGB and Alpha pixel type
    pfd.cColorBits = COLOUR_DEPTH;				 // here we use our #define for the color bits
    pfd.cDepthBits = COLOUR_DEPTH;				 // ignored for RBA
    pfd.cAccumBits = 0;							 // nothing for accumulation
    pfd.cStencilBits = STENCIL_BITS;			 // here we use my #define for the stencil bits
 
	/* Gets a best match on the pixel format as passed in from device */
    if ( ( pixelformat = ChoosePixelFormat( hdc, &pfd ) ) == false ) { 
        MessageBox( NULL, "ChoosePixelFormat failed", "Error", MB_OK ); 
        return false; 
    } 
 
	/* Sets the pixel format if its ok */ 
    if (SetPixelFormat(hdc, pixelformat, &pfd) == false) { 
        MessageBox(NULL, "SetPixelFormat failed", "Error", MB_OK); 
        return false; 
    } 
 
    return true;
}


/*
==============
ResizeGLWindow
==============
*/
void ResizeGLWindow( int width, int height ) {
	/* Prevent a divide by zero error */
	if ( height == 0 ) { 
		height=1; // make the height equal one
	}

	glViewport( 0, 0, width, height );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	/* Calculate aspect ratio */
	gluPerspective( 45.0f, ( GLfloat )width / ( GLfloat )height, 0.1 ,150.0f );
	glMatrixMode( GL_MODELVIEW ); // select the modelview matrix
	glLoadIdentity();             // reset rhe modelview matrix
}


/*
================
InitializeOpenGL
================
*/
void InitializeOpenGL( int width, int height ) {
    ghdc = GetDC( ghwnd ); // sets global HDC

	/* Sets pixel format */
    if ( !SetPixelFormat( ghdc ) ) { 
        PostQuitMessage( 0 );
	}

    ghrc = wglCreateContext( ghdc ); // creates rendering context from hdc
    wglMakeCurrent( ghdc, ghrc );	 // use this HRC.
	ResizeGLWindow( width, height ); // setup the screen
}


/*
===========================
Init
Use this for initialization
===========================
*/
void Init( HWND hwnd ) {
	/* Window initialization */
	ghwnd = hwnd;
	GetClientRect( ghwnd, &gRect );                // get rect into our handy global rect
	InitializeOpenGL( gRect.right, gRect.bottom ); // initialise openGL

	/* OpenGL settings */
	glShadeModel( GL_SMOOTH );							           // enable smooth shading
	glClearColor( 1.0f, 0.0f, 1.0f, 0.5f );				           // black background
	glClearDepth( 1.0f );								           // depth buffer setup
	glEnable( GL_DEPTH_TEST );							           // enables depth testing
	glDepthFunc( GL_LEQUAL );							           // the type of depth testing to do
	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );           // really nice perspective calculations
	glEnable( GL_LIGHTING );
	glClearStencil( 0 );
	glEnable( GL_TEXTURE_2D );							           // enable texturing!
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE ); // specify texture calculation

	/* Also, do any other setting ov variables here for your app if you wish */
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR ); // linear filtering
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR ); // linear filtering
}


/*
=======
Cleanup
=======
*/
void Cleanup() {
	if( ghrc ) {
		wglMakeCurrent( NULL, NULL ); // free rendering memory
		wglDeleteContext( ghrc );     // delete our OpenGL rendering context
	}

	if( ghdc ) 
	ReleaseDC( ghwnd, ghdc );			               // release our HDC from memory
	UnregisterClass( "FirstWindowClass", ghInstance ); // free the window class
	PostQuitMessage( 0 );		                       // post a QUIT message to the window
}


/*
=======
WndProc
=======
*/
LRESULT CALLBACK WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam ) {
    switch( message ) {														
		case WM_CREATE:	
			break;

		case WM_SIZE:
			/* Resize the OpenGL window when the window is resized */
			ResizeGLWindow( LOWORD( lParam ), HIWORD( lParam ) );
			/* Adjust camera mouse controls */
			mCamera.AdjustScreenSize( LOWORD( lParam ), HIWORD( lParam ) );
			GetClientRect( hwnd, &gRect );
			break;	

		case WM_KEYDOWN:
			/* Set key bool as true */
			mInput.mKeys[ wParam ] = true;
			break;

		case WM_KEYUP:
			/* Set key bool as false */
			mInput.mKeys[ wParam ] = false;
			break;

		case WM_MOUSEMOVE:
			/* Update Mouse Position */
			mInput.mMouseX = LOWORD( lParam );
			mInput.mMouseY = HIWORD( lParam );
			break;

		case WM_PAINT:
		    break;		

		case WM_DESTROY:	
			PostQuitMessage( 0 );					
			break;	
	}	

	return DefWindowProc( hwnd, message, wParam, lParam );																
}


/*
=======
WinMain
=======
*/
int WINAPI WinMain ( HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int nCmdShow ) {
	HWND hwnd;
    MSG  msg;	

	/* Initialise and create window */
	hwnd = CreateOurWindow( "Mirrors and Shadows", S_WIDTH, S_HEIGHT, 0, false, hInstance );	
	if( hwnd == NULL ) return true;

	/* Initialise OpenGL and other settings */
	Init( hwnd );

	/* Create the OpenGLApplication */
	OpenGLApplication theApplication( mInput, mCamera );
	theApplication.Init();

	/* Main Loop */
	while( true ) {							
		if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) ) {
		    if( msg.message == WM_QUIT ) {
				break;
			}
			TranslateMessage( &msg );							
			DispatchMessage( &msg );
		/* Render Scene */
		} else {
			/* Update Input and Camera */
			mInput.Update();
			mCamera.Update();

			/* Switch to WireFrame */
			if( mInput.mKeys[ VK_SPACE ] ) {
				glPolygonMode( GL_FRONT, GL_LINE );
			} else {
				glPolygonMode( GL_FRONT, GL_FILL );
			}

			/* Display Controls */
			if( mInput.mKeys[ VK_CONTROL ] ) {
				theApplication.mDisplayControls = true;
			} else {
				theApplication.mDisplayControls = false;
			}

			/* Start OpenGl - clear buffers & load identity matrix */
			glClearStencil( 0 );
			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT ); 
			glLoadIdentity(); 

			/* Set our viewpoint - the camera */
			gluLookAt( mCamera.mPosition.x, mCamera.mPosition.y, mCamera.mPosition.z,                      // where we are
			           mCamera.mPointOfInterest.x, mCamera.mPointOfInterest.y, mCamera.mPointOfInterest.z, // what we look at
			           mCamera.mUp.x, mCamera.mUp.y, mCamera.mUp.z );                                      // which way is up

			/* Render the Application */
			theApplication.Render();

			/* End OpenGL - swap the frame buffers */
			SwapBuffers( ghdc ); 
		}
    }

	return msg.wParam ;										
}