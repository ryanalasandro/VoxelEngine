//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CPostEffect.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CPOSTEFFECT_H
#define CPOSTEFFECT_H

#include "CRenderTexture.h"
#include <Globals/CGlobals.h>

namespace App
{
	class CPanel;
};

namespace Graphics
{
	class CPostEffect
	{
	protected:
		CPostEffect() : m_renderTextureCount(0), m_pRenderTextureDataList(nullptr), m_pRenderTextureList(nullptr),
			m_pPostMat(nullptr), m_pGraphicsAPI(nullptr) { }

		CPostEffect(const CPostEffect&) = delete;
		CPostEffect(CPostEffect&&) = delete;
		CPostEffect& operator = (const CPostEffect&) = delete;
		CPostEffect& operator = (CPostEffect&&) = delete;

	public:
		virtual ~CPostEffect() { }

		virtual void Initialize() = 0;
		virtual void PostInitialize() = 0;
		virtual const CRenderTexture** ApplyEffect(const CRenderTexture** ppTargetTextureList, u32 targetTextureCount,
			const CRenderTexture** ppPostTextureList, u32 postTextureCount, u32& outRenderTextureCount) = 0;
		virtual void OnResize(const App::CPanel* pPanel) = 0;
		virtual void Release() = 0;

	protected:
		u32 m_renderTextureCount;

		CRenderTexture::Data* m_pRenderTextureDataList;
		CRenderTexture* m_pRenderTextureList;

		class CMaterial* m_pPostMat;
		class CGraphicsAPI* m_pGraphicsAPI;
	};
};

#endif
