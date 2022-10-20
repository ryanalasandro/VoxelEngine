//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CSceneNode.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CSCENENODE_H
#define CSCENENODE_H

#include "../Actors/CPlayer.h"
#include "../Universe/CNodeGrid.h"
#include "../Universe/CNodeChunk.h"
#include "../Graphics/CPostLighting.h"
#include "../Physics/CTestCube.h"
#include <Graphics/CPostSky.h>
#include <Graphics/CPostToneMapping.h>
#include <Application/CScene.h>

namespace App
{
	class CSceneNode : public CScene
	{
	public:
		CSceneNode();
		~CSceneNode();
		CSceneNode(const CSceneNode&) = delete;
		CSceneNode(CSceneNode&&) = delete;
		CSceneNode& operator = (const CSceneNode&) = delete;
		CSceneNode& operator = (CSceneNode&&) = delete;

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
		Universe::CNodeGrid m_grid;
		Universe::CNodeChunk m_chunk;
	};
};

#endif
