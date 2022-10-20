//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Universe/CNodeChunk.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CNodeChunk.h"
#include "../Actors/CPlayer.h"
#include <Graphics/CMeshRenderer.h>
#include <Graphics/CMaterial.h>
#include <Graphics/CGraphicsAPI.h>
#include <Factory/CFactory.h>
#include <Resources/CResourceManager.h>
#include <Application/CSceneManager.h>
#include <Utilities/CMemoryFree.h>
#include <Utilities/CJobSystem.h>
#include <Math/CMathVector2.h>
#include <Math/CMathFNV.h>
#include <Windows.h>

namespace Universe
{
	CNodeChunk::CNodeChunk(const wchar_t* pName, u32 sceneHash) : 
		CVObject(pName, sceneHash),
		m_bDirty(false),
		m_meshIndex(0),
		m_transform(this),
		m_volume(this),
		m_callback(this),
		m_meshData(this),
		m_meshContainer(this),
		m_pMeshRendererList{ nullptr, nullptr },
		m_pMaterial(nullptr),
		m_pBlockList(nullptr) {
	}
	
	CNodeChunk::~CNodeChunk() { }
	
	void CNodeChunk::Initialize()
	{
		{ // Create ids.
			std::lock_guard<std::shared_mutex> lk(m_mutex);

			const u32 total = m_data.width * m_data.height * m_data.length;
			m_pBlockList = new Block[total];

			u32 index = 0;
			for(u32 i = 0; i < m_data.width; ++i)
			{
				for(u32 k = 0; k < m_data.length; ++k)
				{
					for(u32 j = m_data.height - 1; j != ~0; --j)
					{
						m_pBlockList[index++] = { (j == 0), 0 };
					}
				}
			}
		}

		BuildMesh();

		{ // Create mesh container.
			Graphics::CMeshContainer::Data data { };
			data.onPreRender = std::bind(&CNodeChunk::PreRender, this);
			data.pMeshRenderer = m_pMeshRendererList[m_meshIndex];
			data.pMaterial = m_pMeshRendererList[m_meshIndex]->GetMaterial();
			m_meshContainer.SetData(data);
			m_meshContainer.Initialize();
		}

		{ // Create collider.
			Physics::CVolumeChunk::Data data { };
			data.pChunk = this;
			data.bAllowRays = true;
			data.colliderType = Physics::ColliderType::Collider;
			m_volume.SetData(data);
			m_volume.Register(m_transform.GetWorldMatrix());
		}

		{ // Setup the logic callback(s).
			Logic::CCallback::Data data { };
			data.callbackMap.insert({ Logic::CALLBACK_INTERACT, std::bind(&CNodeChunk::InteractCallback, this, std::placeholders::_1) });
			m_callback.SetData(data);
		}
	}

	void CNodeChunk::BuildMesh()
	{
		{ // Create the mesh data.
			struct Vertex
			{
				Math::Vector3 position;
				Math::Vector3 normal;
				Math::Vector2 texCoord;
			};

			typedef u32 Index;

			Graphics::CMeshData::Data data { };
			data.topology = Graphics::PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			data.vertexStride = sizeof(Vertex);
			data.indexStride = sizeof(Index);

			{ // Count quads.
				std::lock_guard<std::shared_mutex> lk(m_mutex);

				u32 index = 0;
				u32 quadCount = 0;
				for(u32 i = 0; i < m_data.width; ++i)
				{
					for(u32 k = 0; k < m_data.length; ++k)
					{
						for(u32 j = m_data.height - 1; j != ~0; --j)
						{
							m_pBlockList[index].sideFlag = 0;
							if(m_pBlockList[index].id == 0)
							{
								++index;
								continue;
							}

							if(i == 0 || m_pBlockList[internalGetIndex(i - 1, j, k)].id == 0) { ++quadCount; m_pBlockList[index].sideFlag |= SIDE_FLAG_LEFT; }
							if(i == m_data.width - 1 || m_pBlockList[internalGetIndex(i + 1, j, k)].id == 0) { ++quadCount; m_pBlockList[index].sideFlag |= SIDE_FLAG_RIGHT; }

							if(j == 0 || m_pBlockList[internalGetIndex(i, j - 1, k)].id == 0) { ++quadCount; m_pBlockList[index].sideFlag |= SIDE_FLAG_BOTTOM; }
							if(j == m_data.height - 1 || m_pBlockList[internalGetIndex(i, j + 1, k)].id == 0) { ++quadCount; m_pBlockList[index].sideFlag |= SIDE_FLAG_TOP; }

							if(k == 0 || m_pBlockList[internalGetIndex(i, j, k - 1)].id == 0) { ++quadCount; m_pBlockList[index].sideFlag |= SIDE_FLAG_BACK; }
							if(k == m_data.length - 1 || m_pBlockList[internalGetIndex(i, j, k + 1)].id == 0) { ++quadCount;  m_pBlockList[index].sideFlag |= SIDE_FLAG_FRONT; }

							++index;
						}
					}
				}

				data.vertexCount = quadCount << 2;
				data.indexCount = quadCount * 6;
			}
			
			m_meshData.SetData(data);
			m_meshData.Initialize();

			{ // Generate vertex and index data.
				std::shared_lock<std::shared_mutex> lk(m_mutex);

				const float wh = float(m_data.width) * 0.5f;
				const float hh = float(m_data.height) * 0.5f;
				const float lh = float(m_data.length) * 0.5f;

				u32 vIndex = 0;
				u32 iIndex = 0;

				auto GenerateQuad = [&](const Math::Vector3& center, const Math::Vector3& right, const Math::Vector3& up, const Math::Vector3& normal){
					*(Index*)m_meshData.GetIndexAt(iIndex++) = vIndex;
					*(Index*)m_meshData.GetIndexAt(iIndex++) = vIndex + 1;
					*(Index*)m_meshData.GetIndexAt(iIndex++) = vIndex + 2;
					*(Index*)m_meshData.GetIndexAt(iIndex++) = vIndex;
					*(Index*)m_meshData.GetIndexAt(iIndex++) = vIndex + 2;
					*(Index*)m_meshData.GetIndexAt(iIndex++) = vIndex + 3;

					*(Vertex*)m_meshData.GetVertexAt(vIndex++) = { { center - right * 0.5f - up * 0.5f + normal * 0.5f }, normal, Math::Vector2(0.0f, 0.0f) };
					*(Vertex*)m_meshData.GetVertexAt(vIndex++) = { { center + right * 0.5f - up * 0.5f + normal * 0.5f }, normal, Math::Vector2(1.0f, 0.0f) };
					*(Vertex*)m_meshData.GetVertexAt(vIndex++) = { { center + right * 0.5f + up * 0.5f + normal * 0.5f }, normal, Math::Vector2(1.0f, 1.0f) };
					*(Vertex*)m_meshData.GetVertexAt(vIndex++) = { { center - right * 0.5f + up * 0.5f + normal * 0.5f }, normal, Math::Vector2(0.0f, 1.0f) };
				};

				u32 index = 0;
				for(u32 i = 0; i < m_data.width; ++i)
				{
					for(u32 k = 0; k < m_data.length; ++k)
					{
						for(u32 j = m_data.height - 1; j != ~0; --j)
						{
							if(m_pBlockList[index++].id == 0) continue;

							Math::Vector3 center(-wh + i + 0.5f, -hh + j + 0.5f, -lh + k + 0.5f);

							if(i == 0 || m_pBlockList[internalGetIndex(i - 1, j, k)].id == 0)
							{ // Left.
								GenerateQuad(center, Math::VEC3_BACKWARD, Math::VEC3_UP, Math::VEC3_LEFT);
							}
							if(i == m_data.width - 1 || m_pBlockList[internalGetIndex(i + 1, j, k)].id == 0)
							{ // Right.
								GenerateQuad(center, Math::VEC3_FORWARD, Math::VEC3_UP, Math::VEC3_RIGHT);
							}

							if(j == 0 || m_pBlockList[internalGetIndex(i, j - 1, k)].id == 0)
							{ // Bottom.
								GenerateQuad(center, Math::VEC3_RIGHT, Math::VEC3_BACKWARD, Math::VEC3_DOWN);
							}
							if(j == m_data.height - 1 || m_pBlockList[internalGetIndex(i, j + 1, k)].id == 0)
							{ // Top.
								GenerateQuad(center, Math::VEC3_RIGHT, Math::VEC3_FORWARD, Math::VEC3_UP);
							}

							if(k == 0 || m_pBlockList[internalGetIndex(i, j, k - 1)].id == 0)
							{ // Back.
								GenerateQuad(center, Math::VEC3_RIGHT, Math::VEC3_UP, Math::VEC3_BACKWARD);
							}
							if(k == m_data.length - 1 || m_pBlockList[internalGetIndex(i, j, k + 1)].id == 0)
							{ // Front.
								GenerateQuad(center, Math::VEC3_LEFT, Math::VEC3_UP, Math::VEC3_FORWARD);
							}
						}
					}
				}
			}
		}

		{ // Create the mesh renderer.
			m_pMeshRendererList[m_meshIndex] = CFactory::Instance().CreateMeshRenderer(this);

			Graphics::CMeshRenderer::Data data { };
			data.bSkipRegistration = true;
			data.pMaterial = m_pMaterial = reinterpret_cast<Graphics::CMaterial*>(Resources::CManager::Instance().GetResource(Resources::RESOURCE_TYPE_MATERIAL, Math::FNV1a_64("MATERIAL_VOXEL")));
			data.pMeshData = &m_meshData;
			m_pMeshRendererList[m_meshIndex]->SetData(data);

			m_pMeshRendererList[m_meshIndex]->Initialize();
		}
	}

	void CNodeChunk::LateUpdate()
	{
		static const u32 frameBufferHash = Math::FNV1a_32("DrawBuffer");
		static const u32 vpHash = Math::FNV1a_32("VP");

		Math::SIMDMatrix mtx = App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetViewMatrix();
		mtx *= App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetProjectionMatrix();

		m_pMaterial->SetFloat(frameBufferHash, vpHash, mtx.f32, 16);
	}
	
	void CNodeChunk::PreRender()
	{
		static const u32 maxtrixBufferHash = Math::FNV1a_32("DrawBuffer");
		static const u32 worldHash = Math::FNV1a_32("World");

		Math::SIMDMatrix mtx = m_transform.GetWorldMatrix();

		m_pMaterial->SetFloat(maxtrixBufferHash, worldHash, mtx.f32, 16);

		if(m_meshFuture[0].Ready() && m_meshFuture[1].Ready())
		{
			if(m_bDirty)
			{
				App::CSceneManager::Instance().Garbage().Dispose([=](){ SAFE_RELEASE_DELETE(m_pMeshRendererList[(m_meshIndex + 1) & 0x1]); });
				m_meshContainer.SetMeshRenderer(m_pMeshRendererList[m_meshIndex]);
				m_bDirty = false;
			}

			if(!blockDeque.Empty())
			{
				{
					std::lock_guard<std::shared_mutex> lk(m_mutex);

					BlockUpdateData data;
					while(blockDeque.TryPopFront(data))
					{
						m_pBlockList[data.index].id = data.id;
					}
				}

				m_meshIndex = (m_meshIndex + 1) & 0x1;
				m_bDirty = true;
				m_meshFuture[m_meshIndex] = Util::CJobSystem::Instance().JobGraphics([=](){
					m_meshData.Release();
					BuildMesh();
				}, true);
			}
		}
	}

	void CNodeChunk::Release()
	{
		m_volume.Deregister();
		m_meshContainer.Release();
		SAFE_RELEASE_DELETE(m_pMeshRendererList[1]);
		SAFE_RELEASE_DELETE(m_pMeshRendererList[0]);
		m_meshData.Release();

		SAFE_DELETE_ARRAY(m_pBlockList);
	}

	void CNodeChunk::InteractCallback(void* pVal)
	{
		Actor::CPlayerSelector::PickedInfo* pInfo = reinterpret_cast<Actor::CPlayerSelector::PickedInfo*>(pVal);
		if(pInfo->val == 1)
		{
			std::shared_lock<std::shared_mutex> lk(m_mutex);

			if(pInfo->pPlayerSelector->GetSelectionType() == Actor::CPlayerSelector::SelectionType::Break)
			{
				int i, j, k;
				internalGenerateIndicesFromRaycastInfo(pInfo->info, i, j, k);
				i = Math::Clamp(i, 0, m_data.width - 1);
				j = Math::Clamp(j, 0, m_data.height - 1);
				k = Math::Clamp(k, 0, m_data.length - 1);
				const u32 index = GetIndex(i, j, k);
				if(m_pBlockList[index].id != 0)
				{
					BlockUpdateData data { };
					data.id = 0;
					data.index = index;
					blockDeque.PushBack(data);
				}
			}
			else
			{
				int i, j, k;
				internalGenerateIndicesFromRaycastInfo(pInfo->info, i, j, k);
				i = Math::Clamp(i + static_cast<int>(roundf(pInfo->info.normal[0])), 0, m_data.width - 1);
				j = Math::Clamp(j + static_cast<int>(roundf(pInfo->info.normal[1])), 0, m_data.height - 1);
				k = Math::Clamp(k + static_cast<int>(roundf(pInfo->info.normal[2])), 0, m_data.length - 1);

				//if(i < m_data.width && j < m_data.height && k < m_data.length)
				//{
					BlockUpdateData data { };
					data.id = 1;
					data.index = GetIndex(i, j, k);
					blockDeque.PushBack(data);
				//}
			}
		}
	}
	
	//-----------------------------------------------------------------------------------------------
	// File methods.
	//-----------------------------------------------------------------------------------------------
	
	void CNodeChunk::SaveToFile(std::ofstream& file) const
	{
		std::lock_guard<std::shared_mutex> lk(m_mutex);
		
		const u32 total = m_data.width * m_data.height * m_data.length;
		file.write(reinterpret_cast<const char*>(m_pBlockList), sizeof(Block) * total);
	}

	void CNodeChunk::LoadFromFile(std::ifstream& file)
	{
		{
			std::lock_guard<std::shared_mutex> lk(m_mutex);
		
			const u32 total = m_data.width * m_data.height * m_data.length;
			file.read(reinterpret_cast<char*>(m_pBlockList), sizeof(Block) * total);
		}
		
		m_meshIndex = (m_meshIndex + 1) & 0x1;
		m_bDirty = true;
		m_meshFuture[m_meshIndex] = Util::CJobSystem::Instance().JobGraphics([=](){
			m_meshData.Release();
			BuildMesh();
		}, true);
	}
	
	//-----------------------------------------------------------------------------------------------
	// Utility methods.
	//-----------------------------------------------------------------------------------------------
	
	void CNodeChunk::internalGenerateIndicesFromRaycastInfo(const Physics::RaycastInfo& info, int& i, int& j, int& k) const
	{
		int index = info.index;

		index += 1 * (m_data.length + 2) * (m_data.height + 2);
		index -= 1;
		index += 1 * (m_data.height + 2);

		int sj = index % (m_data.height + 2);
		index -= sj;
		sj = static_cast<int>(m_data.height + 2) - 1 - sj;
		index /= static_cast<int>(m_data.height + 2);
		int sk = index % static_cast<int>(m_data.length + 2);
		index /= static_cast<int>(m_data.length + 2);
		int si = index;

		i = si - 1;
		j = sj - 1;
		k = sk - 1;

	}
};
