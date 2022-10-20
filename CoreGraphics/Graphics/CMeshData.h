//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CMeshData.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CMESHDATA_H
#define CMESHDATA_H

#include "CGraphicsData.h"
#include <Objects/CVComponent.h>
#include <Globals/CGlobals.h>
#include <functional>

namespace Graphics
{
	class CMeshData : public CVComponent
	{
	public:
		struct Data
		{
			PRIMITIVE_TOPOLOGY topology;

			u32 vertexCount;
			u32 indexCount;

			u32 vertexStride;
			u32 indexStride;
		};

	public:
		CMeshData(const class CVObject* pObject);
		~CMeshData();
		CMeshData(const CMeshData&) = delete;
		CMeshData(CMeshData&&) = delete;
		CMeshData& operator = (const CMeshData&) = delete;
		CMeshData& operator = (CMeshData&&) = delete;

		void Initialize() final;
		void Release() final;

		void ProcessVertexList(std::function<void(u32, u8*)> processor);
		void ProcessIndexList(std::function<void(u32, u8*)> processor);

		// Accessors.
		inline PRIMITIVE_TOPOLOGY GetTopology() const { return m_data.topology; }

		inline u32 GetVertexCount() const { return m_data.vertexCount; }
		inline u32 GetVertexStride() const { return m_data.vertexStride; }
		inline u32 GetVertexSize() const { return m_vertexSize; }

		inline u32 GetIndexCount() const { return m_data.indexCount; }
		inline u32 GetIndexStride() const { return m_data.indexStride; }
		inline u32 GetIndexSize() const { return m_indexSize; }

		inline const u8* GetVertexList() const { return m_pVertexList; }
		inline const u8* GetIndexList() const { return m_pIndexList; }

		inline u8* GetVertexAt(u32 offset) { return m_pVertexList + offset * m_data.vertexStride; }
		inline u8* GetIndexAt(u32 offset) { return m_pIndexList + offset * m_data.indexStride; }

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }

	private:
		Data m_data;

		u32 m_vertexSize;
		u32 m_indexSize;

		u8* m_pVertexList;
		u8* m_pIndexList;
		u8* m_pBuffer;
	};
};

#endif
