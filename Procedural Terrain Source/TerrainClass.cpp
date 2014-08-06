#include "TerrainClass.h"


// Default Constructor  //
// NULL object pointers //
TerrainClass::TerrainClass() {
	pVertexBuffer = 0;
	pIndexBuffer  = 0;
	pTextureArray = 0;
	pHeightMap    = 0;
}


// Constructor //
TerrainClass::TerrainClass( const TerrainClass& other ) {
}


// Destructor                   //
// Nothing to tidyup - Shutdown //
TerrainClass::~TerrainClass() {
}


// Initialize                           //
// Added tangent & biNormal calculation //
bool TerrainClass::Initialize( ID3D11Device* device,
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
					           WCHAR* textureFileName8 ) {
	bool result;

	/*
	// Load in the height map for the terrain
	result = LoadHeightMap( heightMapFileName );
	if( !result ) {
		return false;
	}

	// Normalize the height of the height map
	NormalizeHeightMap();
	*/

	// Initialize Terrain //

	// Set terrain height & width
	mTerrainHeight = mTerrainWidth = terrainDimension;

	// Create the structure to hold the height map data
	pHeightMap = new HeightMapType[ mTerrainWidth * mTerrainHeight ];
	if( !pHeightMap ) {
		return false;
	}

	// Initialize terrain data structre(1.0f height)
	int index;
	for( int j = 0; j < mTerrainHeight; j++ ) {
		for( int i = 0; i < mTerrainWidth; i++ ) {			
			index = ( mTerrainHeight * j ) + i;

			pHeightMap[ index ].x = ( float )i;
			pHeightMap[ index ].y = 1.0f; 
			pHeightMap[ index ].z = ( float )j;
		}
	}

	// Generate new terrain
	DiamondSquareAlgorithm( 10.0f, displacementValue, 2.0f );

	// Smooth (5 passes)
	SmoothHeights( smoothingPasses );

	// Calculate the normals for the terrain data
	result = CalculateNormals();
	if( !result ) {
		return false;
	}

	// Calculate the texture coordinates
	CalculateTextureCoordinates();

	// Load the texture
	result = LoadTextures( device,
		                   textureFileName1,
						   textureFileName2,
						   textureFileName3,
						   textureFileName4,
						   textureFileName5,
						   textureFileName6,
						   textureFileName7,
						   textureFileName8 );

	if( !result ) {		   
		return false;	   
	}

	// Calculate the normal, tangent, and biNormal vectors for the model
	CalculateModelVectors();

	// Initialize the vertex and index buffer that hold the geometry for the terrain
	result = InitializeBuffers( device );
	if( !result ) {
		return false;
	}

	return true;
}


// Shutdown                 //
// Tidyup pointer&new usage //
void TerrainClass::Shutdown() {
	// Release the model texture
	ReleaseTexture();

	// Release the vertex and index buffer
	ShutdownBuffers();

	// Release the height map data
	ShutdownHeightMap();

	return;
}


// Render //
void TerrainClass::Render( ID3D11DeviceContext* deviceContext ) {
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing
	RenderBuffers( deviceContext );

	return;
}


// GetIndexCount               //
// Returns terrains indexCount //
int TerrainClass::GetIndexCount() {
	return mIndexCount;
}


// GetTextureArray                                           //
// Returns pointer to the start of the terrains textureArray //
ID3D11ShaderResourceView** TerrainClass::GetTextureArray() {
	return pTextureArray->GetTextureArray();
}


// Initialize                         //
// Added tangent & biNormal loading   //
// The odd row and column are ignored //
bool TerrainClass::InitializeBuffers( ID3D11Device* device ) {
	VertexType* vertices;
	unsigned long* indices;
	int index, i, j;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int index1, index2, index3, index4;
	float tu, tv;

	// Calculate the number of vertices in the terrain mesh
	mVertexCount = ( mTerrainWidth - 2 ) * ( mTerrainHeight - 2 ) * 6;

	// Set the index count to the same as the vertex count
	mIndexCount = mVertexCount;

	// Create the vertex array.
	vertices = new VertexType[ mVertexCount ];
	if( !vertices ) {
		return false;
	}

	// Create the index array
	indices = new unsigned long[ mIndexCount ];
	if( !indices ) {
		return false;
	}

	// Initialize the index to the vertex buffer
	index = 0;

	// Load the vertex and index array with the terrain data
	// ***HACK*** - cutting off the odd row and column (should be -1)
	for( j = 0; j < ( mTerrainHeight - 2 ); j++ ) {
		for( i = 0; i < ( mTerrainWidth - 2 ); i++ ) {
			index1 = ( mTerrainHeight * j ) + i;                 // Bottom left
			index2 = ( mTerrainHeight * j ) + ( i + 1 );         // Bottom right
			index3 = ( mTerrainHeight * ( j + 1 ) ) + i;         // Upper left
			index4 = ( mTerrainHeight * ( j + 1 ) ) + ( i + 1 ); // Upper right

			// Upper left
			tv = pHeightMap[ index3 ].tv;

			// Modify the texture coordinates to cover the top edge
			if( tv == 1.0f ) { 
				tv = 0.0f; 
			}

			vertices[ index ].position = D3DXVECTOR3( pHeightMap[ index3 ].x, pHeightMap[ index3 ].y, pHeightMap[ index3 ].z );
			vertices[ index ].texture  = D3DXVECTOR2( pHeightMap[ index3 ].tu, tv );
			vertices[ index ].normal   = D3DXVECTOR3( pHeightMap[ index3 ].nx, pHeightMap[ index3 ].ny, pHeightMap[ index3 ].nz );
			vertices[ index ].tangent  = D3DXVECTOR3( pHeightMap[ index3 ].tx, pHeightMap[ index3 ].ty, pHeightMap[ index3 ].tz );
			vertices[ index ].biNormal = D3DXVECTOR3( pHeightMap[ index3 ].bx, pHeightMap[ index3 ].by, pHeightMap[ index3 ].bz );
			indices[ index ] = index;
			index++;

			// Upper right
			tu = pHeightMap[ index4 ].tu;
			tv = pHeightMap[ index4 ].tv;

			// Modify the texture coordinates to cover the top and right edge
			if( tu == 0.0f ) { 
				tu = 1.0f; 
			}

			if( tv == 1.0f ) { 
				tv = 0.0f; 
			}

			vertices[ index ].position = D3DXVECTOR3( pHeightMap[ index4 ].x, pHeightMap[ index4 ].y, pHeightMap[ index4 ].z );
			vertices[ index ].texture  = D3DXVECTOR2( tu, tv );
			vertices[ index ].normal   = D3DXVECTOR3( pHeightMap[ index4 ].nx, pHeightMap[ index4 ].ny, pHeightMap[ index4 ].nz );
			vertices[ index ].tangent  = D3DXVECTOR3( pHeightMap[ index4 ].tx, pHeightMap[ index4 ].ty, pHeightMap[ index4 ].tz );
			vertices[ index ].biNormal = D3DXVECTOR3( pHeightMap[ index4 ].bx, pHeightMap[ index4 ].by, pHeightMap[ index4 ].bz );
			indices[ index ] = index;
			index++;

			// Bottom left
			vertices[ index ].position = D3DXVECTOR3( pHeightMap[ index1 ].x, pHeightMap[ index1 ].y, pHeightMap[ index1 ].z );
			vertices[ index ].texture  = D3DXVECTOR2( pHeightMap[ index1 ].tu, pHeightMap[ index1 ].tv );
			vertices[ index ].normal   = D3DXVECTOR3( pHeightMap[ index1 ].nx, pHeightMap[ index1 ].ny, pHeightMap[ index1 ].nz );
			vertices[ index ].tangent  = D3DXVECTOR3( pHeightMap[ index1 ].tx, pHeightMap[ index1 ].ty, pHeightMap[ index1 ].tz );
			vertices[ index ].biNormal = D3DXVECTOR3( pHeightMap[ index1 ].bx, pHeightMap[ index1 ].by, pHeightMap[ index1 ].bz );
			indices[ index ] = index;
			index++;

			// Bottom left
			vertices[ index ].position = D3DXVECTOR3( pHeightMap[ index1 ].x, pHeightMap[ index1 ].y, pHeightMap[ index1 ].z );
			vertices[ index ].texture  = D3DXVECTOR2( pHeightMap[ index1 ].tu, pHeightMap[ index1 ].tv );
			vertices[ index ].normal   = D3DXVECTOR3( pHeightMap[ index1 ].nx, pHeightMap[ index1 ].ny, pHeightMap[ index1 ].nz );
			vertices[ index ].tangent  = D3DXVECTOR3( pHeightMap[ index1 ].tx, pHeightMap[ index1 ].ty, pHeightMap[ index1 ].tz );
			vertices[ index ].biNormal = D3DXVECTOR3( pHeightMap[ index1 ].bx, pHeightMap[ index1 ].by, pHeightMap[ index1 ].bz );
			indices[ index ] = index;
			index++;

			// Upper right
			tu = pHeightMap[ index4 ].tu;
			tv = pHeightMap[ index4 ].tv;

			// Modify the texture coordinates to cover the top and right edge
			if( tu == 0.0f ) {
				tu = 1.0f; 
			}

			if( tv == 1.0f ) {
				tv = 0.0f; 
			}

			vertices[ index ].position = D3DXVECTOR3( pHeightMap[ index4 ].x, pHeightMap[ index4 ].y, pHeightMap[ index4 ].z );
			vertices[ index ].texture  = D3DXVECTOR2( tu, tv );
			vertices[ index ].normal   = D3DXVECTOR3( pHeightMap[ index4 ].nx, pHeightMap[ index4 ].ny, pHeightMap[ index4 ].nz );
			vertices[ index ].tangent  = D3DXVECTOR3( pHeightMap[ index4 ].tx, pHeightMap[ index4 ].ty, pHeightMap[ index4 ].tz );
			vertices[ index ].biNormal = D3DXVECTOR3( pHeightMap[ index4 ].bx, pHeightMap[ index4 ].by, pHeightMap[ index4 ].bz );
			indices[ index ] = index;
			index++;

			// Bottom right
			tu = pHeightMap[ index2 ].tu;

			// Modify the texture coordinates to cover the right edge
			if( tu == 0.0f ) { 
				tu = 1.0f; 
			}

			vertices[ index ].position = D3DXVECTOR3( pHeightMap[ index2 ].x, pHeightMap[ index2 ].y, pHeightMap[ index2 ].z );
			vertices[ index ].texture  = D3DXVECTOR2( tu, pHeightMap[ index2 ].tv );
			vertices[ index ].normal   = D3DXVECTOR3( pHeightMap[ index2 ].nx, pHeightMap[ index2 ].ny, pHeightMap[ index2 ].nz );
			vertices[ index ].tangent  = D3DXVECTOR3( pHeightMap[ index2 ].tx, pHeightMap[ index2 ].ty, pHeightMap[ index2 ].tz );
			vertices[ index ].biNormal = D3DXVECTOR3( pHeightMap[ index2 ].bx, pHeightMap[ index2 ].by, pHeightMap[ index2 ].bz );
			indices[ index ] = index;
			index++;
		}
	}

	// Set up the description of the static vertex buffer
	vertexBufferDesc.Usage               = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth           = sizeof( VertexType ) * mVertexCount;
	vertexBufferDesc.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags      = 0;
	vertexBufferDesc.MiscFlags           = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data
	vertexData.pSysMem          = vertices;
	vertexData.SysMemPitch      = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer
	result = device->CreateBuffer( &vertexBufferDesc, &vertexData, &pVertexBuffer );
	if( FAILED( result ) ) {
		return false;
	}

	// Set up the description of the static index buffer
	indexBufferDesc.Usage               = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth           = sizeof( unsigned long ) * mIndexCount;
	indexBufferDesc.BindFlags           = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags      = 0;
	indexBufferDesc.MiscFlags           = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data
	indexData.pSysMem          = indices;
	indexData.SysMemPitch      = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer
	result = device->CreateBuffer( &indexBufferDesc, &indexData, &pIndexBuffer );
	if( FAILED( result ) ) {
		return false;
	}

	// Release the arrays now that the buffers have been created and loaded
	delete [] vertices;
	vertices = 0;

	delete [] indices;
	indices = 0;

	return true;
}


// ShutdownBuffers //
void TerrainClass::ShutdownBuffers() {
	// Release the index buffer
	if( pIndexBuffer ) {
		pIndexBuffer->Release();
		pIndexBuffer = 0;
	}

	// Release the vertex buffer
	if( pVertexBuffer ) {
		pVertexBuffer->Release();
		pVertexBuffer = 0;
	}

	return;
}


// RenderBuffers //
void TerrainClass::RenderBuffers( ID3D11DeviceContext* deviceContext ) {
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset
	stride = sizeof( VertexType ); 
	offset = 0;
    
	// Set the vertex buffer to active in the input assembler so it can be rendered
	deviceContext->IASetVertexBuffers( 0, 1, &pVertexBuffer, &stride, &offset );

	// Set the index buffer to active in the input assembler so it can be rendered
	deviceContext->IASetIndexBuffer( pIndexBuffer, DXGI_FORMAT_R32_UINT, 0 );

	// Set the type of primitive that should be rendered from this vertex buffer, in this case a line list
	deviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	return;
}


// LoadTexture                                                      //
// Loads the terrains textures and heightmaps into the textureArray //
bool TerrainClass::LoadTextures( ID3D11Device* device,
								 WCHAR* fileName1,
								 WCHAR* fileName2,
								 WCHAR* fileName3,
								 WCHAR* fileName4,
								 WCHAR* fileName5,
								 WCHAR* fileName6,
								 WCHAR* fileName7,
								 WCHAR* fileName8 ) {
	bool result;

	// Create the texture object
	pTextureArray = new TextureArrayClass;
	if( !pTextureArray ) {
		return false;
	}

	// Initialize the texture object
	result = pTextureArray->Initialize( device,
		                                fileName1,
										fileName2,
										fileName3,
										fileName4,
										fileName5,
										fileName6,
										fileName7,
										fileName8 );

	if( !result ) {
		return false;
	}

	return true;
}


// ReleaseTexture //
void TerrainClass::ReleaseTexture() {
	// Release the texture object
	if( pTextureArray ) {
		pTextureArray->Shutdown();
		delete pTextureArray;
		pTextureArray = 0;
	}

	return;
}


/*
// ***REMOVED*** Procedural Terrain!
// LoadHeightMap //
bool TerrainClass::LoadHeightMap( char* filename ) {
	FILE* filePtr;
	int error;
	unsigned int count;
	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;
	int imageSize, i, j, k, index;
	unsigned char* bitmapImage;
	unsigned char height;

	// Open the height map file in binary
	error = fopen_s( &filePtr, filename, "rb" );
	if( error != 0 ) {
		return false;
	}

	// Read in the file header
	count = fread( &bitmapFileHeader, sizeof( BITMAPFILEHEADER ), 1, filePtr );
	if( count != 1 ) {
		return false;
	}

	// Read in the bitmap info header
	count = fread( &bitmapInfoHeader, sizeof( BITMAPINFOHEADER ), 1, filePtr );
	if( count != 1 ) {
		return false;
	}

	// Save the dimensions of the terrain
	mTerrainWidth  = bitmapInfoHeader.biWidth;
	mTerrainHeight = bitmapInfoHeader.biHeight;

	// Calculate the size of the bitmap image data
	imageSize = mTerrainWidth * mTerrainHeight * 3;

	// Allocate memory for the bitmap image data
	bitmapImage = new unsigned char[ imageSize ];
	if( !bitmapImage ) {
		return false;
	}

	// Move to the beginning of the bitmap data
	fseek( filePtr, bitmapFileHeader.bfOffBits, SEEK_SET );

	// Read in the bitmap image data
	count = fread( bitmapImage, 1, imageSize, filePtr );
	if( count != imageSize ) {
		return false;
	}

	// Close the file
	error = fclose( filePtr );
	if( error != 0 ) {
		return false;
	}

	// Create the structure to hold the height map data
	pHeightMap = new HeightMapType[ mTerrainWidth * mTerrainHeight ];
	if( !pHeightMap ) {
		return false;
	}

	// Initialize the position in the image data buffer
	k = 0;

	// Read the image data into the height map
	for( j = 0; j < mTerrainHeight; j++ ) {
		for(i = 0; i < mTerrainWidth; i++ ) {
			height = bitmapImage[ k ];
			
			index = ( mTerrainHeight * j ) + i;

			pHeightMap[ index ].x = ( float )i;
			pHeightMap[ index ].y = ( float )height;
			pHeightMap[ index ].z = ( float )j;

			k += 3;
		}
	}

	// Release the bitmap image data
	delete [] bitmapImage;
	bitmapImage = 0;

	return true;
}
*/


// NormalizeHeightMap                                      //
// Lowered magic number slightly                           //
// ***UNUSED*** - Diamond-Square modifies the final height //
void TerrainClass::NormalizeHeightMap() {
	int i, j;

	for( j = 0; j < mTerrainHeight; j++ ) {
		for( i = 0; i < mTerrainWidth; i++ ) {
			pHeightMap[ ( mTerrainHeight * j ) + i ].y /= 10.0f;
		}
	}

	return;
}


// CalculateNormals //
bool TerrainClass::CalculateNormals() {
	int i, j, index1, index2, index3, index, count;
	float vertex1[ 3 ], vertex2[ 3 ], vertex3[ 3 ], vector1[ 3 ], vector2[ 3 ], sum[ 3 ], length;
	VectorType* normals;

	// Create a temporary array to hold the un-normalized normal vectors
	normals = new VectorType[ ( mTerrainHeight - 1 ) * ( mTerrainWidth - 1 ) ];
	if( !normals ) {
		return false;
	}

	// Go through all the faces in the mesh and calculate their normals
	for( j = 0; j < ( mTerrainHeight - 1 ); j++ ) {
		for( i = 0; i < ( mTerrainWidth - 1 ); i++ ) {
			index1 = ( j * mTerrainHeight ) + i;
			index2 = ( j * mTerrainHeight ) + ( i + 1 );
			index3 = ( ( j + 1 ) * mTerrainHeight ) + i;

			// Get three vertices from the face
			vertex1[ 0 ] = pHeightMap[ index1 ].x;
			vertex1[ 1 ] = pHeightMap[ index1 ].y;
			vertex1[ 2 ] = pHeightMap[ index1 ].z;
					   				   		  
			vertex2[ 0 ] = pHeightMap[ index2 ].x;
			vertex2[ 1 ] = pHeightMap[ index2 ].y;
			vertex2[ 2 ] = pHeightMap[ index2 ].z;
					   				   		  
			vertex3[ 0 ] = pHeightMap[ index3 ].x;
			vertex3[ 1 ] = pHeightMap[ index3 ].y;
			vertex3[ 2 ] = pHeightMap[ index3 ].z;

			// Calculate the two vectors for this face
			vector1[ 0 ] = vertex1[ 0 ] - vertex3[ 0 ];
			vector1[ 1 ] = vertex1[ 1 ] - vertex3[ 1 ];
			vector1[ 2 ] = vertex1[ 2 ] - vertex3[ 2 ];
			vector2[ 0 ] = vertex3[ 0 ] - vertex2[ 0 ];
			vector2[ 1 ] = vertex3[ 1 ] - vertex2[ 1 ];
			vector2[ 2 ] = vertex3[ 2 ] - vertex2[ 2 ];
					   
			index = ( j * ( mTerrainHeight - 1 ) ) + i;

			// Calculate the cross product of those two vectors to get the un-normalized value for this face normal
			normals[ index ].x = ( vector1[ 1 ] * vector2[ 2 ] ) - ( vector1[ 2 ] * vector2[ 1 ] );
			normals[ index ].y = ( vector1[ 2 ] * vector2[ 0 ] ) - ( vector1[ 0 ] * vector2[ 2 ] );
			normals[ index ].z = ( vector1[ 0 ] * vector2[ 1 ] ) - ( vector1[ 1 ] * vector2[ 0 ] );
		}
	}

	// Now go through all the vertices and take an average of each face normal 	
	// that the vertex touches to get the averaged normal for that vertex
	for( j = 0; j < mTerrainHeight; j++ ) {
		for( i = 0; i < mTerrainWidth; i++ ) {
			// Initialize the sum
			sum[ 0 ] = 0.0f;
			sum[ 1 ] = 0.0f;
			sum[ 2 ] = 0.0f;

			// Initialize the count
			count = 0;

			// Bottom left face
			if( ( ( i - 1 ) >= 0 ) && ( ( j - 1 ) >= 0 ) ) {
				index = ( ( j - 1 ) * ( mTerrainHeight - 1 ) ) + ( i - 1 );

				sum[ 0 ] += normals[ index ].x;
				sum[ 1 ] += normals[ index ].y;
				sum[ 2 ] += normals[ index ].z;
				count++;
			}

			// Bottom right face
			if( (i < ( mTerrainWidth - 1 ) ) && ( ( j - 1 ) >= 0 ) ) {
				index = ( ( j - 1 ) * ( mTerrainHeight - 1 ) ) + i;

				sum[ 0 ] += normals[ index ].x;
				sum[ 1 ] += normals[ index ].y;
				sum[ 2 ] += normals[ index ].z;
				count++;
			}

			// Upper left face
			if( ( ( i - 1 ) >= 0 ) && ( j < ( mTerrainHeight - 1 ) ) ) {
				index = ( j * ( mTerrainHeight - 1 ) ) + ( i - 1 );

				sum[ 0 ] += normals[ index ].x;
				sum[ 1 ] += normals[ index ].y;
				sum[ 2 ] += normals[ index ].z;
				count++;
			}

			// Upper right face
			if( ( i < ( mTerrainWidth - 1 ) ) && ( j < ( mTerrainHeight - 1 ) ) ) {
				index = ( j * ( mTerrainHeight - 1 ) ) + i;

				sum[ 0 ] += normals[ index ].x;
				sum[ 1 ] += normals[ index ].y;
				sum[ 2 ] += normals[ index ].z;
				count++;
			}
			
			// Take the average of the faces touching this vertex
			sum[ 0 ] = ( sum[ 0 ] / ( float )count );
			sum[ 1 ] = ( sum[ 1 ] / ( float )count );
			sum[ 2 ] = ( sum[ 2 ] / ( float )count );

			// Calculate the length of this normal
			length = sqrt( ( sum[ 0 ] * sum[ 0 ] ) + ( sum[ 1 ] * sum[ 1 ] ) + ( sum[ 2 ] * sum[ 2 ] ) );
			
			// Get an index to the vertex location in the height map array
			index = ( j * mTerrainHeight ) + i;

			// Normalize the final shared normal for this vertex and store it in the height map array
			pHeightMap[ index ].nx = ( sum[ 0 ] / length );
			pHeightMap[ index ].ny = ( sum[ 1 ] / length );
			pHeightMap[ index ].nz = ( sum[ 2 ] / length );
		}
	}

	// Release the temporary normals
	delete [] normals;
	normals = 0;

	return true;
}


// ShutdownHeightMap //
void TerrainClass::ShutdownHeightMap() {
	if( pHeightMap ) {
		delete[] pHeightMap;
		pHeightMap = 0;
	}

	return;
}


// CalculateTextureCoordinates                      //
// Requires even sized terrain                      //
// Have cheated and lowered height and width by one //
void TerrainClass::CalculateTextureCoordinates() {
	int incrementCount, i, j, tuCount, tvCount;
	float incrementValue, tuCoordinate, tvCoordinate;

	// Calculate how much to increment the texture coordinates by
	incrementValue = ( float )TEXTURE_REPEAT / ( float )mTerrainWidth;

	// Calculate how many times to repeat the texture
	incrementCount = mTerrainWidth / TEXTURE_REPEAT;

	// Initialize the tu and tv coordinate values
	tuCoordinate = 0.0f;
	tvCoordinate = 1.0f;

	// Initialize the tu and tv coordinate indexes
	tuCount = 0;
	tvCount = 0;

	// Loop through the entire height map and calculate the tu and tv texture coordinates for each vertex
	// Cheating here by removing a single row and column off the end(the odd size)
	for( j = 0; j < mTerrainHeight - 1; j++ ) {
		for(i = 0; i < mTerrainWidth - 1; i++ ) {
			// Store the texture coordinate in the height map
			pHeightMap[ ( mTerrainHeight * j ) + i ].tu = tuCoordinate;
			pHeightMap[ ( mTerrainHeight * j ) + i ].tv = tvCoordinate;

			// Increment the tu texture coordinate by the increment value and increment the index by one
			tuCoordinate += incrementValue;
			tuCount++;

			// Check if at the far right end of the texture and if so then start at the beginning again
			if( tuCount == incrementCount ) {
				tuCoordinate = 0.0f;
				tuCount = 0;
			}
		}

		// Increment the tv texture coordinate by the increment value and increment the index by one
		tvCoordinate -= incrementValue;
		tvCount++;

		// Check if at the top of the texture and if so then start at the bottom again
		if( tvCount == incrementCount ) {
			tvCoordinate = 1.0f;
			tvCount = 0;
		}
	}

	return;
}


// CalculateModelVectors //
void TerrainClass::CalculateModelVectors() {
	int faceCount, i, index;
	TempVertexType vertex1, vertex2, vertex3;
	VectorType tangent, biNormal, normal;

	// Calculate the number of faces in the model.
	faceCount = mVertexCount / 3;

	// Initialize the index to the model data.
	index = 0;

	// Go through all the faces and calculate the the tangent, biNormal, and normal vectors
	for( i = 0; i < faceCount; i++ ) {
		// Get the three vertices for this face from the model
		vertex1.x  = pHeightMap[ index ].x;
		vertex1.y  = pHeightMap[ index ].y;
		vertex1.z  = pHeightMap[ index ].z;
		vertex1.tu = pHeightMap[ index ].tu;
		vertex1.tv = pHeightMap[ index ].tv;
		vertex1.nx = pHeightMap[ index ].nx;
		vertex1.ny = pHeightMap[ index ].ny;
		vertex1.nz = pHeightMap[ index ].nz;
		index++;

		vertex2.x  = pHeightMap[ index ].x;
		vertex2.y  = pHeightMap[ index ].y;
		vertex2.z  = pHeightMap[ index ].z;
		vertex2.tu = pHeightMap[ index ].tu;
		vertex2.tv = pHeightMap[ index ].tv;
		vertex2.nx = pHeightMap[ index ].nx;
		vertex2.ny = pHeightMap[ index ].ny;
		vertex2.nz = pHeightMap[ index ].nz;
		index++;

		vertex3.x  = pHeightMap[ index ].x;
		vertex3.y  = pHeightMap[ index ].y;
		vertex3.z  = pHeightMap[ index ].z;
		vertex3.tu = pHeightMap[ index ].tu;
		vertex3.tv = pHeightMap[ index ].tv;
		vertex3.nx = pHeightMap[ index ].nx;
		vertex3.ny = pHeightMap[ index ].ny;
		vertex3.nz = pHeightMap[ index ].nz;
		index++;

		// Calculate the tangent and biNormal of that face
		CalculateTangentBiNormal( vertex1, vertex2, vertex3, tangent, biNormal );

		// Calculate the new normal using the tangent and biNormal
		CalculateNormal( tangent, biNormal, normal );

		// Store the normal, tangent, and biNormal for this face back in the model structure
		pHeightMap[ index - 1 ].nx = normal.x;
		pHeightMap[ index - 1 ].ny = normal.y;
		pHeightMap[ index - 1 ].nz = normal.z;
		pHeightMap[ index - 1 ].tx = tangent.x;
		pHeightMap[ index - 1 ].ty = tangent.y;
		pHeightMap[ index - 1 ].tz = tangent.z;
		pHeightMap[ index - 1 ].bx = biNormal.x;
		pHeightMap[ index - 1 ].by = biNormal.y;
		pHeightMap[ index - 1 ].bz = biNormal.z;

		pHeightMap[ index - 2 ].nx = normal.x;
		pHeightMap[ index - 2 ].ny = normal.y;
		pHeightMap[ index - 2 ].nz = normal.z;
		pHeightMap[ index - 2 ].tx = tangent.x;
		pHeightMap[ index - 2 ].ty = tangent.y;
		pHeightMap[ index - 2 ].tz = tangent.z;
		pHeightMap[ index - 2 ].bx = biNormal.x;
		pHeightMap[ index - 2 ].by = biNormal.y;
		pHeightMap[ index - 2 ].bz = biNormal.z;

		pHeightMap[ index - 3 ].nx = normal.x;
		pHeightMap[ index - 3 ].ny = normal.y;
		pHeightMap[ index - 3 ].nz = normal.z;
		pHeightMap[ index - 3 ].tx = tangent.x;
		pHeightMap[ index - 3 ].ty = tangent.y;
		pHeightMap[ index - 3 ].tz = tangent.z;
		pHeightMap[ index - 3 ].bx = biNormal.x;
		pHeightMap[ index - 3 ].by = biNormal.y;
		pHeightMap[ index - 3 ].bz = biNormal.z;
	}

	return;
}


// CalculateTangentbiNormal // 
void TerrainClass::CalculateTangentBiNormal( TempVertexType vertex1,
	                                         TempVertexType vertex2, 
										     TempVertexType vertex3,
					                         VectorType& tangent, 
										     VectorType& biNormal ) {
	float vector1[ 3 ], vector2[ 3 ];
	float tuVector[ 2 ], tvVector[ 2 ];
	float den;
	float length;

	// Calculate the two vectors for this face
	vector1[ 0 ] = vertex2.x - vertex1.x;
	vector1[ 1 ] = vertex2.y - vertex1.y;
	vector1[ 2 ] = vertex2.z - vertex1.z;

	vector2[ 0 ] = vertex3.x - vertex1.x;
	vector2[ 1 ] = vertex3.y - vertex1.y;
	vector2[ 2 ] = vertex3.z - vertex1.z;

	// Calculate the tu and tv texture space vectors
	tuVector[ 0 ] = vertex2.tu - vertex1.tu;
	tvVector[ 0 ] = vertex2.tv - vertex1.tv;

	tuVector[ 1 ] = vertex3.tu - vertex1.tu;
	tvVector[ 1 ] = vertex3.tv - vertex1.tv;

	// Calculate the denominator of the tangent/biNormal equation
	den = 1.0f / ( tuVector[ 0 ] * tvVector[ 1 ] - tuVector[ 1 ] * tvVector[ 0 ] );

	// Calculate the cross products and multiply by the coefficient to get the tangent and biNormal
	tangent.x = ( tvVector[ 1 ] * vector1[ 0 ] - tvVector[ 0 ] * vector2[ 0 ] ) * den;
	tangent.y = ( tvVector[ 1 ] * vector1[ 1 ] - tvVector[ 0 ] * vector2[ 1 ] ) * den;
	tangent.z = ( tvVector[ 1 ] * vector1[ 2 ] - tvVector[ 0 ] * vector2[ 2 ] ) * den;

	biNormal.x = ( tuVector[ 0 ] * vector2[ 0 ] - tuVector[ 1 ] * vector1[ 0 ] ) * den;
	biNormal.y = ( tuVector[ 0 ] * vector2[ 1 ] - tuVector[ 1 ] * vector1[ 1 ] ) * den;
	biNormal.z = ( tuVector[ 0 ] * vector2[ 2 ] - tuVector[ 1 ] * vector1[ 2 ] ) * den;

	// Calculate the length of this normal
	length = sqrt( ( tangent.x * tangent.x ) + ( tangent.y * tangent.y ) + ( tangent.z * tangent.z ) );
			
	// Normalize the normal and then store it
	tangent.x = tangent.x / length;
	tangent.y = tangent.y / length;
	tangent.z = tangent.z / length;

	// Calculate the length of this normal
	length = sqrt( ( biNormal.x * biNormal.x ) + ( biNormal.y * biNormal.y ) + ( biNormal.z * biNormal.z ) );
			
	// Normalize the normal and then store it
	biNormal.x = biNormal.x / length;
	biNormal.y = biNormal.y / length;
	biNormal.z = biNormal.z / length;

	return;
}


// CalculateNormal //
void TerrainClass::CalculateNormal( VectorType tangent, VectorType biNormal, VectorType& normal ) {
	float length;

	// Calculate the cross product of the tangent and biNormal which will give the normal vector
	normal.x = ( tangent.y * biNormal.z ) - ( tangent.z * biNormal.y );
	normal.y = ( tangent.z * biNormal.x ) - ( tangent.x * biNormal.z );
	normal.z = ( tangent.x * biNormal.y ) - ( tangent.y * biNormal.x );

	// Calculate the length of the normal
	length = sqrt( ( normal.x * normal.x ) + ( normal.y * normal.y ) + ( normal.z * normal.z ) );

	// Normalize the normal
	normal.x = normal.x / length;
	normal.y = normal.y / length;
	normal.z = normal.z / length;

	return;
}


// RandomRange                           //
// Random float between passed max & min //
float RandomRange( float min, float max ) {
	if( max < min ) {
		return 0.0f;
	}

	return ( ( rand() % 20000 ) / 20000.0f ) * ( max - min ) + min;
}


// SmoothHeights                                                    //
// Creates a local copy of the y (height) values then smoothes      //
// Calculates the average height considering up 8 neighbour heights //
void TerrainClass::SmoothHeights( int strength ) {
	// Number of passes = smoothness
	for( int i = 0; i < strength; i++ ) { 
		// Local storage vector for heights 
		std::vector< float > heights;

		// Resize to fit incoming data
		heights.resize( mTerrainHeight * mTerrainWidth );

		// Initialize index counter
		int index = 0;

		// Fill local copy
		for( int j = 0; j < mTerrainHeight; j++ ) {
			for( int i = 0; i < mTerrainWidth; i++ ) {
				// Get current index
				index = ( mTerrainHeight * j ) + i;

				// Set height
				heights[ index ] = ( pHeightMap[ ( mTerrainHeight * j ) + i ].y );
			}
		}

		// Reset index counter
		index = 0;

		// Loop through terrain
		for( int j = 0; j < mTerrainHeight; j++ ) {
			for( int i = 0; i < mTerrainWidth; i++ ) {	
				// Get current index
				index = ( mTerrainHeight * j ) + i;

				// Temp average variables
				float averageHeight = 0;
				int   numOfAverages = 0;

				// Check legal neighbours
				// North
				if( ( j - 1 ) > 0 ) {
					averageHeight += heights[ ( mTerrainHeight * ( j - 1 ) ) + i ];
					numOfAverages++;
				}
				// North-East
				if( ( ( j - 1 ) > 0 ) && ( ( i + 1 ) < mTerrainWidth ) ) {
					averageHeight += heights[ ( mTerrainHeight * ( j - 1 ) ) + ( i + 1 ) ];
					numOfAverages++;
				}
				// East
				if( ( i + 1 ) < mTerrainWidth ) {
					averageHeight += heights[ ( mTerrainHeight * j ) + ( i + 1 ) ];
					numOfAverages++;
				}
				// South-East
				if( ( ( j + 1 ) < mTerrainHeight ) && ( ( i + 1 ) < mTerrainWidth ) ) {
					averageHeight += heights[ ( mTerrainHeight * ( j + 1 ) ) + ( i + 1 ) ];
					numOfAverages++;
				}
				// South
				if( ( j + 1 ) < mTerrainHeight ) {
					averageHeight += heights[ ( mTerrainHeight * ( j + 1 ) ) + i ];
					numOfAverages++;
				}
				// South-West
				if( ( ( j + 1 ) < mTerrainHeight ) && ( ( i - 1 ) > 0 ) ) {
					averageHeight += heights[ ( mTerrainHeight * ( j + 1 ) ) + ( i - 1 ) ];
					numOfAverages++;
				}
				// West
				if( ( i - 1 ) > 0 ) {
					averageHeight += heights[ ( mTerrainHeight * j ) + ( i - 1 ) ];
					numOfAverages++;
				}
				// North-West
				if( ( ( j - 1 ) > 0 ) && ( ( i - 1 ) > 0 ) ) {
					averageHeight += heights[ ( mTerrainHeight * ( j - 1 ) ) + ( i - 1 ) ];
					numOfAverages++;
				}

				// Calculate and set average height
				if( numOfAverages > 0 ) {
					pHeightMap[ index ].y = ( averageHeight / numOfAverages );
				} else {
					pHeightMap[ index ].y = 0.0f;
				}
			}
		}
	}
}


// DiamondSqaureAlgorithm                                                              //
// Run Log base 2 (height or width) times, terrain must be square with (2^n) + 1 sides //
// Set the initial height for the four corners                                         //
// The range(+-) of the offset and a scalar for the final heights(0.0f for none)       //
void TerrainClass::DiamondSquareAlgorithm( float cornerHeight, float randomRange, float heightScalar ) {
	// Step 1 - create and initialize duplicate vector //
	// Storage vector, its 2D dimensions, and its index
	std::vector< float > heights;
	int index, numOfIterations, step;

	// Initialize variables
	step = ( mTerrainHeight - 1 ); // -1 as dimensions are odd
	index = numOfIterations = 0;
	heights.resize( mTerrainHeight * mTerrainWidth );

	// Initialize heights vector
	for( int i = 0; i < ( int )heights.size(); i++ ) {
		heights[ i ] = 0.0f;
	}

	// Set the corner heights
	heights[ 0 ]                                                                       = cornerHeight; // bottom-left
	heights[ ( mTerrainHeight * ( mTerrainHeight - 1 ) ) ]                             = cornerHeight; // top-left
	heights[ ( mTerrainWidth - 1 ) ]                                                   = cornerHeight; // bottom-right
	heights[ ( ( mTerrainHeight * ( mTerrainHeight - 1 ) ) + ( mTerrainWidth - 1 ) ) ] = cornerHeight; // top-right

	// Step 2 - Diamond Square algorithm //
	// Loop till step becomes less than 1 
	while( step > 1 ) {
		// Increment variables for current iteration
		numOfIterations++;
		step /= 2;
		index = 0;

		// Loop through center points
		for( int j = step; j < mTerrainHeight - step; j += ( step * 2 ) ) {
			for( int i = step; i < mTerrainWidth - step; i += ( step * 2 ) ) {
				// DIAMOND //
				// There will always be four diagonal neighbours
				// Initialize average height
				float averageHeight = 0.0f;

				// Top-left
				averageHeight += heights[ ( mTerrainHeight * ( j - step ) ) + ( i - step ) ];
				// Top-right
				averageHeight += heights[ ( mTerrainHeight * ( j - step ) ) + ( i + step ) ];
				// Bottom-left
				averageHeight += heights[ ( mTerrainHeight * ( j + step ) ) + ( i - step ) ];
				// Bottom-right
				averageHeight += heights[ ( mTerrainHeight * ( j + step ) ) + ( i + step ) ];

				// Get current index
				index = ( mTerrainHeight * j ) + i;

				float smoothingValue = ( float )numOfIterations;

				// Set as average of four corners + a random float from -randomRange to randomRange
				heights[ index ] = ( averageHeight / 4.0f ) + RandomRange( -randomRange, randomRange ) / smoothingValue; 

				// SQUARE //
				// Calls GetSquareAverage() for the points NESW of the center point(if within bounds)
				// Calculates its average height based on its NESW neighbours(if within bounds)
				// Smoothing value reduced to 3/4 for square step
				// North
				if( ( j - step ) >= 0 ) {
					heights[ ( mTerrainHeight * ( j - step ) ) + i ] = GetSquareAverage( heights, i, ( j - step ), step, randomRange, smoothingValue * 0.75f );
				}
				// East
				if( ( i + step ) < mTerrainWidth ) {
					heights[ ( mTerrainHeight * j ) + ( i + step ) ] = GetSquareAverage( heights, ( i + step ), j, step, randomRange, smoothingValue * 0.75f );
				}
				// South
				if( ( j + step ) < mTerrainHeight ) {
					heights[ ( mTerrainHeight * ( j + step ) ) + i ] = GetSquareAverage( heights, i, ( j + step ), step, randomRange, smoothingValue * 0.75f );
				}
				// West
				if( ( i - step ) >= 0 ) {
					heights[ ( mTerrainHeight * j ) + ( i - step ) ] = GetSquareAverage( heights, ( i - step ), j, step, randomRange, smoothingValue * 0.75f );
				}
			}
		}
	}

	// Set data
	// Making sure to loop
	for( int j = 0; j < mTerrainHeight; j++ ) {
		for( int i = 0; i < mTerrainWidth; i++ ) {
			// Get current index
			index = ( mTerrainHeight * j ) + i;

			// Set data
			// Displace down by half of the initial height
			// Scalar provided
			pHeightMap[ index ].y = ( ( heights[ index ] - ( cornerHeight / 2.0f ) ) * heightScalar );
		}
	}

	// Tidy vector
	heights.clear();
}


// GetSquareAverage                                                  //
// Gets NESW neighbours (if within bounds) and returns average value //
float TerrainClass::GetSquareAverage( std::vector< float > &vector, int i, int j, int step, float randomRange, float smoothingValue ) {
	// Initialize variables
	float averageHeight = 0.0f;
	float numOfAverages = 0;

	// North
	if( ( j - step ) >= 0 ) {
		averageHeight += vector[ ( mTerrainHeight * ( j - step ) ) + i ];
		numOfAverages++;
	}
	// East
	if( ( i + step ) < ( mTerrainWidth ) ) {
		averageHeight += vector[ ( mTerrainHeight * j ) + ( i + step ) ];
		numOfAverages++;
	}
	// South
	if( ( j + step ) < ( mTerrainHeight ) ) {
		averageHeight += vector[ ( mTerrainHeight * ( j + step ) ) + i ];
		numOfAverages++;
	}
	// West
	if( ( i - step ) >= 0 ) {
		averageHeight += vector[ ( mTerrainHeight * j ) + ( i - step ) ];
		numOfAverages++;
	}

	// Calculate square average plus small random offset
	float newHeight = ( averageHeight / numOfAverages ) + RandomRange( -randomRange, randomRange ) / smoothingValue; 

	// Return newHeight
	return newHeight;
}

