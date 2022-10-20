//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: UI/CUIRenderer.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CUIRENDERER_H
#define CUIRENDERER_H

#include "../Graphics/CRenderer.h"

namespace UI
{
	class CUIRenderer : public Graphics::CRenderer
	{
	public:
		friend class CUICanvas;

	protected:
		CUIRenderer(const CVObject* pObject) : Graphics::CRenderer(pObject) { }
		virtual ~CUIRenderer() { }
		CUIRenderer(const CUIRenderer&) = delete;
		CUIRenderer(CUIRenderer&&) = delete;
		CUIRenderer& operator = (const CUIRenderer&) = delete;
		CUIRenderer& operator = (CUIRenderer&&) = delete;

	public:
		void Initialize() override { }
		void Release() override { }

		// Accessors.
		virtual inline u32 GetInstanceCount() const { return 1; }
		virtual inline u64 GetMaterialHash() const = 0;

	private:
	};
};

#endif
