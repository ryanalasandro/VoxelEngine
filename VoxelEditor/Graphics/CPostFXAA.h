//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Graphics/CPostFXAA.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CPOSTFXAA_H
#define CPOSTFXAA_H

#include <Graphics/CPostEffect.h>

namespace Graphics
{
	class CPostFXAA : public CPostEffect
	{
	public:
		CPostFXAA();
		~CPostFXAA();
		CPostFXAA(const CPostFXAA&) = delete;
		CPostFXAA(CPostFXAA&&) = delete;
		CPostFXAA& operator = (const CPostFXAA&) = delete;
		CPostFXAA& operator = (CPostFXAA&&) = delete;

		void Initialize() final;
		void PostInitialize() final;
		const class CRenderTexture** ApplyEffect(const CRenderTexture** ppTargetTextureList, u32 targetTextureCount,
			const CRenderTexture** ppPostTextureList, u32 postTextureCount, u32& outRenderTextureCount) final;
		void OnResize(const App::CPanel* pPanel) final;
		void Release() final;

	private:
	};
};

#endif
