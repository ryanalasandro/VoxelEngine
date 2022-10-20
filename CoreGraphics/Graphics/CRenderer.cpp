//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CRenderer.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CRenderer.h"
#include "../Application/CSceneManager.h"

namespace Graphics
{
	CRenderer::CRenderer(const CVObject* pObject) :
		CVComponent(pObject),
		m_bActive(true) {
	}

	void CRenderer::Initialize()
	{
		App::CSceneManager::Instance().RenderingSystem().Register(this);
	}

	void CRenderer::Release()
	{
		App::CSceneManager::Instance().RenderingSystem().Deregister(this);
	}
};