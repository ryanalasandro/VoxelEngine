//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Actors/CPlayer.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CPlayer.h"

namespace Actor
{
	CPlayer::CPlayer(const wchar_t* pName, u32 sceneHash) :
		CVObject(pName, sceneHash),
		m_pawn(L"Player Pawn", sceneHash),
		m_hud(L"Player HUD", sceneHash)
	{
	}

	CPlayer::~CPlayer()
	{
	}

	void CPlayer::Initialize()
	{
		{ // Setup the player pawn.
			CPlayerPawn::Data data { };
			data.speed = 3.5f;
			data.jumpPower = 7.5f;
			data.lookRate = 0.1f;
			data.panRate = 0.5f;
			data.zoomRate = 0.5f;
			m_pawn.SetData(data);
			m_pawn.Initialize();
		}

		{ // Setup the player HUD.
			CPlayerHUD::Data data { };
			m_hud.SetData(data);
			m_hud.Initialize();
		}
	}

	void CPlayer::Update()
	{
		m_pawn.Update();
		m_hud.Update();
	}

	void CPlayer::LateUpdate()
	{
		m_pawn.LateUpdate();
	}

	void CPlayer::Release()
	{
		m_hud.Release();
		m_pawn.Release();
	}

	void CPlayer::SaveToFile(std::ofstream& file) const
	{
		m_hud.SaveToFile(file);
		m_pawn.SaveToFile(file);
	}

	void CPlayer::LoadFromFile(std::ifstream& file)
	{
		m_pawn.LoadFromFile(file);
		m_hud.LoadFromFile(file);
	}

	void CPlayer::OnResize(const App::CPanel* pPanel)
	{
		m_pawn.OnResize(pPanel);
		m_hud.OnResize(pPanel);
	}
};
