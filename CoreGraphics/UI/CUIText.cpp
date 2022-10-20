//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: UI/CUIText.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CUIText.h"
#include "../Resources/CResourceManager.h"
#include "../Graphics/CMaterial.h"

namespace UI
{
	CUIText::CUIText(const CVObject* pObject) : 
		CUIRenderer(pObject),
		m_pMaterial(nullptr)
	{
	}

	CUIText::~CUIText()
	{
	}
	
	void CUIText::Initialize()
	{
		m_pMaterial = reinterpret_cast<Graphics::CMaterial*>(Resources::CManager::Instance().GetResource(Resources::RESOURCE_TYPE_MATERIAL, m_data.materialHash));
	}

	void CUIText::Render()
	{
	}

	void CUIText::Release()
	{
	}
};
