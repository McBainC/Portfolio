#include "GraphicsClass.h"
#include "InputSingleton.h"


// Default Constructor               //
// Initializes many pointers to zero //
GraphicsClass::GraphicsClass() 
: pD3D( 0 ), pCamera( 0 ), pLight( 0 ),                                                                            // D3D, Camera and Light pointers
  pTextureShader( 0 ), pTransparentShader( 0 ), pTerrainReflectionShader( 0 ),                                     // Shader pointers
  pTerrainShader( 0 ), pOceanShader( 0 ), pHorizontalBlurShader( 0 ), pVerticalBlurShader( 0 ),
  pRefractionTexture( 0 ), pReflectionTexture( 0 ),                                                                // Ocean render to textures
  pText( 0 ), pCursor( 0 ),                                                                                        // Text and Cursor pointers
  pPostProcessingTexture( 0 ), pPostProcessingWindow( 0 ), pHorizontalBlurTexture( 0 ), pVerticalBlurTexture( 0 ), // Post-Processing render to textures
  pTerrain( 0 ), pSun( 0 ), pOcean( 0 ),                                                                           // Model pointers
  mRotation( 0.0f ), mWaterHeight( 2.95f ), mWaterTranslation( 0.0f ), mWaveHeight( 0.2f ),                        // Scene variables
  mLightOrbit( D3DXVECTOR3( 0.0f, 1000.0f, 0.0f ) ), mLightPosition( D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) ),            // Light vector3's
  mDisplayingUI( false ), mApplyingBlur( false ),                                                                  // Toggle flags
  mSmoothingAmount( 5 ), mDisplacementRange( 10.0f ) {                                                             // Terrain variables
}	


// Constructor //
GraphicsClass::GraphicsClass( const GraphicsClass& other ) {
}


// Destructor //
GraphicsClass::~GraphicsClass() {
}


// Initialize                                        //
// Initializes object pointers - see comments within //
bool GraphicsClass::Initialize( int screenWidth, int screenHeight, HWND hwnd ) {
	bool result;

	// Set screen dimensions
	mScreenWidth  = screenWidth;
	mScreenHeight = screenHeight;

	// DIRECT3D //
	// Create the Direct3D object
	pD3D = new D3DClass;
	if( !pD3D ) {
		return false;
	}

	// Initialize the Direct3D object
	result = pD3D->Initialize( screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR );
	if( !result ) {
		MessageBox( hwnd, L"Could not initialize Direct3D", L"Error", MB_OK );
		return false;
	}

	// CAMERA //
	// Create the camera object
	D3DXMATRIX baseViewMatrix;
	pCamera = new CameraClass;
	if( !pCamera ) {
		return false;
	}
	
	// Set the initial position of the camera for base view matrix
	pCamera->SetPosition( 0.0f, 0.0f, -15.0f );

	// Set view direction vector (mLookAt)
	pCamera->SetLookAt( 0.0f, 0.0f, 1.0f );

	// Initialize a base view matrix with the camera for 2D user interface rendering
	// Always the same matrix, always the same text / cursor location
	pCamera->Render();
	pCamera->GetViewMatrix( baseViewMatrix );

	// Set the initial position of the camera
	pCamera->SetPosition( 0.0f, 8.0f, -15.0f );

	// TEXT //
	// Create the text object
	pText = new TextClass;
	if( !pText ) {
		return false;
	}

	// Initialize the text object
	result = pText->Initialize( pD3D->GetDevice(), pD3D->GetDeviceContext(), hwnd, screenWidth, screenHeight, baseViewMatrix );
	if( !result ) {
		MessageBox( hwnd, L"Could not initialize the text object.", L"Error", MB_OK );
		return false;
	}

	// Set performance text
	pText->CreateSentence( pD3D->GetDevice(), pD3D->GetDeviceContext(), 0, "Framerate = ", 20, 20, 1.0f, 1.0f, 1.0f );
	pText->CreateSentence( pD3D->GetDevice(), pD3D->GetDeviceContext(), 1, "CPU Usage = ", 20, 40, 1.0f, 1.0f, 1.0f );
	pText->CreateSentence( pD3D->GetDevice(), pD3D->GetDeviceContext(), 2, "Mouse Xcoord = ", 20, 60, 1.0f, 1.0f, 1.0f );
	pText->CreateSentence( pD3D->GetDevice(), pD3D->GetDeviceContext(), 3, "Mouse Ycoord = ", 20, 80, 1.0f, 1.0f, 1.0f );
	pText->CreateSentence( pD3D->GetDevice(), pD3D->GetDeviceContext(), 4, "Blur Post-Processing", 20, 100, 1.0f, 1.0, 1.0f );
	pText->CreateSentence( pD3D->GetDevice(), pD3D->GetDeviceContext(), 5, "Refraction Texture", 20, 120, 1.0f, 1.0f, 1.0f );
	pText->CreateSentence( pD3D->GetDevice(), pD3D->GetDeviceContext(), 6, "Reflection Texture", 20, 270, 1.0f, 1.0f, 1.0f );
	pText->CreateSentence( pD3D->GetDevice(), pD3D->GetDeviceContext(), 7, "Smoothing Passes = ", 20, 420, 1.0f, 1.0f, 1.0f );
	pText->CreateSentence( pD3D->GetDevice(), pD3D->GetDeviceContext(), 8, "Displacement Range = ", 20, 440, 1.0f, 1.0f, 1.0f );

	// CURSOR //
	// Create the cursor object
	pCursor = new CursorClass;
	if( !pCursor ) {
		return false;
	}

	// Initialize the cursor object also with baseViewMatrix
	result = pCursor->Initialize( pD3D->GetDevice(), screenWidth, screenHeight, 10, 10, L"CursorTexture.dds", baseViewMatrix );
	if( !result ) {
		MessageBox( hwnd, L"Could not initialize the cursor object.", L"Error", MB_OK );
		return false;
	}

	// LIGHT //
	// Create the light object
	pLight = new LightClass;
	if( !pLight ) {
		return false;
	}

	// Initialize the light object
	pLight->SetAmbientColor( 0.1f, 0.1f, 0.1f, 1.0f );
	pLight->SetDiffuseColor( 0.6f, 0.6f, 0.6f, 1.0f );
	pLight->SetPosition( mLightPosition.x, mLightPosition.y, mLightPosition.z );
	pLight->SetSpecularPower( 200.0f );
	pLight->SetSpecularColor( 1.0f, 1.0f, 1.0f, 1.0f );

	// INITIALIZE MODELS //
	// TERRAIN 
	// Create the terrain object
	pTerrain = new TerrainClass;
	if( !pTerrain ) {
		return false;
	}

	result = pTerrain->Initialize( pD3D->GetDevice(),
								   257,                  // power2 dimension + 1 (must be odd too)
								   mSmoothingAmount,
								   mDisplacementRange,
								   L"BeachTexture.dds",
								   L"GroundTexture.dds",
								   L"RockTexture.dds",
								   L"SnowTexture.dds",
								   L"BeachBumpMap.dds",
								   L"GroundBumpMap.dds",
								   L"RockBumpMap.dds",
								   L"SnowBumpMap.dds" );
	if( !result ) {
		MessageBox( hwnd, L"Could not initialize the Terrain object.", L"Error", MB_OK );
		return false;
	}

	// SUN 
	// Create the Sun object
	pSun = new ModelClass;
	if( !pSun ) {
		return false;
	}

	// Initialize the Sun object
	result = pSun->Initialize( pD3D->GetDevice(), "Sphere.obj", L"SunTexture.dds", NULL, NULL, NULL, NULL, NULL, NULL, NULL, true );
	if( !result ) {
		MessageBox( hwnd, L"Could not initialize the Sun object.", L"Error", MB_OK );
		return false;
	}

	// OCEAN 
	// Create the Ocean object
	pOcean = new OceanClass;
	if( !pOcean ) {
		return false;
	}

	// Initialize the Ocean object
	result = pOcean->Initialize( pD3D->GetDevice(), 256, L"RippleTexture.dds", L"OceanTexture.dds", NULL, NULL, NULL, NULL, NULL, NULL );
	if( !result ) {
		MessageBox( hwnd, L"Could not initialize the Ocean object.", L"Error", MB_OK );
		return false;
	}

	// INITIALIZE SHADERS //
	// TEXTURESHADER 
	// Create the texture shader object
	pTextureShader = new TextureShaderClass;
	if( !pTextureShader ) {
		return false;
	}

	// Initialize the texture shader object
	result = pTextureShader->Initialize( pD3D->GetDevice(), hwnd );
	if( !result ) {
		MessageBox( hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK );
		return false;
	}

	// TRANSPARENTSHADER 
	// Create the transparent shader object
	pTransparentShader = new TransparentShaderClass;
	if( !pTransparentShader ) {
		return false;
	}

	// Initialize the transparent shader object
	result = pTransparentShader->Initialize( pD3D->GetDevice(), hwnd );
	if( !result ) {
		MessageBox( hwnd, L"Could not initialize the transparent shader object.", L"Error", MB_OK );
		return false;
	}

	// TERRAINREFLECTIONSHADER 
	// Create the refraction shader object
	pTerrainReflectionShader = new TerrainReflectionShaderClass;
	if( !pTerrainReflectionShader ) {
		return false;
	}

	// Initialize the refraction shader object
	result = pTerrainReflectionShader->Initialize( pD3D->GetDevice(), hwnd );
	if( !result ) { 
		MessageBox( hwnd, L"Could not initialize the terrain reflection shader object.", L"Error", MB_OK );
		return false;
	}

	// TERRAINSHADER 
	// Create the terrain shader object
	pTerrainShader = new TerrainShaderClass;
	if( !pTerrainShader ) {
		return false;
	}

	// Initialize the terrain shader object
	result = pTerrainShader->Initialize( pD3D->GetDevice(), hwnd );
	if( !result ) {
		MessageBox( hwnd, L"Could not initialize the terrain shader object.", L"Error", MB_OK );
		return false;
	}

	// OCEANSHADER 
	// Create the ocean shader object
	pOceanShader = new OceanShaderClass;
	if( !pOceanShader ) {
		return false;
	}

	// Initialize the ocean shader object
	result = pOceanShader->Initialize( pD3D->GetDevice(), hwnd );
	if( !result ) {
		MessageBox( hwnd, L"Could not initialize the ocean shader object.", L"Error", MB_OK );
		return false;
	}

	// HORIZTONALBLURSHADER 
	// Create the horizontal blur shader object
	pHorizontalBlurShader = new HorizontalBlurShaderClass;
	if( !pHorizontalBlurShader ) {
		return false;
	}

	// Initialize the horizontal blur shader object
	result = pHorizontalBlurShader->Initialize( pD3D->GetDevice(), hwnd );
	if( !result ) {
		MessageBox( hwnd, L"Could not initialize the horizontal blur shader object.", L"Error", MB_OK );
		return false;
	}

	// VERTICALBLURSHADER 
	// Create the vertical blur shader object
	pVerticalBlurShader = new VerticalBlurShaderClass;
	if( !pVerticalBlurShader ) {
		return false;
	}

	// Initialize the vertical blur shader object
	result = pVerticalBlurShader->Initialize( pD3D->GetDevice(), hwnd );
	if( !result ) {
		MessageBox( hwnd, L"Could not initialize the vertical blur shader object.", L"Error", MB_OK );
		return false;
	}

	// INITIALIZE ORTHO WINDOWS //
	// DEBUGWINDOW
	// Create the debug window object
	pDebugWindow = new OrthoWindowClass;
	if( !pDebugWindow ) {
		return false;
	}

	// Initialize the debug window object
	result = pDebugWindow->Initialize( pD3D->GetDevice(), screenWidth, screenHeight, 160, 120, baseViewMatrix );
	if( !result ) {
		MessageBox( hwnd, L"Could not initialize the debug window object.", L"Error", MB_OK );
		return false;
	}

	// POSTPROCESSINGWINDOW
	// Create the post processing window object
	pPostProcessingWindow = new OrthoWindowClass;
	if( !pDebugWindow ) {
		return false;
	}

	// Initialize the post processing window object
	result = pPostProcessingWindow->Initialize( pD3D->GetDevice(), screenWidth, screenHeight, screenWidth, screenHeight, baseViewMatrix );
	if( !result ) {
		MessageBox( hwnd, L"Could not initialize the post processing window object.", L"Error", MB_OK );
		return false;
	}

	// INITIALIZE RENDER TO TEXTURES //
	// REFRACTIONTEXTURE 
	// Create the refraction to texture object
	pRefractionTexture = new RenderTextureClass;
	if( !pRefractionTexture ) {
		return false;
	}

	// Initialize the refraction to texture object
	result = pRefractionTexture->Initialize( pD3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR );
	if( !result ) {
		return false;
	}

	// REFLECTIONTEXTURE 
	// Create the reflection to texture object
	pReflectionTexture = new RenderTextureClass;
	if( !pReflectionTexture ) {
		return false;
	}

	// Initialize the reflection to texture object
	result = pReflectionTexture->Initialize( pD3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR );
	if( !result ) {
		return false;
	}

	// POSTPROCESSINGTEXTURE 
	// Create the post processing to texture object
	pPostProcessingTexture = new RenderTextureClass;
	if( !pPostProcessingTexture ) {
		return false;
	}

	// Initialize the post processing to texture object
	result = pPostProcessingTexture->Initialize( pD3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR );
	if( !result ) {
		return false;
	}

	// HORIZONTALBLURTEXTURE 
	// Create the horizontal blur texture object
	pHorizontalBlurTexture = new RenderTextureClass;
	if( !pHorizontalBlurTexture ) {
		return false;
	}

	// Initialize the horizontal blur texture object
	result = pHorizontalBlurTexture->Initialize( pD3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR );
	if( !result ) {
		return false;
	}

	// VERTICALBLURTEXTURE 
	// Create the vertical blur texture object
	pVerticalBlurTexture = new RenderTextureClass;
	if( !pVerticalBlurTexture ) {
		return false;
	}

	// Initialize the vertical blur texture object
	result = pVerticalBlurTexture->Initialize( pD3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR );
	if( !result ) {
		return false;
	}

	return true;
}


// Shutdown //
void GraphicsClass::Shutdown() {
	// Release the debug window object
	if( pDebugWindow ) {
		pDebugWindow->Shutdown();
		delete pDebugWindow;
		pDebugWindow = 0;
	}

	// Release the post processing window object
	if( pPostProcessingWindow ) {
		pPostProcessingWindow->Shutdown();
		delete pPostProcessingWindow;
		pPostProcessingWindow = 0;
	}

	// Release the reflection render to texture object
	if( pReflectionTexture ) {
		pReflectionTexture->Shutdown();
		delete pReflectionTexture;
		pReflectionTexture = 0;
	}

	// Release the refraction render to texture object
	if( pRefractionTexture ) {
		pRefractionTexture->Shutdown();
		delete pRefractionTexture;
		pRefractionTexture = 0;
	}

	// Release the post processing render to texture object
	if( pPostProcessingTexture ) {
		pPostProcessingTexture->Shutdown();
		delete pPostProcessingTexture;
		pPostProcessingTexture = 0;
	}

	// Release the horizontal blur render to texture object
	if( pHorizontalBlurTexture ) {
		pHorizontalBlurTexture->Shutdown();
		delete pHorizontalBlurTexture;
		pHorizontalBlurTexture = 0;
	}

	// Release the vertical blur render to texture object
	if( pVerticalBlurTexture ) {
		pVerticalBlurTexture->Shutdown();
		delete pVerticalBlurTexture;
		pVerticalBlurTexture = 0;
	}

	// Release the ocean shader object
	if( pOceanShader ) {
		pOceanShader->Shutdown();
		delete pOceanShader;
		pOceanShader = 0;
	}

	// Release the horizontal blur shader object
	if( pHorizontalBlurShader ) {
		pHorizontalBlurShader->Shutdown();
		delete pHorizontalBlurShader;
		pHorizontalBlurShader = 0;
	}

	// Release the vertical blur shader object
	if( pVerticalBlurShader ) {
		pVerticalBlurShader->Shutdown();
		delete pVerticalBlurShader;
		pVerticalBlurShader = 0;
	}

	// Release the terrain reflecction shader object
	if( pTerrainReflectionShader ) {
		pTerrainReflectionShader->Shutdown();
		delete pTerrainReflectionShader;
		pTerrainReflectionShader = 0;
	}

	// Release the debug window object
	if( pCursor ) {
		pCursor->Shutdown();
		delete pCursor;
		pCursor = 0;
	}

	// Release the texture shader object
	if( pTextureShader ) {
		pTextureShader->Shutdown();
		delete pTextureShader;
		pTextureShader = 0;
	}

	// Release the transparent shader object
	if( pTransparentShader ) {
		pTransparentShader->Shutdown();
		delete pTransparentShader;
		pTransparentShader = 0;
	}

	// Release the terrain shader object
	if( pTerrainShader ) {
		pTerrainShader->Shutdown();
		delete pTerrainShader;
		pTerrainShader = 0;
	}

	// Release the terrain object
	if( pTerrain ) {
		pTerrain->Shutdown();
		delete pTerrain;
		pTerrain = 0;
	}

	// Release the sun object
	if( pSun ) {
		pSun->Shutdown();
		delete pSun;
		pSun = 0;
	}

	// Release the ocean object
	if( pOcean ) {
		delete pOcean;
		pOcean = 0;
	}

	// Release the camera object
	if( pCamera ) {
		delete pCamera;
		pCamera = 0;
	}

	// Release the D3D object
	if( pD3D ) {
		pD3D->Shutdown();
		delete pD3D;
		pD3D = 0;
	}

	// Relase the Text object
	if( pText ) {
		pText->Shutdown();
		delete pText;
		pText = 0;
	}

	return;
}


// Frame                         //
// Manages all per frame changes //
bool GraphicsClass::Frame( int fps, int cpu ) {
	bool result;

	// Allow user to change displacement variable //
	// Increase - UP
	if( InputSingleton::GetInstance()->HasKeyBeenPressed( VK_UP ) ) {
		mDisplacementRange++;
		if( mDisplacementRange > 20 ) {
			mDisplacementRange = 20;
		}
	// Decrease - DOWN
	} else if ( InputSingleton::GetInstance()->HasKeyBeenPressed( VK_DOWN ) ) {
		mDisplacementRange--;
		if( mDisplacementRange < 0 ) {
			mDisplacementRange = 0;
		}
	}

	// Allow user to change smoothing amount variable //
	// Decrease - LEFT
	if( InputSingleton::GetInstance()->HasKeyBeenPressed( VK_LEFT ) ) {
		mSmoothingAmount--;
		if( mSmoothingAmount < 0 ) {
			mSmoothingAmount = 0;
		}
	// Increase - RIGHT
	} else if ( InputSingleton::GetInstance()->HasKeyBeenPressed( VK_RIGHT ) ) {
		mSmoothingAmount++; 
		if( mSmoothingAmount > 10 ) {
			mSmoothingAmount = 10;
		} 
	}

	// Set system information 
	pText->SetFpsColour( 0, fps, 20, 20 );
	pText->SetCpuColour( 1, cpu, 20, 40 );

	// Update mouse position and terrain variables
	pText->SetCursorCoord( 2, "Cursor XCoord = ", InputSingleton::GetInstance()->mMouseX - ( mScreenWidth / 2 ), 20, 60 );
	pText->SetCursorCoord( 3, "Cursor YCoord = ", InputSingleton::GetInstance()->mMouseY - ( mScreenHeight / 2 ), 20, 80 );
	pText->SetCursorCoord( 7, "Smoothing Passes = ", mSmoothingAmount, 20, 420 );
	pText->SetCursorCoord( 8, "Displacement Range = ", ( int )mDisplacementRange, 20, 440 );

	// Change post processing sentence string and colour based on mApplyingBlur flag
	if( mApplyingBlur ) {
		pText->UpdateData( 4, "Blur Post-Processing = True", 20, 100, 0.0f, 1.0f, 0.0f );
	} else {
		pText->UpdateData( 4, "Blur Post-Processing = False", 20, 100, 1.0f, 0.0f, 0.0f );
	}

	// Update all sentences
	pText->UpdateSentences( pD3D->GetDevice(), pD3D->GetDeviceContext() );

	// Update the camera
	pCamera->Update();

	// Update the rotation variable each frame - used for the Light/Sun
	mRotation += ( float )D3DX_PI * 0.001f;
	if( mRotation > 360.0f ) {
		mRotation -= 360.0f;
	}

	// Update the position of the water to simulate motion - passed to Ocean shader
	mWaterTranslation += 0.001f;
	if( mWaterTranslation > 1.0f ) {
		mWaterTranslation -= 1.0f;
	}

	// Toggle post processing // Change render mode - wireframe no longer useful
	if( InputSingleton::GetInstance()->HasKeyBeenPressed( VK_TAB ) ) {
		//pD3D->ChangeRenderMode();
		TogglePostProcessing();
	}

	// Toggle UI rendering
	if( InputSingleton::GetInstance()->HasKeyBeenPressed( VK_SHIFT ) ) {
		ChangeUIDisplayMode();
	}

	// Generate New Terrain
	if( InputSingleton::GetInstance()->HasKeyBeenPressed( VK_SPACE ) ) {
		// Release the terrain object
		if( pTerrain ) {
			pTerrain->Shutdown();
			delete pTerrain;
			pTerrain = 0;
		}

		// TERRAIN //
		// Create the terrain object
		pTerrain = new TerrainClass;
		if( !pTerrain ) {
			return false;
		}

		result = pTerrain->Initialize( pD3D->GetDevice(),
									   257,                  // power2 + 1 (must be odd too)
									   mSmoothingAmount,
									   mDisplacementRange,
									   L"BeachTexture.dds",
									   L"GroundTexture.dds",
									   L"RockTexture.dds",
									   L"SnowTexture.dds",
									   L"BeachBumpMap.dds",
									   L"GroundBumpMap.dds",
									   L"RockBumpMap.dds",
									   L"SnowBumpMap.dds" );
	}

	// Render the graphics scene
	result = Render();
	if( !result ) {
		return false;
	}

	return true;
}


// Render                                               //
// Breakdown of the different stages of scene rendering //
bool GraphicsClass::Render() {
	bool result;

	// Render the refraction of the scene to a texture
	result = RenderRefractionToTexture();
	if( !result ) {
		return false;
	}

	// Render the reflection of the scene to a texture
	result = RenderReflectionToTexture();
	if( !result ) {
		return false;
	}

	// Render the scene to a texture
	result = RenderSceneToTexture();
	if( !result ) {
		return false;
	}

	// Post processing
	if( mApplyingBlur ) {
		// Render adding horizontal blur
		result = RenderHorizontalBlurToTexture();
		if( !result ) {
			return false;
		}

		// Render adding vertical blur
		result = RenderVerticalBlurToTexture();
		if( !result ) {
			return false;
		}
	}

	// Render the scene as normal to the back buffer
	result = RenderScene();
	if( !result ) {
		return false;
	}

	return true;
}


// RenderRefractionToTexture                                          //
// Renders the terrains refraction (seen in or through the ocean)     //
// Basically nothing above the waterline (mWaterHeight + mWaveHeight) //
bool GraphicsClass::RenderRefractionToTexture() {
	D3DXVECTOR4 clipPlane;
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	// Setup a clipping plane based on the height of the water to clip everything above it
	// Also allowing the waveHeight variable - to avoid ocean clipping
	clipPlane = D3DXVECTOR4( 0.0f, -1.0f, 0.0f, mWaterHeight + mWaveHeight );

	// Set the render target to be the refraction render to texture
	pRefractionTexture->SetRenderTarget( pD3D->GetDeviceContext(), pD3D->GetDepthStencilView() );

	// Clear the refraction render to texture
	pRefractionTexture->ClearRenderTarget( pD3D->GetDeviceContext(), pD3D->GetDepthStencilView(), 0.0f, 0.5f, 0.5f, 1.0f );

	// Generate the view matrix based on the camera's position
	pCamera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects
	pD3D->GetWorldMatrix( worldMatrix );
	pCamera->GetViewMatrix( viewMatrix );
	pD3D->GetProjectionMatrix( projectionMatrix );

	// Translate to where the terrain model will be rendered
	D3DXMatrixTranslation( &worldMatrix, -128.0f, 3.0f, -128.0f );

	// Put the terrain model vertex and index buffers on the graphics pipeline to prepare them for drawing
	pTerrain->Render( pD3D->GetDeviceContext() );

	// Render the terrain refraction with the terrain reflection shader (no actual refraction sadly)
	result = pTerrainReflectionShader->Render( pD3D->GetDeviceContext(),
		                                       pTerrain->GetIndexCount(),
										       worldMatrix,
										       viewMatrix,
					                           projectionMatrix,
					                           pLight->GetAmbientColor(),
										       pLight->GetDiffuseColor(),
										       pLight->GetPosition(), 
										       pTerrain->GetTextureArray(),
										       clipPlane );
	
	if( !result ) {
		return false;
	}

	// Reset the render target back to the original back buffer and not the render to texture anymore
	pD3D->SetBackBufferRenderTarget();

	return true;
}


// RenderReflectionToTexture                                            //
// Renders the terrains reflection (seen on the oceans surface)         //
// Terrain above the waterline, but using the cameras reflection matrix //
bool GraphicsClass::RenderReflectionToTexture() {
	D3DXMATRIX reflectionViewMatrix, worldMatrix, projectionMatrix, translateMatrix, scaleMatrix;
	bool result;
	D3DXVECTOR4 clipPlane;
	
	// Setup a clipping plane based on the height of the water to clip everything above it
	// Also allowing the waveHeight variable - to avoid ocean clipping
	clipPlane = D3DXVECTOR4( 0.0f, 1.0f, 0.0f, -mWaterHeight + mWaveHeight );

	// Set the render target to be the reflection render to texture.
	pReflectionTexture->SetRenderTarget( pD3D->GetDeviceContext(), pD3D->GetDepthStencilView() );

	// Clear the reflection render to texture
	pReflectionTexture->ClearRenderTarget( pD3D->GetDeviceContext(), pD3D->GetDepthStencilView(), 0.0f, 0.3f, 0.3f, 1.0f );

	// Use the camera to render the reflection and create a reflection view matrix
	pCamera->RenderReflection( mWaterHeight );

	// Get the camera reflection view matrix instead of the normal view matrix
	reflectionViewMatrix = pCamera->GetReflectionViewMatrix();

	// Get the world and projection matrices from the d3d object
	pD3D->GetWorldMatrix( worldMatrix );
	pD3D->GetProjectionMatrix( projectionMatrix );

	// Translate to where the terrain model will be rendered
	D3DXMatrixTranslation( &worldMatrix, -128.0f, 3.0f, -128.0f );

	// Put the terrain model vertex and index buffers on the graphics pipeline to prepare them for drawing
	pTerrain->Render( pD3D->GetDeviceContext() );

	// Render the terrain reflection using the terrain reflection shader
	result = pTerrainReflectionShader->Render( pD3D->GetDeviceContext(),
		                                       pTerrain->GetIndexCount(),
										       worldMatrix,
										       reflectionViewMatrix,
					                           projectionMatrix,
					                           pLight->GetAmbientColor(),
										       pLight->GetDiffuseColor(),
										       pLight->GetPosition(), 
										       pTerrain->GetTextureArray(),
										       clipPlane );
	if( !result ) {
		return false;
	}

	// Reset the render target back to the original back buffer and not the render to texture anymore
	pD3D->SetBackBufferRenderTarget();

	return true;
}


// RenderSceneToTexture                                                                  //
// Renders the full terrain and ocean models                                             //
// Point light orbits the terrain - ocean does not support lighting                      //
// Ocean model takes in refraction and reflection textures to create final surface image //
// Now rendered to texture for post processing                                           //
bool GraphicsClass::RenderSceneToTexture() {
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, reflectionMatrix, translateMatrix, rotateMatrix;
	bool result;

	// Set the render target to be the post processing render to texture
	pPostProcessingTexture->SetRenderTarget( pD3D->GetDeviceContext(), pD3D->GetDepthStencilView() );

	// Clear the post processing render to texture
	pPostProcessingTexture->ClearRenderTarget( pD3D->GetDeviceContext(), pD3D->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f );

	// Generate the view matrix based on the camera's position
	pCamera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects
	pD3D->GetWorldMatrix( worldMatrix );
	pCamera->GetViewMatrix( viewMatrix );
	pD3D->GetProjectionMatrix( projectionMatrix );

	// Rotate the world matrix by the rotation value so that the sun will orbit
	D3DXMatrixRotationZ( &rotateMatrix, mRotation );

	// Translate to pLight's position
	D3DXMatrixTranslation( &translateMatrix, 0.0f, 1000.0f, 0.0f );

	// Apply rotate * translate matrix to world matrix
	D3DXMatrixMultiply( &worldMatrix, &translateMatrix, &rotateMatrix );

	// Transform light vector
	D3DXVec3TransformCoord( &mLightPosition, &mLightOrbit, &rotateMatrix );

	// Update light position
	pLight->SetPosition( mLightPosition.x, mLightPosition.y, mLightPosition.z ); 
					
	// Put the sun vertex and index buffers on the graphics pipeline to prepare them for drawing
	pSun->Render( pD3D->GetDeviceContext() );

	// Render the sun model using the texture shader
	result = pTextureShader->Render( pD3D->GetDeviceContext(),
		                             pSun->GetIndexCount(),
								     worldMatrix,
								     viewMatrix,
								     projectionMatrix,
									 pSun->GetTexture() );            
	if( !result ) {
		return false;
	}

	// Reset matrix
	pD3D->GetWorldMatrix( worldMatrix );

	// Generate the view matrix based on the camera's position
	pCamera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects
	pCamera->GetViewMatrix( viewMatrix );

	// Translate to where the terrain model will be rendered (centered on origin)
	D3DXMatrixTranslation( &worldMatrix, -128.0f, 3.0f, -128.0f ); 

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing
	pTerrain->Render( pD3D->GetDeviceContext() );

	// Render the terrain using the terrain shader
	result = pTerrainShader->Render( pD3D->GetDeviceContext(),
		                             pTerrain->GetIndexCount(),
									 worldMatrix,
								     viewMatrix,
								     projectionMatrix,
									 pLight->GetAmbientColor(),
									 pLight->GetDiffuseColor(),
									 pLight->GetPosition(),
									 pTerrain->GetTextureArray() );
	if( !result ) {
		return false;
	}

	// Get the camera reflection view matrix
	reflectionMatrix = pCamera->GetReflectionViewMatrix();

	// Reset the world matrix
	pD3D->GetWorldMatrix( worldMatrix );

	// Translate to where the ocean model will be rendered
	D3DXMatrixTranslation( &worldMatrix, -128.0f, mWaterHeight, -128.0f ); 

	// Put the ocean model vertex and index buffers on the graphics pipeline to prepare them for drawing
	pOcean->Render( pD3D->GetDeviceContext() );

	// Render the ocean model using the ocean shader
	// Passing the two render to textures to be combined to create the oceans reflection
	result = pOceanShader->Render( pD3D->GetDeviceContext(),
		                           pOcean->GetIndexCount(),
								   worldMatrix,
								   viewMatrix, 
				                   projectionMatrix,
								   reflectionMatrix,
								   pReflectionTexture->GetShaderResourceView(),
				                   pRefractionTexture->GetShaderResourceView(),
								   pOcean->GetTextureArray(), 
								   mWaterTranslation,
								   0.005f,
								   mRotation * 10.0f,
								   mWaveHeight );
	if( !result ) {
		return false;
	}

	// Reset the render target back to the original back buffer and not the render to texture anymore
	pD3D->SetBackBufferRenderTarget();

	return true;
}


// RenderHorizontalBlurToTexture                //
// Applies Horizontal Blur to render to texture //
bool GraphicsClass::RenderHorizontalBlurToTexture() {
	D3DXMATRIX worldMatrix, viewMatrix, orthoMatrix;
	float screenSizeX;
	bool result;

	// Store the screen width in a float that will be used in the horizontal blur shader
	screenSizeX = ( float )pHorizontalBlurTexture->GetTextureWidth();
	
	// Set the render target to be the horiztonal blue render to texture
	pHorizontalBlurTexture->SetRenderTarget( pD3D->GetDeviceContext(), pD3D->GetDepthStencilView() );

	// Clear the render to texture
	pHorizontalBlurTexture->ClearRenderTarget( pD3D->GetDeviceContext(), pD3D->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f );

	// Generate the view matrix based on the camera's position
	pCamera->Render();

	// Get the world and view matrices from the camera and d3d objects
	pCamera->GetViewMatrix( viewMatrix );
	pD3D->GetWorldMatrix( worldMatrix );

	// Get the ortho matrix from the render to texture since texture has different dimensions
	pD3D->GetOrthoMatrix( orthoMatrix );

	// Turn off the Z buffer to begin all 2D rendering
	pD3D->TurnZBufferOff();

	// Put the post processing window vertex and index buffers on the graphics pipeline to prepare them for drawing
	pPostProcessingWindow->Render( pD3D->GetDeviceContext(), 0, 0 );
	
	// Render the post processing window using the horizontal blur shader 
	result = pHorizontalBlurShader->Render( pD3D->GetDeviceContext(), 
		                                    pPostProcessingWindow->GetIndexCount(),
											worldMatrix, 
											pPostProcessingWindow->GetBaseViewMatrix(), 
											orthoMatrix,
					 	                    pPostProcessingTexture->GetShaderResourceView(),
											screenSizeX );
	if( !result ) {
		return false;
	}

	// Turn the Z buffer back on now that all 2D rendering has completed
	pD3D->TurnZBufferOn();

	// Reset the render target back to the original back buffer and not the render to texture anymore
	pD3D->SetBackBufferRenderTarget();

	return true;
}


// RenderVerticalBlurToTexture                //
// Applies Vertical Blur to render to texture //
bool GraphicsClass::RenderVerticalBlurToTexture() {
	D3DXMATRIX worldMatrix, viewMatrix, orthoMatrix;
	float screenSizeY;
	bool result;

	// Store the screen height in a float that will be used in the vertical blur shader
	screenSizeY = ( float )pVerticalBlurTexture->GetTextureHeight();
	
	// Set the render target to be the render to texture
	pVerticalBlurTexture->SetRenderTarget( pD3D->GetDeviceContext(), pD3D->GetDepthStencilView() );

	// Clear the render to texture
	pVerticalBlurTexture->ClearRenderTarget( pD3D->GetDeviceContext(), pD3D->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f );

	// Generate the view matrix based on the camera's position
	pCamera->Render();

	// Get the world and view matrices from the camera and d3d objects
	pCamera->GetViewMatrix( viewMatrix );
	pD3D->GetWorldMatrix( worldMatrix );

	// Get the ortho matrix from the render to texture since texture has different dimensions
	pD3D->GetOrthoMatrix( orthoMatrix );

	// Turn off the Z buffer to begin all 2D rendering
	pD3D->TurnZBufferOff();

	// Put the post processing window vertex and index buffers on the graphics pipeline to prepare them for drawing
	pPostProcessingWindow->Render( pD3D->GetDeviceContext(), 0, 0 );
	
	// Render the post processing window using the vertical blur shader
	result = pVerticalBlurShader->Render( pD3D->GetDeviceContext(),
		                                  pPostProcessingWindow->GetIndexCount(),
										  worldMatrix, 
										  pPostProcessingWindow->GetBaseViewMatrix(), 
										  orthoMatrix,
					                      pHorizontalBlurTexture->GetShaderResourceView(), 
										  screenSizeY );
	if( !result ) {
		return false;
	}

	// Turn the Z buffer back on now that all 2D rendering has completed
	pD3D->TurnZBufferOn();

	// Reset the render target back to the original back buffer and not the render to texture anymore
	pD3D->SetBackBufferRenderTarget();

	return true;
}


// RenderScene                                                                    //
// Renders the scene as a orthowindow with either a straight texture of the scene //
// Or a post-processed image through horizontal and vertical blur                 //
// Also renders the text, debug window aand cursor objects if UI is flagged       //
bool GraphicsClass::RenderScene() {
	D3DXMATRIX viewMatrix, worldMatrix, orthoMatrix; 
	bool result;
	
	// Clear the buffers to begin the scene
	pD3D->BeginScene( 0.0f, 0.0f, 0.0f, 1.0f );

	// Generate the view matrix based on the camera's position
	pCamera->Render();

	// POSTPROCESSING //
	// All 2D rendering
	pD3D->TurnZBufferOff();
	pD3D->TurnOffAlphaBlending();

	// Get the world, view, and ortho matrices from the camera and d3d objects
	pD3D->GetWorldMatrix( worldMatrix );
	pCamera->GetViewMatrix( viewMatrix );
	pD3D->GetOrthoMatrix( orthoMatrix );

	// Put the post processing window vertex and index buffers on the graphics pipeline to prepare them for drawing
	result = pPostProcessingWindow->Render( pD3D->GetDeviceContext(), 0, 0 );
	if( !result ) {
		return false;
	}

	// If applying blur use pVerticalBlurTexture
	if( mApplyingBlur ) {
		// Render the post processing window using the texture shader
		result = pTextureShader->Render( pD3D->GetDeviceContext(),
										 pPostProcessingWindow->GetIndexCount(),
										 worldMatrix,
										 pPostProcessingWindow->GetBaseViewMatrix(),
										 orthoMatrix, 
										 pVerticalBlurTexture->GetShaderResourceView() );
	// Otherwise use PostProcessingTexture
	} else {
		// Render the post processing window using the texture shader
		result = pTextureShader->Render( pD3D->GetDeviceContext(),
										 pPostProcessingWindow->GetIndexCount(),
										 worldMatrix,
										 pPostProcessingWindow->GetBaseViewMatrix(),
										 orthoMatrix, 
										 pPostProcessingTexture->GetShaderResourceView() );
	}
	if( !result ) {
		return false;
	}

	// End of 2D rendering
	pD3D->TurnOnAlphaBlending();
	pD3D->TurnZBufferOn();

	// UI //
	// Only if flagged
	if( mDisplayingUI ) {

		// DEBUG TEXT //
		// More 2D rendering
		pD3D->TurnZBufferOff();
		pD3D->TurnOffAlphaBlending();

		// Stop text rotating
		pD3D->GetWorldMatrix( worldMatrix );

		// Render the text strings
		result = pText->Render( pD3D->GetDeviceContext(), worldMatrix, orthoMatrix );
		if( !result ) { 
			return false;
		}

		// RENDER DEBUGWINDOW & CURSOR //
		// Get the world, view, and ortho matrices from the camera and d3d objects
		pD3D->GetWorldMatrix( worldMatrix );
		pCamera->GetViewMatrix( viewMatrix );
		pD3D->GetOrthoMatrix( orthoMatrix );

		// Put the debug window vertex and index buffers on the graphics pipeline to prepare them for drawing
		result = pDebugWindow->Render( pD3D->GetDeviceContext(), 19, 140 );
		if( !result ) {
			return false;
		}

		// Render the debug window using the texture shader
		result = pTextureShader->Render( pD3D->GetDeviceContext(),
										 pDebugWindow->GetIndexCount(),
										 worldMatrix,
										 pDebugWindow->GetBaseViewMatrix(),
										 orthoMatrix, 
										 pRefractionTexture->GetShaderResourceView() );
		if( !result ) {
			return false;
		}

		// Put the debug window vertex and index buffers on the graphics pipeline to prepare them for drawing
		result = pDebugWindow->Render( pD3D->GetDeviceContext(), 19, 290 );
		if( !result ) {
			return false;
		}

		// Render the debug window using the texture shader
		result = pTextureShader->Render( pD3D->GetDeviceContext(),
										 pDebugWindow->GetIndexCount(),
										 worldMatrix,
										 pDebugWindow->GetBaseViewMatrix(),
										 orthoMatrix, 
										 pReflectionTexture->GetShaderResourceView() );
		if( !result ) {
			return false;
		}

		// Put the cursor vertex and index buffers on the graphics pipeline to prepare them for drawing
		result = pCursor->Render( pD3D->GetDeviceContext(), InputSingleton::GetInstance()->mMouseX - 5, InputSingleton::GetInstance()->mMouseY - 5 );
		if( !result ){ 
			return false;
		}

		// Render the cursor with the transparent shader
		result = pTransparentShader->Render( pD3D->GetDeviceContext(),
											 pCursor->GetIndexCount(),
											 worldMatrix,
											 pCursor->GetBaseViewMatrix(), 
											 orthoMatrix,
											 0.2f,
											 pCursor->GetTexture() ); 
		if( !result ) {
			return false;
		}

		// End of 2D rendering
		pD3D->TurnOnAlphaBlending();
		pD3D->TurnZBufferOn();
	}

	// Present the rendered scene to the screen
	pD3D->EndScene();

	return true;
}


// ChangeRenderMode                                 //
// Toggles displaying DebugWindows and Text Objects //
void GraphicsClass::ChangeUIDisplayMode() {
	if( mDisplayingUI ) {
		mDisplayingUI = false;
	} else {
		mDisplayingUI = true;
	}
}


// TogglePostProcessing    //
// Toggles post processing //
void GraphicsClass::TogglePostProcessing() {
	if( mApplyingBlur ) {
		mApplyingBlur = false;
	} else {
		mApplyingBlur = true;
	}
}

