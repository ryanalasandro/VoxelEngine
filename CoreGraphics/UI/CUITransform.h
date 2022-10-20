//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: UI/CUITransform.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CUITRANSFORM_H
#define CUITRANSFORM_H

#include <Logic/CTransform.h>
#include <Math/CMathVector2.h>
#include <vector>

namespace UI
{
	class CUITransform : public Logic::CTransform
	{
	public:
		CUITransform(const CVObject* pObject);
		~CUITransform();
		CUITransform(const CUITransform&) = delete;
		CUITransform(CUITransform&&) = delete;
		CUITransform& operator = (const CUITransform&) = delete;
		CUITransform& operator = (CUITransform&&) = delete;

		// Accessors.
		inline const CVObject* GetObject() const { return m_pObject; }
		
		// Width.
		inline float GetWidth() const { return m_width; }
		inline void SetWidth(float width)
		{
			m_width = width;
			SetAsDirty(true);
		}

		// Height.
		inline float GetHeight() const { return m_height; }
		inline void SetHeight(float height)
		{
			m_height = height;
			SetAsDirty(true);
		}

		// Anchor.
		inline const Math::SIMDVector& GetAnchor() const { return m_anchor; }
		inline void SetAnchor(const Math::SIMDVector anchor)
		{
			m_anchor = anchor;
			SetAsDirty(true);
		}

		// Pivot.
		inline const Math::SIMDVector& GetPivot() const { return m_pivot; }
		inline void SetPivot(const Math::SIMDVector pivot)
		{
			m_pivot = pivot;
			SetAsDirty(true);
		}

		// Accessors.
		inline const Math::SIMDVector& GetOffset() const { return m_offset; }
		inline void SetOffset(const Math::SIMDVector offset)
		{
			m_offset = offset;
			SetAsDirty(true);
		}

	private:
		void Calculate() final;
		void SetAsDirty(bool bForced) final;

	public:
		void SetParent(CTransform* pParent) final;

	private:
		Math::SIMDVector m_offset;

		Math::SIMDVector m_anchor;
		Math::SIMDVector m_pivot;
		float m_width;
		float m_height;
	};
};

#endif
