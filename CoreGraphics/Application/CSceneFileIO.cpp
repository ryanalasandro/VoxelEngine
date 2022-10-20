//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Application/CSceneFileIO.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CSceneFileIO.h"
#include "../Utilities/CFileSystem.h"
#include <Utilities/CDebugError.h>
#include <Utilities/CConvertUtil.h>
#include "CScene.h"
#include <cassert>

namespace App
{
	CSceneFileIO::CSceneFileIO()
	{
	}

	CSceneFileIO::~CSceneFileIO()
	{
	}

	bool CSceneFileIO::Save()
	{
		std::wstring path = Util::CFileSystem::Instance().GetDataPath();
		Util::SplitWString(m_data.localPath, '/', [&path](u32 index, const std::wstring& dir){
			if(dir.empty()) return;
			path += L"/" + dir;
			if(!Util::CFileSystem::Instance().VerifyDirectory(path.c_str()))
			{
				ASSERT_R(Util::CFileSystem::Instance().NewDirectory(path.c_str()));
			}
		});


		std::ofstream file((path + L"/" + m_data.filename).c_str(), std::ios::out | std::ios::binary);
		if(!file.is_open())
		{
			return false;
		}

		for(auto pScene : m_sceneList)
		{
			pScene->SaveToFile(file);
		}
		
		file.close();
		return true;
	}

	bool CSceneFileIO::Load()
	{
		std::wstring path = Util::CFileSystem::Instance().GetDataPath() + m_data.localPath + L"/";
		std::ifstream file((path + m_data.filename).c_str(), std::ios::in | std::ios::binary);
		if(!file.is_open())
		{
			return Save();
		}

		for(auto pScene : m_sceneList)
		{
			pScene->LoadFromFile(file);
		}

		file.close();
		return true;
	}
};
