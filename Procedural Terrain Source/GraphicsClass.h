#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_


// System Includes //
#include <windows.h>


// Application Includes //
#include "D3Dclass.h"
#include "CameraClass.h"
#include "LightClass.h"

#include "ModelClass.h"
#include "TerrainClass.h"
#include "OceanClass.h"

#include "TextClass.h"

#include "RenderTextureClass.h"

#include "TextureShaderClass.h"
#include "TransparentShaderClass.h"
#include "ReflectionShaderClass.h"
#include "TerrainReflectionShaderClass.h"
#include "TerrainShaderClass.h"
#include "OceanShaderClass.h"

#include "HorizontalBlurShaderClass.h"
#include "VerticalBlurShaderClass.h"

#include "CursorClass.h"
#include "OrthoWindowClass.h"


// Globals //
const bool  FULL_SCREEN   = false;
const bool  VSYNC_ENABLED = true;
const float SCREEN_DEPTH  = 1000.0f;
const float SCREEN_NEAR   = 0.1f;


// GraphicsClass                                                 // 
// Contains and manages all of the scenes Graphical elements     //
// Contains / Intializes / TidiesUp all model and shader objects //
class GraphicsClass {
public:
	GraphicsClass();
	GraphicsClass( const GraphicsClass& );
	~GraphicsClass();

	bool Initialize( int, int, HWND );
	void Shutdown();

	// Frame                              //
	// Manages all frame by frame changes //
	bool Frame( int, int );

private:
	// Render Stage Functions //
	bool Render();
	bool RenderRefractionToTexture();
	bool RenderReflectionToTexture();
	bool RenderSceneToTexture();
	bool RenderHorizontalBlurToTexture();
	bool RenderVerticalBlurToTexture();
	bool RenderScene();

	// Toggle Functions //
	void ChangeUIDisplayMode();
	void TogglePostProcessing();

private:
	// D3D & Camera Objects
	D3DClass*               pD3D;
	CameraClass*            pCamera;
	
	// Shader Objects
	TextureShaderClass*           pTextureShader;
	TransparentShaderClass*       pTransparentShader;
	TerrainReflectionShaderClass* pTerrainReflectionShader;
	TerrainShaderClass*           pTerrainShader;
	OceanShaderClass*             pOceanShader;
	HorizontalBlurShaderClass*    pHorizontalBlurShader;
	VerticalBlurShaderClass*      pVerticalBlurShader;

	// Light Object
	LightClass* pLight;

	// Model Objects
	TerrainClass* pTerrain;
	ModelClass*   pSun;
	OceanClass*   pOcean;

	// RenderToTexture Objects
	RenderTextureClass* pRefractionTexture;
	RenderTextureClass* pReflectionTexture;

	// UI Objects
	TextClass*        pText;
	OrthoWindowClass* pDebugWindow;
	CursorClass*      pCursor;

	// Post Processing
	RenderTextureClass* pPostProcessingTexture;
	OrthoWindowClass*   pPostProcessingWindow;
	RenderTextureClass* pHorizontalBlurTexture;
	RenderTextureClass* pVerticalBlurTexture;

	// Member Variables
	float mRotation;
	float mWaterHeight;
	float mWaterTranslation;
	float mWaveHeight;

	// Light Position Vectors
	D3DXVECTOR3 mLightOrbit;
	D3DXVECTOR3 mLightPosition;

	// Display UI Flag
	bool mDisplayingUI;
	bool mApplyingBlur;

	// Screen Dimensions
	int mScreenHeight, mScreenWidth;

	// Terrain Variables
	int mSmoothingAmount;
	float mDisplacementRange;
};

#endif
