//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CDX12MeshRenderer.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CDX12MeshRenderer.h"
#include "CDX12Graphics.h"
#include "CDX12Data.h"
#include "CMeshData.h"
#include "CMaterial.h"
#include "CShader.h"
#include "CDX12RootSignature.h"
#include "d3dx12.h"
#include "../Factory/CFactory.h"
#include <Utilities/CDebugError.h>
#include <Utilities/CMemoryFree.h>

namespace Graphics
{
	CDX12MeshRenderer::CDX12MeshRenderer(const CVObject* pObject) :
		CMeshRenderer(pObject),
		m_pVertexBuffer(nullptr),
		m_pVertexBufferUpload(nullptr),
		m_pIndexBuffer(nullptr),
		m_pIndexBufferUpload(nullptr),
		m_pBundle(nullptr),
		m_pDX12Graphics(nullptr) {
	}

	CDX12MeshRenderer::~CDX12MeshRenderer() { }

	void CDX12MeshRenderer::Initialize()
	{
		CMeshRenderer::Initialize();

		m_pDX12Graphics = reinterpret_cast<CDX12Graphics*>(CFactory::Instance().GetGraphicsAPI());

		CD3DX12_RESOURCE_BARRIER barrierList[3];
		u32 barrierCount = 0;

		if(m_data.pMeshData->GetVertexSize())
		{
			{ // Create the vertex buffer.
				CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);
				CD3DX12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(m_data.pMeshData->GetVertexSize());
				ASSERT_HR_R(m_pDX12Graphics->GetDevice()->CreateCommittedResource(
					&heapProps,
					D3D12_HEAP_FLAG_NONE,
					&resDesc,
					D3D12_RESOURCE_STATE_COPY_DEST,
					nullptr,
					IID_PPV_ARGS(&m_pVertexBuffer)
				));

				NAME_D3D12_OBJECT(m_pVertexBuffer);
			}

			{ // Create the vertex upload buffer.
				const u64 uploadBufferSize = GetRequiredIntermediateSize(m_pVertexBuffer, 0, 1);
				CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
				CD3DX12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);
				ASSERT_HR_R(m_pDX12Graphics->GetDevice()->CreateCommittedResource(
					&heapProps,
					D3D12_HEAP_FLAG_NONE,
					&resDesc,
					D3D12_RESOURCE_STATE_GENERIC_READ,
					nullptr,
					IID_PPV_ARGS(&m_pVertexBufferUpload)
				));
			}

			// Prepare for the upload of vertex data.
			D3D12_SUBRESOURCE_DATA vertexData { };
			vertexData.pData = m_data.pMeshData->GetVertexList();
			vertexData.RowPitch = static_cast<LONG_PTR>(m_data.pMeshData->GetVertexSize());
			vertexData.SlicePitch = vertexData.RowPitch;

			UpdateSubresources<1>(m_pDX12Graphics->GetAssetCommandList(), m_pVertexBuffer, m_pVertexBufferUpload, 0, 0, 1, &vertexData);

			// Create the vertex buffer's view.
			m_vertexBufferView.BufferLocation = m_pVertexBuffer->GetGPUVirtualAddress();
			m_vertexBufferView.StrideInBytes = m_data.pMeshData->GetVertexStride();
			m_vertexBufferView.SizeInBytes = m_data.pMeshData->GetVertexSize();

			// Make sure to transition the vertex buffer to a state that can be feed through the pipeline for rendering.
			barrierList[barrierCount++] = CD3DX12_RESOURCE_BARRIER::Transition(m_pVertexBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		}

		if(m_data.pMeshData->GetIndexSize())
		{
			{ // Create the index buffer.
				CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);
				CD3DX12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(m_data.pMeshData->GetIndexSize());
				ASSERT_HR_R(m_pDX12Graphics->GetDevice()->CreateCommittedResource(
					&heapProps,
					D3D12_HEAP_FLAG_NONE,
					&resDesc,
					D3D12_RESOURCE_STATE_COPY_DEST,
					nullptr,
					IID_PPV_ARGS(&m_pIndexBuffer)
				));

				NAME_D3D12_OBJECT(m_pIndexBuffer);
			}

			{ // Create the index upload buffer.
				const u64 uploadBufferSize = GetRequiredIntermediateSize(m_pIndexBuffer, 0, 1);
				CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
				CD3DX12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);
				ASSERT_HR_R(m_pDX12Graphics->GetDevice()->CreateCommittedResource(
					&heapProps,
					D3D12_HEAP_FLAG_NONE,
					&resDesc,
					D3D12_RESOURCE_STATE_GENERIC_READ,
					nullptr,
					IID_PPV_ARGS(&m_pIndexBufferUpload)
				));
			}

			// Prepare for the upload of index data.
			D3D12_SUBRESOURCE_DATA indexData { };
			indexData.pData = m_data.pMeshData->GetIndexList();
			indexData.RowPitch = static_cast<LONG_PTR>(m_data.pMeshData->GetIndexSize());
			indexData.SlicePitch = indexData.RowPitch;

			UpdateSubresources<1>(m_pDX12Graphics->GetAssetCommandList(), m_pIndexBuffer, m_pIndexBufferUpload, 0, 0, 1, &indexData);

			// Create the index buffer's view.
			m_indexBufferView.BufferLocation = m_pIndexBuffer->GetGPUVirtualAddress();
			m_indexBufferView.Format = m_data.pMeshData->GetIndexStride() == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
			m_indexBufferView.SizeInBytes = m_data.pMeshData->GetIndexSize();

			// Make sure to transition the vertex buffer to a state that can be feed through the pipeline for rendering.
			barrierList[barrierCount++] = CD3DX12_RESOURCE_BARRIER::Transition(m_pIndexBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER);
		}

		if(barrierCount)
		{
			m_pDX12Graphics->GetAssetCommandList()->ResourceBarrier(barrierCount, barrierList);
		}

		{ // Create bundle for rendering.
			m_pBundle = m_pDX12Graphics->CreateBundle(m_data.pMaterial);
			m_pBundle->IASetPrimitiveTopology(ConvertPrimitiveTopologyToD3D12(m_data.pMeshData->GetTopology()));

			if(m_data.pMeshData->GetVertexSize())
			{
				m_pBundle->IASetVertexBuffers(0, 1, &m_vertexBufferView);
			}
			else
			{
				//m_pBundle->IASetVertexBuffers(0, 0, nullptr);
			}

			if(m_data.pMeshData->GetIndexSize())
			{
				m_pBundle->IASetIndexBuffer(&m_indexBufferView);
				m_pBundle->DrawIndexedInstanced(m_data.pMeshData->GetIndexCount(), 1, 0, 0, 0);
			}
			else
			{
				//m_pBundle->IASetIndexBuffer(nullptr);
				m_pBundle->DrawInstanced(m_data.pMeshData->GetVertexCount(), 1, 0, 0);
			}

			ASSERT_HR_R(m_pBundle->Close());
		}
	}

	void CDX12MeshRenderer::PostInitialize()
	{
		SAFE_RELEASE(m_pIndexBufferUpload);
		SAFE_RELEASE(m_pVertexBufferUpload);
	}

	void CDX12MeshRenderer::Render()
	{
		CMeshRenderer::Render();
		m_pDX12Graphics->GetRealtimeCommandList()->ExecuteBundle(m_pBundle);
	}

	void CDX12MeshRenderer::Release()
	{
		CMeshRenderer::Release();

		SAFE_RELEASE(m_pBundle);

		SAFE_RELEASE(m_pIndexBuffer);
		SAFE_RELEASE(m_pVertexBuffer);

		SAFE_RELEASE(m_pIndexBufferUpload);
		SAFE_RELEASE(m_pVertexBufferUpload);
	}
};
