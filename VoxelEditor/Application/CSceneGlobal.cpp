//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CSceneGlobal.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CSceneGlobal.h"
#include "CAppData.h"
#include <Application/CSceneManager.h>
#include <Application/CAppVersion.h>
#include <Application/CPlatform.h>
#include <Application/CPanel.h>
#include <Resources/CResourceManager.h>
#include <Factory/CFactory.h>
#include <Utilities/CTimer.h>

namespace App
{
	CSceneGlobal::CSceneGlobal() :
		CScene(SCENE_NAME_GLOBAL, SceneType::Global),
		m_player(L"Player", SCENE_HASH_GLOBAL) {
	}

	CSceneGlobal::~CSceneGlobal() { }

	void CSceneGlobal::Initialize()
	{
		{ // Post effects.
			m_postSky.Initialize();
			m_postEdge.Initialize();
			m_postSSAO.Initialize();
			m_postFXAA.Initialize();
			
			{
				Graphics::CPostLighting::Data data { };
				data.m_pPostEdge = &m_postEdge;
				data.m_pPostSSAO = &m_postSSAO;
				m_postLighting.SetData(data);
				m_postLighting.Initialize();
			}

			m_postToneMapping.Initialize();
			CSceneManager::Instance().PostProcessor().RegisterPostEffect(&m_postSky);
			CSceneManager::Instance().PostProcessor().RegisterPostEffect(&m_postEdge);
			CSceneManager::Instance().PostProcessor().RegisterPostEffect(&m_postSSAO);
			CSceneManager::Instance().PostProcessor().RegisterPostEffect(&m_postLighting);
			CSceneManager::Instance().PostProcessor().RegisterPostEffect(&m_postToneMapping);
			CSceneManager::Instance().PostProcessor().RegisterPostEffect(&m_postFXAA);
		}

		{ // Player.
			m_player.Initialize();
		}
	}

	void CSceneGlobal::PostInitialize()
	{
		m_postSky.PostInitialize();
		m_postLighting.PostInitialize();
		m_postEdge.PostInitialize();
		m_postSSAO.PostInitialize();
		m_postToneMapping.PostInitialize();
		m_postFXAA.PostInitialize();

		// This is where production grade resources can be generated.
		//Resources::CManager::Instance().BuildProductionResources();
	}

	void CSceneGlobal::Load() { }
	void CSceneGlobal::Unload() { }
	
	//------------------------------------------------------------------------------------------------
	// FileIO.
	//------------------------------------------------------------------------------------------------
	
	void CSceneGlobal::SaveToFile(std::ofstream& file) const
	{
		// Temp dummy variable for versioning.
		u32 version = App::CVersion::Instance().GetVersion();
		file.write(reinterpret_cast<const char*>(&version), sizeof(version));
		m_player.SaveToFile(file);
	}

	void CSceneGlobal::LoadFromFile(std::ifstream& file)
	{
		u32 version;
		file.read(reinterpret_cast<char*>(&version), sizeof(version));
		m_player.LoadFromFile(file);
	}

	//------------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------------

	void CSceneGlobal::Update()
	{
		m_player.Update();
	}

	void CSceneGlobal::LateUpdate() 
	{
		m_player.LateUpdate();

		static const float speed = 10.0f;
		static float t = 0.0f;
		const float target = CSceneManager::Instance().GetLoadSceneHash() == SCENE_HASH_VIEW ? 0.0f : 1.0f;
		t = Math::Lerp(t, target, speed * Util::CTimer::Instance().GetDelta());
		m_postSky.SetSkyInterpolation(t);
	}

	void CSceneGlobal::Release()
	{
		m_player.Release();

		m_postFXAA.Release();
		m_postToneMapping.Release();
		m_postSSAO.Release();
		m_postEdge.Release();
		m_postLighting.Release();
		m_postSky.Release();
	}

	void CSceneGlobal::OnResize(const class CPanel* pPanel)
	{
		m_player.OnResize(pPanel);

		m_postSky.OnResize(pPanel);
		m_postLighting.OnResize(pPanel);
		m_postEdge.OnResize(pPanel);
		m_postSSAO.OnResize(pPanel);
		m_postToneMapping.OnResize(pPanel);
		m_postFXAA.OnResize(pPanel);
	}
};
