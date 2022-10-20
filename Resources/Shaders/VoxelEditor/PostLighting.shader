//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: ../Resources/Shaders/VoxelEditor/PostLighting.shader
//
//-------------------------------------------------------------------------------------------------

$
	cbv								( DESCRIPTOR:PIXEL:DYNAMIC )

	texture						( STATIC_EXECUTE:PIXEL )
	texture						( STATIC_EXECUTE:PIXEL )
	texture						( STATIC_EXECUTE:PIXEL )
	texture						( STATIC_EXECUTE:PIXEL )
	texture						( STATIC_EXECUTE:PIXEL )
	texture						( STATIC_EXECUTE:PIXEL )

	vertex_entry			VShader
	vertex_version		vs_5_0
	pixel_entry				PShader
	pixel_version			ps_5_0

	topology					TRIANGLELIST
	color							R32G32B32A32_FLOAT
$

#include "../Includes/ProcSky.hlsli"

cbuffer DataBuffer : register(b0) {
	float4x4 ProjInv;
	float4x4 ViewInv;
	float4 padding[8];
};

// Vertex output structure.
struct v2p {
	float4 Position : SV_POSITION;
	float2 UV : TEXCOORD;
};

// Pixel output structure.
struct p2f {
	float4 Color : SV_TARGET0;
};

// Vertex shader function.
v2p VShader(in uint i : SV_VertexID) {
	v2p output;
	
	// Full screen triangle.
	output.Position = float4(((i & 1) - 0.25f), ((i >> 1) & 1) - 0.25f, 0.0f, 0.25f);
	output.UV = float2((i & 1) * 2.0f, 1.0f - ((i >> 1) & 1) * 2.0f);

	return output;
}

Texture2D colorTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D depthTexture : register(t2);
Texture2D edgeTexture : register(t3);
Texture2D ssaoTexture : register(t4);
Texture2D skyTexture : register(t5);

// Pixel shader function.
p2f PShader(in v2p input) {	
	p2f output;

	if(dot(normalTexture[input.Position.xy].xyz, normalTexture[input.Position.xy].xyz) > 0.5f) {
		float3 lightDir = SUN_DIR;
		float d = saturate(dot(normalTexture[input.Position.xy].xyz, lightDir) * 0.5f + 0.5f) * 0.75f + 0.25f;

		float4 position = float4(float2(input.UV.x, (1.0f - input.UV.y)) * 2.0f - 1.0f, depthTexture[input.Position.xy].r, 1.0f);
		position = mul(position, ProjInv);
		position.xyz /= position.w;
		position = mul(position, ViewInv);
		position.xyz /= position.w;
		
		//float3 halfVec = normalize(lightDir - normalize(position.xyz));
		//float spec = pow(max(dot(normalTexture[input.Position.xy].xyz, halfVec), 0.0f), 96.0f);
		
		float4 color = float4(saturate(colorTexture[input.Position.xy].rgb * d/* + SUN_COLOR * spec.x * 0.625f*/), colorTexture[input.Position.xy].a);

		output.Color = lerp(skyTexture[input.Position.xy], color, colorTexture[input.Position.xy].a);
	} else {
		output.Color = lerp(skyTexture[input.Position.xy], colorTexture[input.Position.xy], colorTexture[input.Position.xy].a);
	}

	output.Color.rgb *= edgeTexture[input.Position.xy].r;
	output.Color.rgb *= ssaoTexture[input.Position.xy].r;

	return output;
}
