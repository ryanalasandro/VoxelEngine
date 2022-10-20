//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: NetRunner App
//
// File: ../Resources/Shaders/PostLighting.shader
//
//-------------------------------------------------------------------------------------------------

$
	cbv								( DESCRIPTOR:PIXEL:DYNAMIC )

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

#include "Includes/ProcSky.hlsli"

cbuffer DataBuffer : register(b0) {
	float4 CameraPosition;
	float4 padding[15];
};

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
	
	// Full screen triangle.
	output.Position = float4(((i & 1) - 0.25f), ((i >> 1) & 1) - 0.25f, 0.0f, 0.25f);

	return output;
}

Texture2D colorTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D positionTexture : register(t2);
Texture2D skyTexture : register(t3);

// Pixel shader function.
p2f PShader(in v2p input) {	
	p2f output;

	if(dot(normalTexture[input.Position.xy].xyz, normalTexture[input.Position.xy].xyz) > 0.5f) {
		float3 lightDir = SUN_DIR;//normalize(float3(0.25f, 0.75f, -1.0f));
		float d = saturate(dot(normalTexture[input.Position.xy].xyz, lightDir)) * 0.75f + 0.25f;

		float3 halfVec = normalize(lightDir + normalize(CameraPosition - positionTexture[input.Position.xy].xyz));
		float spec = pow(max(dot(normalTexture[input.Position.xy].xyz, halfVec), 0.0f), 96.0f);
		
		float4 color = float4(saturate(colorTexture[input.Position.xy].rgb * d + SUN_COLOR * spec.x * 0.625f), colorTexture[input.Position.xy].a);

		/*float3 cool = float3(0.0f, 0.0f, 0.55f) + 0.25f * colorTexture[input.Position.xy].rgb;
		float3 warm = float3(0.3f, 0.3f, 0.0f) + 0.25f * colorTexture[input.Position.xy].rgb;
		float3 highlight = float3(1.0f, 1.0f, 1.0f);
		float t = (dot(normalTexture[input.Position.xy].xyz, lightDir) + 1.0f) * 0.5f;
		float3 r = normalTexture[input.Position.xy].xyz * dot(normalTexture[input.Position.xy].xyz, lightDir) * 2.0f - lightDir;
		float s = saturate(dot(r, normalize(CameraPosition - positionTexture[input.Position.xy].xyz)) * 100.0f - 97.0f);

		float4 color = float4(highlight * s + (warm * t + cool * (1.0f - t)) * (1.0f - s), colorTexture[input.Position.xy].a);*/

		output.Color = lerp(skyTexture[input.Position.xy], color, colorTexture[input.Position.xy].a);
	} else {
		output.Color = lerp(skyTexture[input.Position.xy], colorTexture[input.Position.xy], colorTexture[input.Position.xy].a);
	}

	return output;
}
