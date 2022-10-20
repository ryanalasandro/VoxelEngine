//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: ../Resources/App.res
//
//-------------------------------------------------------------------------------------------------

#include "Engine.res"

SHADERS {
	( SHADER_POST_FXAA, "Shaders/VoxelEditor/PostFXAA.shader" )
	( SHADER_POST_LIGHTING, "Shaders/VoxelEditor/PostLighting.shader" )
	( SHADER_POST_EDGE, "Shaders/VoxelEditor/PostEdge.shader" )
	( SHADER_POST_SSAO, "Shaders/VoxelEditor/PostSSAO.shader" )
	( SHADER_POST_BLUR, "Shaders/VoxelEditor/PostBlur.shader" )

	( SHADER_CYBER_GRID, "Shaders/VoxelEditor/CyberGrid.shader" )
	( SHADER_CYBER_NODE, "Shaders/VoxelEditor/CyberNode.shader" )
	( SHADER_NODE_GRID, "Shaders/VoxelEditor/NodeGrid.shader" )
	( SHADER_VOXEL, "Shaders/VoxelEditor/Voxel.shader" )
	( SHADER_TEXTURED_QUAD, "Shaders/VoxelEditor/TexturedQuad.shader" )
}

MATERIALS {
	( MATERIAL_POST_FXAA, "Materials/VoxelEditor/PostFXAA.mat" )
	( MATERIAL_POST_LIGHTING, "Materials/VoxelEditor/PostLighting.mat" )
	( MATERIAL_POST_EDGE, "Materials/VoxelEditor/PostEdge.mat" )
	( MATERIAL_POST_SSAO, "Materials/VoxelEditor/PostSSAO.mat" )
	( MATERIAL_POST_HBLUR, "Materials/VoxelEditor/PostHBlur.mat" )
	( MATERIAL_POST_VBLUR, "Materials/VoxelEditor/PostVBlur.mat" )
	
	( MATERIAL_DEBUG_GREEN, "Materials/VoxelEditor/DebugGreen.mat" )

	( MATERIAL_CYBER_GRID, "Materials/VoxelEditor/CyberGrid.mat" )
	( MATERIAL_CYBER_NODE, "Materials/VoxelEditor/CyberNode.mat" )
	( MATERIAL_NODE_GRID, "Materials/VoxelEditor/NodeGrid.mat" )
	( MATERIAL_VOXEL, "Materials/VoxelEditor/Voxel.mat" )
	( MATERIAL_PLACEMENT_QUAD, "Materials/VoxelEditor/PlacementQuad.mat" )
	( MATERIAL_DELETION_QUAD, "Materials/VoxelEditor/DeletionQuad.mat" )
}

/*TEXTURES {
	( TEXTURE_FONT_DEFAULT, "Texture/VoxelEditor/Fonts/FontDefault.fnt )
}*/

