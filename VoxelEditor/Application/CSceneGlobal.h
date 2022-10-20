//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CSceneGlobal.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CSCENEGLOBAL_H
#define CSCENEGLOBAL_H

#include "../Actors/CPlayer.h"
#include "../Graphics/CPostLighting.h"
#include "../Graphics/CPostEdge.h"
#include "../Graphics/CPostSSAO.h"
#include "../Graphics/CPostFXAA.h"
#include <Graphics/CPostSky.h>
#include <Graphics/CPostToneMapping.h>
#include <Application/CScene.h>

namespace App
{
	class CSceneGlobal : public CScene
	{
	public:
		CSceneGlobal();
		~CSceneGlobal();
		CSceneGlobal(const CSceneGlobal&) = delete;
		CSceneGlobal(CSceneGlobal&&) = delete;
		CSceneGlobal& operator = (const CSceneGlobal&) = delete;
		CSceneGlobal& operator = (CSceneGlobal&&) = delete;

	protected:
		void Initialize() final;
		void PostInitialize() final;
		void Load() final;
		void Unload() final;
		void SaveToFile(std::ofstream& file) const final;
		void LoadFromFile(std::ifstream& file) final;

		void Update() final;
		void LateUpdate() final;
		void Release() final;

		virtual void OnResize(const class CPanel* pPanel) final;

	private:
		Actor::CPlayer m_player;

		Graphics::CPostSky m_postSky;
		Graphics::CPostLighting m_postLighting;
		Graphics::CPostEdge m_postEdge;
		Graphics::CPostSSAO m_postSSAO;
		Graphics::CPostToneMapping m_postToneMapping;
		Graphics::CPostFXAA m_postFXAA;
	};
};

#endif
