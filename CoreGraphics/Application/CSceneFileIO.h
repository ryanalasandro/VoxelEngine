//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Application/CSceneFileIO.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CSCENEFILEIO_H
#define CSCENEFILEIO_H

#include <string>
#include <vector>

namespace App
{
	class CSceneFileIO
	{
	public:
		struct Data
		{
			std::wstring localPath;
			std::wstring filename;
		};

	public:
		CSceneFileIO();
		~CSceneFileIO();
		CSceneFileIO(const CSceneFileIO&) = delete;
		CSceneFileIO(CSceneFileIO&&) = delete;
		CSceneFileIO& operator = (const CSceneFileIO&) = delete;
		CSceneFileIO& operator = (CSceneFileIO&&) = delete;

		bool Save();
		bool Load();
		
		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }

		inline void RegisterScene(class CScene* pScene) { m_sceneList.push_back(pScene); }
		inline void ClearSceneList() { m_sceneList.clear(); }

	private:
		Data m_data;
		std::vector<class CScene*> m_sceneList;
	};
};

#endif
