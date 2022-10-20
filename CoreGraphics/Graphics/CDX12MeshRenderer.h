//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CDX12MeshRenderer.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CDX12MESHRENDERER_H
#define CDX12MESHRENDERER_H

#include "CMeshRenderer.h"
#include <d3d12.h>

namespace Graphics
{
	class CDX12MeshRenderer : public CMeshRenderer
	{
	public:
		CDX12MeshRenderer(const CVObject* pObject);
		~CDX12MeshRenderer();
		CDX12MeshRenderer(const CDX12MeshRenderer&) = delete;
		CDX12MeshRenderer(CDX12MeshRenderer&&) = delete;
		CDX12MeshRenderer& operator = (const CDX12MeshRenderer&) = delete;
		CDX12MeshRenderer& operator = (CDX12MeshRenderer&&) = delete;

	private:
		void Initialize() final;
		void PostInitialize() final;
		void Render() final;
		void Release() final;

	private:
		D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
		D3D12_INDEX_BUFFER_VIEW m_indexBufferView;

		ID3D12Resource* m_pVertexBuffer;
		ID3D12Resource* m_pVertexBufferUpload;
		ID3D12Resource* m_pIndexBuffer;
		ID3D12Resource* m_pIndexBufferUpload;

		ID3D12GraphicsCommandList* m_pBundle;

		class CDX12Graphics* m_pDX12Graphics;
	};
};

#endif
