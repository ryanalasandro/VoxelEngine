//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: ../Resources/Shaders/VoxelEditor/Voxel.shader
//
//-------------------------------------------------------------------------------------------------

$
	/* { pSemanticName, semanticIndex, format, inputSlot, alignedByteOffset, inputSlotClass, instanceDataStepRate } */
	input							( POSITION:0:R32G32B32_FLOAT:0:0:VERTEX:0 )
	input							( NORMAL:0:R32G32B32_FLOAT:0:12:VERTEX:0 )
	input							( TEXCOORD:0:R32G32_FLOAT:0:24:VERTEX:0 )
	
	cbv								( DESCRIPTOR:PIXEL:FIXED )
	cbv								( CONSTANTS:VERTEX:DYNAMIC )

	vertex_entry			VShader
	vertex_version		vs_5_0
	pixel_entry				PShader
	pixel_version			ps_5_0

	topology					TRIANGLELIST
	color							R32G32B32A32_FLOAT|R32G32B32A32_FLOAT
	depth							D32_FLOAT
$

cbuffer DataBuffer : register(b0)
{
	float4 Color;
	float4 padding0[15];
};

cbuffer DrawBuffer : register(b1)
{
	float4x4 VP;
	float4x4 World;
};

struct a2v
{
	float4 Position : POSITION;
	float4 Normal : NORMAL;
	float2 TexCoord : TEXCOORD;
};

struct v2p
{
	float4 Position : SV_POSITION;
	float3 Normal : NORMAL;
	float2 Edge : TEXCOORD;
};

struct p2f
{
	float4 Color : SV_TARGET0;
	float4 Normal : SV_TARGET1;
};

v2p VShader(in a2v input)
{
	v2p output;
	
	input.Position.w = 1.0f;
	output.Position = mul(input.Position, World);
	output.Position = mul(output.Position, VP);

	output.Normal.xyz = mul(input.Normal.xyz, (float3x3)World);
	output.Edge = input.TexCoord;

	return output;
}

p2f PShader(in v2p input)
{
	p2f output;
	//float2 edge = abs(input.Edge.xy - 0.5f) * 2.0f;
	output.Color = Color;//float4(0.1f, 0.1f, 0.1f, 1.0f);
	output.Normal = float4(normalize(input.Normal.xyz), 0.0f);
	return output;
}
