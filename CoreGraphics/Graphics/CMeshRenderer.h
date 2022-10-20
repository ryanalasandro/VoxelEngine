//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CMeshRenderer.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CMESHRENDERER_H
#define CMESHRENDERER_H

#include "CRenderer.h"
#include <functional>

namespace Graphics
{
	class CMeshRenderer : public CRenderer
	{
	public:
		friend class CRenderingSystem;
		friend class CMeshContainer;

	public:
		struct Data
		{
			bool bSkipRegistration;
			std::function<void()> onPreRender;
			class CMeshData* pMeshData;
			class CMaterial* pMaterial;
		};

	protected:
		CMeshRenderer(const CVObject* pObject);
		CMeshRenderer(const CMeshRenderer&) = delete;
		CMeshRenderer(CMeshRenderer&&) = delete;
		CMeshRenderer& operator = (const CMeshRenderer&) = delete;
		CMeshRenderer& operator = (CMeshRenderer&&) = delete;

	public:
		virtual ~CMeshRenderer();

		void Initialize() override;
		void Release() override;

		// Accessors.
		inline class CMaterial* GetMaterial() const final { return m_data.pMaterial; }

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }
		//inline void SetMaterial(class CMaterial* pMaterial) { m_data.pMaterial = pMaterial; }

	protected:
		virtual void PostInitialize() = 0;
		virtual void Render() override;

	protected:
		Data m_data;
	};
};

#endif
