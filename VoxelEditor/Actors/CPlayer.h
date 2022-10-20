//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Actors/CPlayer.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CPLAYER_H
#define CPLAYER_H

#include "CPlayerPawn.h"
#include "CPlayerHUD.h"
#include <Objects/CVObject.h>

namespace Actor
{
	class CPlayer : public CVObject
	{
	public:
		CPlayer(const wchar_t* pName, u32 sceneHash);
		~CPlayer();

		CPlayer(const CPlayer&) = delete;
		CPlayer(CPlayer&&) = delete;
		CPlayer& operator = (const CPlayer&) = delete;
		CPlayer& operator = (CPlayer&&) = delete;

		void Initialize() final;
		void Update() final;
		void LateUpdate() final;
		void Release() final;
		
		void SaveToFile(std::ofstream& file) const;
		void LoadFromFile(std::ifstream& file);

		void OnResize(const App::CPanel* pPanel) final;
		
	private:
		CPlayerPawn m_pawn;
		CPlayerHUD m_hud;
	};
};

#endif
