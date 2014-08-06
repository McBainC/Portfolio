#ifndef _GAMETEXTURECONTROLLER_H_
#define _GAMETEXTURECONTROLLER_H_


#include "texture.h"
#include "font.h"
#include <string>
#include <vector>


/*
===================================================================
TextureInfo
Child class of CTexture to hold a string for its file name
Its purpose is to load itself into PS2's static memory when created
===================================================================
*/
class TextureInfo : public CTexture {
public:
	TextureInfo::TextureInfo( string textureName );
	TextureInfo::~TextureInfo();
	
private:
	string mTextureName; // bitmap file name
	
	void Init( void );
};


/*
============================================================
GameTextureController
Singleton to controller PS2 Texture Buffers
Contains all games sprite sheet bitmaps in a single vector
Checks when called to change texture if it is already loaded
============================================================
*/


/* Mask for singleton object */
#define GAMETEXTURES GameTextureController::GetInstance()


class GameTextureController {
public:
	static GameTextureController* GetInstance();
	GameTextureController::~GameTextureController();
	
	void Init( void );
	
	/* External Texture Setters */
	void SelectTexture( int textureNumber );
	void UploadTexture( int textureNumber, uint64 texBuffer );
	void DrawText( float x, float y, int r, int g, int b, string text );
	
private:
	GameTextureController::GameTextureController();
	static GameTextureController* instance;
	
	CFont font_;                         // the font class
	CTexture font_tex;                   // font texture
	
	vector< TextureInfo* > mTextures;    // texture vector
	
	uint64 mTexBuffer480, mTexBuffer496; // texture buffers
	int mTexBuffer1, mTexBuffer2;        // what index each buffer has loaded
	
	/* Vector checks and saftey functions */
	bool CheckBuffers( int newTexture );
	void CheckVectorIndex( int index );
};

#endif