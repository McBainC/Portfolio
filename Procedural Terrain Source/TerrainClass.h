#ifndef _TERRAINCLASS_H_
#define _TERRAINCLASS_H_


// Includes //
#include <d3d11.h>
#include <d3dx10math.h>
#include <stdio.h>
#include <vector>


// Application Includes //
#include "TextureArrayClass.h"


// Texture Repeat Variable
const int TEXTURE_REPEAT = 8;


// TerrainClass - based off rastertek                                                  //
// Added tangent and biNormal variables and calculations to allow bumpmapping          //
// DiamondSqaureAlgorithm - my own work (see report for references)                    //
// Replaced terrain texture loading with Diamond-Square Algorithm                      //
// Though an odd size is passed to create the terrain (as required by Diamond-Square ) //
// The odd row and column are ignored as it breaks the texturing                       //
class TerrainClass {
private:
	// Vertex data
	struct VertexType {
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
		D3DXVECTOR3 tangent;
		D3DXVECTOR3 biNormal;
	};

	// HeightMap data
	struct HeightMapType { 
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
		float tx, ty, tz;
		float bx, by, bz;
	};

	// Tangent / BiNormal temp data
	struct TempVertexType {
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

	// Vertex
	struct VectorType { 
		float x, y, z;
	};

public:
	TerrainClass();
	TerrainClass( const TerrainClass& other );
	~TerrainClass();

	bool Initialize( ID3D11Device* deviceContext,
					 int terrainDimension,
					 int smoothingPasses,
					 float displacementValue,
					 WCHAR* textureFileName1,
					 WCHAR* textureFileName2,
					 WCHAR* textureFileName3,
					 WCHAR* textureFileName4,
					 WCHAR* textureFileName5,
					 WCHAR* textureFileName6,
					 WCHAR* textureFileName7,
					 WCHAR* textureFileName8 );

	void Shutdown();

	void Render( ID3D11DeviceContext* deviceContext );

	int GetIndexCount();

	ID3D11ShaderResourceView** GetTextureArray();

	void GenerateNewTerrain();

private:
	// Initialization functions
	//bool LoadHeightMap( char* ); // ***REMOVED*** - procedural terrain generation
	void NormalizeHeightMap();     // ***UNUSED***  - duplicated in DiamondSquareAlgorithm
	bool CalculateNormals();
	void ShutdownHeightMap();

	// Texture functions
	void CalculateTextureCoordinates();
	bool LoadTextures( ID3D11Device* device,
		               WCHAR* fileName1,
					   WCHAR* fileName2,
					   WCHAR* fileName3,
					   WCHAR* fileName4,
					   WCHAR* fileName5,
					   WCHAR* fileName6,
					   WCHAR* fileName7,
					   WCHAR* fileName8 );
	void ReleaseTexture();

	// Buffer functions
	bool InitializeBuffers( ID3D11Device* device );
	void ShutdownBuffers();
	void RenderBuffers( ID3D11DeviceContext* deviceContext );

	// Tangent / BiNormal functions
	void CalculateModelVectors();
	void CalculateTangentBiNormal( TempVertexType vertex1,
		                           TempVertexType vertex2,
								   TempVertexType vertex3,
								   VectorType& tangent,
								   VectorType& biNormal );

	void CalculateNormal( VectorType tangent,
		                  VectorType biNormal,
						  VectorType& normal );

	// Procedural terrain functions
	void SmoothHeights( int strength );
	void DiamondSquareAlgorithm( float cornerHeight, float randomRange, float heightScalar );
	float GetSquareAverage( std::vector< float > &vector, int i, int j, int step, float randomRange, float smoothingValue );

private:
	// Terrain variables
	int mTerrainWidth, mTerrainHeight;
	int mVertexCount, mIndexCount;

	// Object pointers
	ID3D11Buffer *pVertexBuffer, *pIndexBuffer;
	TextureArrayClass* pTextureArray;
	HeightMapType*     pHeightMap;
};


#endif