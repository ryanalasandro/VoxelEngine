//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Actors/CPlayerSelector.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CPlayerSelector.h"
#include "../Application/CAppData.h"
#include "../Universe/CNodeChunk.h"
#include "../Application/CApplication.h"
#include <Physics/CPhysics.h>
#include <Physics/CVolume.h>
#include <Graphics/CMaterial.h>
#include <Resources/CResourceManager.h>
#include <Application/CSceneManager.h>

namespace Actor
{
	CPlayerSelector::CPlayerSelector(const wchar_t* pName, u32 sceneHash) :
		CVObject(pName, sceneHash),
		m_selectionType(SelectionType::Place),
		m_pickedInfo{},
		m_pPickedCallback(nullptr),
		m_quadPlacement(L"Player Selector (Placement)", sceneHash),
		m_quadDeletion(L"Player Selector (Deletion)", sceneHash)
	{
	}

	CPlayerSelector::~CPlayerSelector()
	{
	}

	void CPlayerSelector::Initialize()
	{
		{ // Setup quad.
			Graphics::CPrimQuad::Data data { };
			data.halfSize = 0.5f;
			data.up = Math::VEC3_UP;
			data.right = Math::VEC3_RIGHT;
			data.matHash = Math::FNV1a_64("MATERIAL_PLACEMENT_QUAD");
			m_quadPlacement.SetData(data);
			m_quadPlacement.Initialize();
			m_quadPlacement.SetActive(false);

			data.matHash = Math::FNV1a_64("MATERIAL_DELETION_QUAD");
			m_quadDeletion.SetData(data);
			m_quadDeletion.Initialize();
			m_quadDeletion.SetActive(false);
		}
		
		m_pickedInfo.pPlayerSelector = this;
		m_ray.SetDistance(100.0f);
	}
	
	// Application thread update.
	void CPlayerSelector::Update(bool bPlaying)
	{
		bool bValidState = !bPlaying;
		const bool bValidScene = App::CSceneManager::Instance().GetLoadSceneHash() == App::SCENE_HASH_NODE;
		if(!bPlaying && bValidScene)
		{
			Physics::RaycastInfo rayInfo;

			{
				std::lock_guard<std::mutex> lk(m_pickingMutex);
				rayInfo = m_pickedInfo.info;
			}

			if(rayInfo.pVolume)
			{
				const Universe::CNodeChunk* pChunk = dynamic_cast<const Universe::CNodeChunk*>(rayInfo.pVolume->GetVObject());
		
				if(pChunk)
				{
					int i, j, k;
					pChunk->GenerateIndicesFromRaycastInfo(rayInfo, i, j, k);
					Math::SIMDVector euler = Math::SIMDVector(asinf(rayInfo.normal[1]), atan2f(-rayInfo.normal[0], -rayInfo.normal[2]), 0.0f);
					Math::SIMDVector coord = pChunk->GetPositionFromIndex(i, j, k) + rayInfo.normal * 0.5f;

					m_quadPlacement.GetTransform()->SetEuler(euler);
					m_quadPlacement.GetTransform()->SetPosition(coord);

					m_quadDeletion.GetTransform()->SetEuler(euler);
					m_quadDeletion.GetTransform()->SetPosition(coord);
				}
				else
				{
					bValidState = false;
				}
			}
			else
			{
				bValidState = false;
			}
		}

		UpdateSelectionQuadState(bValidState && bValidScene);
	}

	// Physics thread update.
	void CPlayerSelector::PhysicsUpdate()
	{
		// Cast ray for picking.
		Physics::QueryRay ray { };
		ray.callback = [this](const std::vector<Physics::RaycastInfo>& contacts){
			Physics::RaycastInfo raycastInfo { };
			raycastInfo.distance = FLT_MAX;

			for(size_t i = 0; i < contacts.size(); ++i)
			{
				if(contacts[i].distance < raycastInfo.distance)
				{
					raycastInfo = contacts[i];
				}
			}

			std::lock_guard<std::mutex> lk(m_pickingMutex);
			m_pickedInfo.info = raycastInfo;
		};

		{
			std::lock_guard<std::mutex> lk(m_rayMutex);
			ray.ray = m_ray;
		}

		Physics::CPhysics::Instance().CastRay(ray);
	}

	void CPlayerSelector::Release()
	{
		m_quadDeletion.Release();
		m_quadPlacement.Release();
	}
	
	//-----------------------------------------------------------------------------------------------
	// File methods.
	//-----------------------------------------------------------------------------------------------
	
	void CPlayerSelector::SaveToFile(std::ofstream& file)
	{
		file.write(reinterpret_cast<const char*>(&m_selectionType), sizeof(m_selectionType));
	}

	void CPlayerSelector::LoadFromFile(std::ifstream& file)
	{
		file.read(reinterpret_cast<char*>(&m_selectionType), sizeof(m_selectionType));
		UpdateSelectionQuadState(false);
	}

	//-----------------------------------------------------------------------------------------------
	// Utility methods.
	//-----------------------------------------------------------------------------------------------

	void CPlayerSelector::SetRay(const Math::SIMDVector& origin, const Math::SIMDVector& direction)
	{
		std::lock_guard<std::mutex> lk(m_rayMutex);
		m_ray.SetOrigin(origin);
		m_ray.SetDirection(direction);
	}
	
	void CPlayerSelector::OnSelect(const App::InputCallbackData& callback)
	{
		std::lock_guard<std::mutex> lk(m_pickingMutex);
		if(m_pickedInfo.info.pVolume)
		{
			Logic::CCallback* pCallback = m_pickedInfo.info.pVolume->GetVObject()->FindComponent<Logic::CCallback>();
			if(pCallback)
			{
				m_pickedInfo.val = callback.bPressed ? 1 : 2;
				pCallback->CallCallback(Logic::CALLBACK_INTERACT, &m_pickedInfo);
				m_pPickedCallback = callback.bPressed ? pCallback : nullptr;
			}
		}
		else if(m_pPickedCallback)
		{
			m_pickedInfo.val = 0;
			m_pPickedCallback->CallCallback(Logic::CALLBACK_INTERACT, &m_pickedInfo);
			m_pPickedCallback = nullptr;
		}
	}

	void CPlayerSelector::ToggleSelectionType()
	{
		m_selectionType = static_cast<SelectionType>((static_cast<u32>(m_selectionType) + 1) % static_cast<u32>(SelectionType::COUNT));
	}

	void CPlayerSelector::UpdateSelectionQuadState(bool bEnabled)
	{
		m_quadPlacement.SetActive(m_selectionType == SelectionType::Place && bEnabled);
		m_quadDeletion.SetActive(m_selectionType == SelectionType::Break && bEnabled);
	}
};
