//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: UI/CUIText.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CUITEXT_H
#define CUITEXT_H

#include "CUIRenderer.h"
#include <string>

namespace UI
{
	class CUIText : public CUIRenderer
	{
	public:
		friend class CUICanvas;

	public:
		struct Data
		{
			float fontSize;
			std::wstring text;

			u64 materialHash;
			class CUITransform* pTransform;
		};

	public:
		CUIText(const CVObject* pObject);
		~CUIText();
		CUIText(const CUIText&) = delete;
		CUIText(CUIText&&) = delete;
		CUIText& operator = (const CUIText&) = delete;
		CUIText& operator = (CUIText&&) = delete;

		void Initialize() final;
		void Release() final;

		// Accessors.
		inline u64 GetMaterialHash() const final { return m_data.materialHash; }
		inline Graphics::CMaterial* GetMaterial() const final { return m_pMaterial; }

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }

	private:
		void Render() final;

	private:
		Data m_data;

		Graphics::CMaterial* m_pMaterial;
	};
};

#endif
