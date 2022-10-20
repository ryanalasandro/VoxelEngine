//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Universe/CNodeChunk.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CNODECHUNK_H
#define CNODECHUNK_H

#include "../Physics/CVolumeChunk.h"
#include <Globals/CGlobals.h>
#include <Objects/CVObject.h>
#include <Graphics/CMeshData.h>
#include <Graphics/CMeshContainer.h>
#include <Logic/CTransform.h>
#include <Logic/CCallback.h>
#include <Math/CMathVector3.h>
#include <Utilities/CFuture.h>
#include <Utilities/CTSDeque.h>
#include <shared_mutex>
#include <fstream>

namespace Graphics
{
	class CMeshRenderer;
	class CMaterial;
};

namespace Universe
{
	enum SIDE : u8
	{
		SIDE_LEFT,
		SIDE_RIGHT,
		SIDE_BOTTOM,
		SIDE_TOP,
		SIDE_BACK,
		SIDE_FRONT,
	};
	
	enum SIDE_FLAG : u8
	{
		SIDE_FLAG_LEFT = 0x1 << SIDE_LEFT,
		SIDE_FLAG_RIGHT = 0x1 << SIDE_RIGHT,
		SIDE_FLAG_BOTTOM = 0x1 << SIDE_BOTTOM,
		SIDE_FLAG_TOP = 0x1 << SIDE_TOP,
		SIDE_FLAG_BACK = 0x1 << SIDE_BACK,
		SIDE_FLAG_FRONT = 0x1 << SIDE_FRONT,
	};

	const Math::Vector3 SIDE_NORMAL[] = {
		Math::VEC3_LEFT,
		Math::VEC3_RIGHT,
		Math::VEC3_DOWN,
		Math::VEC3_UP,
		Math::VEC3_BACKWARD,
		Math::VEC3_FORWARD,
	};

	class CNodeChunk : public CVObject
	{
	private:
		struct BlockUpdateData
		{
			u32 index;
			u16 id;
		};

	public:
		struct Data
		{
			u32 width;
			u32 height;
			u32 length;
		};

		struct Block
		{
			u16 id;
			u8 sideFlag;
			u8 padding;
		};

	public:
		CNodeChunk(const wchar_t* pName, u32 sceneHash);
		~CNodeChunk();
		CNodeChunk(const CNodeChunk&) = delete;
		CNodeChunk(CNodeChunk&&) = delete;
		CNodeChunk& operator = (const CNodeChunk&) = delete;
		CNodeChunk& operator = (CNodeChunk&&) = delete;

		void Initialize() final;
		void LateUpdate() final;
		void Release() final;

		void SaveToFile(std::ofstream& file) const;
		void LoadFromFile(std::ifstream& file);
		
		// Accessors.
		inline u32 GetWidth() const
		{
			std::shared_lock<std::shared_mutex> lk(m_mutex);
			return m_data.width;
		}

		inline u32 GetHeight() const
		{
			std::shared_lock<std::shared_mutex> lk(m_mutex);
			return m_data.height;
		}

		inline u32 GetLength() const
		{
			std::shared_lock<std::shared_mutex> lk(m_mutex);
			return m_data.length;
		}
		
		inline void GenerateIndicesFromRaycastInfo(const Physics::RaycastInfo& info, int& i, int& j, int& k) const
		{
			std::shared_lock<std::shared_mutex> lk(m_mutex);
			return internalGenerateIndicesFromRaycastInfo(info, i, j, k);
		}

		inline Math::SIMDVector GetPositionFromIndex(int i, int j, int k) const
		{
			std::shared_lock<std::shared_mutex> lk(m_mutex);
			return internalGetPositionFromIndex(i, j, k);
		}
		
		inline u32 GetIndex(u32 i, u32 j, u32 k) const
		{
			std::shared_lock<std::shared_mutex> lk(m_mutex);
			return internalGetIndex(i, j, k);
		}
		
		inline int GetIndexInt(int i, int j, int k) const
		{
			std::shared_lock<std::shared_mutex> lk(m_mutex);
			return internalGetIndexInt(i, j, k);
		}

		inline Block GetBlock(u32 index) const
		{
			std::shared_lock<std::shared_mutex> lk(m_mutex);
			return m_pBlockList[index];
		}

		inline Block GetBlock(u32 i, u32 j, u32 k) const 
		{
			std::shared_lock<std::shared_mutex> lk(m_mutex);
			return m_pBlockList[internalGetIndex(i, j, k)];
		}

		// Modifiers.
		inline void SetData(const Data& data)
		{
			std::lock_guard<std::shared_mutex> lk(m_mutex);
			m_data = data;
		}
		
	private:
		void BuildMesh();
		void PreRender();
		void InteractCallback(void* pVal);
		
		void internalGenerateIndicesFromRaycastInfo(const Physics::RaycastInfo& info, int& i, int& j, int& k) const;
		inline Math::SIMDVector internalGetPositionFromIndex(int i, int j, int k) const
		{
			return m_transform.GetPosition() +
				Math::SIMDVector(static_cast<float>(i) - (m_data.width >> 1), static_cast<float>(j) - (m_data.height >> 1), static_cast<float>(k) - (m_data.length >> 1)) + 0.5f;
		}
		
		inline u32 internalGetIndex(u32 i, u32 j, u32 k) const
		{
			return i * m_data.length * m_data.height + k * m_data.height + (m_data.height - 1 - j);
		}
		
		inline int internalGetIndexInt(int i, int j, int k) const
		{
			return i * static_cast<int>(m_data.length + 2) * static_cast<int>(m_data.height + 2) + k * static_cast<int>(m_data.height + 2) + (static_cast<int>(m_data.height + 2) - 1 - j);
		}

	private:
		mutable std::shared_mutex m_mutex;

		bool m_bDirty;
		u8 m_meshIndex;

		Data m_data;
		
		Logic::CTransform m_transform;
		Physics::CVolumeChunk m_volume;
		Logic::CCallback m_callback;

		Util::CDeque<BlockUpdateData> blockDeque;

		Graphics::CMeshData m_meshData;
		Graphics::CMeshContainer m_meshContainer;
		Graphics::CMeshRenderer* m_pMeshRendererList[2];
		Util::CFuture<void> m_meshFuture[2];
		Graphics::CMaterial* m_pMaterial;

		Block* m_pBlockList;
	};
};

#endif
