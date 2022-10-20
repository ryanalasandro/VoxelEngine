//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CMeshData.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CMeshData.h"

namespace Graphics
{
	CMeshData::CMeshData(const class CVObject* pObject) :
		CVComponent(pObject),
		m_pVertexList(nullptr),
		m_pIndexList(nullptr),
		m_pBuffer(nullptr) {
	}

	CMeshData::~CMeshData() { }

	void CMeshData::Initialize()
	{
		m_vertexSize = m_data.vertexCount * m_data.vertexStride;
		m_indexSize = m_data.indexCount * m_data.indexStride;
		const u32 totalSize = m_vertexSize + m_indexSize;

		m_pBuffer = new u8[totalSize];
		m_pVertexList = m_pBuffer;
		m_pIndexList = m_pVertexList + m_vertexSize;
	}

	void CMeshData::Release()
	{
		delete[] m_pBuffer;
		m_pIndexList = m_pVertexList = m_pBuffer = nullptr;
	}

	//-----------------------------------------------------------------------------------------------
	// Data processors.
	//-----------------------------------------------------------------------------------------------

	void CMeshData::ProcessVertexList(std::function<void(u32, u8*)> processor)
	{
		u8* pVertex = m_pVertexList;
		for(u32 i = 0; i < m_data.vertexCount; ++i)
		{
			processor(i, pVertex);
			pVertex += m_data.vertexStride;
		}
	}

	void CMeshData::ProcessIndexList(std::function<void(u32, u8*)> processor)
	{
		u8* pIndex = m_pIndexList;
		for(u32 i = 0; i < m_data.indexCount; ++i)
		{
			processor(i, pIndex);
			pIndex += m_data.indexStride;
		}
	}
};
