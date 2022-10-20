//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: UI/CUICanvas.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CUICanvas.h"
#include "CUITransform.h"
#include "CUIRenderer.h"
#include "../Application/CSceneManager.h"
#include <cassert>

namespace UI
{
	CUICanvas::CUICanvas(const CVObject* pObject) : 
		CVComponent(pObject),
		m_bActive(true)
	{
	}

	CUICanvas::~CUICanvas()
	{
	}
	
	void CUICanvas::Initialize()
	{
		GatherRenderers(m_data.pTransform);

		for(auto elem : m_instanceCounts)
		{

		}

		App::CSceneManager::Instance().RenderingSystem().Register(this);
	}
	
	void CUICanvas::Update()
	{
	}
	
	void CUICanvas::Render()
	{
	}
	
	void CUICanvas::Release()
	{
		App::CSceneManager::Instance().RenderingSystem().Deregister(this);
	}

	//-----------------------------------------------------------------------------------------------
	// Utility methods.
	//-----------------------------------------------------------------------------------------------
	
	void CUICanvas::GatherRenderers(CUITransform* pUITransform)
	{
		assert(pUITransform);
		if(pUITransform == nullptr) { return; }

		CUIRenderer* pRenderer = pUITransform->GetObject()->FindComponent<CUIRenderer>();
		if(pRenderer)
		{
			m_instanceCounts[pRenderer->GetMaterialHash()] += pRenderer->GetInstanceCount();
		}

		for(Logic::CTransform* child : pUITransform->GetChildren())
		{
			GatherRenderers(dynamic_cast<CUITransform*>(child));
		}
	}
};
