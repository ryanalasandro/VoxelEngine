//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CRenderer.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CRENDERER_H
#define CRENDERER_H

#include <Objects/CVComponent.h>

namespace Graphics 
{
	class CRenderer : public CVComponent
	{
	public:
		friend class CRenderingSystem;

	protected:
		CRenderer(const CVObject* pObject);
		CRenderer(const CRenderer&) = delete;
		CRenderer(CRenderer&&) = delete;
		CRenderer& operator = (const CRenderer&) = delete;
		CRenderer& operator = (CRenderer&&) = delete;

	public:
		void Initialize() override;
		void Release() override;

		// Accessors.
		virtual inline const CVObject* GetObject() const { return m_pObject; }

		inline bool IsActive() const { return m_bActive; }
		virtual inline class CMaterial* GetMaterial() const = 0;

		// Modifiers.
		inline void SetActive(bool bActive) { m_bActive = bActive; }

	protected:
		virtual void Render() = 0;
		virtual void PostInitialize() { }

	private:
		bool m_bActive;
	};
};

#endif
