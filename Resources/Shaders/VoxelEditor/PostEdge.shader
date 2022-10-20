//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: ../Resources/Shaders/VoxelEditor/PostEdge.shader
//
//-------------------------------------------------------------------------------------------------

$
	cbv								( DESCRIPTOR:PIXEL:DYNAMIC )

	texture						( STATIC_EXECUTE:PIXEL )

	vertex_entry			VShader
	vertex_version		vs_5_0
	pixel_entry				PShader
	pixel_version			ps_5_0

	topology					TRIANGLELIST
	color							R8_UNORM
$

cbuffer DataBuffer : register(b0)
{
	float4 ProjInv;
	float4 padding[15];
};

// Vertex output structure.
struct v2p
{
	float4 Position : SV_POSITION;
};

// Pixel output structure.
struct p2f
{
	float Color : SV_TARGET0;
};

// Vertex shader function.
v2p VShader(in uint i : SV_VertexID)
{
	v2p output;
	
	// Full screen triangle.
	output.Position = float4(((i & 1) - 0.25f), ((i >> 1) & 1) - 0.25f, 0.0f, 0.25f);

	return output;
}

Texture2D depthTexture : register(t0);

inline float GetDepth(in float2 uv)
{
	return 1.0f / (depthTexture[uv].r * ProjInv.z + ProjInv.w);
}

float CalculateEdge(in v2p input)
{
	float CenterDepth = GetDepth(input.Position.xy);
	
	float4 DepthsDiag = float4(
		GetDepth(input.Position.xy + float2(-1, -1)),
		GetDepth(input.Position.xy + float2( 1, -1)),
		GetDepth(input.Position.xy + float2(-1,  1)),
		GetDepth(input.Position.xy + float2( 1,  1))
	);

	float4 DepthsAxis = float4(
		GetDepth(input.Position.xy + float2( 0, -1)),
		GetDepth(input.Position.xy + float2(-1,  0)),
		GetDepth(input.Position.xy + float2( 1,  0)),
		GetDepth(input.Position.xy + float2( 0,  1))
	);

	const float4 HorizDiagCoeff = float4(-1.0f, 1.0f, -1.0f, 1.0f);
	const float4 VertDiagCoeff = float4(-1.0f, -1.0f, 1.0f, 1.0f);

	const float4 HorizAxisCoeff = float4(0.0f, -2.0f, 2.0f, 0.0f);
	const float4 VertAxisCoeff = float4(-2.0f, 0.0f, 0.0f, 2.0f);

	DepthsDiag = (DepthsDiag > CenterDepth.xxxx) ? DepthsDiag : CenterDepth.xxxx;
	DepthsAxis = (DepthsAxis > CenterDepth.xxxx) ? DepthsAxis : CenterDepth.xxxx;

	DepthsDiag -= CenterDepth.xxxx;
	DepthsAxis -= CenterDepth.xxxx;
	DepthsDiag /= CenterDepth.xxxx;
	DepthsAxis /= CenterDepth.xxxx;

	float4 SobelH = DepthsDiag * HorizDiagCoeff + DepthsAxis * HorizAxisCoeff;
	float4 SobelV = DepthsDiag * VertDiagCoeff + DepthsAxis * VertAxisCoeff;

	float SobelX = dot(SobelH, float4(1.0f, 1.0f, 1.0f, 1.0f));
	float SobelY = dot(SobelV, float4(1.0f, 1.0f, 1.0f, 1.0f));
	float Sobel = sqrt(SobelX * SobelX + SobelY * SobelY);
	return pow(saturate(Sobel), 1.0f);
}

// Pixel shader function.
p2f PShader(in v2p input)
{	
	p2f output;

	output.Color = saturate(1.0f - CalculateEdge(input));

	/*float p01 = depthTexture[input.Position.xy + float2( 0, -1)].r;
	if(p01 == 0.0f) discard;
	float p10 = depthTexture[input.Position.xy + float2(-1,  0)].r;
	if(p10 == 0.0f) discard;
	float p12 = depthTexture[input.Position.xy + float2( 1,  0)].r;
	if(p12 == 0.0f) discard;
	float p21 = depthTexture[input.Position.xy + float2( 0,  1)].r;
	if(p21 == 0.0f) discard;

	float p00 = depthTexture[input.Position.xy + float2(-1, -1)].r;
	float p02 = depthTexture[input.Position.xy + float2( 1, -1)].r;
	//float p11 = depthTexture[input.Position.xy + float2( 0,  0)].r;
	float p20 = depthTexture[input.Position.xy + float2(-1,  1)].r;
	float p22 = depthTexture[input.Position.xy + float2( 1,  1)].r;

	float d00 = 1.0f / (p00 * ProjInv.z + ProjInv.w);
	float d01 = 1.0f / (p01 * ProjInv.z + ProjInv.w);
	float d02 = 1.0f / (p02 * ProjInv.z + ProjInv.w);
	float d10 = 1.0f / (p10 * ProjInv.z + ProjInv.w);
	//float d11 = 1.0f / (p11 * ProjInv.z + ProjInv.w);
	float d12 = 1.0f / (p12 * ProjInv.z + ProjInv.w);
	float d20 = 1.0f / (p20 * ProjInv.z + ProjInv.w);
	float d21 = 1.0f / (p21 * ProjInv.z + ProjInv.w);
	float d22 = 1.0f / (p22 * ProjInv.z + ProjInv.w);

	float Gx = (-1.0f * d00 + 0.0f * d01 + 1.0f * d02) + (-2.0f * d10 + 2.0f * d12) + (-1.0f * d20 + 0.0f * d21 + 1.0f * d22);
	float Gy = (-1.0f * d00 - 2.0f * d01 - 1.0f * d02) + ( 0.0f * d10 + 0.0f * d12) + ( 1.0f * d20 + 2.0f * d21 + 1.0f * d22);

	output.Color = float4(saturate(step(0.999f, sqrt(Gx * Gx + Gy * Gy) * 0.5f)).rrr, 1.0f);*/

	return output;
}
