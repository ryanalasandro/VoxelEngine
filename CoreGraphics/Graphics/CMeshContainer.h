//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CMeshContainer.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CMESHCONTAINER_H
#define CMESHCONTAINER_H

#include "CRenderer.h"
#include <functional>

namespace Graphics
{
	class CMeshContainer : public CRenderer
	{
	public:
		friend class CRenderingSystem;

	public:
		struct Data
		{
			std::function<void()> onPreRender;
			class CMaterial* pMaterial;
			class CMeshRenderer* pMeshRenderer;
		};

	public:
		CMeshContainer(const CVObject* pObject);
		CMeshContainer(const CMeshContainer&) = delete;
		CMeshContainer(CMeshContainer&&) = delete;
		CMeshContainer& operator = (const CMeshContainer&) = delete;
		CMeshContainer& operator = (CMeshContainer&&) = delete;

	public:
		void Initialize() final;
		void Release() final;
		
		// Accessors.
		virtual inline class CMaterial* GetMaterial() const final { return m_data.pMaterial; }

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }
		inline void SetMeshRenderer(class CMeshRenderer* pMeshRenderer) { m_data.pMeshRenderer = pMeshRenderer; }
		
	protected:
		virtual void PostInitialize() { }
		virtual void Render() final;

	private:
		Data m_data;
	};
};

#endif
