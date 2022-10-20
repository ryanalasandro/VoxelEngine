//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CMeshContainer.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CMeshContainer.h"
#include "CMeshRenderer.h"
#include "../Application/CSceneManager.h"

namespace Graphics
{
	CMeshContainer::CMeshContainer(const CVObject* pObject) :
		CRenderer(pObject) {
	}

	void CMeshContainer::Initialize()
	{
		App::CSceneManager::Instance().RenderingSystem().Register(this);
	}

	void CMeshContainer::Render()
	{
		if(m_data.onPreRender) { m_data.onPreRender(); }
		if(m_data.pMeshRenderer)
		{
			m_data.pMeshRenderer->Render();
		}
	}

	void CMeshContainer::Release()
	{
		App::CSceneManager::Instance().RenderingSystem().Deregister(this);
	}
};