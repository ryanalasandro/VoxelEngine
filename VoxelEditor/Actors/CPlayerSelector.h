//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Actors/CPlayerSelector.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CPLAYERSELECTOR_H
#define CPLAYERSELECTOR_H

#include <Objects/CVObject.h>
#include <Graphics/CPrimQuad.h>
#include <Physics/CPhysicsData.h>
#include <Logic/CCallback.h>
#include <Math/CMathFNV.h>
#include <Application/CInputData.h>
#include <shared_mutex>
#include <fstream>

namespace Graphics
{
	class CMaterial;
};

namespace Actor
{
	class CPlayerSelector : public CVObject
	{
	public:
		struct Data
		{
			float maxDistance = 100.0f;
		};
		
		struct PickedInfo
		{
			u32 val;
			Physics::RaycastInfo info;
			Math::SIMDVector coord;
			CPlayerSelector* pPlayerSelector;
		};

		enum class SelectionType
		{
			Place,
			Break,
			COUNT,
		};

	public:
		CPlayerSelector(const wchar_t* pName, u32 sceneHash);
		~CPlayerSelector();
		CPlayerSelector(const CPlayerSelector&) = delete;
		CPlayerSelector(CPlayerSelector&&) = delete;
		CPlayerSelector& operator = (const CPlayerSelector&) = delete;
		CPlayerSelector& operator = (CPlayerSelector&&) = delete;

		void Initialize() final;
		void Update(bool bPlaying);
		void PhysicsUpdate();
		void Release() final;

		void SaveToFile(std::ofstream& file);
		void LoadFromFile(std::ifstream& file);

		void SetRay(const Math::SIMDVector& origin, const Math::SIMDVector& direction);
		void OnSelect(const App::InputCallbackData& callback);
		void ToggleSelectionType();

		// Accessors.
		inline SelectionType GetSelectionType() const { return m_selectionType; }

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }

	private:
		void UpdateSelectionQuadState(bool bEnabled);

	private:
		mutable std::mutex m_pickingMutex;
		mutable std::mutex m_rayMutex;

		Data m_data;
		
		SelectionType m_selectionType;
		Math::CSIMDRay m_ray;
		Logic::CCallback* m_pPickedCallback;

		Graphics::CPrimQuad m_quadPlacement;
		Graphics::CPrimQuad m_quadDeletion;
		PickedInfo m_pickedInfo;
	};
};

#endif
