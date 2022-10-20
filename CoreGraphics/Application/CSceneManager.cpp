//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Application/CSceneManager.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CSceneManager.h"
#include "CScene.h"
#include "../Utilities/CJobSystem.h"
#include "../Resources/CResourceManager.h"
#include "../Factory/CFactory.h"
#include "../Graphics/CGraphicsAPI.h"
#include <Application/CCoreManager.h>
#include <Physics/CPhysics.h>

namespace App
{
	CSceneManager::CSceneManager() :
		m_pTargetScene(nullptr),
		m_pGlobalScene(nullptr),
		m_pDefaultScene(nullptr),
		m_pScene(nullptr) {
	}

	CSceneManager::~CSceneManager() { }

	void CSceneManager::Initialize()
	{
		Util::CJobSystem::Instance().Initialize();

		Resources::CManager::Instance().Initialize();
		Physics::CPhysics::Instance().Initialize();
		m_audioMixer.Intialize();
		
		m_postProcessor.Initialize();

		m_pGlobalScene->Initialize();
		for(auto& scenePair : m_sceneMap)
		{
			scenePair.second->Initialize();
		}

		m_pGlobalScene->Load();

		m_pTargetScene = m_pScene = m_pDefaultScene;
		if(m_pScene)
		{
			m_pScene->Load();
		}

		Util::CJobSystem::Instance().Execute();
	}

	void CSceneManager::PostInitialize()
	{
		Resources::CManager::Instance().PostInitialize();
		m_postProcessor.PostInitialize();
		m_renderingSystem.PostInitialize();
		m_computeSystem.PostInitialize();

		m_pGlobalScene->PostInitialize();
		for(auto& scenePair : m_sceneMap)
		{
			scenePair.second->PostInitialize();
		}

		Util::CJobSystem::Instance().Open();
	}

	void CSceneManager::Update()
	{
		m_pGlobalScene->Update();
		if(m_pScene)
		{
			m_pScene->Update();
		}

		App::CCoreManager::Instance().Update();

		m_pGlobalScene->LateUpdate();
		if(m_pScene)
		{
			m_pScene->LateUpdate();
		}

		Util::CJobSystem::Instance().Sync();
	}

	void CSceneManager::Compute()
	{
		m_computeSystem.Dispatch();
	}

	void CSceneManager::Render()
	{
		m_postProcessor.BeginRenderTo(Graphics::CLEAR_ALL, 0, nullptr);
		m_renderingSystem.Render();
		m_postProcessor.EndRenderTo();

		m_postProcessor.RenderQuad();
	}

	void CSceneManager::PostRender()
	{
		m_garbage.Process();

		// Check if a new scene should be loaded.
		if(m_pScene != m_pTargetScene) {
			Util::CJobSystem::Instance().JobGraphics([this](){
				if(m_pScene) { m_pScene->Unload(); }
				if(m_pTargetScene)
				{
						m_pTargetScene->Load();
				}
			}, false);

			CFactory::Instance().GetGraphicsAPI()->Sync();
			Util::CJobSystem::Instance().Process();

			m_pScene = m_pTargetScene;
		}

		Util::CJobSystem::Instance().Process();
	}

	void CSceneManager::Release()
	{
		if(m_pScene)
		{
			m_pScene->Unload();
		}

		m_pGlobalScene->Unload();

		Physics::CPhysics::Instance().Halt();
		Util::CJobSystem::Instance().Close();

		for(auto& scenePair : m_sceneMap)
		{
			scenePair.second->Release();
		}

		m_pGlobalScene->Release();
		m_pGlobalScene = m_pDefaultScene = nullptr;

		m_postProcessor.Release();
		
		m_garbage.Release();
		Util::CJobSystem::Instance().Release();

		m_audioMixer.Release();
		Physics::CPhysics::Instance().Release();
		Resources::CManager::Instance().Release();
	}

	//-----------------------------------------------------------------------------------------------
	// Panel adjustment methods.
	//-----------------------------------------------------------------------------------------------

	void CSceneManager::OnMove(const CPanel* pPanel)
	{
		if(m_pGlobalScene)
		{
			m_pGlobalScene->OnMove(pPanel);
		}

		if(m_pScene)
		{
			m_pScene->OnMove(pPanel);
		}
	}

	void CSceneManager::OnResize(const CPanel* pPanel)
	{
		Util::CJobSystem::Instance().JobGraphics([=](){
			m_postProcessor.OnResize(pPanel);

			if(m_pGlobalScene)
			{
				m_pGlobalScene->OnResize(pPanel);
			}

			if(m_pScene)
			{
				m_pScene->OnResize(pPanel);
			}
		}, false);

		
		Util::CJobSystem::Instance().Process();
	}

	//-----------------------------------------------------------------------------------------------
	// Utility methods.
	//-----------------------------------------------------------------------------------------------

	void CSceneManager::Register(CScene* pScene, u32 hash, SceneType sceneType)
	{
		if(sceneType == SceneType::Global)
		{
			m_pGlobalScene = pScene;
			if(m_pDefaultScene == nullptr)
			{
				m_pDefaultScene = m_pGlobalScene;
			}
		}
		else
		{
			m_sceneMap.insert({ hash, pScene });
			if(sceneType == SceneType::Default)
			{
				m_pDefaultScene = pScene;
			}
		}
	}

	bool CSceneManager::LoadScene(u32 hash)
	{
		auto elem = m_sceneMap.find(hash);
		if(elem != m_sceneMap.end())
		{
			m_pTargetScene = elem->second;
			return true;
		}

		return false;
	}
};
