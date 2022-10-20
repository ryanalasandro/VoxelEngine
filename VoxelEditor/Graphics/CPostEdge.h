//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Graphics/CPostEdge.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CPOSTEDGE_H
#define CPOSTEDGE_H

#include <Graphics/CPostEffect.h>

namespace Graphics
{
	class CPostEdge : public CPostEffect
	{
	public:
		CPostEdge();
		~CPostEdge();
		CPostEdge(const CPostEdge&) = delete;
		CPostEdge(CPostEdge&&) = delete;
		CPostEdge& operator = (const CPostEdge&) = delete;
		CPostEdge& operator = (CPostEdge&&) = delete;

		void Initialize() final;
		void PostInitialize() final;
		const class CRenderTexture** ApplyEffect(const CRenderTexture** ppTargetTextureList, u32 targetTextureCount,
			const CRenderTexture** ppPostTextureList, u32 postTextureCount, u32& outRenderTextureCount) final;
		void OnResize(const App::CPanel* pPanel) final;
		void Release() final;

		// Accessors.
		inline u32 GetRenderTextureCount() const { return m_renderTextureCount; }
		inline const class CRenderTexture** GetRenderTextureList() const { return const_cast<const class CRenderTexture**>(&m_pRenderTextureList); }

	private:
	};
};

#endif
