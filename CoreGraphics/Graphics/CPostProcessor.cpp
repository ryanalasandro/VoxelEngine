//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CPostProcessor.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CPostProcessor.h"
#include "CPostEffect.h"
#include "CGraphicsAPI.h"
#include "../Application/CPanel.h"
#include "CMaterial.h"
#include "CShader.h"
#include "CRootSignature.h"
#include "../Factory/CFactory.h"
#include "../Resources/CResourceManager.h"
#include <Math/CMathFNV.h>

namespace Graphics
{
	CPostProcessor::CPostProcessor() :
		m_pPostMat(nullptr),
		m_pGraphicsAPI(nullptr) {
	}

	CPostProcessor::~CPostProcessor() { }

	void CPostProcessor::Initialize()
	{
		m_pGraphicsAPI = CFactory::Instance().GetGraphicsAPI();

		m_pPostMat = reinterpret_cast<CMaterial*>(Resources::CManager::Instance().GetResource(Resources::RESOURCE_TYPE_MATERIAL, "MATERIAL_POST"));

		{ // Setup post render texture.
			memset(&m_renderTextureData, 0, sizeof(m_renderTextureData));
			m_renderTextureData.colorBufferCount = 3;
			m_renderTextureData.bUseDepthStencil = true;
			m_renderTextureData.width = static_cast<u32>(m_pGraphicsAPI->GetWidth());
			m_renderTextureData.height = static_cast<u32>(m_pGraphicsAPI->GetHeight());
			m_renderTextureData.downScale = 0;
			m_renderTextureData.colorFormat[0] = GFX_FORMAT_R32G32B32A32_FLOAT;
			m_renderTextureData.colorFormat[1] = GFX_FORMAT_R32G32B32A32_FLOAT;
			m_renderTextureData.colorFormat[2] = GFX_FORMAT_R32G32B32A32_FLOAT;
			m_renderTextureData.depthFormat = GFX_FORMAT_D32_FLOAT;

			m_renderTextureData.clearColor[0] = m_pGraphicsAPI->GetData().clearValue.color;
			m_renderTextureData.clearColor[0].a = 0.0f;
			m_renderTextureData.clearColor[1] = Math::Color(0.0f, 0.0f);
			m_renderTextureData.clearColor[2] = Math::Color(0.0f, 0.0f);

			m_renderTextureData.clearDepthStencil.depth = 1.0f;
			m_renderTextureData.clearDepthStencil.stencil = 0;

			m_renderTexture.SetData(m_renderTextureData);
			m_renderTexture.Initialize();
		}
	}

	void CPostProcessor::PostInitialize()
	{
		m_renderTexture.PostInitialize();
	}

	void CPostProcessor::BeginRenderTo(CLEAR_MODE clearMode, u32 numRects, const Math::RectLTRB* pRects)
	{
		m_renderTexture.BeginRender(clearMode, numRects, pRects);
	}

	void CPostProcessor::EndRenderTo()
	{
		m_renderTexture.EndRender();
	}

	void CPostProcessor::PostProcess()
	{
		const CRenderTexture* ppInputRenderTextureList[] = { &m_renderTexture };
		u32 renderTextureCount = 0;
		const CRenderTexture** ppRenderTextureList = nullptr;

		for(size_t i = 0; i < m_postEffectList.size(); ++i)
		{
			ppRenderTextureList = m_postEffectList[i]->ApplyEffect(ppInputRenderTextureList, 1, ppRenderTextureList, renderTextureCount, renderTextureCount);
		}

		assert(renderTextureCount == 1);
		static const u32 textureHash = Math::FNV1a_32("inputTexture");
		m_pPostMat->SetTexture(textureHash, ppRenderTextureList[0]->GetRTVTexture(0));
	}

	void CPostProcessor::RenderQuad()
	{
		PostProcess();

		m_pPostMat->GetShader()->GetRootSignature()->Bind();
		m_pPostMat->Bind();
		m_pGraphicsAPI->RenderEmptyBuffer(PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, 3, 1);
	}

	void CPostProcessor::OnResize(const App::CPanel* pPanel)
	{
		m_renderTextureData.width = static_cast<u32>(m_pGraphicsAPI->GetWidth());
		m_renderTextureData.height = static_cast<u32>(m_pGraphicsAPI->GetHeight());
		m_renderTexture.SetData(m_renderTextureData);

		m_renderTexture.Resize();
		m_renderTexture.PostInitialize();
	}

	void CPostProcessor::Release()
	{
		m_renderTexture.Release();
	}
};
