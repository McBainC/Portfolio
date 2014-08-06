// Scene Matrices - VS buffer 0
cbuffer MatrixBuffer : register(b0) {
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

// Vertex Data
struct VertexInputType {
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
	float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
};

// Pixel Data
struct PixelInputType {
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
	float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
	float3 position3D : TEXCOORD1;
};

// TerrainVS
PixelInputType TerrainVertexShader( VertexInputType input ) {
    PixelInputType output;
   
    // Change the position vector to be 4 units for proper matrix calculations
    input.position.w = 1.0f;

    // Calculate the position of the vertex against the world, view, and projection matrices
    output.position = mul( input.position, worldMatrix );
    output.position = mul( output.position, viewMatrix );
    output.position = mul( output.position, projectionMatrix );

    // Store the texture coordinates for the pixel shader
    output.tex = input.tex;

    // Calculate the normal vector against the world matrix only
	output.normal = mul( input.normal, ( float3x3 )worldMatrix );
	output.normal = normalize( output.normal );

	// Calculate the tangent vector against the world matrix only
	output.tangent = mul( input.tangent, ( float3x3 )worldMatrix );
	output.tangent = normalize( output.tangent );

	// Calculate the binormal vector against the world matrix only 
	output.binormal = mul( input.binormal, ( float3x3 )worldMatrix );
	output.binormal = normalize( output.binormal );

	// 3D position of the vertex
	output.position3D = mul( input.position, worldMatrix );

    return output;
}