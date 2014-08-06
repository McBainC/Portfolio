// Scene Matrices - VS buffer 0
cbuffer MatrixBuffer : register(b0) {
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

// Reflection Matrix - VS buffer 1
cbuffer ReflectionBuffer : register(b1) {
    matrix reflectionMatrix;
};

// Wave Data - VS buffer 3
cbuffer WaveBuffer : register(b3) {
	float time;
	float waveHeight;
	float2 padding;
};

// Vertex Data
struct VertexInputType {
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

// Pixel Data
struct PixelInputType {
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
	float4 reflectionPosition : TEXCOORD1;
    float4 refractionPosition : TEXCOORD2;
	float3 position3D : TEXCOORD3;
};

// OceanVS
PixelInputType OceanVertexShader( VertexInputType input ) {
    PixelInputType output;
    matrix reflectProjectWorld;
    matrix viewProjectWorld;

	// Change the position vector to be 4 units for proper matrix calculations
    input.position.w = 1.0f;

    // Offset y position based on sine wave
    input.position.y = waveHeight * sin( input.position.x + time );

	// Add cos wave to x position 
	input.position.x += waveHeight * cos( input.position.z + ( time * 0.75f ) );

	// Add sine wave to the y position
	input.position.z += waveHeight * 0.5f * sin( input.position.x + ( time * 0.5f ) ); 

	// Calculate the position of the vertex against the world, view, and projection matrices
    output.position = mul( input.position, worldMatrix );
    output.position = mul( output.position, viewMatrix );
    output.position = mul( output.position, projectionMatrix );
    
    // Store the texture coordinates for the pixel shader
    output.tex = input.tex;

	// Create the reflection projection world matrix
    reflectProjectWorld = mul( reflectionMatrix, projectionMatrix );
    reflectProjectWorld = mul( worldMatrix, reflectProjectWorld );

    // Calculate the input position against the reflectProjectWorld matrix
    output.reflectionPosition = mul( input.position, reflectProjectWorld );

	// Create the view projection world matrix for refraction
    viewProjectWorld = mul( viewMatrix, projectionMatrix );
    viewProjectWorld = mul( worldMatrix, viewProjectWorld );
   
    // Calculate the input position against the viewProjectWorld matrix
    output.refractionPosition = mul( input.position, viewProjectWorld );

	// 3D position of the vertex
	output.position3D = mul( input.position, worldMatrix );

    return output;
}