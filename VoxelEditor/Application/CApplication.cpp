//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CApplication.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CApplication.h"
#include "CAppData.h"
#include <Application/CInput.h>
#include <Application/CSceneManager.h>
#include <Application/CAppVersion.h>
#include <Resources/CResourceManager.h>
#include <Physics/CPhysics.h>
#include <Utilities/CTimer.h>
#include <Utilities/CFileSystem.h>

namespace App
{
	CApplication::CApplication()
	{
		App::CVersion::Instance().SetVersion(0, 0, 0, 0, L"alpha");
	}

	CApplication::~CApplication() { }

	void CApplication::Create()
	{
		{ // Setup the resource manager.
			Resources::CManager::Data data { };
			data.filepath = L"../Resources/";
#ifndef PRODUCTION_BUILD
			data.prodPath = L"../Production/Resources/";
#else
			data.prodPath = L"../Resources/";
#endif
			data.filename = L"Editor.res";
			Resources::CManager::Instance().SetData(data);
		}

		{ // Setup the physics system.
			Physics::CPhysics::Data data { };
			data.targetFPS = 50.0f;
			data.idleIterations = 4;
			data.rayCastIterations = 2;
			data.gravity = Math::SIMD_VEC_DOWN * 20.0f;
			Physics::CPhysics::Instance().SetData(data);
		}

		{ // Setup input.
			CInput::Instance().CreateLayout(INPUT_HASH_LAYOUT_EDITOR, true);
			CInput::Instance().CreateLayout(INPUT_HASH_LAYOUT_PLAYER, false);
		}

		{
			// Setup app state.
			m_state.Reset();
		}

		{ // Setup scene manager file system.
			CSceneFileIO::Data data { };
			data.localPath = L"/.starshade/editor/scenes";
			data.filename = L"game.sav";
			CSceneManager::Instance().FileIO().SetData(data);
			CSceneManager::Instance().FileIO().RegisterScene(&m_sceneGlobal);
			CSceneManager::Instance().FileIO().RegisterScene(&m_sceneNode);
		}
	}

	void CApplication::Run()
	{
		Util::CTimer::Instance().Tick();
	}

	void CApplication::Destroy() { }
};
