//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: ../Resources/Shader/Post.shader
//
//-------------------------------------------------------------------------------------------------

$
	texture						( STATIC_EXECUTE:PIXEL )

	vertex_entry			VShader
	vertex_version		vs_5_0
	pixel_entry				PShader
	pixel_version			ps_5_0

	topology					TRIANGLELIST
	color							R8G8B8A8_UNORM
$

#include "Includes/Color.hlsli"

// Vertex output structure.
struct v2p {
	float4 Position : SV_POSITION;
};

// Pixel output structure.
struct p2f {
	float4 Color : SV_TARGET0;
};

// Vertex shader function.
v2p VShader(in uint i : SV_VertexID) {
	v2p output;
	
	/*
		-1.0f -1.0f
		 3.0f -1.0f
		-1.0f  3.0f
	*/

	// Full screen triangle.
	output.Position = float4(((i & 1) - 0.25f), ((i >> 1) & 1) - 0.25f, 0.0f, 0.25f);

	return output;
}

Texture2D inputTexture : register(t0);

// Pixel shader function.
p2f PShader(in v2p input) {	
	p2f output;
	output.Color = inputTexture[input.Position.xy];
	output.Color.a = 1.0f;
	return output;
}
