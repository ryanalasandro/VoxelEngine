//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Graphics/CPostSSAO.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CPOSTSSAO_H
#define CPOSTSSAO_H

#include <Graphics/CPostEffect.h>

namespace Graphics
{
	class CPostSSAO : public CPostEffect
	{
	public:
		CPostSSAO();
		~CPostSSAO();
		CPostSSAO(const CPostSSAO&) = delete;
		CPostSSAO(CPostSSAO&&) = delete;
		CPostSSAO& operator = (const CPostSSAO&) = delete;
		CPostSSAO& operator = (CPostSSAO&&) = delete;

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
		class CMaterial* m_pHBlurMat;
		class CMaterial* m_pVBlurMat;
	};
};

#endif
