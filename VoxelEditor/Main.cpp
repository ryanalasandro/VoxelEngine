//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Main.cpp
//
//-------------------------------------------------------------------------------------------------

#include "Main.h"
#include "Application/CApplication.h"
#include <Application/CWinPlatform.h>
#include <Factory/CFactory.h>
#include <Windows.h>

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	App::CWinPlatform* pWinPlatform = dynamic_cast<App::CWinPlatform*>(CFactory::Instance().GetPlatform());
	App::CWinPlatform::WinData winData { };
	winData.hInstance = hInstance;
	winData.nCmdShow = nCmdShow;
	winData.wuiFilename = L"../Resources/UI/WUI/VoxelEditor/Main.wui";

	return pWinPlatform->Run(&App::CApplication::Instance(), &winData);
}
