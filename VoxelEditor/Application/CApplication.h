//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CApplication.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CAPPLICATION_H
#define CAPPLICATION_H

#include <Application/CAppBase.h>
#include "CAppState.h"
#include "CSceneGlobal.h"
#include "CSceneView.h"
#include "CSceneNode.h"

namespace App
{
	class CApplication : public CAppBase
	{
	public:
		static CApplication& Instance()
		{
			static CApplication instance;
			return instance;
		}

	private:
		CApplication();
		~CApplication();
		CApplication(const CApplication&) = delete;
		CApplication(CApplication&) = delete;
		CApplication& operator = (const CApplication&) = delete;
		CApplication& operator = (CApplication&) = delete;
		
	public:
		// Accessors.
		inline CAppState& State() { return m_state; }

	private:
		void Create();
		void Run();
		void Destroy();

	private:
		CAppState m_state;

		CSceneGlobal m_sceneGlobal;
		CSceneView m_sceneView;
		CSceneNode m_sceneNode;
	};
};

#endif
