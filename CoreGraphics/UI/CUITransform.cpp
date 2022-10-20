//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: UI/CUITransform.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CUITransform.h"
#include <cassert>

namespace UI
{
	CUITransform::CUITransform(const CVObject* pObject) : 
		CTransform(pObject)
	{
	}

	CUITransform::~CUITransform()
	{
	}

	void CUITransform::Calculate()
	{
		m_offset = Math::SIMDVector::PointwiseProduct(Math::Vector2(m_width, m_height), m_pivot);

		if(m_pParent)
		{ // Has parent.
			CUITransform* pUIParent = dynamic_cast<CUITransform*>(m_pParent);
			assert(pUIParent);
			
			m_localMatrix = Math::SIMDMatrix::Scale(m_localScale);

			const float* localEuler = m_localEuler.ToFloat();
			m_localMatrix *= Math::SIMDMatrix::RotateZ(localEuler[2]);
			m_localMatrix *= Math::SIMDMatrix::RotateX(localEuler[0]);
			m_localMatrix *= Math::SIMDMatrix::RotateY(localEuler[1]);

			m_offset -= Math::SIMDVector::PointwiseProduct(Math::Vector2(pUIParent->GetWidth(), pUIParent->GetHeight()), m_anchor);
			m_localMatrix *= Math::SIMDMatrix::Translate(m_localPosition - m_offset);
			
			// Update world transforms.
			m_position = pUIParent->GetPosition() - pUIParent->GetOffset() + m_localPosition;

			m_euler = pUIParent->GetEuler() + m_localEuler;
			m_rotation.RotateEuler(m_euler);

			m_scale = Math::SIMDVector::PointwiseProduct(pUIParent->GetScale(), m_localScale);

			m_worldMatrix = m_localMatrix * pUIParent->GetWorldMatrix();
		}
		else
		{ // At root.
			m_worldMatrix = Math::SIMDMatrix::Scale(m_scale);

			const float* euler = m_euler.ToFloat();
			m_worldMatrix *= Math::SIMDMatrix::RotateZ(euler[2]);
			m_worldMatrix *= Math::SIMDMatrix::RotateX(euler[0]);
			m_worldMatrix *= Math::SIMDMatrix::RotateY(euler[1]);

			m_worldMatrix *= Math::SIMDMatrix::Translate(m_position - m_offset);
			m_localMatrix = m_worldMatrix;
		}
	}

	void CUITransform::SetAsDirty(bool bForced)
	{
		CTransform::SetAsDirty(bForced);
	}

	void CUITransform::SetParent(CTransform* pParent)
	{
		if(m_pParent == pParent)
		{
			return;
		}

		Math::SIMDVector prev;
		if(m_pParent)
		{
			CUITransform* pUIParent = dynamic_cast<CUITransform*>(m_pParent);
			assert(pUIParent);
			prev = Math::SIMDVector::PointwiseProduct(Math::Vector2(pUIParent->GetWidth(), pUIParent->GetHeight()), m_anchor);
		}
		else
		{
			prev = Math::SIMD_VEC_ZERO;
		}
		
		Math::SIMDVector next;
		if(pParent)
		{
			CUITransform* pUIParent = dynamic_cast<CUITransform*>(pParent);
			assert(pUIParent);
			next = Math::SIMDVector::PointwiseProduct(Math::Vector2(pUIParent->GetWidth(), pUIParent->GetHeight()), m_anchor);
		}
		else
		{
			next = Math::SIMD_VEC_ZERO;
		}

		m_pivot -= Math::SIMDVector::PointwiseQuotient(next - prev, Math::SIMDVector(m_width, m_height, 1.0f));
	}
};
