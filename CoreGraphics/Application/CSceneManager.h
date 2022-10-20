//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Application/CSceneManager.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CSCENEMANAGER_H
#define CSCENEMANAGER_H

#include "CScene.h"
#include "CSceneFileIO.h"

#include "../Graphics/CRenderingSystem.h"
#include "../Graphics/CComputeSystem.h"
#include "../Graphics/CPostProcessor.h"
#include "../Audio/CAudioMixer.h"
#include "../Utilities/CGarbage.h"

#include <Objects/CCompBatch.h>
#include <Globals/CGlobals.h>
#include <Logic/CTransform.h>
#include <Logic/CCameraManager.h>
#include <unordered_map>

namespace App
{
	class CSceneManager
	{
	private:
		CSceneManager();
		~CSceneManager();
		CSceneManager(const CSceneManager&) = delete;
		CSceneManager(CSceneManager&&) = delete;
		CSceneManager& operator = (const CSceneManager&) = delete;
		CSceneManager& operator = (CSceneManager&&) = delete;

	public:
		static CSceneManager& Instance()
		{
			static CSceneManager instance;
			return instance;
		}

		void Initialize();
		void PostInitialize();
		void Update();
		void Compute();
		void Render();
		void PostRender();
		void Release();

		void OnMove(const class CPanel* pPanel);
		void OnResize(const class CPanel* pPanel);

		void Register(CScene* pScene, u32 hash, SceneType sceneType);
		bool LoadScene(u32 hash);

		// Accessors.
		inline u32 GetLoadSceneHash() const { return m_pScene ? m_pScene->GetHash() : 0; }

		inline CSceneFileIO& FileIO() { return m_fileIO; }
		inline Logic::CCameraManager& CameraManager() { return m_cameraManager; }
		inline Graphics::CRenderingSystem& RenderingSystem() { return m_renderingSystem; }
		inline Graphics::CComputeSystem& ComputeSystem() { return m_computeSystem; }
		inline Graphics::CPostProcessor& PostProcessor() { return m_postProcessor; }
		inline Audio::CAudioMixer& AudioMixer() { return m_audioMixer; }
		inline Util::CGarbage& Garbage() { return m_garbage; }

	private:
		std::unordered_map<u32, CScene*> m_sceneMap;

		Logic::CCameraManager m_cameraManager;
		Graphics::CRenderingSystem m_renderingSystem;
		Graphics::CComputeSystem m_computeSystem;
		Graphics::CPostProcessor m_postProcessor;
		Audio::CAudioMixer m_audioMixer;
		Util::CGarbage m_garbage;
		CSceneFileIO m_fileIO;

		CScene* m_pTargetScene;

		CScene* m_pGlobalScene;
		CScene* m_pDefaultScene;
		CScene* m_pScene;
	};
};

#endif
