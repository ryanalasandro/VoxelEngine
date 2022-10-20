//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CRenderingSystem.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CRenderingSystem.h"
#include "CRenderer.h"
#include "CMaterial.h"
#include "CShader.h"
#include "CRootSignature.h"
#include "../UI/CUICanvas.h"
#include "../Application/CSceneManager.h"
#include <Objects/CVObject.h>

namespace Graphics
{
	CRenderingSystem::CRenderingSystem() { }
	CRenderingSystem::~CRenderingSystem() { }

	void CRenderingSystem::PostInitialize()
	{
		for(CRenderer* pRenderer : m_rendererList)
		{
			pRenderer->PostInitialize();
		};
	}

	void CRenderingSystem::Render()
	{
		CRootSignature* rootSig = nullptr;

		for(CRenderer* pRenderer : m_rendererList)
		{
			if(!pRenderer->IsActive()) { continue; }

			if(rootSig != pRenderer->GetMaterial()->GetShader()->GetRootSignature())
			{
				rootSig = pRenderer->GetMaterial()->GetShader()->GetRootSignature();
				rootSig->Bind();
			}

			pRenderer->Render();
		};
		
		const Math::SIMDVector camPos = App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetTransform()->GetPosition();
		SortBackToFront(camPos, m_depthlessList);

		for(CRenderer* pRenderer : m_depthlessList)
		{
			if(!pRenderer->IsActive()) { continue; }

			if(rootSig != pRenderer->GetMaterial()->GetShader()->GetRootSignature())
			{
				rootSig = pRenderer->GetMaterial()->GetShader()->GetRootSignature();
				rootSig->Bind();
			}

			pRenderer->Render();
		};

		// UI.
		SortBackToFront(camPos, m_canvasList);

		for(UI::CUICanvas* pCanvas : m_canvasList)
		{
			if(!pCanvas->IsActive()) { continue; }
			pCanvas->Render();
		};
	}

	//-----------------------------------------------------------------------------------------------
	// (De)registration methods.
	//-----------------------------------------------------------------------------------------------

	// NOTE: As a more advanced culling system is put in place, this system will need to be adjusted for it.

	// CRenderer.
	void CRenderingSystem::Register(CRenderer* pRenderer)
	{
		if(!pRenderer->GetMaterial()->GetShader()->GetDepthWrite())
		{
			Register<CRenderer>(pRenderer, m_depthlessList, nullptr);
		}
		else
		{
			Register<CRenderer>(pRenderer, m_rendererList, [](CRenderer* a, CRenderer* b){
				return reinterpret_cast<uintptr_t>(a->GetMaterial()->GetShader()->GetRootSignature()) >
					reinterpret_cast<uintptr_t>(b->GetMaterial()->GetShader()->GetRootSignature());
			});
		}
	}

	void CRenderingSystem::Deregister(CRenderer* pRenderer)
	{
		if(!pRenderer->GetMaterial()->GetShader()->GetDepthWrite())
		{
			Deregister<CRenderer>(pRenderer, m_depthlessList);
		}
		else
		{
			Deregister<CRenderer>(pRenderer, m_rendererList);
		}
	}
	
	// CUICanvas.
	void CRenderingSystem::Register(UI::CUICanvas* pCanvas)
	{
		Register<UI::CUICanvas>(pCanvas, m_canvasList, nullptr);
	}

	void CRenderingSystem::Deregister(UI::CUICanvas* pCanvas)
	{
		Deregister<UI::CUICanvas>(pCanvas, m_canvasList);
	}
};
