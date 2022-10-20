//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CMeshRenderer.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CMeshRenderer.h"
#include "CMaterial.h"
#include "../Application/CSceneManager.h"

namespace Graphics
{
	CMeshRenderer::CMeshRenderer(const CVObject* pObject) :
		CRenderer(pObject) {
	}

	CMeshRenderer::~CMeshRenderer() { }

	void CMeshRenderer::Initialize()
	{
		if(!m_data.bSkipRegistration)
		{
			CRenderer::Initialize();
		}
	}

	void CMeshRenderer::Render()
	{
		if(m_data.onPreRender) { m_data.onPreRender(); }
		m_data.pMaterial->Bind();
	}

	void CMeshRenderer::Release()
	{
		if(!m_data.bSkipRegistration)
		{
			CRenderer::Release();
		}
	}
};
