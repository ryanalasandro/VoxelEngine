//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CPostProcessor.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CPOSTPROCESSOR_H
#define CPOSTPROCESSOR_H

#include "CRenderTexture.h"
#include <vector>

namespace App
{
	class CPanel;
};

namespace Graphics
{
	class CPostProcessor
	{
	public:
		CPostProcessor();
		~CPostProcessor();
		CPostProcessor(const CPostProcessor&) = delete;
		CPostProcessor(CPostProcessor&&) = delete;
		CPostProcessor& operator = (const CPostProcessor&) = delete;
		CPostProcessor& operator = (CPostProcessor&&) = delete;

		void Initialize();
		void PostInitialize();
		void BeginRenderTo(CLEAR_MODE clearMode, u32 numRects, const Math::RectLTRB* pRects);
		void EndRenderTo();
		void RenderQuad();
		void OnResize(const App::CPanel* pPanel);
		void Release();

		inline void RegisterPostEffect(class CPostEffect* pPostEffect) { m_postEffectList.push_back(pPostEffect); }

	private:
		void PostProcess();

	private:
		CRenderTexture m_renderTexture;
		CRenderTexture::Data m_renderTextureData;

		std::vector<class CPostEffect*> m_postEffectList;

		class CMaterial* m_pPostMat;
		class CGraphicsAPI* m_pGraphicsAPI;
	};
};

#endif
