//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CSceneNode.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CSceneNode.h"
#include "CAppData.h"
#include <Application/CSceneManager.h>
#include <Application/CPlatform.h>
#include <Application/CPanel.h>
#include <Factory/CFactory.h>

namespace App
{
	CSceneNode::CSceneNode() :
		CScene(SCENE_NAME_NODE, SceneType::Default),
		m_grid(L"NodeGrid", SCENE_HASH_NODE),
		m_chunk(L"NodeChunk", SCENE_HASH_NODE) {
	}

	CSceneNode::~CSceneNode() { }

	void CSceneNode::Initialize() { }

	void CSceneNode::PostInitialize() { }

	void CSceneNode::Load()
	{
		{ // Grid.
			Universe::CNodeGrid::Data data { };
			data.cellSize = 1.0f;
			data.right = Math::VEC3_RIGHT;
			data.up = Math::VEC3_UP;
			data.forward = Math::VEC3_FORWARD;
			data.width = 33;
			data.height = 33;
			data.length = 33;
			m_grid.SetData(data);
			m_grid.Initialize();
		}

		{ // Chunk.
			Universe::CNodeChunk::Data data { };
			data.width = 32;
			data.height = 32;
			data.length = 32;
			m_chunk.SetData(data);
			m_chunk.Initialize();
		}

		CSceneManager::Instance().FileIO().Load();
	}
	void CSceneNode::Unload()
	{
		CSceneManager::Instance().FileIO().Save();

		m_chunk.Release();
		m_grid.Release();
	}
	
	//------------------------------------------------------------------------------------------------
	// FileIO.
	//------------------------------------------------------------------------------------------------
	
	void CSceneNode::SaveToFile(std::ofstream& file) const
	{
		m_chunk.SaveToFile(file);
	}

	void CSceneNode::LoadFromFile(std::ifstream& file)
	{
		m_chunk.LoadFromFile(file);
	}

	//------------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------------

	void CSceneNode::Update() { }
	void CSceneNode::LateUpdate()
	{
		m_grid.LateUpdate();
		m_chunk.LateUpdate();
	}

	void CSceneNode::Release() 
	{
		m_chunk.Release();
		m_grid.Release();
	}

	void CSceneNode::OnResize(const class CPanel* pPanel) { }
};
