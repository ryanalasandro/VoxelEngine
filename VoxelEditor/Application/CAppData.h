//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CAppData.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CAPPDATA_H
#define CAPPDATA_H

#include <Globals/CGlobals.h>
#include <Math/CMathFNV.h>

namespace App
{
	// Scene.
	const wchar_t SCENE_NAME_GLOBAL[] = L"Global";
	const wchar_t SCENE_NAME_VIEW[] = L"View";
	const wchar_t SCENE_NAME_NODE[] = L"Node";

	const u32 SCENE_HASH_GLOBAL = Math::FNV1a_32(SCENE_NAME_GLOBAL);
	const u32 SCENE_HASH_VIEW = Math::FNV1a_32(SCENE_NAME_VIEW);
	const u32 SCENE_HASH_NODE = Math::FNV1a_32(SCENE_NAME_NODE);

	// Input.
	const wchar_t INPUT_LAYOUT_EDITOR[] = L"Editor";
	const wchar_t INPUT_LAYOUT_PLAYER[] = L"Player";

	const u32 INPUT_HASH_LAYOUT_EDITOR = Math::FNV1a_32(INPUT_LAYOUT_EDITOR);
	const u32 INPUT_HASH_LAYOUT_PLAYER = Math::FNV1a_32(INPUT_LAYOUT_PLAYER);

	const char INPUT_KEY_MOVE_FORWARD[] = "Move Forward";
	const char INPUT_KEY_MOVE_BACKWARD[] = "Move Backward";
	const char INPUT_KEY_MOVE_UP[] = "Move Up";
	const char INPUT_KEY_MOVE_DOWN[] = "Move Down";
	const char INPUT_KEY_MOVE_LEFT[] = "Move Left";
	const char INPUT_KEY_MOVE_RIGHT[] = "Move Right";

	const char INPUT_KEY_MOUSE_H[] = "Mouse Horizontal";
	const char INPUT_KEY_MOUSE_V[] = "Mouse Vertical";

	const char INPUT_KEY_SELECT[] = "Select";
	const char INPUT_KEY_LOOK[] = "Look";
	const char INPUT_KEY_PAN[] = "Pan";
	const char INPUT_KEY_ZOOM[] = "Zoom";
	const char INPUT_KEY_MODE[] = "Mode";
	const char INPUT_KEY_PLAY[] = "Play";
	const char INPUT_KEY_JUMP[] = "Jump";
	const char INPUT_KEY_QUIT[] = "Quit";
};

#endif
